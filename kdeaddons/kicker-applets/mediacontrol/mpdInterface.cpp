/***************************************************************************
                           Interface to access mpd
                             -------------------
    begin                : Tue Apr 19 18:31:00 BST 2005
    copyright            : (C) 2005 by William Robinson
    email                : airbaggins@yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mpdInterface.h"

#include <cstring>

#include <qregexp.h>

#include <kmessagebox.h>
#include <kdebug.h>
#include <kurldrag.h>

MpdInterface::MpdInterface()
:   PlayerInterface()
,   sock()
,   sock_mutex()
,   messagebox_mutex()
,   hostname("localhost")
,   port(6600)
,   slider_timer(0)
,   reconnect_timer(0)
{
    connect(&sock, SIGNAL(error(int)), this, SLOT(connectionError(int)));
    connect(&sock, SIGNAL(error(int)), this, SLOT(stopSliderClock()));

    connect(&sock, SIGNAL(connected()), this, SLOT(startSliderClock()));
    connect(&sock, SIGNAL(connected()), this, SLOT(stopReconnectClock()));
    connect(&sock, SIGNAL(connected()), this, SLOT(connected()));

    connect(&sock, SIGNAL(connectionClosed()), this, SLOT(stopSliderClock()));
    connect(&sock, SIGNAL(connectionClosed()), this, SLOT(startReconnectClock()));
    connect(&sock, SIGNAL(connectionClosed()), this, SIGNAL(playerStopped()));

    reconnect();
}

MpdInterface::~MpdInterface()
{
}

void MpdInterface::startSliderClock()
{
    if (!slider_timer)
    {
        //kdDebug(90200) << "Starting slider clock\n";
        slider_timer = startTimer(SLIDER_TIMER_INTERVAL);
    }
}

void MpdInterface::stopSliderClock()
{
    if (slider_timer)
    {
        //kdDebug(90200) << "Stopping slider clock\n";
        killTimer(slider_timer);
        slider_timer=0;
    }
}
void MpdInterface::startReconnectClock()
{
    if (!reconnect_timer)
    {
        //kdDebug(90200) << "Starting Reconnect clock\n";
        reconnect_timer = startTimer(RECONNECT_TIMER_INTERVAL);
    }
}

void MpdInterface::stopReconnectClock()
{
    if (reconnect_timer)
    {
        //kdDebug(90200) << "Stopping Reconnect clock\n";
        killTimer(reconnect_timer);
        reconnect_timer=0;
    }
}


void MpdInterface::timerEvent(QTimerEvent* te)
{
    if (te->timerId() == slider_timer) updateSlider();
    else if (te->timerId() == reconnect_timer) reconnect();
}


void MpdInterface::reconnect() const
{
    if (sock.state()==QSocket::Idle)
    {
        sock_mutex.tryLock();
        //kdDebug(90200) << "Connecting to " << hostname.latin1() << ":" << port << "...\n";
        sock.connectToHost(hostname,port);
    }
}

void MpdInterface::connected()
{
    if (fetchOk()) // unlocks
    {
        //kdDebug(90200) << "Connected ok\n";
        emit playerStarted();
        emit playingStatusChanged(playingStatus());
    }
    else
    {
        //kdDebug(90200) << "Connection error\n";
        emit playerStopped();
    }
}

void MpdInterface::connectionError(int e)
{
    sock_mutex.unlock();
    emit playerStopped();
    QString message;
    if (messagebox_mutex.tryLock())
    {
        switch (e)
        {
            case QSocket::ErrConnectionRefused:
                message=i18n("Connection refused to %1:%2.\nIs mpd running?").arg(hostname).arg(port);
                break;
            case QSocket::ErrHostNotFound:
                message=i18n("Host '%1' not found.").arg(hostname);
                break;
            case QSocket::ErrSocketRead:
                message=i18n("Error reading socket.");
                break;
            default:
                message=i18n("Connection error");
                break;
        }
        // :TODO: KSimpleConfig to prompt for hostname/port values ?
        if (KMessageBox::warningContinueCancel( 0, message,
                                                i18n("MediaControl MPD Error"),
                                                i18n("Reconnect"))==KMessageBox::Continue)
        {
            startReconnectClock();
        }
        else
        {
            stopReconnectClock();
        }
        messagebox_mutex.unlock();
    }
}

bool MpdInterface::dispatch(const char* cmd) const
{
    if (sock.state()==QSocket::Connected && sock_mutex.tryLock())
    {
        long cmd_len=strlen(cmd);
        //kdDebug(90200) << "sending: " << cmd;
        long written=sock.writeBlock(cmd,cmd_len);
        if (written==cmd_len)
        {
            //kdDebug(90200) << "All bytes written\n";
            sock.flush();
            return true;
        }
        else
        {
            //kdDebug(90200) << written << '/' << cmd_len << " bytes written\n";
        }
        sock.flush();
    }
    return false;
}

bool MpdInterface::fetchLine(QString& res) const
{
    QString errormessage;
    while (sock.state()==QSocket::Connected)
    {
        if (!sock.canReadLine())
        {
            sock.waitForMore(20);
            continue;
        }
        res=sock.readLine().stripWhiteSpace();
        //kdDebug(90200) << "received: " << res.latin1() << "\n";
        if (res.startsWith("OK"))
        {
            sock_mutex.unlock();
            // if theres a message and we clear it and there's no other messagebox
            if (!errormessage.isEmpty()
                && dispatch("clearerror\n") && fetchOk()
                && messagebox_mutex.tryLock())
            {
                KMessageBox::error(0,errormessage,i18n("MediaControl MPD Error"));
                messagebox_mutex.unlock();
            }
            return false;
        }
        else if (res.startsWith("ACK"))
        {
            sock_mutex.unlock();
            return false;
        }
        else if (res.startsWith("error: "))
        {
            errormessage=i18n(res.latin1());
        }
        else
        {
            return true;
        }
    }
    sock_mutex.unlock();
    return false;
}

bool MpdInterface::fetchOk() const
{
    QString res;
    while (fetchLine(res)) { }
    if (res.startsWith("OK"))
        return true;
    else
        return false;
}

void MpdInterface::updateSlider()
{
    //kdDebug(90200) << "update slider\n";
    if (!dispatch("status\n")) return;

    QString res;
    QRegExp time_re("time: (\\d+):(\\d+)");
    while(fetchLine(res))
    {
        if (res.startsWith("state: "))
        {
            if (res.endsWith("play"))
            {
                emit playingStatusChanged(Playing);
            }
            else if (res.endsWith("pause"))
            {
                emit playingStatusChanged(Paused);
            }
            else
            {
                emit playingStatusChanged(Stopped);
            }
        }
        else if (time_re.search(res)>=0)
        {
            QStringList timeinfo=time_re.capturedTexts();
            timeinfo.pop_front();
            int elapsed_seconds=timeinfo.first().toInt();
            timeinfo.pop_front();
            int total_seconds=timeinfo.first().toInt();
            emit newSliderPosition(total_seconds,elapsed_seconds);
        }
    }
}

void MpdInterface::sliderStartDrag()
{
    stopSliderClock();
}

void MpdInterface::sliderStopDrag()
{
    startSliderClock();
}

void MpdInterface::jumpToTime(int sec)
{
    reconnect();
    if (!dispatch("status\n")) return;

    long songid=-1;

    QString res;
    QRegExp songid_re("songid: (\\d+)");
    while(fetchLine(res))
    {
        if (songid_re.search(res)>=0)
        {
            QStringList songidinfo=songid_re.capturedTexts();
            songidinfo.pop_front();
            songid=songidinfo.first().toInt();
        }
    }

    if (songid>-1)
    {
        if (dispatch(QString("seekid %1 %2\n").arg(songid).arg(sec).latin1()))
        {
            fetchOk(); // unlocks
        }
    }
}

void MpdInterface::playpause()
{
    reconnect();
    if (playingStatus()==Stopped ? dispatch("play\n") : dispatch("pause\n"))
    {
        fetchOk();
    }
}

void MpdInterface::stop()
{
    reconnect();
    if (dispatch("stop\n")) fetchOk();
}

void MpdInterface::next()
{
    reconnect();
    if (dispatch("next\n")) fetchOk();
}

void MpdInterface::prev()
{
    reconnect();
    if (dispatch("previous\n")) fetchOk();
}


void MpdInterface::changeVolume(int delta)
{
    reconnect();

    if (!dispatch("status\n")) return;

    int volume=-1;

    QString res;
    QRegExp volume_re("volume: (\\d+)");
    while(fetchLine(res))
    {
        if (volume_re.search(res)>=0)
        {
            QStringList info=volume_re.capturedTexts();
            info.pop_front();
            volume=info.first().toInt();
        }
    }

    if (volume>-1)
    {
        volume+=delta;
        if (volume<0) volume=0;
        if (volume>100) volume=100;
        if (dispatch(QString("setvol %1\n").arg(volume).latin1()))
        {
            fetchOk();
        }
    }
}

void MpdInterface::volumeUp()
{
    reconnect();
    changeVolume(5);
}

void MpdInterface::volumeDown()
{
    reconnect();
    changeVolume(-5);
}

void MpdInterface::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept( KURLDrag::canDecode(event) );
}

void MpdInterface::dropEvent(QDropEvent* event)
{
    reconnect();

    KURL::List list;
    if (KURLDrag::decode(event, list))
    {
        if (list.count()==1) // just one file dropped
        {
            // check to see if its in the playlist already
            if (dispatch("playlistid\n"))
            {
                long songid=-1;
                QString file;
                QString res;
                while(fetchLine(res))
                {
                    QRegExp file_re("file: (.+)");
                    QRegExp id_re("Id: (.+)");
                    if (file.isEmpty() && file_re.search(res)>=0)
                    {
                        QStringList info=file_re.capturedTexts();
                        info.pop_front();
                        // if the dropped file ends with the same name, record it
                        if (list.front().path().endsWith(info.first()))
                        {
                            file=info.first().toInt();
                        }
                    }
                    else if (!file.isEmpty() && id_re.search(res)>=0)
                    {
                        // when we have the file, pick up the id (file scomes first)
                        QStringList info=id_re.capturedTexts();
                        info.pop_front();
                        songid=info.first().toInt();
                        fetchOk(); // skip to the end
                        break;
                    }
                }

                // found song, so lets play it
                if (songid>-1)
                {
                    if (dispatch((QString("playid %1\n").arg(songid)).latin1()))
                    {
                        if (fetchOk()) list.pop_front();
                        return;
                    }
                }
            }
        }

        // now if we have got this far, just try to add any files
        for (KURL::List::const_iterator i = list.constBegin(); i!=list.constEnd(); ++i)
        {
            if ((*i).isLocalFile())
            {
                QStringList path=QStringList::split("/",(*i).path());

                while (!path.empty())
                {
                    if (dispatch((QString("add \"")
                                  +path.join("/").replace("\"","\\\"")
                                  +QString("\"\n")).latin1()))
                    {
                        if (fetchOk()) break;
                    }
                    path.pop_front();
                }
            }
            else
            {
                // :TODO: can handle http:// urls but maybe should check port or something
            }
        }
    }
}

const QString MpdInterface::getTrackTitle() const
{
    QString result;

    reconnect();

    if (!dispatch("status\n")) return result;

    long songid=-1;
    QString res;
    while(fetchLine(res))
    {
        QRegExp songid_re("songid: (\\d+)");
        if (songid_re.search(res)>=0)
        {
            QStringList songidinfo=songid_re.capturedTexts();
            songidinfo.pop_front();
            songid=songidinfo.first().toInt();
        }
    }

    if (!(songid>-1)) return result;

    if (!dispatch(QString("playlistid %1\n").arg(songid).latin1()))
        return result;

    QString artist;
    QString album;
    QString title;
    QString track;
    QString file;
    while(fetchLine(res))
    {
        QRegExp artist_re("Artist: (.+)");
        QRegExp album_re("Album: (.+)");
        QRegExp track_re("Album: (.+)");
        QRegExp title_re("Title: (.+)");
        QRegExp file_re("file: (.+)");
        if (artist_re.search(res)>=0)
        {
            QStringList info=artist_re.capturedTexts();
            info.pop_front();
            artist=info.first();
        }
        else if (album_re.search(res)>=0)
        {
            QStringList info=album_re.capturedTexts();
            info.pop_front();
            album=info.first();
        }
        else if (title_re.search(res)>=0)
        {
            QStringList info=title_re.capturedTexts();
            info.pop_front();
            title=info.first();
        }
        else if (track_re.search(res)>=0)
        {
            QStringList info=track_re.capturedTexts();
            info.pop_front();
            track=info.first();
        }
        else if (file_re.search(res)>=0)
        {
            QStringList info=file_re.capturedTexts();
            info.pop_front();
            file=info.first();
        }
    }

    if (!artist.isEmpty())
    {
        if (!title.isEmpty())
            return artist.append(" - ").append(title);
        else if (!album.isEmpty())
            return artist.append(" - ").append(album);
    }
    else if (!title.isEmpty())
    {
        if (!album.isEmpty())
            return album.append(" - ").append(title);
        else
            return title;
    }
    else if (!album.isEmpty())
    {
        if (!track.isEmpty())
            return album.append(" - ").append(track);
        else
            return album;
    }
    return i18n("No tags: %1").arg(file);
}

int MpdInterface::playingStatus()
{
    //kdDebug(90200) << "looking up playing status\n";
    if (!dispatch("status\n")) return Stopped;

    PlayingStatus status=Stopped;
    QString res;
    while(fetchLine(res))
    {
        if (res.startsWith("state: "))
        {
            if (res.endsWith("play")) status=Playing;
            else if (res.endsWith("pause")) status=Paused;
            else status=Stopped;
        }
    }

    return status;
}

#include "mpdInterface.moc"
