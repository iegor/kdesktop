/**************************************************************************

    kmidclient.h  - The main client widget of KMid
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#ifndef _KMIDCLIENT_H
#define _KMIDCLIENT_H

#include "qslidertime.h"
#include <libkmid/player.h>
#include <libkmid/track.h>
#include <libkmid/notearray.h>
#include <libkmid/libkmid.h>
#include <qtimer.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "kdisptext.h"
#include "slman.h"
#include "version.h"
#include "kmidIface.h"

#include <kio/job.h>

class DeviceManager;


class KApplication;
class KConfig;
class KLCDNumber;
class QLabel;
class QComboBox;
class RhythmView;
class ChannelView;
class QString;

class kmidClient : public QWidget, virtual public KMidIface
{
    Q_OBJECT
private:
#ifdef KMidDEBUG
    long passcount;
#endif

    DeviceManager *midi;
    MidiPlayer *player;

    struct kMidData m_kMid;

    QTimer *timer4timebar;
    QTimer *timer4events;

    ulong beginmillisec;
    ulong pausedatmillisec;
    double currentTempo;
    SpecialEvent *spev;
    NoteArray *noteArray;

    bool downloaded;

    int	itsme;
    bool shuttingdown;
    int visiblevolumebar;
    
    char *midifile_opened;
    int hasbeenopened;

    int typeoftextevents;

    SLManager *slman;
    int activecollection;
    QString collectionsfile;
    SongList *currentsl;
    int initializing_songs;
    int loopsong;
    int collectionplaymode;
    int *collectionplaylist; // the list of songs ordered in the
				// user selected mode

    class KActionCollection *actionCollection;
   
    void generateCPL(void);
    int searchInCPL(int song); // Returns the index of song

    void fillInComboSongs(void);

    int openFile(const char *filename);
    void allNotesOff(void);
public:
    kmidClient(QWidget *parent, KActionCollection *ac, const char *name=0);
    ~kmidClient();

    char *midiFileName(void) {return midifile_opened;};
			// If it returns NULL then there isn't any file opened

    int isPlaying(void) {return m_kMid.pctl->playing;};
    int isPaused(void) {return (m_kMid.pctl->playing)&&(m_kMid.pctl->paused);};

    int openURL(const QString s);

    void repaintText(int typeoftextevents);
    static void kmidOutput(void);

    int ChooseTypeOfTextEvents(void);

    QFont *getFont(void);
    void fontChanged(void); // The new font is already in KConfig

    SLManager *getSLManager(void) {return slman;};
    void setSLManager(SLManager *slm);
		// setSLManager only change the pointer, so DO NOT DELETE
		// the objectr you pass to it
    int getActiveCollection(void) {return activecollection;};
    void setActiveCollection(int i);
    int getSelectedSong(void);
    void saveCollections(void);

    void setSongType(int i);
    void setSongLoop(int i);
    void setCollectionPlayMode(int i);

    void visibleVolumeBar(int i); // 1 shows it, and 0 hides it
    //    int  isVisibleVolumeBar(void) {return visiblevolumebar;};
    void visibleChannelView(int i);

    void shuttingDown(void);

    ulong timeOfNextEvent(int *type=NULL);
    void rethinkNextEvent(void);
            // Recalculates time of next event and updates the timer4events according to it


    void moveEventPointersTo(ulong ms);

protected:
//    void resizeEvent(QResizeEvent *qre);


public slots:
//    void help_Help();
//    void help_About();
    void slotPlay();
    void slotPause();
    void slotStop();
    void slotRewind();
    void slotForward();
    void slotPrevSong();
    void slotNextSong();

    void timebarUpdate();
    void slotSeek(int i);
    void slotSetVolume(int i);
    void slotSelectSong(int i);
    void slotSelectEncoding(int i);

    void downloadFinished( KIO::Job * );

    void processSpecialEvent();

    void channelViewDestroyed();

    void communicationFromChannelView(int *);

    void slotSetTempo(double value);
    
signals:
    void mustRechooseTextEvent();
    void stopPause();
//    void channelView_Destroyed();

public:
    void saveLyrics(FILE *fh);

    DeviceManager *devman(void) {return midi;};
    void setMidiDevice(int i);
    void setMidiMapFilename(const char *mapfilename);

    ChannelView *getChannelView(void) { return channelView; };
    KDisplayText *getKDisplayText(void) { return kdispt; };

    QSizePolicy sizePolicy();

    QComboBox *getComboSongs() { return comboSongs; };


    void play();
    void pause();
    void stop();
    void rewind();
    void forward();
    void seek(int ms);
    void prevSong();
    void nextSong();
    void setVolume(int i);
    void setTempo(int i);
    void setSongEncoding( int i );
    void setLyricEvents( int i );
    void setCurrentSong(int i);
    void setPlayListMode(int i);

    QSize sizeHint() const;
private:
    QSlider *timebar;
    QSliderTime *timetags;
    QSlider *volumebar;

    KLCDNumber *tempoLCD;
    KDisplayText *kdispt;
    QLabel *qlabelTempo;
    QComboBox *comboSongs;
    QComboBox *comboEncodings;
    RhythmView *rhythmview;

    
    ChannelView *channelView;

};

char *extractFilename(const char *in,char *out); // returns a pointer to out

#endif

