/*
   Copyright (c) 2000 Stefan Schimanski <1Stein@gmx.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   aint with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <unistd.h>

#include <noatun/effects.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/pref.h>
#include "userinterface.h"
#include "pref.h"

#include <qcursor.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qdropsite.h>
#include <qdragobject.h>
#include <qtimer.h>

#include <kfiledialog.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kwin.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <kurldrag.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

Kaiman* Kaiman::kaiman=0;
const char Kaiman::DEFAULT_SKIN[]="car-preset";

Kaiman::Kaiman()
	: KMainWindow(0, "NoatunKaiman"), UserInterface()
{
	NOATUNPLUGINC(Kaiman);
	kaiman=this;

	//setCaption( i18n("Kaiman") );
	KWin::setType( this->winId(), NET::Override );
	setBackgroundMode( NoBackground );
	setAcceptDrops(true);

	_style = 0;
	_seeking = false;
	_altSkin = false;

	// init config
	KConfig *config=KGlobal::config();
	config->setGroup("Kaiman");

	// load skin
	QString skinName = config->readEntry( "SkinResource", DEFAULT_SKIN );

	if ( !changeStyle(skinName, "skindata") )
	{
		KMessageBox::sorry( this, i18n("Cannot load skin %1. Switching to default skin.").arg(skinName) );
		if ( !changeStyle( DEFAULT_SKIN, "skindata" ) )
		{
			KMessageBox::error( this, i18n("Cannot load default skin %1.").arg(DEFAULT_SKIN) );
			QTimer::singleShot( 0, this, SLOT(close()) );
			return;
		}
	}

	// global connects
	connect( napp, SIGNAL(hideYourself()), this, SLOT(hide()) );
	connect( napp, SIGNAL(showYourself()), this, SLOT(show()) );

	connect( napp->player(), SIGNAL(playing()), this, SLOT(updateMode()));
	connect( napp->player(), SIGNAL(stopped()), this, SLOT(updateMode()));
	connect( napp->player(), SIGNAL(paused()), this, SLOT(updateMode()));
	connect( napp->player(), SIGNAL(timeout()), this, SLOT(timeout()));
	connect( napp->player(), SIGNAL(loopTypeChange(int)), this, SLOT(loopTypeChange(int)));
	connect( napp->player(), SIGNAL(newSongLen(int,int)), this, SLOT(newSongLen(int,int)));
	connect( napp->player(), SIGNAL(newSong()), this, SLOT(newSong()));

	if( napp->player()->isPlaying() )
		newSong();

	new KaimanPrefDlg(this);

	show();
}


Kaiman::~Kaiman()
{
}


bool Kaiman::changeStyle( const QString &style, const QString &desc )
{
	QString ldesc = desc;
	if ( ldesc.isEmpty() )
	ldesc = _altSkin ? "alt_skindata" : "skindata";

	bool vis = isVisible();
	if ( vis )
		hide();

	bool ret = loadStyle( style, ldesc );

	newSongLen(0,0);
	timeout();
	loopTypeChange(0);
	updateMode();

	if ( vis )
		show();
	return ret;
}


bool Kaiman::loadStyle( const QString &style, const QString &desc )
{
	if ( _style ) delete _style;
	_style = new KaimanStyle(this);
	if ( !_style->loadStyle( style, desc ) )
	{
		delete _style;
		_style = 0;
		return false;
	}

	if ( _style->Mask() != 0 )
	{
		// Set the shaped window form
		XShapeCombineMask( qt_xdisplay(), winId(), ShapeBounding, 0,0,
		                   _style->Mask()->handle(), ShapeSet );
	}

	KaimanStyleElement* item = _style->find("Background");
	setBackgroundMode(QWidget::NoBackground);
	if ( item!=0 )
	{
		_style->resize( item->width(), item->height());
		resize( item->width(), item->height());
		setFixedSize( item->width(), item->height());
	}

	item = _style->find("Playlist_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(toggleListView()) );

	item = _style->find("Play_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(playpause()) );

	item = _style->find("Pause_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(playpause()) );

	item = _style->find("Stop_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(stop()) );

	item = _style->find("Next_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(forward()) );

	item = _style->find("Prev_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp->player(), SLOT(back()) );

	item = _style->find("Exit_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(close()) );

	item = _style->find("Mixer_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(execMixer()) );

	item = _style->find("Iconify_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(showMinimized()) );

	item = _style->find("Alt_Skin_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(toggleSkin()) );

	item = _style->find("Repeat_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(toggleLoop()) );

	item = _style->find("Shuffle_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this, SLOT(toggleShuffle()) );

	item = _style->find("Config_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), napp, SLOT(preferences()) );

	item = _style->find("Volume_Up_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this,SLOT(volumeUp()));

	item = _style->find("Volume_Down_Button");
	if( item!=0 ) connect( item, SIGNAL(clicked()), this,SLOT(volumeDown()));

	KaimanStyleSlider* slider =
		static_cast<KaimanStyleSlider*>(_style->find("Position_Slider"));
	if( slider!=0 )
	{
		connect( slider, SIGNAL(newValueDrag(int)), this, SLOT(seekStart(int)) );
		connect( slider, SIGNAL(newValue(int)), this, SLOT(seekDrag(int)) );
		connect( slider, SIGNAL(newValueDrop(int)), this, SLOT(seekStop(int)) );
		slider->setValue( 0, 0, 1000 );
	}

	slider = static_cast<KaimanStyleSlider*>(_style->find("Volume_Slider"));
	if ( slider!=0 )
	{
		connect(slider, SIGNAL(newValue(int)), this, SLOT(setVolume(int)));
		slider->setValue( napp->player()->volume(), 0, 100 );
	}

	KaimanStyleValue* volItem = static_cast<KaimanStyleValue*>(_style->find("Volume_Item"));
	if ( volItem )
		volItem->setValue( napp->player()->volume(), 0, 100 );

	KaimanStyleText* titleItem = static_cast<KaimanStyleText*>(_style->find("Title"));
	if ( titleItem )
		titleItem->startAnimation( 300 );

	return true;
}


void Kaiman::closeEvent(QCloseEvent*)
{
	unload();
}


void Kaiman::dragEnterEvent( QDragEnterEvent *event )
{
	event->accept( KURLDrag::canDecode(event) );
}


void Kaiman::dropEvent( QDropEvent *event )
{
	doDropEvent(event);
}


void Kaiman::doDropEvent(QDropEvent *event)
{
	KURL::List uri;
	if (KURLDrag::decode(event, uri))
	{
		for (KURL::List::Iterator i = uri.begin(); i != uri.end(); ++i)
			napp->player()->openFile(*i, false);
	}
}


void Kaiman::seekStart( int )
{
	_seeking = true;
}


void Kaiman::seekDrag( int value )
{
	int length = napp->player()->getLength()/1000;
	if ( length < 0)
		length = 0;

	if ( !_style ) return;

	KaimanStyleValue* posItem =
		static_cast<KaimanStyleValue*>(_style->find("Position_Item"));
	if ( posItem )
		posItem->setValue( value, 0, length );

	KaimanStyleSlider* posSlider =
		static_cast<KaimanStyleSlider*>(_style->find("Position_Slider"));
	if ( posSlider )
		posSlider->setValue( value, 0, length );

	// update time

	KaimanStyleNumber* numItem =
		static_cast<KaimanStyleNumber*>(_style->find("Minute_Number"));
	if ( numItem )
		numItem->setValue( value/60%60 );

	numItem = static_cast<KaimanStyleNumber*>(_style->find("Second_Number"));
	if ( numItem )
		numItem->setValue( value % 60 );
}


void Kaiman::seekStop( int value )
{
	seek( value );
	_seeking = false;
}


void Kaiman::seek( int value )
{
	napp->player()->skipTo( value*1000 ); // skipTo() takes milliseconds
}


void Kaiman::toggleSkin()
{
	_altSkin = !_altSkin;

	QString skinName = _style->skinName();

	QString oldDesc, newDesc;
	if ( _altSkin )
	{
		oldDesc = QString::fromLatin1("skindata");
		newDesc = QString::fromLatin1("alt_skindata");
	}
	else
	{
		newDesc = QString::fromLatin1("skindata");
		oldDesc = QString::fromLatin1("alt_skindata");
	}

	if ( !changeStyle(skinName, newDesc) )
		changeStyle(skinName, oldDesc);
}


void Kaiman::setVolume( int vol )
{
	if ( vol<0 ) vol=0;
	if ( vol>=100 ) vol=100;

	napp->player()->setVolume( vol );
}


void Kaiman::volumeUp()
{
	setVolume( napp->player()->volume()+10 );
}


void Kaiman::volumeDown()
{
	setVolume( napp->player()->volume()-10 );
}


void Kaiman::execMixer()
{
	kapp->startServiceByDesktopName ( QString::fromLatin1("kmix"), QString::null );
}


void Kaiman::timeout()
{
	if ( !_style ) return;

	if (!napp->player()->current())
		return;

	// update volume
	KaimanStyleSlider* l_elem_volslider = static_cast<KaimanStyleSlider*>(_style->find("Volume_Slider"));
	KaimanStyleValue* l_elem_volitem = static_cast<KaimanStyleValue*>(_style->find("Volume_Item"));
	if ( l_elem_volslider!=0 )
		l_elem_volslider->setValue( napp->player()->volume(), 0, 100 );
	if ( l_elem_volitem!=0 )
		l_elem_volitem->setValue( napp->player()->volume(), 0, 100 );

	// update position
	if ( !_seeking )
	{
		int sec = napp->player()->getTime()/1000;
		if ( sec < 0 )
			sec = 0;

		KaimanStyleValue* posItem =
			static_cast<KaimanStyleValue*>(_style->find("Position_Item"));
		if ( posItem ) posItem->setValue( sec, 0, napp->player()->getLength()/1000 );

		KaimanStyleSlider* posSlider =
			static_cast<KaimanStyleSlider*>(_style->find("Position_Slider"));
		if ( posSlider ) posSlider->setValue( sec, 0, napp->player()->getLength()/1000 );

		// update time
		KaimanStyleNumber* numItem =
			static_cast<KaimanStyleNumber*>(_style->find("Minute_Number"));
		if ( numItem )
			numItem->setValue( sec/60%60 );

		numItem = static_cast<KaimanStyleNumber*>(_style->find("Second_Number"));
		if ( numItem )
			numItem->setValue( sec%60 );
	}

	const PlaylistItem &item = napp->playlist()->current();
	KaimanStyleNumber* numItem = 0;

	numItem = static_cast<KaimanStyleNumber*>(_style->find("In_Rate_Number"));
	if ( numItem )
		numItem->setValue(item.property("bitrate").toInt());

	QString hzString = item.property("samplerate");
	hzString.truncate(2);

	numItem = static_cast<KaimanStyleNumber*>(_style->find("In_Hz_Number"));
	if ( numItem )
		numItem->setValue(hzString.toInt());
}


void Kaiman::updateMode()
{
	if ( !_style ) return;

	KaimanStyleButton* pause =
		static_cast<KaimanStyleButton*>(_style->find("Pause_Button"));
	KaimanStyleButton* play =
		static_cast<KaimanStyleButton*>(_style->find("Play_Button"));
	KaimanStyleState* status =
		static_cast<KaimanStyleState*>(_style->find("Status_Item"));

	if (napp->player()->isStopped() )
	{
		if ( pause ) pause->setLit( false );
		if ( play ) play->setLit( false );
		if ( status ) status->setValue( 0 );
	}
	else if ( napp->player()->isPlaying() )
	{
		if ( pause ) pause->setLit( false );
		if ( play ) play->setLit( true );
		if ( status ) status->setValue( 2 );
	}
	else if ( napp->player()->isPaused() )
	{
		if ( pause ) pause->setLit( true );
		if ( play ) play->setLit( false );
		if ( status ) status->setValue( 1 );
	}
}


void Kaiman::loopTypeChange( int )
{
	if ( !_style ) return;

	KaimanStyleState* rep = static_cast<KaimanStyleState*>(_style->find("Repeat_Item"));
	if ( rep )
		rep->setValue( napp->player()->loopStyle() );
}


void Kaiman::newSongLen( int, int )
{
	if ( !_style )
		return;

	int len = napp->player()->getLength()/1000; // convert milliseconds -> seconds
	if ( len < 0 ) // getLength returns -1 if there's no Playobject
		len = 0;

	// update time
	KaimanStyleNumber* numItem =
		static_cast<KaimanStyleNumber*>(_style->find("Minute_Total_Number"));
	if ( numItem )
		numItem->setValue( len/60%60 );

	numItem = static_cast<KaimanStyleNumber*>(_style->find("Second_Total_Number"));
	if ( numItem )
		numItem->setValue( len%60 );

	numItem = static_cast<KaimanStyleNumber*>(_style->find("Song_Minute_Number"));
	if ( numItem )
		numItem->setValue( len/60%60 );

	numItem = static_cast<KaimanStyleNumber*>(_style->find("Second_Minute_Number"));
	if ( numItem )
		numItem->setValue( len%60 );

	newSong();
}


void Kaiman::newSong()
{
	if ( !_style ) return;

	KaimanStyleText* titleItem = static_cast<KaimanStyleText*>(_style->find("Title"));
	if ( titleItem )
	{
		QString title = i18n("Noatun");
		if ( napp->player()->current() )
		{
			title = napp->player()->current().title();
			if ( title.isEmpty() )
				title = napp->player()->current().file();

			title = i18n("TITLE (LENGTH)", "%1 (%2)").arg(title,
				napp->player()->current().lengthString());
		}
		titleItem->setValue( title );
	}
}


#undef None
void Kaiman::toggleLoop()
{
	KPopupMenu *loopMenu = new KPopupMenu(this, "loopMenu");
	int selectedItem = 0;

	loopMenu->setCheckable(true);
	loopMenu->insertTitle(i18n("Loop Style"));
	loopMenu->insertItem(i18n("&None"),
		(int)Player::None);
	loopMenu->insertItem(i18n("&Song"),
		(int)Player::Song);
	loopMenu->insertItem(i18n("&Playlist"),
	 (int)(Player::Playlist));
	loopMenu->insertItem(i18n("&Random"),
	(int)(Player::Random));

	loopMenu->setItemChecked((int)napp->player()->loopStyle(), true); // select current loopstyle in menu
	selectedItem = loopMenu->exec(QCursor::pos());
	if (selectedItem != -1)
		napp->player()->loop(selectedItem); // set new loopstyle

	delete loopMenu;
}


void Kaiman::toggleShuffle()
{
	napp->player()->loop(Player::Random); // set loopstyle to be random
}

#include "userinterface.moc"
