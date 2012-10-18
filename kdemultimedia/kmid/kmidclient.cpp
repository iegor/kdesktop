/**************************************************************************

    kmidclient.cpp  - The main client widget of KMid
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>   // kill is declared on signal.h on bsd, not sys/signal.h
#include <sys/signal.h>

#include <qkeycode.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qlabel.h>
#include <qfile.h>
#include <qcombobox.h>
#include <qlayout.h>

#include <kapplication.h>
#include <kcharsets.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <kaction.h>
#include <kdebug.h>

#include <libkmid/midimapper.h>
#include <libkmid/fmout.h>
#include <libkmid/track.h>
#include <libkmid/midispec.h>
#include <libkmid/deviceman.h>
#include <libkmid/mt32togm.h>
#include "kmidclient.h"
#include "klcdnumber.h"
#include "randomlist.h"
#include "channelview.h"
#include "channel.h"
#include "version.h"
#include "rhythmview.h"

//#define TEMPHACK


kmidClient::kmidClient(QWidget *parent, KActionCollection *ac, const char *name)
    :  DCOPObject("KMidIface"), QWidget(parent,name)
{
    actionCollection=ac;
    KConfig *cfg=kapp->config();
    cfg->setGroup("KMid");
    midifile_opened=0L;
    loopsong=cfg->readNumEntry("Loop",0);
    collectionplaymode=0;
    collectionplaylist=0L;
    channelView=0L;
    noteArray=0L;
    shuttingdown=false;

    KConfig *kconf=KGlobal::instance()->config();

    kconf->setGroup("KMid");
    QString tmp2 = locateLocal("appdata", "collections");
    collectionsfile=kconf->readPathEntry("CollectionsFile",tmp2);
    slman=new SLManager();
    slman->loadConfig(QFile::encodeName(collectionsfile));
    currentsl=NULL;
    //	currentsl=slman->getCollection(activecollection);
    itsme=0;
    m_kMid.pid=0;
    timebar = new QSlider(0,240000,30000,60000,QSlider::Horizontal, this);
    timebar->setSteps(30000,60000);
    timebar->setValue(0);
    connect (timebar,SIGNAL(valueChanged(int)),this,SLOT(slotSeek(int)));

    timetags = new QSliderTime(timebar,this);
    timetags->setMinimumSize(timetags->sizeHint());

    qlabelTempo= new QLabel(i18n("Tempo:"), this,"tempolabel",
                            QLabel::NoFrame);

    tempoLCD = new KLCDNumber( true, 3, this, "TempoLCD");
    tempoLCD->setValue(120);
    tempoLCD->display(120);
    currentTempo=120;
    tempoLCD->setRange(3,999);
    tempoLCD->setDefaultValue(120);
    tempoLCD->setUserSetDefaultValue(true);
    tempoLCD->setMinimumSize(tempoLCD->sizeHint());
    connect(tempoLCD,SIGNAL(valueChanged(double)),this,SLOT(slotSetTempo(double)));

    comboSongs = new QComboBox(FALSE, this,"Songs");
    connect (comboSongs,SIGNAL(activated(int)),this,SLOT(slotSelectSong(int)));
    comboSongs->setMinimumWidth(200);
    
    comboEncodings = new QComboBox(FALSE, this, "Encodings");
    connect (comboEncodings,SIGNAL(activated(int)),this,SLOT(slotSelectEncoding(int)));
    comboEncodings->insertItem(i18n("Default"));
    comboEncodings->insertStringList( KGlobal::charsets()->descriptiveEncodingNames() );
    comboEncodings->setCurrentItem(0);

    rhythmview= new RhythmView( this, "RhythmView");
    rhythmview->setMaximumHeight(7);
    rhythmview->setMinimumHeight(7);

    volumebar = new QSlider(0,200,10,100,QSlider::Vertical, this );
    volumebar->setSteps(10,20);
    volumebar->setValue(100);
    volumebar->setTickmarks(QSlider::NoMarks);
    volumebar->setTickInterval(50);
    connect (volumebar,SIGNAL(valueChanged(int)),this,SLOT(slotSetVolume(int)));

    visiblevolumebar=cfg->readNumEntry("ShowVolumeBar",0);
    if (visiblevolumebar) volumebar->show();
    else volumebar->hide();

    typeoftextevents=1;
    kdispt=new KDisplayText( this, "KaraokeWindow");
    kdispt->show();

    timer4timebar=new QTimer(this);
    connect (timer4timebar,SIGNAL(timeout()),this,SLOT(timebarUpdate()));
    timer4events=new QTimer(this);
    connect (timer4events,SIGNAL(timeout()),this,SLOT(processSpecialEvent()));

    QString samplefile =
      KGlobal::dirs()->findAllResources("appdata", "fm/*.o3").last();
    samplefile.truncate(samplefile.findRev('/'));
    FMOut::setFMPatchesDirectory(QFile::encodeName(samplefile));

    m_kMid.pctlsmID=shmget(IPC_PRIVATE,sizeof(PlayerController),0666 | IPC_CREAT );
    if (m_kMid.pctlsmID==-1)
    {
        printf("ERROR: Cannot allocate shared memory !!!\n"
               "Please report to larrosa@kde.org\n");
	exit(1);
    }

    m_kMid.pctl=(PlayerController *)shmat(m_kMid.pctlsmID,0L,0);
    if (!m_kMid.pctl)
        printf("ERROR: Cannot get shared memory !!! "
               "Please report to larrosa@kde.org\n");
    m_kMid.pctl->playing=0;
    m_kMid.pctl->gm=1;
    m_kMid.pctl->volumepercentage=100;
    m_kMid.pctl->tempo=500000;
    m_kMid.pctl->ratioTempo=1.0;
    for (int i=0;i<16;i++)
    {
        m_kMid.pctl->forcepgm[i]=0;
        m_kMid.pctl->pgm[i]=0;
    }


    kconf->setGroup("KMid");
    int mididev=kconf->readNumEntry("MidiPortNumber",-1);

    midi = new DeviceManager(mididev);
    midi->initManager();
    m_kMid.midi=midi;
    player= new MidiPlayer(midi,m_kMid.pctl);

    kconf->setGroup("Midimapper");
    QCString qs=QFile::encodeName(kconf->readPathEntry("Loadfile","gm.map"));

#ifdef KMidDEBUG
    printf("Read Config file: %s\n",qs.data());
#endif
    setMidiMapFilename(qs.data());

    initializing_songs=1;
    kconf->setGroup("KMid");
    setActiveCollection(kconf->readNumEntry("ActiveCollection",0));
    initializing_songs=0;

    QVBoxLayout *lv=new QVBoxLayout( this );
    lv->addWidget( timebar );
    lv->addWidget( timetags );
    lv->addSpacing(5);
    QHBoxLayout *lh=new QHBoxLayout( lv );
    lh->addWidget( qlabelTempo );
    lh->addWidget( tempoLCD );
    lh->addWidget( comboSongs, 6 );
    lv->addSpacing(5);    
    lh->addWidget( comboEncodings, 1 );    
    lv->addSpacing(5);        
    lv->addWidget( rhythmview );
    lv->addSpacing(2);
    QHBoxLayout *lh2=new QHBoxLayout( lv );
    lh2->addWidget( volumebar );
    lh2->addWidget( kdispt );
}

/*void kmidClient::resizeEvent(QResizeEvent *)
{
    //timebar->resize(width()-5,timebar->height());
    timebar->setGeometry(5,10,width()-5,timebar->height());
    timetags->setGeometry(5,10+timebar->height(),width()-5,timetags->getFontHeight());
    comboSongs->setGeometry(tempoLCD->x()+tempoLCD->width()+15,tempoLCD->y(),width()-(tempoLCD->x()+tempoLCD->width()+25),tempoLCD->height());
    rhythmview->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+2,width()-10,7);
    volumebar->setGeometry(5,10+timebar->height()+timetags->height()+5+tempoLCD->height()+10,15,height()-(10+timebar->height()+timetags->height()+5+tempoLCD->height()+15));
    kdispt->setGeometry(((visiblevolumebar)?25:5),10+timebar->height()+timetags->height()+5+tempoLCD->height()+10,width()-(5+((visiblevolumebar)?25:5)),height()-(10+timebar->height()+timetags->height()+5+tempoLCD->height()+10));
}
*/

kmidClient::~kmidClient()
{
    if (m_kMid.pctl->playing==1)
    {
        stop();
        //    sleep(1);
    }

    if (m_kMid.pid!=0)
    {
        kill(m_kMid.pid,SIGTERM);
        waitpid(m_kMid.pid, 0L, 0);
	m_kMid.midi->closeDev();
        m_kMid.pid=0;
    }

    allNotesOff();

    delete midifile_opened;
    delete player;
    delete midi;
    delete [] collectionplaylist;

    saveCollections();
    delete slman;

// Let's detach and delete shared memory
    shmdt((char *)m_kMid.pctl);
    shmctl(m_kMid.pctlsmID, IPC_RMID, 0L);
    m_kMid.pctl=0L;
}

// Use KURL::filename ! (David)
char *extractFilename(const char *in,char *out)
{
    char *p=(char *)in;
    char *result=out;
    char *filename=(char *)in;
    while (*p!=0)
    {
        if (*p=='/') filename=p+1;
        p++;
    }
    while (*filename!=0)
    {
        *out=*filename;
        out++;
        filename++;
    }
    *out=0;
    return result;
}

int kmidClient::openFile(const char *filename)
{
    m_kMid.pctl->message|=PLAYER_HALT;
    stop();
    int r;
    player->setGenerateBeats(true);
    if ((r=player->loadSong(filename))!=0)
    {
        QString errormsg;
        switch (r)
        {
        case (-1) : errormsg =
                            i18n("The file %1 does not exist or cannot be opened.").arg(filename);
        break;
        case (-2) : errormsg =
                            i18n("The file %1 is not a MIDI file.").arg(filename);break;
        case (-3) : errormsg =
                            i18n("Ticks per quarter note is negative. Please send this file to larrosa@kde.org");break;
        case (-4) : errormsg =
                            i18n("Not enough memory.");break;
        case (-5) : errormsg =
                            i18n("This file is corrupted or not well built.");break;
        case (-6) : errormsg =
                            i18n("%1 is not a regular file.").arg(filename);break;
        default :   errormsg = i18n("Unknown error message");break;
        }
        KMessageBox::error(this, errormsg);
        //	player->loadSong(midifile_opened);
        if (midifile_opened) delete midifile_opened;
        midifile_opened=0L;
        timebar->setRange(0,240000);
        timebar->setValue(0);
        timetags->repaint(TRUE);
        kdispt->ClearEv();
        kdispt->repaint(TRUE);
        topLevelWidget()->setCaption("KMid");

        return -1;
    }

    if (midifile_opened) delete midifile_opened;
    midifile_opened=new char[strlen(filename)+1];
    strcpy(midifile_opened,filename);
#ifdef KMidDEBUG
    printf("TOTAL TIME: %g milliseconds\n",player->information()->millisecsTotal);
#endif
    //    noteArray=player->parseNotes();
    noteArray=player->noteArray();
    timebar->setRange(0,(int)(player->information()->millisecsTotal));
    timetags->repaint(TRUE);
    kdispt->ClearEv();
    spev=player->specialEvents();
    while (spev)
    {
        if ((spev->type==1) || (spev->type==5))
        {
            kdispt->AddEv(spev);
        }
        spev=spev->next;
    }

    kdispt->calculatePositions();
    kdispt->CursorToHome();
//    kdispt->updateScrollBars();
    emit mustRechooseTextEvent();
    kdispt->repaint(TRUE);
    tempoLCD->display(tempoToMetronomeTempo(m_kMid.pctl->tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(m_kMid.pctl->tempo)*m_kMid.pctl->ratioTempo);

    char *fn=new char[strlen(filename)+20];
    extractFilename(filename,fn);
    char *capt=new char[strlen(fn)+20];
    sprintf(capt,"KMid - %s",fn);
    delete fn;
    topLevelWidget()->setCaption(capt);
    delete capt;

    timebar->setValue(0);
    return 0;
}

int kmidClient::openURL(const QString _url)
{
    KURL u(_url);
    if (!u.isValid()) {printf("Malformed URL\n");return -1;};

    QString filename;
    bool deleteFile=false;
    if (!u.isLocalFile())
    {
	filename = QString("/tmp/") + u.filename();
	KIO::Job *iojob = KIO::copy( u, KURL::fromPathOrURL( filename ) );
	downloaded=false;
	connect( iojob, SIGNAL( result( KIO::Job *) ), this, SLOT(downloadFinished( KIO::Job * ) ) );

	if (!downloaded)
            kapp->enter_loop();
	deleteFile=true;

    }
    else
    {
	filename=u.path();
    }

    QCString filename_8bit = QFile::encodeName(filename);
    int r=-1;
    if (!filename_8bit.isEmpty())
    {
        r=openFile(filename_8bit.data());

        KConfig *cfg=KGlobal::instance()->config();
        if (cfg->readBoolEntry("deleteTmpNonLocalFiles",false))
        {
            unlink(filename_8bit.data());
        }
    }
    return r;
}

ulong kmidClient::timeOfNextEvent(int *type)
{
    int t=0;
    ulong x=0;


    if (!channelView)
    {
        if ((spev)&&(spev->type!=0))
        {
            t=1;
            x=spev->absmilliseconds;
        }
    }
    else
    {
        if (noteArray)
        {
            NoteArray::noteCmd *ncmd=noteArray->get();
            if (!ncmd)
            {
                if ((spev)&&(spev->type!=0))
                {
                    t=1;
                    x=spev->absmilliseconds;
                }
            }
            else
            {
                if ((!spev)||(spev->type==0))
                {
                    t=2;
                    x=ncmd->ms;
                }
                else
                {
                    if (spev->absmilliseconds<ncmd->ms)
                    {
                        t=1;
                        x=spev->absmilliseconds;
                    }
                    else
                    {
                        t=2;
                        x=ncmd->ms;
                    }

                }
            }
        }
    }

    if (type) *type=t;
    return x;
    /*

     if (type!=NULL) *type=0;
     if (channelView==NULL)
     {
     if ((spev!=NULL)&&(spev->type!=0))
     {
     if (type!=NULL) *type=1;
     return spev->absmilliseconds;
     }
     else return 0;
     }

    if (noteArray==NULL) return 0;
    noteCmd *ncmd=noteArray->get();
    if (ncmd==NULL)
    {
        if ((spev!=NULL)&&(spev->type!=0))
        {
            if (type!=NULL) *type=1;
            return spev->absmilliseconds;
        }
        else return 0;
    }
    else
    {
        if ((spev==NULL)||(spev->type==0))
        {
            if (type!=NULL) *type=2;
            return ncmd->ms;
        }
        else
        {
            if (spev->absmilliseconds<ncmd->ms)
            {
                if (type!=NULL) *type=1;
                return spev->absmilliseconds;
            }
            else
            {
                if (type!=NULL) *type=2;
                return ncmd->ms;
            }

        }
    }
    */
}

void kmidClient::slotPlay()
{
    if (!player->isSongLoaded())
    {
        KMessageBox::sorry(this,
                         i18n("You must load a file before playing it."));
        return;
    }
    if (m_kMid.pctl->playing==1)
    {
        KMessageBox::sorry(this,
                         i18n("A song is already being played."));
        return;
    }
    if (midi->checkInit()==-1)
    {
        KMessageBox::error(this,
			i18n("Could not open /dev/sequencer.\nProbably there is another program using it."));
        return;
    }

    kdispt->CursorToHome();
    m_kMid.pctl->message=0;
    m_kMid.pctl->playing=0;
    m_kMid.pctl->finished=0;
    m_kMid.pctl->error=0;
    m_kMid.pctl->SPEVplayed=0;
    m_kMid.pctl->SPEVprocessed=0;
#ifdef KMidDEBUG
    passcount=0;
#endif
    noteArray->iteratorBegin();

    QApplication::flushX();
    if ((m_kMid.pid=fork())==0)
    {
#ifdef KMidDEBUG
        printf("PlayerProcessID: %d\n",getpid());
#endif
        player->play(0,(void (*)(void))kmidOutput);
#ifdef KMidDEBUG
        printf("End of child process\n");
#endif
        _exit(0);
    }
    m_kMid.pctl->millisecsPlayed=0;


    spev=player->specialEvents();
#ifdef KMidDEBUG
    printf("writing SPEV\n");
    player->debugSpecialEvents();
    printf("writing SPEV(END)\n");
#endif

    while ((m_kMid.pctl->playing==0)&&(m_kMid.pctl->error==0)) ;

    if (m_kMid.pctl->error==1) return;
    beginmillisec=m_kMid.pctl->beginmillisec;

    int type;
    ulong x=timeOfNextEvent(&type);
    if (type!=0)
        timer4events->start(x,TRUE);

    timer4timebar->start(1000);

#ifdef KMidDEBUG
    printf("PlayerProcess: %d . ParentProcessID: %d\n",m_kMid.pid,getpid());
    printf("******************************-\n");
#endif
}

void kmidClient::timebarUpdate()
{
    itsme=1;
    if (m_kMid.pctl->playing==0)
    {
        timer4timebar->stop();
    }

    timeval tv;
    gettimeofday(&tv, NULL);
    ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    m_kMid.pctl->millisecsPlayed=(currentmillisec-beginmillisec);

    timebar->setValue((int)(m_kMid.pctl->millisecsPlayed));
    itsme=0;
    if ((m_kMid.pctl->playing==0)&&(m_kMid.pctl->finished==1))
    {
        waitpid(m_kMid.pid, NULL, 0);
        if (loopsong)
        {
            play();
            return;
        }
        else
            nextSong();
    }
}

void kmidClient::slotSeek(int i)
{
    if (itsme) return;

    if (m_kMid.pctl->playing==0)
    {
        itsme=1;
        timebar->setValue(0);
        itsme=0;
        return;
    }

    if (m_kMid.pctl->paused) return;

    if (m_kMid.pid!=0)
    {
        kill(m_kMid.pid,SIGTERM);
#ifdef KMidDEBUG
        printf("Waiting for Process %d to be killed\n",m_kMid.pid);
#endif
        waitpid(m_kMid.pid, NULL, 0);
	m_kMid.midi->closeDev();
        m_kMid.pid=0;
    }
    allNotesOff();


#ifdef KMidDEBUG
    printf("change Time: %d\n",i);
#endif

    timer4events->stop();
    if (channelView!=NULL) channelView->reset(0);

    moveEventPointersTo((ulong)i);

    m_kMid.pctl->playing=0;
    m_kMid.pctl->OK=0;
    m_kMid.pctl->error=0;
    m_kMid.pctl->gotomsec=i;
    m_kMid.pctl->message|=PLAYER_SETPOS;

    QApplication::flushX();
    if ((m_kMid.pid=fork())==0)
    {
#ifdef KMidDEBUG
        printf("Player_ProcessID: %d\n",getpid());
#endif

        player->play(0,(void (*)(void))kmidOutput);

#ifdef KMidDEBUG
        printf("End of child process\n");
#endif
        _exit(0);
    }

    while ((m_kMid.pctl->playing==0)&&(m_kMid.pctl->error==0)) ;

    if (m_kMid.pctl->error==1) return;
    beginmillisec=m_kMid.pctl->beginmillisec-i;
    ulong currentmillisec=m_kMid.pctl->beginmillisec;

    int type;
    ulong x=timeOfNextEvent(&type);
    if (type!=0)
        timer4events->start(x-(currentmillisec-beginmillisec),TRUE);

    /*
     if (spev==NULL) return;
     ulong delaymillisec=spev->absmilliseconds-(currentmillisec-beginmillisec);
     timer4events->start(delaymillisec,TRUE);
     */

    m_kMid.pctl->OK=0;
/*
    tempoLCD->display(tempoToMetronomeTempo(m_kMid.pctl->tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(m_kMid.pctl->tempo)*m_kMid.pctl->ratioTempo);
*/
}

void kmidClient::moveEventPointersTo(ulong ms)
{
#ifdef KMidDEBUG
    printf("Move To: %lu\n",ms);
#endif
    spev=player->specialEvents();

    ulong tempo=(ulong)(500000 * m_kMid.pctl->ratioTempo);
    int num=4;
    int den=4;

    while ((spev!=NULL)&&(spev->absmilliseconds<ms))
    {
        if (spev->type==3) tempo=spev->tempo;
        else if (spev->type==6) {num=spev->num;den=spev->den;}
        spev=spev->next;
    }
    tempoLCD->display(tempoToMetronomeTempo(tempo));
    currentTempo=tempoLCD->getValue();
    tempoLCD->setDefaultValue(tempoToMetronomeTempo(tempo)*m_kMid.pctl->ratioTempo);

    rhythmview->setRhythm(num,den);

    kdispt->gotomsec(ms);
//    if (noteArray!=NULL) noteArray->moveIteratorTo(ms);
    if (noteArray!=NULL)
    {
        int pgm[16];
        noteArray->moveIteratorTo(ms,pgm);
        if (channelView!=NULL)
        {
            for (int j=0;j<16;j++)
            {
                if (!m_kMid.pctl->forcepgm[j]) channelView->changeInstrument(j,(m_kMid.pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                else channelView->changeInstrument(j,(m_kMid.pctl->pgm[j]));
            }
        }
    }

    /*
     if (noteArray!=NULL)
     {
     noteCmd *ncmd;
     noteArray->iteratorBegin();
     ncmd=noteArray->get();
     while ((ncmd!=NULL)&&(ncmd->ms<ms))
     {
     noteArray->next();
     ncmd=noteArray->get();
     }
     }
     */
}

void kmidClient::slotSetVolume(int i)
{
    int autochangemap=0;
    if ((m_kMid.pctl->playing==1)&&(m_kMid.pctl->paused==0)) autochangemap=1;

    if (autochangemap)
    {
        pause();
    }
    i=200-i;
    m_kMid.pctl->volumepercentage=i;

    if (autochangemap)
    {
        pause();
    }
}


void kmidClient::slotPrevSong()
{
    if (currentsl==NULL) return;
    if (collectionplaylist==NULL) generateCPL();
    if (collectionplaylist==NULL) return;
    /*
     if (collectionplaymode==0)
     {
     if (currentsl->getActiveSongID()==1) return;
     currentsl->previous();
     }
     else
     {
     int r;
     while ((r=1+(int) ((double)(currentsl->NumberOfSongs())*rand()/(RAND_MAX+1.0)))==currentsl->getActiveSongID()) ;

     currentsl->setActiveSong(r);
     }
     */
    int idx=searchInCPL(currentsl->getActiveSongID());
    if (idx==0) return;
    idx--;
    currentsl->setActiveSong(collectionplaylist[idx]);

    if (currentsl->getActiveSongID()==-1)
    {
        //    comboSongs->setCurrentItem(0);
        //    currentsl->setActiveSong(1);
        return;
    }

    if (m_kMid.pctl->paused) emit stopPause();
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    play();

}

void kmidClient::slotNextSong()
{
    if (currentsl==NULL) return;
    if (collectionplaylist==NULL) generateCPL();
    if (collectionplaylist==NULL) return;

    /*if (collectionplaymode==0)
     {
     if (currentsl->getActiveSongID()==currentsl->NumberOfSongs()) return;
     currentsl->next();
     }
     else
     {
     int r;
     while ((r=1+(int) ((double)(currentsl->NumberOfSongs())*rand()/(RAND_MAX+1.0)))==currentsl->getActiveSongID()) ;

     #ifdef KMidDEBUG
     printf("random number:%d\n",r);
     #endif
     currentsl->setActiveSong(r);
     }
     */
    int idx=searchInCPL(currentsl->getActiveSongID());
    idx++;
    if (idx==currentsl->NumberOfSongs()) return;
    currentsl->setActiveSong(collectionplaylist[idx]);
    if (currentsl->getActiveSongID()==-1)
    {
        ////    comboSongs->setCurrentItem(0);
        //    currentsl->setActiveSong(1);
        return;
    }

    if (m_kMid.pctl->paused) emit stopPause();
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    play();
}

void kmidClient::slotPause()
{
    if (m_kMid.pctl->playing==0) return;
#ifdef KMidDEBUG
    printf("song Pause\n");
#endif
    if (m_kMid.pctl->paused==0)
    {
        if (m_kMid.pid!=0)
        {
            kill(m_kMid.pid,SIGTERM);
            waitpid(m_kMid.pid, NULL, 0);
	    m_kMid.midi->closeDev();
            m_kMid.pid=0;
        }
        pausedatmillisec=(ulong)m_kMid.pctl->millisecsPlayed;
        m_kMid.pctl->paused=1;
        timer4timebar->stop();
        timer4events->stop();
        allNotesOff();
        //    kill(m_kMid.pid,SIGSTOP);
        //   The previous line doesn't work because it stops the two processes (!?)
    }
    else
    {
        m_kMid.pctl->playing=0;
        m_kMid.pctl->OK=0;
        m_kMid.pctl->error=0;
        m_kMid.pctl->gotomsec=pausedatmillisec;
        m_kMid.pctl->message|=PLAYER_SETPOS;

	QApplication::flushX();
        if ((m_kMid.pid=fork())==0)
        {
#ifdef KMidDEBUG
            printf("PlayerProcessID: %d\n",getpid());
#endif
            player->play(0,(void (*)(void))kmidOutput);
#ifdef KMidDEBUG
            printf("End of child process\n");
#endif
            _exit(0);
        }

        while ((m_kMid.pctl->playing==0)&&(m_kMid.pctl->error==0)) ;

        if (m_kMid.pctl->error) return;

        m_kMid.pctl->OK=0;
        m_kMid.pctl->paused=0;

        beginmillisec=m_kMid.pctl->beginmillisec-pausedatmillisec;
        ulong currentmillisec=m_kMid.pctl->beginmillisec;

        int type;
        ulong x=timeOfNextEvent(&type);
        if (type!=0)
            timer4events->start(x-(currentmillisec-beginmillisec),TRUE);
        timer4timebar->start(1000);

        if (noteArray!=NULL)
        {
            int pgm[16];
            noteArray->moveIteratorTo(pausedatmillisec,pgm);
            if (channelView!=NULL)
            {
                for (int j=0;j<16;j++)
                {
                    if (!m_kMid.pctl->forcepgm[j]) channelView->changeInstrument(j,(m_kMid.pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                    else channelView->changeInstrument(j,(m_kMid.pctl->pgm[j]));
                }
            }

        }

    }
}

void kmidClient::shuttingDown(void)
{
    shuttingdown=true;
    stop();
}

void kmidClient::slotStop()
{
    if (!m_kMid.pctl) return;

    if (!shuttingdown)
    {
         for (int i=0;i<16;i++) m_kMid.pctl->forcepgm[i]=FALSE;
       if (channelView) channelView->reset();
       if (tempoLCD)
       {
         tempoLCD->display(tempoToMetronomeTempo(m_kMid.pctl->tempo));
         currentTempo=tempoLCD->getValue();
         tempoLCD->setDefaultValue(tempoToMetronomeTempo(m_kMid.pctl->tempo)*m_kMid.pctl->ratioTempo);
       }
    }

    if (m_kMid.pctl->playing==0) return;

    if (m_kMid.pctl->paused) return;
#ifdef KMidDEBUG
    printf("song Stop\n");
#endif
    if (m_kMid.pid!=0)
    {
        kill(m_kMid.pid,SIGTERM);
#ifdef KMidDEBUG
	printf("Killing\n");
#endif
        waitpid(m_kMid.pid, NULL, 0);
	m_kMid.midi->closeDev();
        m_kMid.pid=0;
    }

    m_kMid.pctl->playing=0;
    ////////m_kMid.pctl->OK=0;
    ////////m_kMid.pctl->message|=PLAYER_HALT;
    timer4timebar->stop();
    timer4events->stop();

    allNotesOff();

    //m_kMid.pctl->playing=0;
    //m_kMid.pctl->paused=0;
    ////////while (m_kMid.pctl->OK==0) ;
}

void kmidClient::slotRewind()
{
    if ((m_kMid.pctl->playing)&&(!m_kMid.pctl->paused))
    {
        timebar->subtractPage();
        slotSeek(timebar->value());
    }
}

void kmidClient::slotForward()
{
    if ((m_kMid.pctl->playing)&&(!m_kMid.pctl->paused))
    {
        timebar->addPage();
        slotSeek(timebar->value());
    }
}


void kmidClient::allNotesOff()
{
    bool done=false;
    m_kMid.pctl->isSendingAllNotesOff=true;
    DeviceManager *_midi=new DeviceManager();
    _midi->initManager();
    _midi->openDev();
    _midi->allNotesOff();
    _midi->closeDev();
    delete _midi;
    done=true;
    m_kMid.pctl->isSendingAllNotesOff=false;
}

void kmidClient::kmidOutput(void)
{
// Should do nothing
    /*
    Midi_event *ev=pctl->ev;

    timeval tv;
    gettimeofday(&tv, NULL);
    ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;

    if ((ev->command==MIDI_SYSTEM_PREFIX)&&((ev->command|ev->chn)==META_EVENT))
    {
        if ((ev->d1==5)||(ev->d1==1))
        {
            char *text=new char[ev->length+1];
            strncpy(text,(char *)ev->data,ev->length);
            text[ev->length]=0;
#ifdef KMidDEBUG
            printf("%s , played at: %ld\n",text,currentmillisec-beginmillisec);
#endif
        }
        else if (ev->d1==ME_SET_TEMPO)
        {
            int tempo=(ev->data[0]<<16)|(ev->data[1]<<8)|(ev->data[2]);
            //	    printf("Change tempo: %d , %g, played at:%ld\n",tempo,tempoToMetronomeTempo(tempo),currentmillisec-beginmillisec);
        }

    }
     */
}


void kmidClient::processSpecialEvent()
{
/*
    if (spev==NULL)
    {
        printf("SPEV == NULL !!!!!\n");
        return;
    }
*/

//#ifdef KMidDEBUG
//    printf(":::: %ld",passcount++);
//    printf("%d %s %ld",spev->type,spev->text,spev->absmilliseconds);
//#endif

    int processNext=1;
    int type;
    ulong x;

    long delaymillisec=~0;

    while (processNext)
    {
        /*
         timeval tv;
         gettimeofday(&tv, NULL);
         ulong currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
         */

        x=timeOfNextEvent(&type);

        if (type==0) return;
        if (type==1)
        {
            if ((spev->type==1) || (spev->type==5))
            {
                kdispt->PaintIn(spev->type);
            }
            else if (spev->type==3)
            {
                tempoLCD->display(tempoToMetronomeTempo(spev->tempo));
#ifdef KMidDEBUG
                printf("Changing lcd tempo: spev->tempo: %d , ratio: %.9g\n",spev->tempo,m_kMid.pctl->ratioTempo);
                printf("Result: %g %.9g %d\n",tempoToMetronomeTempo(spev->tempo),tempoToMetronomeTempo(spev->tempo),(int)tempoToMetronomeTempo(spev->tempo));
#endif
                currentTempo=tempoLCD->getValue();
                tempoLCD->setDefaultValue(tempoToMetronomeTempo(spev->tempo)*m_kMid.pctl->ratioTempo);
            }
            else if (spev->type==6)
            {
                rhythmview->setRhythm(spev->num,spev->den);
            }
            else if (spev->type==7)
            {
#ifdef KMidDEBUG
                printf("Beat: %d/%d\n",spev->num,spev->den);
#endif
                rhythmview->Beat(spev->num);
            }
            m_kMid.pctl->SPEVprocessed++;
            spev=spev->next;
        }
        if (type==2)
        {
            NoteArray::noteCmd *ncmd=noteArray->get();
            if (ncmd==NULL) {printf("ncmd is NULL !!!");return;}
            if (channelView!=NULL)
            {
                if (ncmd->cmd==1) channelView->noteOn(ncmd->chn,ncmd->note);
                else if (ncmd->cmd==0) channelView->noteOff(ncmd->chn,ncmd->note);
                else if (ncmd->cmd==2)
                    if (!m_kMid.pctl->forcepgm[ncmd->chn]) channelView->changeInstrument(ncmd->chn,(m_kMid.pctl->gm==1)?(ncmd->note):(MT32toGM[ncmd->note]));
                    else channelView->changeInstrument(ncmd->chn,(m_kMid.pctl->pgm[ncmd->chn]));

                noteArray->next();
            }
        }
        processNext=0;

        x=timeOfNextEvent(&type);

        if (type==0) return;

        timeval tv;
        ulong currentmillisec;
        gettimeofday(&tv, NULL);
        currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
        delaymillisec=x-(currentmillisec-beginmillisec);
        if (delaymillisec<10) processNext=1;
    }

    if (delaymillisec!=~(long)0) timer4events->start(delaymillisec,TRUE);

}

void kmidClient::repaintText(int type)
{
    kdispt->ChangeTypeOfTextEvents(type);
    typeoftextevents=type;
    kdispt->repaint(TRUE);
}

int kmidClient::ChooseTypeOfTextEvents(void)
{
    return kdispt->ChooseTypeOfTextEvents();
}

void kmidClient::setSongType(int i)
{
    int autochangetype=0;
    if ((m_kMid.pctl->playing==1)&&(m_kMid.pctl->paused==0)) autochangetype=1;

    if (autochangetype)
    {
        pause();
    }
    m_kMid.pctl->gm=i;

    if (autochangetype)
    {
        pause();
    }

}


QFont * kmidClient::getFont(void)
{
return kdispt->getFont();
}

void  kmidClient::fontChanged(void)
{
    kdispt->fontChanged();
}

void kmidClient::setMidiDevice(int i)
{
    midi->setDefaultDevice(i);
}

void kmidClient::setMidiMapFilename(const char *mapfilename)
{
    MidiMapper *map=new MidiMapper(mapfilename);
    if (map->ok()==-1)
    {
        QString tmp = locate("appdata", QString("maps/") + mapfilename);
        delete map;
        map=new MidiMapper(tmp.local8Bit());
        if (map->ok()!=1)
        {
            delete map;
            map=new MidiMapper(NULL);
        }
    }
    int autochangemap=0;
    if ((m_kMid.pctl->playing==1)&&(m_kMid.pctl->paused==0)) autochangemap=1;

    if (autochangemap)
    {
        pause();
    }
    midi->setMidiMap(map);
    if (autochangemap)
    {
        pause();
    }
}

void kmidClient::setSLManager(SLManager *slm)
{
    if (slman!=NULL) delete slman;
    slman=slm;
}

void kmidClient::setActiveCollection(int i)
{
    activecollection=i;
    KConfig *kconf=KGlobal::instance()->config();

    kconf->setGroup("KMid");
    kconf->writeEntry("ActiveCollection",activecollection);
    currentsl=slman->getCollection(activecollection);
    generateCPL();
    initializing_songs=1;
    fillInComboSongs();
    initializing_songs=0;
}

void kmidClient::fillInComboSongs(void)
{
    //int oldselected=comboSongs->currentItem();
    comboSongs->clear();
    //comboSongs->setCurrentItem(-1);
    if (currentsl==NULL) return;
    currentsl->iteratorStart();
    char temp[FILENAME_MAX];
    char temp2[FILENAME_MAX];
    QString qs;
    while (!currentsl->iteratorAtEnd())
    {
	qs=currentsl->getIteratorName();
	//KURL::decode(qs);
        sprintf(temp,"%d - %s",currentsl->getIteratorID(),
                extractFilename(KURL::decode_string(qs).ascii(),temp2));
        comboSongs->insertItem(temp);
        currentsl->iteratorNext();
    }
    if (currentsl->getActiveSongID()==-1) return;
    comboSongs->setCurrentItem(currentsl->getActiveSongID()-1);
    /*
     if (oldselected==currentsl->getActiveSongID()-1)
     {
     slotSelectSong(currentsl->getActiveSongID()-1);
     }
     */
    slotSelectSong(currentsl->getActiveSongID()-1);
}

void kmidClient::slotSelectSong(int i)
{
    if (currentsl==NULL) return;
    i++;
    if ((i<=0))  // The collection may be empty, or it may be just a bug :-)
    {
#ifdef KMidDEBUG
        printf("Empty\n");
#endif
        emit stopPause();
        if (m_kMid.pctl->playing) stop();
        if (midifile_opened!=NULL) delete midifile_opened;
        midifile_opened=NULL;
        player->removeSong();
        timebar->setRange(0,240000);
        timebar->setValue(0);
        timetags->repaint(TRUE);
        kdispt->ClearEv();
        kdispt->repaint(TRUE);
        comboSongs->clear();
        comboSongs->repaint(TRUE);
        topLevelWidget()->setCaption("KMid");
        return;
    }

    if ((i==currentsl->getActiveSongID())&&(!initializing_songs)) return;
    int pl=0;
    if (m_kMid.pctl->playing==1) pl=1;

    if (m_kMid.pctl->paused) emit stopPause();
    if (/*(i!=currentsl->getActiveSongID())&&*/(pl==1)) stop();
    currentsl->setActiveSong(i);
    if (openURL(currentsl->getActiveSongName())==-1) return;
    if (pl) play();

}


int kmidClient::getSelectedSong(void)
{
    if (currentsl==NULL) return -1;
    return currentsl->getActiveSongID();
}


void kmidClient::setSongLoop(int i)
{
    loopsong=i;
}


void kmidClient::generateCPL(void)
{
    delete [] collectionplaylist;
    collectionplaylist=0;

    if (currentsl==NULL) return;

    if (collectionplaymode==0)
        collectionplaylist=generate_list(currentsl->NumberOfSongs());
    else
        collectionplaylist=generate_random_list(currentsl->NumberOfSongs());
}


void kmidClient::setCollectionPlayMode(int i)
{
    collectionplaymode=i;
    generateCPL();
}

void kmidClient::saveCollections(void)
{
    if (slman==NULL) return;
#ifdef KMidDEBUG
    printf("Saving collections in: %s\n",collectionsfile.ascii());
#endif
    slman->saveConfig(QFile::encodeName(collectionsfile));
}

void kmidClient::saveLyrics(FILE *fh)
{
    if (kdispt!=NULL) kdispt->saveLyrics(fh);
}

int kmidClient::searchInCPL(int song)
{
    if (currentsl==NULL) return -1;
    int i=0;
    int n=currentsl->NumberOfSongs();
    while ((i<n)&&(collectionplaylist[i]!=song)) i++;
    if (i<n) return i;
    return -1;
}

void kmidClient::visibleVolumeBar(int i)
{
#ifndef TEMPHACK
    visiblevolumebar=i;

    if (visiblevolumebar)
        volumebar->show();
    else
        volumebar->hide();
#endif
}

void kmidClient::visibleChannelView(int i)
{
    if ((channelView==NULL)&&(i==1))
    {
        channelView=new ChannelView();
        if (noteArray!=NULL)
        {
            int pgm[16],j;
            noteArray->moveIteratorTo((ulong)m_kMid.pctl->millisecsPlayed,pgm);
            for (j=0;j<16;j++)
            {
                if (!m_kMid.pctl->forcepgm[j]) channelView->changeInstrument(j,(m_kMid.pctl->gm==1)?(pgm[j]):(MT32toGM[pgm[j]]));
                else channelView->changeInstrument(j,(m_kMid.pctl->pgm[j]));
                channelView->changeForceState(j,m_kMid.pctl->forcepgm[j]);
            }
        }
        channelView->show();
        connect(channelView,SIGNAL(signalToKMidClient(int *)),this,SLOT(communicationFromChannelView(int *)));
	connect(kapp,SIGNAL(shutDown()),parentWidget(),SLOT(shuttingDown()));

    }
    else if ((channelView!=NULL)&&(i==0))
    {
        delete channelView;
        channelView=NULL;

    }
    rethinkNextEvent();
}

void kmidClient::channelViewDestroyed()
{
    channelView=NULL;
    rethinkNextEvent();
}


void kmidClient::rethinkNextEvent(void)
{
    if (m_kMid.pctl->playing==0) return;
    timer4events->stop();

    int type;
    ulong delaymillisec;
    ulong x=timeOfNextEvent(&type);

    if (type==0) return;

    timeval tv;
    ulong currentmillisec;
    gettimeofday(&tv, NULL);
    currentmillisec=tv.tv_sec*1000+tv.tv_usec/1000;
    delaymillisec=x-(currentmillisec-beginmillisec);

    timer4events->start(delaymillisec,TRUE);
}

void kmidClient::communicationFromChannelView(int *i)
{
    if (i==NULL) return;
    int autocontplaying=0;
    if ((i[0]==CHN_CHANGE_PGM)||((i[0]==CHN_CHANGE_FORCED_STATE)&&(i[3]==1)))
    {
        if ((m_kMid.pctl->playing==1)&&(m_kMid.pctl->paused==0)) autocontplaying=1;

        if (autocontplaying)
        {
            pause();
        }
    }
    if (i[0]==CHN_CHANGE_PGM)
        m_kMid.pctl->pgm[i[1]-1]=i[2];
    else if (i[0]==CHN_CHANGE_FORCED_STATE)
        m_kMid.pctl->forcepgm[i[1]-1]=i[2];
    if ((i[0]==CHN_CHANGE_PGM)||((i[0]==CHN_CHANGE_FORCED_STATE)&&(i[3]==1)))
    {
        if (autocontplaying)
        {
            pause();
        }
    }

}

void kmidClient::slotSetTempo(double value)
{
    if (!player->isSongLoaded())
    {
        tempoLCD->display(120);
        currentTempo=120;
        tempoLCD->setDefaultValue(120);
        return;
    }

#ifdef KMidDEBUG
    printf("Change tempo to %g\n",value);
#endif
    int autocontplaying=0;

    if ((m_kMid.pctl->playing==1)&&(m_kMid.pctl->paused==0)) autocontplaying=1;


    if (autocontplaying)
    {
        pause();
    }

//    double ratio=(tempoToMetronomeTempo(m_kMid.pctl->tempo)*m_kMid.pctl->ratioTempo)/(value);
//    double ratio=(tempoLCD->getOldValue()*m_kMid.pctl->ratioTempo)/(value);
    double ratio=(currentTempo*m_kMid.pctl->ratioTempo)/value;

    char s[20];
    sprintf(s,"%g",ratio);
    if (strcmp(s,"1")!=0) tempoLCD->setLCDColor (255,100,100);
    else tempoLCD->setLCDColor (100,255,100);
#ifdef KMidDEBUG
    printf("ratio: (%.9g = %g ) tempo now: %g , new tempo %g\n",ratio,ratio,tempoToMetronomeTempo(m_kMid.pctl->tempo),value);
    printf("OldValue: %g , value %g\n",tempoLCD->getOldValue(),value);
#endif

    if (m_kMid.pctl->paused==1)
    {
        pausedatmillisec=(long)(((double)pausedatmillisec/m_kMid.pctl->ratioTempo)*ratio);
#ifdef KMidDEBUG
        printf("pausedat: %ld\n",pausedatmillisec);
#endif
    }
    player->setTempoRatio(ratio);

    timebar->setRange(0,(int)(player->information()->millisecsTotal));
    timebar->setValue(pausedatmillisec);
    timetags->repaint(TRUE);

    kdispt->ClearEv(false);

    noteArray=player->noteArray();
    spev=player->specialEvents();
    currentTempo=value;

    while (spev!=NULL)
    {
        if ((spev->type==1) || (spev->type==5))
        {
            kdispt->AddEv(spev);
        }
        spev=spev->next;
    }

    kdispt->calculatePositions();
    kdispt->CursorToHome();
    if (m_kMid.pctl->paused==1)
        moveEventPointersTo(pausedatmillisec);

    if (autocontplaying)
    {
        pause();
    }

}

void kmidClient::downloadFinished(KIO::Job *)
{
    downloaded=true;
    kapp->exit_loop();
}

QSize kmidClient::sizeHint() const
{
    QSize sh = QWidget::sizeHint();
    return sh.expandedTo(QSize(560,420));
}

QSizePolicy kmidClient::sizePolicy()
{
    return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


void kmidClient::play()
{
  slotPlay();
}
void kmidClient::pause()
{
  slotPause();
}
void kmidClient::stop()
{
  slotStop();
}
void kmidClient::rewind()
{
  slotRewind();
}
void kmidClient::forward()
{
  slotForward();
}
void kmidClient::seek(int ms)
{
  slotSeek(ms);
}
void kmidClient::prevSong()
{
  slotPrevSong();
}
void kmidClient::nextSong()
{
  slotNextSong();
}
void kmidClient::setVolume(int i)
{
  slotSetVolume(200-i);
}
void kmidClient::setTempo(int i)
{
  slotSetTempo(i);
}
void kmidClient::setSongEncoding( int i )
{
   KListAction *tmplistaction=
    ((KListAction*)actionCollection->action("file_type"));

  tmplistaction->setCurrentItem(i);
}
void kmidClient::setLyricEvents( int i )
{
   KListAction *tmplistaction=
    ((KListAction*)actionCollection->action("display_events"));
  tmplistaction->setCurrentItem(i);
}
void kmidClient::setCurrentSong(int i)
{
  getComboSongs()->setCurrentItem(i-1);
  slotSelectSong(i-1);
}
void kmidClient::setPlayListMode(int i)
{
  ((KListAction*)actionCollection->action("play_order"))->setCurrentItem(i);
}
void kmidClient::slotSelectEncoding(int i)
{
  if (i == 0)
     kdispt->setLyricsEncoding(QString::null); // Default
  else
     kdispt->setLyricsEncoding(KGlobal::charsets()->encodingForName(comboEncodings->text(i)));
}
#include "kmidclient.moc"
