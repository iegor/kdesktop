/**************************************************************************

    kmidframe.cpp  - The main widget of KMid
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
    Foundation, Inc., 59 Temple

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <qkeycode.h>
#include <qstring.h>

#include <kaccel.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kcmdlineargs.h>
#include <kfiledialog.h>
#include <kfontdialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolbar.h>
#include <kurl.h>
#include <kdebug.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kurldrag.h>
#include <kedittoolbar.h>
#include <dcopclient.h>

#include "kmidframe.h"
#include "kmidclient.h"
#include "midicfgdlg.h"
#include "collectdlg.h"
#include "channelcfgdlg.h"
#include "channelview.h"
#include "version.h"

kmidFrame::kmidFrame(const char *name)
    :KMainWindow(0, name)
{
    kmidclient=new kmidClient(this,actionCollection(), "KMidClient");
    kmidclient->setSongType(1);
    kmidclient->show();
    setCentralWidget( kmidclient );
/*
    kKeysAccel=new KAccel(this);
    kKeysAccel->insertItem(i18n("Play/Pause"),"Play/Pause", Key_Space);
    kKeysAccel->connectItem("Play/Pause", this, SLOT(spacePressed()));
    kKeysAccel->insertItem(i18n("Stop"),"Stop", Key_Backspace);
    kKeysAccel->connectItem("Stop",kmidclient,SLOT(song_Stop()));
    kKeysAccel->insertItem(i18n("Previous Song"),"Previous Song", Key_Left);
    kKeysAccel->connectItem("Previous Song",kmidclient,SLOT(song_PlayPrevSong()));
    kKeysAccel->insertItem(i18n("Next Song"),"Next Song", Key_Right);
    kKeysAccel->connectItem("Next Song",kmidclient,SLOT(song_PlayNextSong()));
    kKeysAccel->insertItem(i18n("Scroll Down Karaoke"),"Scroll down karaoke",Key_Down);
    kKeysAccel->connectItem("Scroll Down karaoke",kmidclient->getKDisplayText(),SLOT(ScrollDown()));
    kKeysAccel->insertItem(i18n("Scroll Up Karaoke"),"Scroll up karaoke",Key_Up);
    kKeysAccel->connectItem("Scroll Up Karaoke",kmidclient->getKDisplayText(),SLOT(ScrollUp()));
    kKeysAccel->insertItem(i18n("Scroll Page Down Karaoke"),"Scroll page down karaoke",Key_PageDown);
    kKeysAccel->connectItem("Scroll Page Down Karaoke",kmidclient->getKDisplayText(),SLOT(ScrollPageDown()));
    kKeysAccel->insertItem(i18n("Scroll Page Up Karaoke"),"Scroll page up karaoke",Key_PageUp);
    kKeysAccel->connectItem("Scroll Page Up Karaoke",kmidclient->getKDisplayText(),SLOT(ScrollPageUp()));

    kKeysAccel->readSettings();
*/
    KStdAction::open(this, SLOT(file_Open()), actionCollection());
    (void)new KAction(i18n("&Save Lyrics..."), 0, this,
	SLOT(file_SaveLyrics()), actionCollection(), "file_save_lyrics");
    KStdAction::quit(kapp, SLOT(quit()), actionCollection());

    (void)new KAction(i18n("&Play"), "player_play", Qt::Key_Space,
		kmidclient, SLOT(slotPlay()), actionCollection(), "song_play");
    (void)new KAction(i18n("P&ause"), "player_pause", Qt::Key_P, kmidclient,
			SLOT(slotPause()), actionCollection(), "song_pause");
    (void)new KAction(i18n("&Stop"), "player_stop", Qt::Key_Backspace,
		kmidclient, SLOT(slotStop()), actionCollection(), "song_stop");

    (void)new KAction(i18n("P&revious Song"), "player_start", Key_Left,
		kmidclient, SLOT(slotPrevSong()), actionCollection(),
		"song_previous");
    (void)new KAction(i18n("&Next Song"), "player_end", Key_Right,
		kmidclient, SLOT(slotNextSong()), actionCollection(),
		"song_next");
    (void)new KToggleAction(i18n("&Loop"), 0, this, SLOT(song_Loop()),
		actionCollection(), "song_loop");

    (void)new KAction(i18n("Rewind"), "2leftarrow", 0, kmidclient,
		SLOT(slotRewind()), actionCollection(), "song_rewind");

    (void)new KAction(i18n("Forward"), "2rightarrow", 0, kmidclient,
		SLOT(slotForward()), actionCollection(), "song_forward");

    (void)new KAction(i18n("&Organize..."), 0, this, SLOT(collect_organize()),
		actionCollection(), "collect_organize");

    QStringList playmodes;
    playmodes.append(i18n("In Order"));
    playmodes.append(i18n("Shuffle"));

    KSelectAction *act=new KSelectAction(i18n("Play Order"), 0, /*this, SLOT(collect_PlayOrder(int)),*/
                      actionCollection(), "play_order");
    connect(act,SIGNAL(activated(int)),this, SLOT(collect_PlayOrder(int)));
    act->setItems(playmodes);

    (void)new KToggleAction(i18n("Auto-Add to Collection"), 0, this,
	SLOT(collect_AutoAdd()), actionCollection(), "collect_autoadd");

    playmodes.clear();
    playmodes.append(i18n("&General MIDI"));
    playmodes.append(i18n("&MT-32"));

    act=new KSelectAction(i18n("File Type"), 0,/* this, SLOT(options_FileType(int)),*/
                      actionCollection(), "file_type");
    connect(act,SIGNAL(activated(int)),this, SLOT(options_FileType(int)));
    act->setItems(playmodes);

    playmodes.clear();
    playmodes.append(i18n("&Text Events"));
    playmodes.append(i18n("&Lyric Events"));

    act=new KSelectAction(i18n("Display Events"), Key_T, /*this,
		SLOT(options_DisplayEvents(int)),*/ actionCollection(),
		"display_events");
    connect(act,SIGNAL(activated(int)),this, SLOT(options_DisplayEvents(int)));
    act->setItems(playmodes);

    (void)new KToggleAction(i18n("Automatic Text Chooser"), 0, this,
		SLOT(options_AutomaticText()), actionCollection(),
		"option_automatictext");

    KToggleAction* togact = new KToggleAction(i18n("Show &Volume Bar"), "volume",
		0, this, SLOT(options_ShowVolumeBar()), actionCollection(),
		"toggle_volumebar");
    togact->setCheckedState(i18n("Hide &Volume Bar"));

    togact = new KToggleAction(i18n("Show &Channel View"), "piano",
		0, this, SLOT(options_ShowChannelView()), actionCollection(),
		"toggle_channelview");
    togact->setCheckedState(i18n("Hide &Channel View"));

    (void)new KAction(i18n("Channel View &Options..."), 0, this,
		SLOT(options_ChannelViewOptions()), actionCollection(),
		"channelview_options");

    (void)new KAction(i18n("&Font Change..."), 0, this,
		SLOT(options_FontChange()), actionCollection(),
		"change_font");

    (void)new KAction(i18n("MIDI &Setup..."), 0, this,
		SLOT(options_MidiSetup()), actionCollection(), "midi_setup");


    setupGUI((ToolBar | Keys | StatusBar | Save | Create ), "kmidui.rc" );

    KConfig *cfg=kapp->config();
    cfg->setGroup("KMid");
    KSelectAction *tmplistaction=
		((KSelectAction*)actionCollection()->action("display_events"));

    if (cfg->readNumEntry("TypeOfTextEvents",5)==5)
        tmplistaction->setCurrentItem(1);
    else
        tmplistaction->setCurrentItem(0);

    tmplistaction=((KSelectAction*)actionCollection()->action("file_type"));
    if (cfg->readNumEntry("TypeOfMidiFile",0)==0)
        tmplistaction->setCurrentItem(0);
    else
        tmplistaction->setCurrentItem(1);

    if (cfg->readNumEntry("Loop",0)==1)
	((KToggleAction*)actionCollection()->action("song_loop"))->setChecked(true);

    if (cfg->readNumEntry("ShowVolumeBar",0)==1)
	((KToggleAction*)actionCollection()->action("toggle_volumebar"))->setChecked(true);


    tmplistaction=((KSelectAction*)actionCollection()->action("play_order"));
    if (cfg->readNumEntry("CollectionPlayMode",0)==0)
        tmplistaction->setCurrentItem(0);
    else
        tmplistaction->setCurrentItem(1);

    if ((cfg->readNumEntry("AutoAddToCollection",0))==1)
	((KToggleAction*)actionCollection()->action("collect_autoadd"))->setChecked(true);

    if ((cfg->readNumEntry("AutomaticTextEventChooser",1))==1)
	((KToggleAction*)actionCollection()->action("option_automatictext"))->setChecked(true);

    setAcceptDrops(true);

    connect( kmidclient, SIGNAL( mustRechooseTextEvent() ),
             this, SLOT( rechooseTextEvent() ) );

    connect( kmidclient, SIGNAL( stopPause() ),
             this, SLOT( song_stopPause() ) );

//    connect( kmidclient, SIGNAL( channelView_Destroyed() ),
//             this, SLOT( channelViewDestroyed() ) );

    KCmdLineArgs * args = KCmdLineArgs::parsedArgs();

    if ( args->count() > 0 )
    {
//        printf("Opening command line file...\n");
        int backautoadd=cfg->readNumEntry("AutoAddToCollection",0);
        cfg->writeEntry("AutoAddToCollection",0);

        char ttt[40];
        sprintf(ttt,"%d",kapp->argc());
        int i=0;
        int c=autoAddSongToCollection( args->url( 0 ).path() , 1 );
        i++;
        while (i<args->count())
        {
            autoAddSongToCollection( args->url( i ).path() , 0 );
            i++;
        }

        kmidclient->setActiveCollection(c);

        ///	    kmidclient->openURL((kapp->argv())[1]);
        /*	    if ((cfg->readNumEntry("AutomaticTextEventChooser",1))==1)
         {
         if (kmidclient->ChooseTypeOfTextEvents()==1)
         options_Text();
         else
         options_Lyrics();
         }*/
        if (kmidclient->midiFileName()!=NULL) kmidclient->play();
        cfg->writeEntry("AutoAddToCollection",backautoadd);
    }

    args->clear();
        /*
         kKeys->addKey("Play/Pause",Key_Space);
         kKeys->registerWidget("KMidFrame",this);
         kKeys->connectFunction("KMidFrame","Play/Pause",this,SLOT(spacePressed()));
         */
//    kKeysAccel->writeSettings(cfg);

    DCOPClient *client = kapp->dcopClient();
    if (!client->isRegistered())  // just in case we're embeeded
    {
      client->attach();
      client->registerAs("kmid");
    }
}

kmidFrame::~kmidFrame()
{
}

void kmidFrame::file_Open()
{
  KURL url = KFileDialog::getOpenURL(QString::null, "*.kar *.mid *.kar.gz *.mid.gz\n*.kar *.kar.gz\n*.mid *.mid.gz\n*",this);

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry( 0L, i18n( "Only local files are currently supported." ) );
    return;
  }

  openURL(url.path());
}



void kmidFrame::song_stopPause()
{
  if (kmidclient->isPaused())
  {
    //   song_Pause();
    ((KToggleAction*)actionCollection()->action("song_pause"))->setChecked(false);
    kmidclient->pause();
  }
}

void kmidFrame::options_FileType(int i)
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("TypeOfMidiFile",i);
  kmidclient->setSongType(1-i);
}

void kmidFrame::options_DisplayEvents(int i)
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("TypeOfTextEvents",(i==0)?1:5);
  cfg->sync();
  kmidclient->repaintText((i==0)?1:5);
}

void kmidFrame::options_AutomaticText()
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("AutomaticTextEventChooser",1-cfg->readNumEntry("AutomaticTextEventChooser",1));
}

void kmidFrame::options_FontChange()
{
  KFontDialog *kfd=new KFontDialog(this);
  QFont font;
  font=*kmidclient->getFont();
  kfd->getFont(font);
  delete kfd;
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("KaraokeFont",font);
  cfg->sync();
  kmidclient->fontChanged();
}

int kmidFrame::autoAddSongToCollection(const QString& filename,int setactive)
{
  int r;
  SLManager *slman;
  SongList *sl;
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  if (cfg->readNumEntry("AutoAddToCollection",0)==0)
  {
    r=0;
    slman=kmidclient->getSLManager();
    if (setactive) slman->createTemporaryCollection();
    sl=slman->getCollection(0);
    if (filename==NULL) sl->AddSong(kmidclient->midiFileName());
    else sl->AddSong(QFile::encodeName(filename));
  }
  else
  {
    slman=kmidclient->getSLManager();
    sl=slman->getCollection(kmidclient->getActiveCollection());
    r=kmidclient->getActiveCollection();
    if (sl==NULL) return 0;
    int id;
    if (filename==NULL) id=sl->AddSong(kmidclient->midiFileName());
    else id=sl->AddSong(QFile::encodeName(filename));
    if (setactive)
    {
      sl->setActiveSong(id);
    }
  }
  return r;
}

void kmidFrame::urlDrop_slot(QDropEvent* e)
{
  dropEvent(e);
}

void kmidFrame::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(KURLDrag::canDecode(e));
}

void kmidFrame::dropEvent( QDropEvent * event )
{
  KURL::List list;
  KURLDrag::decode(event, list);

  if (list.count()==0) return;

  bool first = true;
  int c = true;

  for (KURL::List::ConstIterator it = list.begin();
       it != list.end(); ++it)
  {
     if (!(*it).isLocalFile())
        continue;

     if (first)
     {
         c = autoAddSongToCollection((*it).path(),1);
         first = false;
     }
     else
     {
         autoAddSongToCollection((*it).path(),0);
     }
  }

  if (!first)
  {
     kmidclient->setActiveCollection(c);

     if ((!kmidclient->isPlaying())&&(kmidclient->midiFileName()!=NULL))
		kmidclient->play();
  }
}

void kmidFrame::shuttingDown()
{
   if (kmidclient->isPlaying()==1)
   {
//      kmidclient->stop();
      kmidclient->shuttingDown();
   }
   kmidclient->saveCollections();
}

void kmidFrame::saveProperties(KConfig *cfg)
{
  kmidclient->saveCollections();
  int play=kmidclient->isPlaying();
  cfg->writeEntry("File",kmidclient->midiFileName());
  cfg->writeEntry("ActiveCollection",kmidclient->getActiveCollection());
  cfg->writeEntry("ActiveSong",kmidclient->getSelectedSong());
  cfg->writeEntry("Playing",play);
}

void kmidFrame::readProperties(KConfig *cfg)
{
  int activecol=cfg->readNumEntry("ActiveCollection",0);
  int activesong=cfg->readNumEntry("ActiveSong",0);
  int wasplaying=cfg->readNumEntry("Playing",0);
  SLManager *slman=kmidclient->getSLManager();
  SongList *sl=slman->getCollection(activecol);
  sl->setActiveSong(activesong);
  kmidclient->setActiveCollection(activecol);
  kmidclient->slotSelectSong(activesong-1);

  if ((activecol==0)&&(wasplaying)) // It was the temporary collection,
  {		// surely the user would like to continue
    // hearing the song
    QString s = cfg->readPathEntry("File");
    int c=autoAddSongToCollection(s,1);
    kmidclient->setActiveCollection(c);
  }

  if ((wasplaying)&&(kmidclient->midiFileName()!=NULL)) kmidclient->play();
}

void kmidFrame::options_MidiSetup()
{
  if (kmidclient->devman()->checkInit()<0)
  {
    KMessageBox::error(this,
	i18n("Could not open /dev/sequencer to get some info.\nProbably there is another program using it."));
    return;
  }
  MidiConfigDialog *dlg;

  dlg=new MidiConfigDialog(kmidclient->devman(),NULL,"MidiDialog");
  if (dlg->exec() == QDialog::Accepted)
  {
    KConfig *cfg=kapp->config();
    cfg->setGroup("KMid");
    cfg->writeEntry("MidiPortNumber",MidiConfigDialog::selecteddevice);
    kmidclient->setMidiDevice(MidiConfigDialog::selecteddevice);
    cfg->setGroup("Midimapper");
    cfg->writePathEntry("LoadFile",
	(MidiConfigDialog::selectedmap==NULL)? QString::null:MidiConfigDialog::selectedmap);
    kmidclient->setMidiMapFilename(MidiConfigDialog::selectedmap);
  }
  delete dlg;
}

void kmidFrame::collect_organize()
{
  CollectionDialog *dlg;
  SLManager *slman=new SLManager(*kmidclient->getSLManager());
  int idx=kmidclient->getActiveCollection();

  dlg=new CollectionDialog(slman,idx,NULL,"MidiDialog");
  if (dlg->exec() == QDialog::Accepted)
    {
    kmidclient->setSLManager(slman);
    kmidclient->setActiveCollection(CollectionDialog::selectedC);
    kmidclient->slotSelectSong(
     (slman->getCollection(CollectionDialog::selectedC))->getActiveSongID()-1);
    }
   else
    {
    delete slman;
    }
delete dlg;
}

void kmidFrame::rechooseTextEvent()
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  if ((cfg->readNumEntry("AutomaticTextEventChooser",1))==1)
  {
    int t=kmidclient->ChooseTypeOfTextEvents();
    kmidclient->repaintText(t);
    if (t==1)
      ((KSelectAction*)actionCollection()->action("display_events"))->setCurrentItem(0);
    else
      ((KSelectAction*)actionCollection()->action("display_events"))->setCurrentItem(1);
  }
}

void kmidFrame::song_Loop()
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  int i=1-cfg->readNumEntry("Loop",0);
  cfg->writeEntry("Loop",i);
  cfg->sync();
  kmidclient->setSongLoop(i);
}

void kmidFrame::collect_PlayOrder(int i)
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("CollectionPlayMode", i);
  cfg->sync();
  kmidclient->setCollectionPlayMode(i);
}

void kmidFrame::collect_AutoAdd()
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  cfg->writeEntry("AutoAddToCollection",1-cfg->readNumEntry("AutoAddToCollection",0));
  cfg->sync();
}

void kmidFrame::file_SaveLyrics()
{
  KURL url = KFileDialog::getSaveURL(QString::null,"*",this);

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry( 0L, i18n( "Only local files are currently supported." ) );
    return;
  }

  QString filename = url.path();
  struct stat statbuf;

  if (stat(QFile::encodeName(filename), &statbuf)!=-1)
  {
    QString s = i18n("File %1 already exists\nDo you want to overwrite it?").arg(filename);
    if (KMessageBox::warningYesNo(this,s,QString::null,i18n("Overwrite"),KStdGuiItem::cancel())==1)
      return;
  }

  FILE *fh=fopen(QFile::encodeName(filename),"wt");
  kmidclient->saveLyrics(fh);
  fclose(fh);
}

void kmidFrame::spacePressed()
{
  if (!kmidclient->isPlaying()) kmidclient->play();
  else pause();
}
void kmidFrame::options_ShowVolumeBar()
{
  KConfig *cfg=kapp->config();
  cfg->setGroup("KMid");
  int i=1-cfg->readNumEntry("ShowVolumeBar",0);
  cfg->writeEntry("ShowVolumeBar",i);
  kmidclient->visibleVolumeBar(i);
}

void kmidFrame::options_ShowChannelView()
{
  if (!((KToggleAction*)actionCollection()->action("toggle_channelview"))->isChecked())
  {
    kmidclient->visibleChannelView(0);
  } else
  {
    kmidclient->visibleChannelView(1);
    connect (kmidclient->getChannelView(),SIGNAL(destroyMe()),this,SLOT(channelViewDestroyed()));
  }

}

void kmidFrame::channelViewDestroyed()
{
  kmidclient->channelViewDestroyed();
  ((KToggleAction*)actionCollection()->action("toggle_channelview"))->setChecked(false);
}

void kmidFrame::options_ChannelViewOptions()
{
  ChannelViewConfigDialog *dlg;

  dlg=new ChannelViewConfigDialog(NULL,"ChannelViewConfigDialog");
  if (dlg->exec() == QDialog::Accepted)
  {
    if (kmidclient->getChannelView())
      kmidclient->getChannelView()->lookMode(
	  ChannelViewConfigDialog::selectedmode );
  }
  delete dlg;
}

void kmidFrame::openURL( const QString url )
{
  int c=autoAddSongToCollection(url,1);
  kmidclient->setActiveCollection(c);
}

/*void kmidFrame::play()
{
  kmidclient->play();
}

void kmidFrame::pause()
{
//  kmidclient->pause();
  ((KAction*)actionCollection()->action("pause"))->activate();
}

void kmidFrame::stop()
{
  kmidclient->stop();
}

void kmidFrame::rewind()
{
  kmidclient->rewind();
}

void kmidFrame::forward()
{
  kmidclient->forward();
}

void kmidFrame::seek(int ms)
{
  kmidclient->timebarChange(ms);
}

void kmidFrame::prevSong()
{
  kmidclient->prevSong();
}

void kmidFrame::nextSong()
{
  kmidclient->nextSong();
}

void kmidFrame::setSongLoop(int i)
{
  ((KToggleAction*)actionCollection()->action("song_loop"))->setChecked(i!=0);
}

void kmidFrame::setVolume(int i)
{
  kmidclient->volumebarChange(200-i);
}

void kmidFrame::setTempo(int i)
{
  kmidclient->changeTempo(i);
}

void kmidFrame::setSongType( int i )
{
  KSelectAction *tmplistaction=
    ((KSelectAction*)actionCollection()->action("file_type"));

  tmplistaction->setCurrentItem(i);
}

void kmidFrame::setLyricEvents( int i )
{
  KSelectAction *tmplistaction=
    ((KSelectAction*)actionCollection()->action("display_events"));
  tmplistaction->setCurrentItem(i);
}

void kmidFrame::selectSong(int i)
{
  kmidclient->getComboSongs()->setCurrentItem(i-1);
  kmidclient->selectSong(i-1);
}

void kmidFrame::setActiveCollection( int i )
{
  kmidclient->setActiveCollection(i);
}

void kmidFrame::setCollectionPlayMode(int i)
{
  ((KSelectAction*)actionCollection()->action("play_order"))->setCurrentItem(i);
}

void kmidFrame::setMidiDevice(int i)
{


}
*/


#include "kmidframe.moc"
