/*
 * disc.cpp
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <qstringlist.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qvaluelist.h>
#include <qcursor.h>
#include <qtimer.h>
#include <qfile.h>
#include <qpopupmenu.h>
#include <qapplication.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kinputdialog.h>
#include <dcopref.h>

#include "cddb.h"
#include "mrl.h"
#include "disc.h"
#include "disc.moc"

MLabel::MLabel( QWidget *parent ) : QLabel( parent )
{
	setText("<center><font size=\"6\"><b>"+i18n("Audio CD")+"</b></font></center>");
}

void MLabel::paintEvent( QPaintEvent *pe )
{
	QLabel::paintEvent( pe );
	QColorGroup cg = parentWidget()->colorGroup();
	QColor base = cg.base();
	QColor selection = cg.highlight();
	int r = (base.red() + selection.red()) / 2;
	int b = (base.blue() + selection.blue()) / 2;
	int g = (base.green() + selection.green()) / 2;
	setPaletteBackgroundColor( QColor(r, g, b) );
}

MListView::MListView( QWidget *parent ) : KListView( parent )
{
}

void MListView::resizeEvent( QResizeEvent *rev )
{
	int width = contentsRect().width();
	setColumnWidth(0, 40);  /* Track */
	setColumnWidth(1, width-90);  /* title */
	setColumnWidth(2, 50);  /* duration */

	KListView::resizeEvent(rev);
}

Disc::Disc( QWidget* parent, QObject *objParent, const char *name ) : KaffeineInput(objParent , name)
{
	mainWidget = new QVBox( parent );
	mainWidget->setSizePolicy( QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred) );
	split = new QSplitter( mainWidget );
	split->setOpaqueResize( true );
	widg = new QWidget( split );
	widg->setMinimumWidth( 200 );
	widg->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );
	QVBoxLayout *wb = new QVBoxLayout( widg, 0, 0 );
	discLab = new MLabel( widg );
	wb->addWidget( discLab );
	playerBox = new QVBox( widg );
	wb->addWidget( playerBox );
	playerBox->setMinimumWidth( 200 );
	playerBox->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );
	panel = new QFrame( split );
	split->moveToFirst( panel );
	panel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::MinimumExpanding ) );
	split->setResizeMode( panel, QSplitter::KeepSize );

	QVBoxLayout *vb = new QVBoxLayout( panel, 3, 3 );
	cdBtn = new QToolButton( panel );
	cdBtn->setTextLabel( i18n("Play CD") );
	cdBtn->setTextPosition( QToolButton::Under );
	cdBtn->setUsesTextLabel( true );
	cdBtn->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
	QToolTip::add( cdBtn, i18n("Play CD"));
	ripBtn = new QToolButton( panel );
	ripBtn->setTextLabel( i18n("Rip CD") );
	ripBtn->setTextPosition( QToolButton::Under );
	ripBtn->setUsesTextLabel( true );
	ripBtn->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed ) );
	QToolTip::add( ripBtn, i18n("Rip CD"));

	QHBoxLayout *h1 = new QHBoxLayout();
	h1->addWidget( cdBtn );
	h1->addWidget( ripBtn );
	vb->addLayout( h1 );

	cdBtn->setIconSet( KGlobal::iconLoader()->loadIconSet("cdaudio_unmount", KIcon::Toolbar) );
	ripBtn->setIconSet( KGlobal::iconLoader()->loadIconSet("kilogram", KIcon::Toolbar) );

	connect( cdBtn, SIGNAL(clicked()), this, SLOT(startCD()) );
	connect( ripBtn, SIGNAL(clicked()), this, SLOT(startRIP()) );

	h1 = new QHBoxLayout();
	h1->addWidget( new QLabel( i18n("Artist:"), panel ) );
	artistLab = new QLabel( "", panel );
	artistLab->setLineWidth(1);
	artistLab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	artistLab->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ) );
	h1->addWidget( artistLab );
	vb->addLayout( h1 );
	h1 = new QHBoxLayout();
	h1->addWidget( new QLabel( i18n("Album:"), panel ) );
	albumLab = new QLabel( "", panel );
	albumLab->setLineWidth(1);
	albumLab->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	albumLab->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred ) );
	h1->addWidget( albumLab );
	vb->addLayout( h1 );

	list = new MListView( panel );
	list->setHScrollBarMode( QListView::AlwaysOff );
	connect( list, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(trackSelected(QListViewItem*)) );
	list->addColumn( i18n("Track") );
	list->addColumn( i18n("Title") );
	list->addColumn( i18n("Duration") );
	//list->setSortColumn( -1 );
	list->setAllColumnsShowFocus( true );
	list->setSelectionMode(QListView::Extended);
	list->setColumnWidthMode( 0, QListView::Manual );
	list->setColumnWidthMode( 1, QListView::Manual );
	list->setColumnWidthMode( 2, QListView::Manual );
	list->setResizeMode( QListView::NoColumn );
	vb->addWidget( list );

	encodeWidget = new QWidget( panel );
	QGridLayout *grid = new QGridLayout( encodeWidget, 2, 2, 0, 3 );
	QLabel *lab = new QLabel( i18n("Select the tracks you want to rip and click the <b>Encode</b> button."), encodeWidget );
	grid->addMultiCellWidget( lab, 0, 1, 0, 0);
	enc = new QToolButton( encodeWidget );
	grid->addMultiCellWidget( enc, 0, 0, 1, 1);
	enc->setTextLabel( i18n("Encode...") );
	enc->setTextPosition( QToolButton::Under );
	enc->setUsesTextLabel( true );
	enc->setIconSet( KGlobal::iconLoader()->loadIconSet("kilogram", KIcon::Small) );
	enc->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	connect( enc, SIGNAL(clicked()), this, SLOT(encode()) );
	vb->addWidget( encodeWidget );
	encodeWidget->hide();

	progressBar = new QProgressBar( 100, panel );
	vb->addWidget( progressBar );
	progressBar->hide();

	vb->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Preferred ) );

	artistLab->setPaletteBackgroundColor( list->paletteBackgroundColor() );
	albumLab->setPaletteBackgroundColor( list->paletteBackgroundColor() );

	connect( &encodeTimer, SIGNAL(timeout()), this, SLOT(encodeProgress()) );

	setXMLFile("kaffeinedisc.rc");
	setupActions();

	loadConfig( KGlobal::config() );

	para = NULL;
	trackCurrent = 0;
	currentPixmap = UserIcon("playing");
}

Disc::~Disc()
{
}

void Disc::togglePanel()
{
	if ( panel->isHidden() )
		panel->show();
	else
		panel->hide();
}

void Disc::getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames )
{
	uiNames.append( i18n("Audio CD encoding") );
	iconNames.append( "kilogram" );
	targetNames.append( "DISC_RIP" );
	uiNames.append( i18n("Play Audio CD") );
	iconNames.append( "cdaudio_unmount" );
	targetNames.append( "DISC_CDDA" );
	uiNames.append( i18n("Play DVD") );
	iconNames.append( "dvd_unmount" );
	targetNames.append( "DISC_DVD" );
	uiNames.append( i18n("Play VCD") );
	iconNames.append( "cdrom_unmount" );
	targetNames.append( "DISC_VCD" );
}

bool Disc::execTarget( const QString &target )
{
	if ( target=="DISC_CDDA" ) {
		//emit showMe( this );
		QTimer::singleShot( 100, this, SLOT(startCD()) );
		return true;
	}
	else if ( target=="DISC_RIP" ) {
		QTimer::singleShot( 100, this, SLOT(startRIP()) );
		return true;
	}
	else if ( target=="DISC_DVD" ) {
		QTimer::singleShot( 100, this, SLOT(startDVD()) );
		return true;
	}
	else if ( target=="DISC_VCD" ) {
		QTimer::singleShot( 100, this, SLOT(startVCD()) );
		return true;
	}
	return false;
}

void Disc::loadConfig( KConfig* config )
{
	QValueList<int> sl;

	config->setGroup("Disc");
	sl = config->readIntListEntry("SplitSizes");
	split->setSizes( sl );
}

void Disc::saveConfig()
{
	KConfig* config = KGlobal::config();

	config->setGroup("Disc");
	config->writeEntry( "SplitSizes", split->sizes() );
}

QWidget* Disc::wantPlayerWindow()
{
	return playerBox;
}

QWidget* Disc::inputMainWidget()
{
	return mainWidget;
}

void Disc::setupActions()
{
	new KAction(i18n("Open &DVD"), "dvd_unmount", 0, this, SLOT(startDVD()), actionCollection(), "file_open_dvd");
	new KAction(i18n("Open &VCD"), "cdrom_unmount", 0, this, SLOT(startVCD()), actionCollection(), "file_open_vcd");
	new KAction(i18n("Open &Audio-CD"), "cdaudio_unmount", 0, this, SLOT(startCD()), actionCollection(), "file_open_audiocd");
}

void Disc::playerStopped()
{
	trackCurrent = 0;
	if ( !list->isEnabled() )
		return;
	list->clear();
	artistLab->setText( "" );
	albumLab->setText( "" );
	encodeWidget->hide();
}

void Disc::setEncoding( bool b )
{
	list->setEnabled( !b );
	artistLab->setEnabled( !b );
	albumLab->setEnabled( !b );
	cdBtn->setEnabled( !b );
	ripBtn->setEnabled( !b );
	enc->setEnabled( !b );
	if ( b ) {
		progressBar->setProgress( 0 );
		progressBar->show();
	}
	else
		progressBar->hide();
}

void Disc::encode()
{
	QListViewItem *it;
	QStringList tracklist;

	it = list->firstChild();
	if ( !it )
		return;
	tracklist.append( artistLab->text() );
	tracklist.append( albumLab->text() );
	while ( it!=0 ) {
		if ( it->isSelected() )
			tracklist.append( it->text(0)+"-"+it->text(1) );
		it = it->nextSibling();
	}
	if ( (int)tracklist.count()<3 ) {
		KMessageBox::information( mainWidget, i18n("You must select the tracks to rip."), i18n("Warning") );
		return;
	}
	setEncoding( true );
	if ( trackCurrent )
		emit stop();
	para = new Paranoia();
	if ( !para->init( currentDevice ) ) {
		KMessageBox::information( mainWidget, i18n("Unable to initialize cdparanoia."), i18n("Warning") );
		delete para;
		para = NULL;
		setEncoding( false );
		return;
	}

	if ( para->encode( tracklist, mainWidget ) )
		encodeTimer.start( 1000 );
	else {
		delete para;
		para = NULL;
		setEncoding( false );
	}
}

void Disc::encodeProgress()
{
	if ( para->running() )
		progressBar->setProgress( para->getProgress() );
	else {
		encodeTimer.stop();
		delete para;
		para = NULL;
		setEncoding( false );
	}

}

void Disc::setCurrent( int n )
{
	QListViewItem *it;

	it = list->firstChild();
	while ( it!=0 ) {
		if ( it->text(0).toInt()==n )
			it->setPixmap( 1, currentPixmap );
		else
			it->setPixmap( 1, QPixmap() );
		it = it->nextSibling();
	}
}

void Disc::trackSelected( QListViewItem *it )
{
	if ( !it )
		return;

	MRL mrl( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
	mrl.setTitle( it->text(1) );
	mrl.setArtist( artistLab->text() );
	mrl.setAlbum( albumLab->text() );
	mrl.setTrack( it->text(0) );
	mrl.setMime( QString("audio/cdda") );
	trackCurrent = it->text(0).toInt();
	setCurrent( trackCurrent );
	emit play( mrl, this );
}

void Disc::startRIP()
{
	startCD( "", true );
}

void Disc::startCD( const QString &device, bool rip )
{
	QStringList s;
	bool init=false;
	QValueList<int> qvl;
	int i;
	KListViewItem *it;
	MRL mrl;
	QStringList dcopList, devList;
	bool ok=false;

	if ( !cdBtn->isEnabled() ) {
		emit showMe( this );
		return;
	}


	if ( !device.isEmpty() )
		s.append( device );
	else {
		DCOPRef mediamanager("kded","mediamanager");
		DCOPReply reply = mediamanager.call("fullList()");
		if ( reply.isValid() ) {
			dcopList = reply;
			i=0;
			while ( i<(int)dcopList.count() ) {
				//kdDebug() << dcopList[i+5] << " * " << dcopList[i+6] << " * " << dcopList[i+10] << endl;
				if ( dcopList[i+10]=="media/audiocd" ) {
					devList.append( dcopList[i+5] );
				}
				i+=13;
			}
			if ( devList.count()>1 ) {
				QString choice = KInputDialog::getItem( i18n("Audio CD"), i18n("Several Audio CD found. Choose one:"),
					devList, 0, false, &ok );
				if ( ok )
					s.append( choice );
				else
					return;
			}
			else if ( devList.count()==1 )
				s.append( devList[0] );
			else {
				s.append( "/dev/cdrom" );
				s.append( "/dev/dvd" );
			}
		}
		else {
			s.append( "/dev/cdrom" );
			s.append( "/dev/dvd" );
		}
	}

	QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
	qApp->processEvents();

	para = new Paranoia();
	for ( i=0; i<(int)s.count(); i++ ) {
		if ( (init = para->init( s[i] )) ) {
			currentDevice = s[i];
			break;
		}
	}
	if ( init ) {
		list->clear();
		artistLab->setText( "" );
		albumLab->setText( "" );
		for ( i=0; i<para->getTracks(); i++)
			qvl.append( para->trackFirstSector(i+1) + 150 );
		qvl.append( para->discFirstSector() );
		qvl.append( para->discLastSector() );
		CDDB *cddb = new CDDB();
		cddb->save_cddb( true );
		if ( cddb->queryCD(qvl) ) {
			artistLab->setText( cddb->artist() );
			albumLab->setText( cddb->title() );
			for ( i=0; i<para->getTracks(); i++ ) {
				it = new KListViewItem( list, QString().sprintf("%02d", i+1), cddb->track( i ), para->trackTime(i) );
				if ( i==0 ) {
					mrl.setURL( QString("cdda://%1/1").arg( currentDevice ) );
					mrl.setTitle( cddb->track(i) );
					mrl.setArtist( artistLab->text() );
					mrl.setAlbum( albumLab->text() );
					mrl.setTrack( QString().sprintf("%02d", i+1) );
				}
			}
		}
		else {
			cddb->set_server( "freedb.freedb.org", 8880 );
			if ( cddb->queryCD(qvl) ) {
				artistLab->setText( cddb->artist() );
				albumLab->setText( cddb->title() );
				for ( i=0; i<para->getTracks(); i++ ) {
					it = new KListViewItem( list, QString().sprintf("%02d", i+1), cddb->track( i ), para->trackTime(i) );
					if ( i==0 ) {
						mrl.setURL( QString("cdda://%1/1").arg( currentDevice ) );
						mrl.setTitle( cddb->track(i) );
						mrl.setArtist( artistLab->text() );
						mrl.setAlbum( albumLab->text() );
						mrl.setTrack( QString().sprintf("%02d", i+1) );
					}
				}
			}
			else {
				artistLab->setText( i18n("Unknown") );
				albumLab->setText( i18n("Unknown") );
				for ( i=0; i<para->getTracks(); i++ ) {
					it = new KListViewItem( list, QString().sprintf("%02d", i+1), i18n("Track")+QString().sprintf("%02d", i+1), para->trackTime(i) );
					if ( i==0 ) {
						mrl.setURL( QString("cdda://%1/1").arg( currentDevice ) );
						mrl.setTitle( i18n("Track")+QString().sprintf("%02d", i+1) );
						mrl.setArtist( artistLab->text() );
						mrl.setAlbum( albumLab->text() );
						mrl.setTrack( QString().sprintf("%02d", i+1) );
					}
				}
			}
		}
		delete cddb;
		QApplication::restoreOverrideCursor();
		encodeWidget->show();
		emit showMe( this );
		if ( !rip && !mrl.isEmpty() ) {
			mrl.setMime( QString("audio/cdda") );
			trackCurrent = mrl.track().toInt();
			setCurrent( trackCurrent );
			emit play( mrl, this );
		}
	}
	else {
		QApplication::restoreOverrideCursor();
		KMessageBox::information( 0, i18n("No audio CD found."), i18n("Warning") );
	}
	delete para;
	para = NULL;
}

void Disc::startDVD( const QString &device )
{
	MRL mrl;
	QStringList dcopList, devList;
	int i;
	bool ok=false;

	if ( !device.isEmpty() )
		mrl.setURL( QString("dvd://%1").arg(device) );
	else {
		DCOPRef mediamanager("kded","mediamanager");
		DCOPReply reply = mediamanager.call("fullList()");
		if ( reply.isValid() ) {
			dcopList = reply;
			i=0;
			while ( i<(int)dcopList.count() ) {
				//kdDebug() << dcopList[i+5] << " * " << dcopList[i+6] << " * " << dcopList[i+10] << endl;
				if ( dcopList[i+10]=="media/dvdvideo" ) {
					devList.append( dcopList[i+5] );
				}
				else if ( dcopList[i+10]=="media/cdrom_mounted"
				|| dcopList[i+10]=="media/cdwriter_mounted"
				|| dcopList[i+10]=="media/dvd_mounted" ) {
					if ( QFile::exists(dcopList[i+6]+"/video_ts") || QFile::exists(dcopList[i+6]+"/VIDEO_TS") )
						devList.append( dcopList[i+5] );
				}
				i+=13;
			}
			if ( devList.count()>1 ) {
				QString choice = KInputDialog::getItem( i18n("DVD Video"), i18n("Several DVD Video found. Choose one:"),
					devList, 0, false, &ok );
				if ( ok )
					mrl.setURL( QString("dvd://%1").arg(choice) );
				else
					return;
			}
			else if ( devList.count()==1 )
				mrl.setURL( QString("dvd://%1").arg(devList[0]) );
			else {
				//KMessageBox::information( 0, i18n("No DVD Video found."), i18n("Warning") );
				//return;
				mrl.setURL( QString("dvd://") );
			}
		}
		else
			mrl.setURL( QString("dvd://") );
	}

	mrl.setMime( QString("video/dvd") );

	if ( !progressBar->isVisible() ) {
		list->clear();
		artistLab->setText( "" );
		albumLab->setText( "" );
		encodeWidget->hide();
	}
	trackCurrent = 0;
	emit play( mrl, this );
}

void Disc::startVCD( const QString &device )
{
	MRL mrl;
	QStringList dcopList, devList;
	int i;
	bool ok=false;

	if ( !device.isEmpty() )
		mrl.setURL( QString("vcd://%1").arg(device) );
	else {
		DCOPRef mediamanager("kded","mediamanager");
		DCOPReply reply = mediamanager.call("fullList()");
		if ( reply.isValid() ) {
			dcopList = reply;
			i=0;
			while ( i<(int)dcopList.count() ) {
				//kdDebug() << dcopList[i+5] << " * " << dcopList[i+6] << " * " << dcopList[i+10] << endl;
				if ( dcopList[i+10]=="media/vcd" || dcopList[i+10]=="media/svcd" ) {
					devList.append( dcopList[i+5] );
				}
				i+=13;
			}
			if ( devList.count()>1 ) {
				QString choice = KInputDialog::getItem( i18n("VCD-SVCD"), i18n("Several (S)VCD found. Choose one:"),
					devList, 0, false, &ok );
				if ( ok )
					mrl.setURL( QString("vcd://%1").arg(choice) );
				else
					return;
			}
			else if ( devList.count()==1 )
				mrl.setURL( QString("vcd://%1").arg(devList[0]) );
			else {
				//KMessageBox::information( 0, i18n("No (S)VCD found."), i18n("Warning") );
				//return;
				mrl.setURL( QString("vcd://") );
			}
		}
		else
			mrl.setURL( QString("vcd://") );
	}

	mrl.setMime( QString("video/vcd") );

	if ( !progressBar->isVisible() ) {
		list->clear();
		artistLab->setText( "" );
		albumLab->setText( "" );
		encodeWidget->hide();
	}
	trackCurrent = 0;
	emit play( mrl, this );
}

bool Disc::currentTrack( MRL &mrl )
{
	QListViewItem *it;

	if ( !trackCurrent )
		return false;

	it = list->firstChild();
	if ( !it )
		return false;
	while ( it!=0 ) {
		if ( it->text(0).toInt()==trackCurrent ) {
			mrl.setURL( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
			mrl.setTitle( it->text(1) );
			mrl.setArtist( artistLab->text() );
			mrl.setAlbum( albumLab->text() );
			mrl.setTrack( it->text(0) );
			mrl.setMime( QString("audio/cdda") );
			setCurrent( trackCurrent );
			return true;
		}
		it = it->nextSibling();
	}

	return false;
}

bool Disc::playbackFinished( MRL &mrl )
{
	return nextTrack( mrl );
}

bool Disc::nextTrack( MRL &mrl )
{
	QListViewItem *it;

	if ( !trackCurrent )
		return false;

	it = list->firstChild();
	if ( !it )
		return false;
	while ( it!=0 ) {
		if ( it->text(0).toInt()==trackCurrent+1 ) {
			mrl.setURL( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
			mrl.setTitle( it->text(1) );
			mrl.setArtist( artistLab->text() );
			mrl.setAlbum( albumLab->text() );
			mrl.setTrack( it->text(0) );
			mrl.setMime( QString("audio/cdda") );
			++trackCurrent;
			setCurrent( trackCurrent );
			return true;
		}
		it = it->nextSibling();
	}

	it = list->firstChild();
	mrl.setURL( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
	mrl.setTitle( it->text(1) );
	mrl.setArtist( artistLab->text() );
	mrl.setAlbum( albumLab->text() );
	mrl.setTrack( it->text(0) );
	mrl.setMime( QString("audio/cdda") );
	trackCurrent = 1;
	setCurrent( trackCurrent );
	return true;
}

bool Disc::previousTrack( MRL &mrl )
{
	QListViewItem *it;

	if ( !trackCurrent )
		return false;

	it = list->firstChild();
	if ( !it )
		return false;
	while ( it!=0 ) {
		if ( it->text(0).toInt()==trackCurrent-1 ) {
			mrl.setURL( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
			mrl.setTitle( it->text(1) );
			mrl.setArtist( artistLab->text() );
			mrl.setAlbum( albumLab->text() );
			mrl.setTrack( it->text(0) );
			mrl.setMime( QString("audio/cdda") );
			--trackCurrent;
			setCurrent( trackCurrent );
			return true;
		}
		it = it->nextSibling();
	}

	return false;
}

bool Disc::trackNumber( int number, MRL &mrl )
{
	QListViewItem *it;

	if ( !trackCurrent )
		return false;

	it = list->firstChild();
	if ( !it )
		return false;
	while ( it!=0 ) {
		if ( it->text(0).toInt()==number ) {
			mrl.setURL( QString("cdda://%1/%2").arg( currentDevice ).arg( it->text(0).toInt() ) );
			mrl.setTitle( it->text(1) );
			mrl.setArtist( artistLab->text() );
			mrl.setAlbum( albumLab->text() );
			mrl.setTrack( it->text(0) );
			mrl.setMime( QString("audio/cdda") );
			trackCurrent=number;
			setCurrent( trackCurrent );
			return true;
		}
		it = it->nextSibling();
	}

	return false;
}

void Disc::mergeMeta( const MRL& )
{
}
