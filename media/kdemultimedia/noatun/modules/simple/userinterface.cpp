/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <noatun/app.h>
#include <noatun/engine.h>
#include <noatun/player.h>
#include <noatun/stdaction.h>

#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kurldrag.h>
#include <kmenubar.h>
#include <kpopupmenu.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstdguiitem.h>
#include <kurl.h>

#include <qaccel.h>
#include <qdragobject.h>
#include <qhbox.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstrlist.h>
#include <qtooltip.h>
#include <qvbox.h>

#include "userinterface.h"

#include "back.xpm"
#include "forward.xpm"
#include "pause.xpm"
#include "play.xpm"
#include "playlist.xpm"
#include "stop.xpm"
#include "volume.xpm"

SimpleUI::SimpleUI()
     : KMainWindow(0, "NoatunSimpleUI"), UserInterface()
{
    setAcceptDrops( true );
    setCaption( i18n("Noatun") );
    setIcon( SmallIcon( "noatun" ) );

    setupCentralWidget();
    setupActions();

    contextMenu = video->popupMenu( this );

    setupGUI( StatusBar|Create, "simpleui.rc" );

    connect( napp->player(), SIGNAL(playing()), SLOT(slotPlaying()) );
    connect( napp->player(), SIGNAL(stopped()), SLOT(slotStopped()) );
    connect( napp->player(), SIGNAL(paused()), SLOT(slotPaused()) );
    connect( napp->player(), SIGNAL(timeout()), SLOT(slotTimeout()) );
    connect( napp->player(), SIGNAL(newSong()), SLOT(slotChanged()) );
    connect( napp->player(), SIGNAL(volumeChanged(int)), SLOT(slotVolumeChanged(int)) );
    connect( napp, SIGNAL(hideYourself()), SLOT(hide()) );
    connect( napp, SIGNAL(showYourself()), SLOT(show()) );

    napp->player()->handleButtons();

    resize( minimumSize() );

    // Show UI and calculate video widget frame
    show();

    extra_width  = (width() - video->width());
    extra_height = (height() - video->height());

    // Load configuration
    KConfig &config = *KGlobal::config();
    config.setGroup( "Simple" );
    QString str = config.readEntry( "View", "NormalSize" );

    if (str == "HalfSize")
        video->setHalfSize();
    else if (str == "NormalSize")
        video->setNormalSize();
    else if (str == "DoubleSize")
        video->setDoubleSize();
    else
        applyMainWindowSettings( &config, "Simple" );

    // PlayObject could be running, update video widget
    slotChanged();

    video->give();
}

SimpleUI::~SimpleUI()
{
    KConfig &config = *KGlobal::config();
    saveMainWindowSettings( &config, "Simple" );
    config.setGroup( "Simple" );
    QString str;

    if (video->isHalfSize())
        str = "HalfSize";
    else if (video->isNormalSize())
        str = "NormalSize";
    else if (video->isDoubleSize())
        str = "DoubleSize";
    else
        str = "CustomSize";

    config.writeEntry( "View", str );
    config.sync();
}



void SimpleUI::setupActions()
{
    KStdAction::open( napp, SLOT(fileOpen()), actionCollection(), "_file_open" );
    new KAction( i18n("&Properties"), 0, propertiesDialog, SLOT(show()),
        actionCollection(), "_file_properties" );
    KStdAction::quit( napp, SLOT(quit()), actionCollection(), "_file_quit");

     NoatunStdAction::playlist( actionCollection(), "view_playlist" );
    actionCollection()->insert(video->action( "half_size" ));
    actionCollection()->insert(video->action( "normal_size" ));
    actionCollection()->insert(video->action( "double_size" ));
    actionCollection()->insert(video->action( "fullscreen_mode" ));

    actionCollection()->insert(napp->pluginActionMenu());

    menubarAction = KStdAction::showMenubar(this, SLOT(showMenubar()),
        actionCollection());
    statusbarAction = KStdAction::showStatusbar(this, SLOT(showStatusbar()),
        actionCollection());
    NoatunStdAction::effects( actionCollection(), "effects" );
    NoatunStdAction::equalizer( actionCollection(), "equalizer" );
    NoatunStdAction::loop( actionCollection(), "loop_style" );
    KStdAction::preferences( napp, SLOT(preferences()), actionCollection() );
}

void SimpleUI::showMenubar()
{
    if(menubarAction->isChecked())
        menuBar()->show();
    else
        menuBar()->hide();
}

void SimpleUI::showStatusbar()
{
    if(statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}


void SimpleUI::setupCentralWidget()
{
    QVBox *npWidget = new QVBox( this );
    npWidget->setMargin( 0 );
    npWidget->setSpacing( 0 );

    positionLabel = new QLabel( statusBar() );
    positionLabel->setAlignment( AlignVCenter | AlignCenter );
    positionLabel->setFixedSize( fontMetrics().size( 0, " 00:00/00:00 " ) );
    statusBar()->addWidget( positionLabel, 0, true );

    video = new VideoFrame( npWidget );
    connect( video, SIGNAL(adaptSize(int,int)),
         SLOT(slotAdaptSize(int,int)) );
    connect( video, SIGNAL(rightButtonPressed(const QPoint &)),
         SLOT(slotContextMenu(const QPoint &)) );

    QHBox *ctlFrame = new QHBox( npWidget );
    ctlFrame->setFixedHeight( 38 );
    ctlFrame->setFrameShape( QFrame::StyledPanel );
    ctlFrame->setFrameShadow( QFrame::Raised );
    ctlFrame->setMargin( 6 );
    ctlFrame->setSpacing( 6 );

    QPushButton *backButton = new QPushButton( ctlFrame );
    backButton->setFixedSize( 24, 24 );
    backButton->setPixmap( QPixmap( back_xpm ) );
    QToolTip::add( backButton, i18n("Back") );
    connect( backButton, SIGNAL(clicked()), napp->player(), SLOT(back()) );

    stopButton = new QPushButton( ctlFrame );
    stopButton->setFixedSize( 24, 24 );
    stopButton->setPixmap( QPixmap( stop_xpm ) );
    QToolTip::add( stopButton, i18n("Stop") );
    connect( stopButton, SIGNAL(clicked()), napp->player(), SLOT(stop()) );

    playButton = new QPushButton( ctlFrame );
    playButton->setFixedSize( 24, 24 );
    playButton->setPixmap( QPixmap( play_xpm ) );
    QToolTip::add( playButton, i18n("Play / Pause") );
    connect( playButton, SIGNAL(clicked()), napp->player(), SLOT(playpause()) );

    QPushButton *forwButton = new QPushButton( ctlFrame );
    forwButton->setFixedSize( 24, 24 );
    forwButton->setPixmap( QPixmap( forward_xpm ) );
    QToolTip::add( forwButton, i18n("Forward") );
    connect( forwButton, SIGNAL(clicked()), napp->player(), SLOT(forward()) );

    slider = new L33tSlider( 0, 1000, 10, 0, L33tSlider::Horizontal, ctlFrame );
    slider->setFixedHeight( 24 );
    slider->setMinimumWidth( 100 );
    slider->setTickmarks( QSlider::NoMarks );
    connect( slider, SIGNAL(userChanged(int)), SLOT(slotSkipTo(int)) );
    connect( slider, SIGNAL(sliderMoved(int)), SLOT(slotSliderMoved(int)) );

    QPushButton *playlistButton = new QPushButton( ctlFrame );
    playlistButton->setFixedSize( 24, 24 );
    playlistButton->setPixmap( QPixmap( playlist_xpm ) );
    QToolTip::add( playlistButton, i18n("Playlist") );
    connect( playlistButton, SIGNAL(clicked()), napp->player(), SLOT(toggleListView()) );

    volumeButton = new QPushButton( ctlFrame );
    volumeButton->setFixedSize( 24, 24 );
    volumeButton->setPixmap( QPixmap( volume_xpm ) );
    QToolTip::add( volumeButton, i18n("Volume") );

    volumeFrame = new QVBox( this, "Volume", WStyle_Customize | WType_Popup );
    volumeFrame->setFrameStyle( QFrame::PopupPanel );
    volumeFrame->setMargin( 4 );

    volumeLabel = new QLabel( volumeFrame );
    volumeLabel->setText( "100%" );
    volumeLabel->setAlignment( AlignCenter );
    volumeLabel->setFixedSize( volumeLabel->sizeHint() );

    QHBox *volumeSubFrame = new QHBox( volumeFrame );
    volumeSlider = new L33tSlider( 0, 100, 10, 0, Vertical, volumeSubFrame );
    volumeSlider->setValue( 100 - napp->player()->volume() );
    volumeSlider->setFixedSize( volumeSlider->sizeHint() );

    volumeFrame->resize( volumeFrame->sizeHint() );

    connect( volumeSlider, SIGNAL(sliderMoved(int)), SLOT(slotVolumeSliderMoved(int)) );
    connect( volumeSlider, SIGNAL(userChanged(int)), SLOT(slotVolumeSliderMoved(int)) );
    connect( volumeButton, SIGNAL(clicked()), SLOT(slotVolumeFrame()) );

    setCentralWidget( npWidget );

    video->setMinimumSize( minimumSizeHint().width(), 1 );

    // Create properties dialog
    propertiesDialog = new PropertiesDialog( this );
    propertiesDialog->resize( 375, 285 );
}

void SimpleUI::closeEvent( QCloseEvent * )
{
    unload();
}

void SimpleUI::dragEnterEvent( QDragEnterEvent *event )
{
    event->accept( KURLDrag::canDecode( event ) );
}

void SimpleUI::dropEvent( QDropEvent *event )
{
    KURL::List uri;
    if (KURLDrag::decode( event, uri ))
        napp->player()->openFile( uri, false );
}

void SimpleUI::slotAdaptSize( int width, int height )
{
    resize( width + extra_width, height + extra_height );
}

void SimpleUI::slotPlaying()
{
    playButton->setPixmap( QPixmap( pause_xpm ) );
    stopButton->setEnabled( true );
    slider->setEnabled( true );

    if (napp->player()->current())
        statusBar()->message( napp->player()->current().title() );
}

void SimpleUI::slotStopped()
{
    playButton->setPixmap( QPixmap( play_xpm ) );
    stopButton->setEnabled( false );
    slider->setEnabled( false );
    slider->setValue( 0 );
    positionLabel->setText( "" );
    statusBar()->message( "" );
}

void SimpleUI::slotPaused()
{
    playButton->setPixmap( QPixmap( play_xpm ) );
    slider->setEnabled( true );
}

void SimpleUI::slotTimeout()
{
    if (napp->player()->current() && !slider->currentlyPressed())
    {
        positionLabel->setText( napp->player()->lengthString() );
        slider->setRange( 0, (int)napp->player()->getLength() / 1000 );
        slider->setValue( (int)napp->player()->getTime() / 1000 );
    }
}

void SimpleUI::slotSkipTo( int sec )
{
    napp->player()->skipTo( sec * 1000 );
}

void SimpleUI::slotChanged()
{
    propertiesDialog->setPlayObject( napp->player()->current(),
        napp->player()->engine()->playObject() );
}

void SimpleUI::slotContextMenu( const QPoint &pos )
{
    contextMenu->exec( pos );
}

void SimpleUI::slotSliderMoved( int sec )
{
    if (napp->player()->current())
        positionLabel->setText( napp->player()->lengthString( sec * 1000 ) );
}

void SimpleUI::slotVolumeChanged( int volume )
{
    volumeLabel->setText( QString::number( volume ) + "%" );
    volumeSlider->setValue( 100 - volume );
}

void SimpleUI::slotVolumeFrame()
{
    if (volumeFrame->isVisible())
    {
        volumeFrame->hide();
    }
    else
    {
        int x = (volumeButton->width() - volumeFrame->width()) / 2;
        int y = -(volumeFrame->height() + 5);

        QPoint point( volumeButton->mapToGlobal( QPoint( x, y ) ) );
        QRect deskRect = KGlobalSettings::desktopGeometry( point );

        bool bottom = (point.y() + volumeFrame->height()) > (deskRect.y() + deskRect.height());
        bool right = (point.x() + volumeFrame->width()) > (deskRect.x() + deskRect.width());

        volumeFrame->move(
            right ? (deskRect.x() + deskRect.width()) - volumeFrame->width() : ( point.x() < 0 ? 0 : point.x() ),
            bottom ? (deskRect.y() + deskRect.height()) - volumeFrame->height() : ( point.y() < 0 ? 0 : point.y() ) );
        volumeFrame->show();
    }
}

void SimpleUI::slotVolumeSliderMoved( int slider )
{
    napp->player()->setVolume( 100 - slider );
}

#include "userinterface.moc"
