/*
 * dvbpanel.cpp
 *
 * Copyright (C) 2004-2007 Christophe Thommeret <hftom@free.fr>
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

#include <sys/statvfs.h>

#include <qlayout.h>
#include <qfile.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmap.h>

#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kinputdialog.h>
#include <kicondialog.h>
#include <kaction.h>
#include <kprocess.h>
#include <kfiledialog.h>
#include <ktrader.h>
#include <kxmlguifactory.h>
#include <kparts/componentfactory.h>

#include "kaffeinedvbplugin.h"
#include "dvbpanel.h"
#include "channeldesc.h"
#include "dvbstream.h"
#include "kevents.h"
#include "broadcasteditor.h"
#include "channeleditor.h"

#define CHANICONSIZE 28

#define MODE_FREE 0
#define MODE_LIVE 100
#define MODE_BROADCAST 200
#define MODE_RECORD 300
#define MODE_CANTDO 400



DIconViewItem::DIconViewItem( DvbPanel *pan, QIconView *parent, const QString &text, const QPixmap &icon )
	: KIconViewItem( parent, text, icon )
{
	panel = pan;
}



void DIconViewItem::dropped( QDropEvent *e, const QValueList<QIconDragItem> & )
{
	QString s;

	if ( !dropEnabled() )
		return;
	if ( QTextDrag::decode( e, s ) )
		panel->moveChannel( text(), s );
}



DListView::DListView( QWidget *parent ) : KListView( parent )
{
	visibleItems = 0;
}



QDragObject* DListView::dragObject()
{
	if ( currentItem() )
		return new QTextDrag( currentItem()->text(1), this );
	else
		return 0;
}



DvbPanel::DvbPanel( QWidget *parent, QObject *objParent, const char *name ) : KaffeineInput(objParent,name)
{
	browseDvbStream = -1;
	plug = NULL;

	isTuning = false;
	timeShiftFileName = "";
	timersDialog = 0;
	currentCategory = "All";
	channels.setAutoDelete( true );
	timers.setAutoDelete( true );
	dvb.setAutoDelete( true );

	mainWidget = new QVBox( parent );
	mainWidget->setSizePolicy( QSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred) );
	split = new QSplitter( mainWidget );
	split->setOpaqueResize( true );
	pbox = new QVBox( split );
	iconView = new KIconView( pbox );
	iconView->setVScrollBarMode( QScrollView::AlwaysOff );
	iconView->setHScrollBarMode( QScrollView::AlwaysOff );
	iconView->horizontalScrollBar()->setFixedHeight( 0 );
	connect( iconView, SIGNAL(rightButtonPressed(QIconViewItem*,const QPoint&)), this, SLOT(catContextMenu(QIconViewItem*,const QPoint&)) );
	connect( iconView, SIGNAL(clicked(QIconViewItem*)), this, SLOT(catSelected(QIconViewItem*)) );
	iconView->setArrangement(QIconView::TopToBottom);
	iconView->setMargin(0);
	iconView->setSpacing(0);
	iconView->setItemsMovable(false);
	iconView->setResizeMode(QIconView::Adjust);
	iconView->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	playerBox = new QVBox( pbox );
	playerBox->setMinimumWidth( 200 );
	panel = new QFrame( split );
	panel->setLineWidth(1);
	panel->setFrameStyle(QFrame::Panel|QFrame::Sunken);
	split->moveToFirst( panel );
	split->setResizeMode( panel, QSplitter::KeepSize );

	QVBoxLayout *vb = new QVBoxLayout( panel, 3, 3 );
	channelsBtn = new QToolButton( panel );
	channelsBtn->setAutoRaise( true );
	QToolTip::add( channelsBtn, i18n("Channels"));
	dateBtn = new QToolButton( panel );
	dateBtn->setAutoRaise( true );
	QToolTip::add( dateBtn, i18n("Timers"));
	infoBtn = new QToolButton( panel );
	infoBtn->setAutoRaise( true );
	QToolTip::add( infoBtn, i18n("Electronic Program Guide"));
	osdBtn = new QToolButton( panel );
	osdBtn->setAutoRaise( true );
	QToolTip::add( osdBtn, i18n("OSD"));
	configBtn = new QToolButton( panel );
	configBtn->setAutoRaise( true );
	QToolTip::add( configBtn, i18n("DVB settings"));
	recallBtn = new QToolButton( panel );
	recallBtn->setAutoRaise( true );
	QToolTip::add( recallBtn, i18n("Recall"));
	QHBoxLayout *h1 = new QHBoxLayout();
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	h1->addWidget( channelsBtn );
	h1->addWidget( dateBtn );
	h1->addWidget( infoBtn );
	h1->addWidget( osdBtn );
	h1->addWidget( configBtn );
	h1->addWidget( recallBtn );
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	vb->addLayout( h1 );

	h1 = new QHBoxLayout();
	searchBtn = new QToolButton( panel );
	searchBtn->setAutoRaise( true );
	QToolTip::add( searchBtn, i18n("Search channel(s)"));
	QLabel* filterLabel = new QLabel( i18n("Filter") + ":", panel );
	searchLE = new KLineEdit( panel );
	searchLE->setFocusPolicy( QWidget::ClickFocus );
	h1->addWidget( searchBtn );
	h1->addWidget( filterLabel );
	h1->addWidget( searchLE );
	vb->addLayout( h1 );

	channelsCb = new DListView( panel );
	channelsCb->setHScrollBarMode( QListView::AlwaysOff );
	//channelsCb->setPaletteBackgroundColor( QColor(255,255,200) );
	connect( channelsCb, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(channelNumberChanged(QListViewItem*)) );
	connect( channelsCb, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(channelSelected(QListViewItem*)) );
	connect( channelsCb, SIGNAL(clicked(QListViewItem*)), this, SLOT(channelClicked(QListViewItem*)) );
	connect( channelsCb, SIGNAL(contextMenuRequested(QListViewItem*,const QPoint&,int)), this, SLOT(channelPopup(QListViewItem*,const QPoint&,int)) );
	channelsCb->setItemsRenameable( true );
	channelsCb->addColumn( i18n("Number") );
	channelsCb->addColumn( i18n("Name") );
	channelsCb->addColumn( i18n("Source") );
	channelsCb->setAllColumnsShowFocus( true );
	channelsCb->setSizePolicy( QSizePolicy (QSizePolicy::Preferred, QSizePolicy::MinimumExpanding) );
	//channelsCb->setEnabled( false );
	vb->addWidget( channelsCb );

	h1 = new QHBoxLayout();
	recordBtn = new QToolButton( panel );
	QToolTip::add( recordBtn, i18n("Instant Record") );
	recordBtn->setToggleButton( true );
	h1->addWidget( recordBtn );
	broadcastBtn = new QToolButton( panel );
	QToolTip::add( broadcastBtn, i18n("Broadcast") );
	h1->addWidget( broadcastBtn );
	vb->addLayout( h1 );

	h1 = new QHBoxLayout();
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	shiftLed = new KLed( panel );
	shiftLed->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	shiftLed->setDarkFactor( 500 );
	shiftLed->off();
	QToolTip::add( shiftLed, i18n("Time shifting") );
	h1->addWidget( shiftLed );
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	recordLed = new KLed( panel );
	recordLed->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	recordLed->setColor( QColor( 255,0,0 ) );
	recordLed->setDarkFactor( 500 );
	recordLed->off();
	QToolTip::add( recordLed, i18n("Recording") );
	h1->addWidget( recordLed );
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	broadcastLed = new KLed( panel );
	broadcastLed->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
	broadcastLed->setColor( QColor( 255,128,0 ) );
	broadcastLed->setDarkFactor( 500 );
	broadcastLed->off();
	QToolTip::add( broadcastLed, i18n("Broadcasting") );
	h1->addWidget( broadcastLed );
	h1->addItem( new QSpacerItem( 1, 1, QSizePolicy::Preferred, QSizePolicy::Minimum ) );
	vb->addLayout( h1 );

	KIconLoader *icon = new KIconLoader();
	tvPix = icon->loadIcon( "kdvbtv", KIcon::Small );
	tvcPix = icon->loadIcon( "kdvbtvc", KIcon::Small );
	raPix = icon->loadIcon( "kdvbra", KIcon::Small );
	racPix = icon->loadIcon( "kdvbrac", KIcon::Small );
	QIconSet iconset;
	iconset.setPixmap( icon->loadIcon( "filesave", KIcon::Small ), QIconSet::Small );
	iconset.setPixmap( icon->loadIcon( "player_record", KIcon::Small), QIconSet::Small, QIconSet::Normal, QIconSet::On );
	recordBtn->setIconSet( iconset );
	broadcastBtn->setIconSet( icon->loadIconSet("network_local", KIcon::Small) );
	dateBtn->setIconSet( icon->loadIconSet("date", KIcon::Small) );
	infoBtn->setIconSet( icon->loadIconSet("view_text", KIcon::Small) );
	osdBtn->setIconSet( icon->loadIconSet("info", KIcon::Small) );
	channelsBtn->setIconSet( icon->loadIconSet("kdvbtv", KIcon::Small) );
	configBtn->setIconSet( icon->loadIconSet("configure", KIcon::Small) );
	recallBtn->setIconSet( icon->loadIconSet("reload", KIcon::Small) );
	searchBtn->setIconSet( icon->loadIconSet("locationbar_erase", KIcon::Small) );

	setXMLFile("kaffeinedvb.rc");
	setupActions();

	connect( this, SIGNAL(zap(ChannelDesc*)), SLOT(dvbZap(ChannelDesc*)) );
	connect( configBtn, SIGNAL(clicked()), this, SLOT(showConfigDialog()));
	connect( recallBtn, SIGNAL(clicked()), this, SLOT(recallZap()));

	connect( recordBtn, SIGNAL(clicked()), this, SLOT(setRecord()) );
	connect( broadcastBtn, SIGNAL(clicked()), this, SLOT(setBroadcast()) );
	connect( infoBtn, SIGNAL(clicked()), this, SLOT(showEvents()) );
	connect( channelsBtn, SIGNAL(clicked()), this, SLOT(scanner()) );
	connect( dateBtn, SIGNAL(clicked()), this, SLOT(showTimers()) );
	connect( osdBtn, SIGNAL(clicked()), this, SLOT(dvbOSD()));
	connect( searchBtn, SIGNAL(clicked()), this, SLOT(resetSearch()) );
	connect(searchLE, SIGNAL(textChanged(const QString&)), this, SLOT(searchChannel(const QString&)));

	connect( &timersTimer, SIGNAL(timeout()), this, SLOT(checkTimers()) );
	connect( &osdTimer, SIGNAL(timeout()), this, SLOT(resetOSD()) );
	connect( &showOsdTimer, SIGNAL(timeout()), this, SLOT(dvbOSD()) );
	connect( &tuningTimer, SIGNAL(timeout()), this, SLOT(setTuning()) );
	connect( &stopTuningTimer, SIGNAL(timeout()), this, SLOT(setTuning()) );
	connect( &diskTimer, SIGNAL(timeout()), this, SLOT(diskStatus()) );

	setConfig();

	updown = 1;
	autocount = 0;
	delete icon;

	events.loadEpg();
}



void DvbPanel::togglePanel()
{
	if ( panel->isHidden() )
		panel->show();
	else
		panel->hide();
}



void DvbPanel::diskStatus()
{
	double freemb;
	struct statvfs buf;

	if ( statvfs( dvbConfig->shiftDir.local8Bit(), &buf ) ) {
		fprintf(stderr,"Couldn't get file system statistics\n");
		return;
	}

	freemb = (double)(((double)(buf.f_bavail)*(double)(buf.f_bsize))/(1024.0*1024.0));
	if ( freemb<300 )
		emit showOSD( i18n("Warning: low disc space")+QString(" (%1").arg((int)freemb)+i18n("MB")+")", 5000, 3 );
}



void DvbPanel::channelPopup( QListViewItem *it, const QPoint &pos, int col )
{
	QPoint p=pos;

	if ( !it )
		return;

	int i=col, j;
	QImage img;
	QPixmap pix;
	QString name = it->text(1);
	QString s;

	QPopupMenu *pop = new QPopupMenu();
	pop->insertItem( i18n("Select icon..."), 1 );
	pop->insertItem( i18n("Edit..."), 2 );
	i = 0;
	i = pop->exec( QCursor::pos() );
	switch ( i ) {
		case 0 :
			break;
		case 1 :
			s = KFileDialog::getOpenFileName( QString::null, "image/png image/jpeg image/gif image/x-bmp image/x-xpm", 0, i18n("Choose channel icon"));
			if ( s.isEmpty() )
				break;
			img = QImage( s );
			if ( img.isNull() )
				break;
			if ( img.width()>img.height() )
				img = img.smoothScale( CHANICONSIZE, img.height()*CHANICONSIZE/img.width() );
			else
				img = img.smoothScale( img.width()*CHANICONSIZE/img.height(), CHANICONSIZE );
			pix.convertFromImage( img );
			for ( j=0; j<(int)channels.count(); j++ ) {
				if ( channels.at(j)->name==name ) {
					channels.at(j)->pix = pix;
					break;
				}
			}
			pix.save( dvbConfig->dvbConfigIconsDir+name, "PNG" );
			fillChannelList();
			break;
		case 2:
			editChannel( name );
			break;

	}
	delete pop;
}

bool DvbPanel::editChannel( QString &name )
{
	int j;
	ChannelDesc *chan=0;
	QStringList list;
	QPixmap pix;

	for ( j=0; j<(int)channels.count(); j++ ) {
		chan = channels.at(j);
		if ( chan->name==name ) {
			j = -1;
			list.append( chan->tp.source );
			break;
		}
	}
	if ( j==-1 ) {
		ChannelEditor dlg( list, false, chan, &channels, mainWidget );
		int ret = dlg.exec();
		if ( ret==ChannelEditor::Accepted ) {
			pix.load( dvbConfig->dvbConfigIconsDir+chan->name );
			if ( !pix.isNull() )
				chan->pix = pix;
			else {
				if ( chan->type==1 ) {
					if ( chan->fta )
						pix = tvcPix;
					else
						pix = tvPix;
				}
				else {
					if ( chan->fta )
						pix = racPix;
					else
						pix = raPix;
				}
			}
			fillChannelList( chan );
			return true;
		}
	}
	return false;
}




void DvbPanel::getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames )
{
	uiNames.append( i18n("Digital TV") );
	iconNames.append( "tv" );
	targetNames.append( "DVB" );
}



bool DvbPanel::execTarget( const QString &target )
{
	if ( target=="DVB" ) {
		if ( !channelsCb->isEnabled() ) {
			KMessageBox::information( mainWidget, i18n("Live digital TV only works with the xine engine.") );
			return true;
		}
		emit showMe( this );
		QTimer::singleShot( 100, this, SLOT(playLastChannel()) );
		return true;
	}
	return false;
}



void DvbPanel::setupActions()
{
	new KAction(i18n("OSD Next Channel"), "next", CTRL+SHIFT+Key_W, this, SLOT(dvbOSDNext()), actionCollection(), "dvb_browse_next");
	new KAction(i18n("OSD Previous Channel"), "previous", CTRL+SHIFT+Key_Q, this, SLOT(dvbOSDPrev()), actionCollection(),"dvb_browse_prev");
	new KAction(i18n("OSD Zap"), "ok", CTRL+SHIFT+Key_E, this, SLOT(dvbOSDZap()), actionCollection(), "dvb_browse_zap");
	new KAction(i18n("OSD Next Event"), "down", CTRL+SHIFT+Key_S, this, SLOT(dvbOSDAdvance()), actionCollection(), "dvb_browse_advance");
	new KAction(i18n("OSD Previous Event"), "up", CTRL+SHIFT+Key_A, this, SLOT(dvbOSDRetreat()), actionCollection(), "dvb_browse_retreat");
	new KAction(i18n("Instant Record"), "filesave", 0, this, SLOT(setRecord()), actionCollection(), "dvb_instant_record");
	new KAction(i18n("Recall"), "reload", CTRL+SHIFT+Key_Z, this, SLOT(recallZap()), actionCollection(), "dvb_recall");
	new KAction(i18n("Show OSD"), "info", Key_O, this, SLOT(dvbOSD()), actionCollection(), "dvb_show_osd");
	new KAction(i18n("EPG..."), "view_text", Key_G, this, SLOT(showEvents()), actionCollection(), "dvb_show_epg");
	new KAction(i18n("Timers..."), "date", Key_T, this, SLOT(showTimers()), actionCollection(), "dvb_show_timers");
	new KAction(i18n("Broadcasting..."), "network_local", Key_B, this, SLOT(setBroadcast()), actionCollection(), "dvb_show_broadcast");
	new KAction(i18n("Channels..."), "kdvbtv", Key_C, this, SLOT(scanner()), actionCollection(), "dvb_channels");
	new KAction(i18n("Configure DVB..."), "configure", CTRL|Key_C, this, SLOT(showConfigDialog()), actionCollection(), "dvb_config");
}



QWidget *DvbPanel::wantPlayerWindow()
{
	return playerBox;
}



QWidget *DvbPanel::inputMainWidget()
{
	return mainWidget;
}



void DvbPanel::searchChannel( const QString &text )
{
	int i, c=-1;
	QListViewItemIterator it( channelsCb );
	if ( !it.current() )
		return;
	for ( i=0; i<channelsCb->columns(); i++ ) {
		if ( channelsCb->columnText(i)==i18n("Name") ) {
			c = i;
			break;
		}
	}
	if ( c==-1 )
		return;

	channelsCb->visibleItems = 0;

	while ( it.current() ) {
		if ( text.isEmpty() || it.current()->text( c ).contains( text, false ) )	{
			it.current()->setVisible(true);
			++channelsCb->visibleItems;
		}
		else {
			it.current()->setVisible(false);
		}
		++it;
	}
}



void DvbPanel::resetSearch()
{
	searchLE->clear();
	searchChannel( "" );
}



void DvbPanel::channelNumberChanged( QListViewItem *it )
{
	int i, j;
	bool ok;
	unsigned int oldNum=0;
	unsigned int num = it->text(0).toUInt( &ok, 10 );

	for ( i=0; i<(int)channels.count(); i++ ) {
		if ( channels.at(i)->name==it->text(1) ) {
			oldNum = channels.at(i)->num;
			break;
		}
	}

	if ( num && oldNum ) {
		for ( j=0; j<(int)channels.count(); j++ ) {
			if ( channels.at(j)->num==num ) {
				channels.at(j)->num = oldNum;
				channels.at(i)->num = num;
				break;
			}
		}
	}

	if ((int)num > maxChannelNumber)
		maxChannelNumber = num;
	if ((int)num < minChannelNumber)
		minChannelNumber = num;

	fillChannelList();
}



void DvbPanel::catSelected( QIconViewItem *it )
{
	if ( !it ) return;

	if ( it->text()==i18n("All") )
		currentCategory = "All";
	else if ( it->text()==i18n("TV") )
		currentCategory = "TV";
	else if ( it->text()==i18n("Radio") )
		currentCategory = "Radio";
	else
		currentCategory = it->text();
	fillChannelList();
}



void DvbPanel::catContextMenu( QIconViewItem *it, const QPoint &p )
{
	bool ok;
	QString name, icon;
	int i=0, ret;

	QPopupMenu *pop = new QPopupMenu();
	if ( !it ) {
		pop->insertItem( i18n("New Category..."), 0 );
	}
	else {
		pop->insertItem( i18n("Change Icon..."), 1 );
		if ( it->text()!=i18n("All") && it->text()!=i18n("Radio") && it->text()!=i18n("TV") )
			pop->insertItem( i18n("Delete Category..."), 2 );
	}
	i = pop->exec( p );
	switch ( i ) {
		case 0 :
			name = KInputDialog::getText( i18n("New Category"), i18n("Enter a name for this category:"), QString::null, &ok);
			for ( QIconViewItem *qitem = iconView->firstItem(); qitem; qitem = qitem->nextItem() ) {
				if ( qitem->text()==name )
					ok = false;
			}
			if ( ok ) {
				KIconViewItem* item = new DIconViewItem(this, iconView, name, KGlobal::iconLoader()->loadIcon("kaffeine", KIcon::NoGroup, KIcon::SizeSmallMedium));
				item->setDropEnabled( true );
				dvbConfig->addCategory( name, "kaffeine" );
			}
			break;
		case 1 :
			icon = KIconDialog::getIcon();
			if ( !icon.isEmpty() ) {
				it->setPixmap( KGlobal::iconLoader()->loadIcon(icon, KIcon::NoGroup, KIcon::SizeSmallMedium) );
				dvbConfig->changeIconCategory( it->text(), icon );
			}
			break;
		case 2 :
			ret = KMessageBox::questionYesNo( mainWidget, i18n("Do you really want to delete this category?") );
			if ( ret==KMessageBox::Yes ) {
				dvbConfig->removeCategory( it->text() );
				delete it;
				currentCategory = "All";
				fillChannelList();
			}
			break;
	}
	delete pop;
}



void DvbPanel::moveChannel( const QString &cat, const QString &name )
{
	int i;
	ChannelDesc *chan;
	QListViewItem *it;

	for ( i=0; i<(int)channels.count(); i++ ) {
		chan = channels.at(i);
		if ( chan->name==name ) {
			if ( cat==i18n("All") )
				chan->category = "";
			else
				chan->category = cat;
			it = channelsCb->currentItem();
			if ( it && currentCategory!="All" && currentCategory!="TV" && currentCategory!="Radio" )
				delete it;
			break;
		}
	}
}



void DvbPanel::dumpEvents()
{
	EventSource *esrc;
	EventTsid *et;
	EventSid *es;
	QPtrList<EventDesc> *esEvents;
	EventDesc *desc;
	int i, k, j, m, n;
	QString s;

	QFile f( QDir::homeDirPath()+"/kaffeine_dvb_events.txt" );
	if ( f.open(IO_WriteOnly|IO_Truncate) ) {
		fprintf( stderr, "Creating events file.\n");
		QTextStream tt( &f );
		tt << "Saved: "+QDateTime::currentDateTime().toString( "dd-MM-yyyy hh:mm:ss" )+"\n";
		k= 0;
		for( i=0; i<events.getNSource(); i++ ) {
			if ( !(esrc=events.getNEventSource( i )) )
				continue;
			tt << esrc->getSource()+QString(" : %1 TS with events.\n").arg( esrc->getNTsid() );
			for ( m=0; m<esrc->getNTsid(); m++ ) {
				if ( !(et=esrc->getNEventTsid( m )) )
					continue;
				tt << QString("TSID %1 : %2 services with events\n").arg( et->getTsid() ).arg( et->getNSid() );
				for ( n=0; n<et->getNSid(); n++ ) {
					if ( !(es=et->getNEventSid( n )) )
						continue;
					tt << QString("     SID %1 : %2 events\n").arg( es->getSid() ).arg( es->getNDesc() );
					k+= es->getNDesc();
				}
			}
		}
		tt << "Number of events : "+s.setNum( k )+"\n";
		fprintf( stderr, "Number of events : %d\n", k );
		tt << "-------------------------\n";
		tt << "\n";
		for( i=0; i<events.getNSource(); i++ ) {
			if ( !(esrc=events.getNEventSource( i )) )
				continue;
			for ( m=0; m<esrc->getNTsid(); m++ ) {
				if ( !(et=esrc->getNEventTsid( m )) )
					continue;
				for ( n=0; n<et->getNSid(); n++ ) {
					if ( !(es=et->getNEventSid( n )) )
						continue;
					esEvents = es->getEvents();
					es->lock();
					for ( j=0; j<(int)esEvents->count(); j++ ) {
						if ( !(desc=esEvents->at( j )) )
							continue;
						tt << "Source: "+desc->source+"\n";
						tt << QString("Network ID: %1\n").arg(desc->nid);
						tt << QString("Transport Stream ID: %1\n").arg(desc->tsid);
						tt << QString("Service ID: %1\n").arg(desc->sid);
						tt << QString("Event ID: %1\n").arg(desc->eid);
						tt << "Title: "+desc->title+"\n";
						tt << "Subtitle: "+desc->subtitle+"\n";
						tt << desc->startDateTime.toString( "dd-MM-yyyy hh:mm:ss" )+"\n";
						tt << desc->duration.toString( "hh:mm:ss" )+"\n";
						s = "";
						for ( k=0; k<(int)desc->shortEvents.count(); k++ ) {
							s = s + desc->shortEvents.at(k)->name;
							if (!desc->shortEvents.at(k)->name.isEmpty() && !desc->shortEvents.at(k)->text.isEmpty()) {
								s = s + " : ";
								s = s + desc->shortEvents.at(k)->text;
								s = s+". ";
							}
						}
						for ( k=0; k<(int)desc->extEvents.count(); k++ )
							s = s+ *desc->extEvents.at(k);
						tt << s+"\n";
						tt << "\n";
					}
					es->unlock();
				}
			}
		}
		f.close();
	}
	else fprintf( stderr, "Can't create events file!\n");
}



void DvbPanel::channelClicked( QListViewItem *it )
{
	QPtrListIterator<ChannelDesc> iter( channels );
	ChannelDesc *itdesc, *desc=0;
	DvbStream *d = 0;

	if ( !it )
		return;

	iter.toFirst();
	while ( (itdesc=iter.current())!=0 ) {
		if ( itdesc->name==it->text(1) ) {
			desc = itdesc;
			break;
		}
		++iter;
	}

	if ( !desc )
		return;

	for (int j = 0; j < (int)dvb.count(); j ++) {
		if (dvb.at(j)->canSource(desc)) {
			d = dvb.at(j);
			break;
		}
	}

	osdMode = 0;
	dvbOSD( *desc, d );
}



void DvbPanel::resetOSD()
{
	osdMode = 0;
}



void DvbPanel::dvbOSD()
{
	DvbStream *d = 0;

	if (browseDvbStream != -1 && osdMode != 0) {
		dvbOSDSkip(0);
		return;
	}

	for (int i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			d = dvb.at(i);
			break;
		}
	}

	if ( d )
		dvbOSD( d->getLiveChannel(), d );
}



void DvbPanel::dvbOSDZap()
{
	ChannelDesc *chan = 0;

	if (browseDvbStream == -1)
		return;

	osdMode = 0;

	for (int i=0; i<(int)channels.count(); i++) {
		if ((int) channels.at(i)->num == browseDvbStream) {
			chan = channels.at(i);
			break;
		}
	}

	if (!chan)
		return;

	dvbZap(chan);
}



void DvbPanel::dvbOSDNext()
{
	browseDvbTimeShift = -1;
	osdMode = 0;

	dvbOSDSkip(1);
}



void DvbPanel::dvbOSDSkip(int skip, int timeShift /* = 0 */)
{
	DvbStream *d = 0;
	ChannelDesc *chan = 0;

	int mychan = -1;

	if (browseDvbStream == -1)
		mychan = dvbConfig->lastChannel;
	else
		mychan = browseDvbStream + skip;

	if (mychan < minChannelNumber)
		mychan = maxChannelNumber;
	else if (mychan > maxChannelNumber)
		mychan = minChannelNumber;

	for (int i = 0; i < (int)channels.count(); i ++) {
		if ((int) channels.at(i)->num == mychan) {
			chan = channels.at(i);
			break;
		}
	}

	if (!chan)
		return;

	for (int j = 0; j < (int)dvb.count(); j ++) {
		if (dvb.at(j)->canSource(chan)) {
			d = dvb.at(j);
			break;
		}
	}

	if (!d)
		return;

	browseDvbStream = chan->num;

	dvbOSD(*chan, d, timeShift);
}



void DvbPanel::dvbOSDPrev()
{
	browseDvbTimeShift = -1;
	osdMode = 0;

	dvbOSDSkip(-1);
}



void DvbPanel::dvbOSDHide()
{
	browseDvbStream = -1;
	browseDvbTimeShift = -1;
}



void DvbPanel::dvbOSDAdvance()
{
	osdMode = 0;

	dvbOSDSkip(0, 1);
}



void DvbPanel::dvbOSDRetreat()
{
	osdMode = 0;

	dvbOSDSkip(0, -1);
}



void DvbPanel::dvbOSD(ChannelDesc liveChannel, DvbStream *d, int timeShift /* = 0 */)
{
	QStringList list;
	EventSource *esrc;
	EventSid *es;
	QPtrList<EventDesc> *esEvents;
	EventDesc *desc;
	QString s;
	int i, j, k;
	int first=0;

	if ( !d || liveChannel.name.isEmpty() )
		return;

	if ( osdMode==2 ) {
		osdTimer.stop();
		osdMode = 0;
		list.append( "STOP" );
		emit showDvbOSD( s.setNum( liveChannel.num )+" - "+liveChannel.name, list );
		return;
	}

	if ( osdMode==1 )
		osdTimer.stop();

	if ( osdMode==0 ) {
		if ( d->liveIsRecording() )
			list.append( "R" );
		if ( d->timeShiftMode() )
			list.append( "T" );
	}
	else
		list.append( "E" );

	int myshift = 0;

	if (browseDvbTimeShift == -1)
		myshift = 0;
	else
		myshift = browseDvbTimeShift + timeShift;

	if (myshift < 0)
		myshift = 0;

	browseDvbTimeShift = myshift;

	k = 0;

	esrc = events.getEventSource( liveChannel.tp.source );
	es = esrc->getEventSid( liveChannel.tp.nid, liveChannel.tp.tsid, liveChannel.sid );
	if ( !es )
		goto end;
	es->lock();
	esEvents = es->getEvents();

	for ( j=myshift; j<(myshift+2); j++ ) {
		desc = esEvents->at( j );
		if ( !desc )
			continue;

		if ( !desc->title.isEmpty() ) {
			s = desc->startDateTime.toString( "hh:mm" );
			s = s+" - ";
			s = s+desc->title;
			if ( !osdMode && !first && !myshift ) {
				QDateTime dt = QDateTime::currentDateTime();
				int secs = desc->startDateTime.secsTo( dt );
				i = QTime().secsTo( desc->duration );
				if ( i!=0 )
					i = secs*100/i;
				if ( i>100 )
					i = 100;
				list.append( QString("BAR")+QString().setNum( i ) );
				++first;
			}
			list.append( s );
			k++;
		}
		else if ( osdMode )
			list.append( "" );
		if ( osdMode==0 ) {
			if ( k>1 )
				break;
			else
				continue;
		}
		list.append( desc->subtitle );
		s = "";
		for ( i=0; i<(int)desc->shortEvents.count(); i++ ) {
			s = s + desc->shortEvents.at(i)->name;
			if (!desc->shortEvents.at(i)->name.isEmpty() && !desc->shortEvents.at(i)->text.isEmpty()) {
				s+= " : ";
				s+= desc->shortEvents.at(i)->text;
				s+= ". ";
			}
		}
		for ( i=0; i<(int)desc->extEvents.count(); i++ )
			s+= *desc->extEvents.at(i);
		list.append( s );
		break;
	}
	es->unlock();

end:
	if ( osdMode==0 )
		osdTimer.start( 5000, true );
	osdMode++;
	emit showDvbOSD( s.setNum( liveChannel.num )+" - "+liveChannel.name, list );
}



void DvbPanel::enableLiveDvb( bool on )
{
	channelsCb->setEnabled( on );
}



void DvbPanel::checkFirstRun()
{
	if ( dvbConfig->firstRun() )
		showConfigDialog();
}



void DvbPanel::setConfig()
{
	int i;
	int error = 0;
	KIconViewItem* item = NULL;
	DvbStream *d;

	QString s = locateLocal("appdata", "");
	dvbConfig = new DVBconfig( s );
	item = new DIconViewItem(this, iconView, i18n("All"), KGlobal::iconLoader()->loadIcon(dvbConfig->allIcon, KIcon::NoGroup, KIcon::SizeSmallMedium));
	iconView->setFixedHeight( item->height()+iconView->horizontalScrollBar()->height() );
	item->setDropEnabled( true );
	item = new DIconViewItem(this, iconView, i18n("Radio"), KGlobal::iconLoader()->loadIcon(dvbConfig->radioIcon, KIcon::NoGroup, KIcon::SizeSmallMedium));
	item->setDropEnabled( false );
	item = new DIconViewItem(this, iconView, i18n("TV"), KGlobal::iconLoader()->loadIcon(dvbConfig->tvIcon, KIcon::NoGroup, KIcon::SizeSmallMedium));
	item->setDropEnabled( false );

	KTrader::OfferList offers = KTrader::self()->query("KaffeineDvbPlugin");
	KTrader::OfferList::Iterator it = offers.begin();
	if ( it!=offers.end() ) {
		KService::Ptr ptr = (*it);
		plugName = ptr->desktopEntryName();
		plug = KParts::ComponentFactory::createPartInstanceFromService<KaffeineDvbPlugin>(ptr, 0, ptr->name().ascii(), 0, 0, 0, &error);
		if (error > 0) {
			fprintf( stderr, "Loading of DVB plugin failed: %s\n", KLibLoader::self()->lastErrorMessage().ascii() );
			plug = NULL;
		}
		else {
			fprintf( stderr, "DVB plugin loaded\n.");
		}
	}

	for ( i=0; i<(int)dvbConfig->categories.count(); i++ ) {
		item = new DIconViewItem(this, iconView, dvbConfig->categories.at(i)->name, KGlobal::iconLoader()->loadIcon(dvbConfig->categories.at(i)->icon, KIcon::NoGroup, KIcon::SizeSmallMedium));
		item->setDropEnabled( true );
	}

	for ( i=0; i<(int)dvbConfig->devList.count(); i++ ) {
		d = new DvbStream( dvbConfig->devList.at(i), dvbConfig->defaultCharset, &events );
		d->setPlug( plug );
		dvb.append( d );
		connect( d, SIGNAL(shifting(bool)), this, SLOT(setShiftLed(bool)) );
		connect( d, SIGNAL(isBroadcasting(bool)), this, SLOT(setBroadcastLed(bool)) );
		connect( d, SIGNAL(timerEnded(RecTimer*)), this, SLOT(killTimer(RecTimer*)) );
		connect( d, SIGNAL(isRecording(bool)), this, SLOT(setRecordLed(bool)) );
		connect( d, SIGNAL(playDvb()), this, SLOT(pipeOpened()) );
	}
	fifoName = QDir::homeDirPath()+"/.kaxtv.ts";
	QFile f( fifoName );
	if ( f.exists() )
		f.remove();
	if ( (mkfifo( fifoName.ascii(), 0644 ))<0 ) {
		perror( fifoName.latin1() );
		fifoName = "";
	}
	fifoName1 = QDir::homeDirPath()+"/.kaxtv1.ts";
	QFile f1( fifoName1 );
	if ( f1.exists() )
		f1.remove();
	if ( (mkfifo( fifoName1.ascii(), 0644 ))<0 ) {
		perror( fifoName1.latin1() );
		fifoName1 = "";
	}
	currentFifo = fifoName;
	getTimerList();
	timersTimer.start( 5000 );
	getChannelList();
	rtp = new Ts2Rtp();
	rtp->setSocket( dvbConfig->broadcastAddress, dvbConfig->broadcastPort, dvbConfig->senderPort );
	cleaner = new Cleaner( dvbConfig->shiftDir, dvbConfig->recordDir );
	split->setSizes( dvbConfig->splitSizes );
	recallChannel = dvbConfig->lastChannel;
}



void DvbPanel::showConfigDialog()
{
	int i, ret;

loop:
	if ( !dvbConfig->haveData() ) {
		ret = KMessageBox::questionYesNo( mainWidget, i18n("<qt>Can't get DVB data from http://hftom.free.fr/kaxtv/dvbdata.tar.gz!<br>\
			Check your internet connection, and say Yes to try again.<br>\
			Or say No to cancel.<br>\
			If you already have this archive, copy it to ~/.kde/share/apps/kaffeine/dvbdata.tar.gz and say Yes.<br><br>Should I try again?</qt>") );
		if ( ret==KMessageBox::Yes )
			goto loop;
		return;
	}

	for ( i=0; i<(int)dvb.count(); i++ )
		dvb.at(i)->probeCam();
	DvbConfigDialog dlg( this, dvbConfig, mainWidget, plug );
	connect( dlg.dumpBtn, SIGNAL(clicked()), this, SLOT(dumpEvents()) );
	ret = dlg.exec();
	disconnect( dlg.dumpBtn, SIGNAL(clicked()), this, SLOT(dumpEvents()) );
	if ( ret==DvbConfigDialog::Rejected )
		return;
	rtp->setSocket( dvbConfig->broadcastAddress, dvbConfig->broadcastPort, dvbConfig->senderPort );
	cleaner->setPaths( dvbConfig->shiftDir, dvbConfig->recordDir );
	fillChannelList();
}



void DvbPanel::camClicked( int devNum )
{
	dvb.at(devNum)->showCamDialog();
}



QPtrList<Transponder> DvbPanel::getSourcesStatus()
{
	int i, j;
	QStringList list;
	QPtrList<Transponder> ss;
	Transponder t;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( !dvb.at(i)->getPriority() ) // priority==0==don't use
			continue;
		list = dvb.at(i)->getSources();
		for ( j=0; j<(int)list.count(); j++ ) {
			if ( dvb.at(i)->hasRec() || dvb.at(i)->hasBroadcast() )
				t =  dvb.at(i)->getCurrentTransponder();
			else {
				t = Transponder();
				t.source = list[j];
			}
			ss.append( new Transponder( t ) );
		}
	}
	return ss;
}



void DvbPanel::fillChannelList( ChannelDesc *ch )
{
	int i, j;
	ChannelDesc *chan;
	KListViewItem *it, *visible=0;
	bool cont=false;
	QPtrList<Transponder> trans = getSourcesStatus();
	trans.setAutoDelete( true );

	searchLE->clear();
	channelsCb->clear();
	for ( i=0; i<(int)channels.count(); i++ ) {
		chan = channels.at(i);
		for ( j=0; j<(int)trans.count(); j++ ) {
			cont = false;
			if ( trans.at(j)->source==chan->tp.source ) {
				if ( trans.at(j)->freq ) {
					if ( chan->tp==*trans.at(j) ) break;
				}
				else
					break;
			}
			cont = true;
		}
		if ( cont )
			continue;
		if ( currentCategory=="TV" ) {
			if ( chan->type!=1 )
				continue;
		}
		else if ( currentCategory=="Radio" ) {
			if ( chan->type!=2 )
				continue;
		}
		else if ( currentCategory!="All" && chan->category!=currentCategory )
			continue;
		it = new KListViewItem( channelsCb, QString().sprintf("%05d", chan->num), chan->name, chan->tp.source );
		if ( ch && ch==chan )
			visible = it;
		it->setDragEnabled( true );
		chan->pix.load( dvbConfig->dvbConfigIconsDir+chan->name );
		if ( !chan->pix.isNull() )
			it->setPixmap( 1, chan->pix );
		else {
			if ( chan->type==1 ) {
				if ( chan->fta )
					it->setPixmap( 1, tvcPix );
				else
					it->setPixmap( 1, tvPix );
			}
			else {
				if ( chan->fta )
					it->setPixmap( 1, racPix );
				else
					it->setPixmap( 1, raPix );
			}
		}
	}
	channelsCb->visibleItems = channelsCb->childCount();
	trans.clear();
	if ( visible ) {
		channelsCb->ensureItemVisible( visible );
		channelsCb->setSelected( visible, true );
	}
}



DvbStream* DvbPanel::getWorkingDvb( int mode, ChannelDesc *chan )
{
	int i, ret;
	QValueList<int> working; //  free=0, hasLive=100, hasBroadcast=200, hasRec=300, can'tDoChannel=400

	for ( i=0; i<(int)dvb.count(); i++ )
		working.append( 100-dvb.at(i)->getPriority() );

	// fill in working status
	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( !dvb.at(i)->canSource( chan ) || working[i]==100 ) {
			working[i] = MODE_CANTDO;
			continue;
		}
		if ( dvb.at(i)->isTuned() ) {
			if ( dvb.at(i)->getCurrentTransponder()==chan->tp ) {
				return dvb.at(i); // use that one since it's already tuned on the good mplex
			}
			else if ( dvb.at(i)->hasRec() )
				working[i] += MODE_RECORD;
			else if ( dvb.at(i)->hasBroadcast() )
				working[i] += MODE_BROADCAST;
			else {
				if ( mode==MODE_LIVE )
					working[i] += MODE_FREE;
				else
					working[i] += MODE_LIVE;
			}
		}
		else
			working[i] += MODE_FREE;
	}
	ret = 0;
	// search for least working card
	for ( i=1; i<(int)working.count(); i++ ) {
		if ( working[i]<working[0] ) {
			working[0] = working[i];
			ret = i;
		}
	}
	if ( working[0]<mode )
		return dvb.at(ret);
	else
		return 0;
}



void DvbPanel::setBroadcast()
{
	int ret, i;
	QPtrList<ChannelDesc> list;
	bool live=false;
	DvbStream *d;

	BroadcastEditor dlg( mainWidget, &channels, &list );
	ret = dlg.exec();
	if ( ret==BroadcastEditor::Rejected )
		return;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasBroadcast() )
			dvb.at(i)->stopBroadcast();
	}
	if ( ! list.count() ) {
		return;
	}

	d = getWorkingDvb( MODE_BROADCAST, list.at(0) );

	if ( d )
		ret = d->canStartBroadcast( live, list.at(0) );
	else
		ret = -1;
	if ( ret==0 ) {
		if ( live ) {
			stopLive();
			emit dvbStop();
		}
		if ( !d->startBroadcast( &list, rtp ) ) {
			KMessageBox::information( mainWidget, i18n("Broadcasting failed.") );
		}
	}
	else
		KMessageBox::information( mainWidget, i18n("Can't start broadcasting.") );
}



void DvbPanel::checkTimers()
{
	int i, j, ret;
	bool live=false;
	RecTimer *t;
	ChannelDesc *chan;
	QDateTime cur=QDateTime::currentDateTime();
	DvbStream *d;

	for ( i=0; i<(int)timers.count(); i++ ) {
		t = timers.at(i);
		if ( t->running )
			continue;
		if ( t->begin<=cur && cur<t->begin.addSecs(QTime().secsTo(t->duration)) ) {
			chan = 0;
			for ( j=0; j<(int)channels.count(); j++ ) {
				if ( channels.at(j)->name==t->channel ) {
					chan = channels.at(j);
					break;
				}
			}
			if ( !chan )
				continue;
			d = getWorkingDvb( MODE_RECORD, chan );
			live = false;
			if ( d )
				ret = d->canStartTimer( live, chan );
			else
				ret = -1;
			if ( ret==0 ) {
				if ( live ) {
					stopLive();
					emit dvbStop();
				}
				if ( d->startTimer( chan, dvbConfig->recordDir, dvbConfig->sizeFile, t, dvbConfig->filenameFormat ) ) {
					KProcess proc;
					proc << QDir::homeDirPath()+"/bin/kaffeine_recording";
					proc << "On";
					proc.start( KProcess::DontCare );
					t->running = 1;
					if ( timersDialog )
						emit timersChanged();
					saveTimerList();
					i--;
				}
				else
					fprintf( stderr, "start timer failed!!!\n" );
			}
			else
				fprintf( stderr, "Cant start timer !!!\n" );
		}
		else if ( t->mode ) {
			if (t->begin.addSecs(t->duration.hour()*3600+t->duration.minute()*60+60)<cur) {
				updateModeTimer( t );
				if ( timersDialog )
					emit timersChanged();
			}
		}
	}
}



void DvbPanel::setRecord()
{
	ChannelDesc curchan;
	QString s="";
	int ret, i, r=0;
	bool live=false;
	RecTimer *t, *rt;
	EventDesc *desc;
	DvbStream *d=0;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			d = dvb.at(i);
			curchan = d->getLiveChannel();
			break;
		}
	}
	if ( !d ) {
		recordBtn->setOn( false );
		showTimers();
		return;
	}

	desc = events.getEventDesc( curchan.tp.source, curchan.tp.nid, curchan.tp.tsid, curchan.sid, 0 );

	if ( desc )
		s = desc->title;

	if ( s.isEmpty() ) {
		if ( !dvbConfig->filenameFormat.contains("%chan") )
			s = curchan.name;
		if ( !dvbConfig->filenameFormat.contains("%date") )
			s+="_"+QDateTime::currentDateTime().toString( "yyyyMMdd-hhmmss" );
	}

	rt = new RecTimer();
	rt->name = s;
	rt->channel = curchan.name;
	rt->begin = QDateTime::currentDateTime();
	rt->duration = QTime( 0,0,0).addSecs( dvbConfig->instantDuration*60 ) ;
	rt->running = 1;
	rt->mode = 0;

	ret = d->canStartTimer( live, &curchan );
	if ( ret==0 ) {
		if ( d->startTimer( &curchan, dvbConfig->recordDir, dvbConfig->sizeFile, rt, dvbConfig->filenameFormat ) ) {
			KProcess proc;
			proc << QDir::homeDirPath()+"/bin/kaffeine_recording";
			proc << "On";
			proc.start( KProcess::DontCare );

			for ( i=0; i<(int)timers.count(); i++ ) {
				t = timers.at(i);
				if ( rt->begin>t->begin )
					r=i+1;
			}
			timers.insert( r, rt );
			if ( timersDialog )
				emit timersChanged();
			saveTimerList();
			emit showOSD( i18n("Instant Record successfully started"), 5000, 3 );
		}
		else {
			KMessageBox::information( mainWidget, i18n("Instant Recording failed to start.") );
			fprintf( stderr, "DvbPanel::setRecord:start timer failed!\n" );
			delete rt;
		}
	}
	else {
		delete rt;
		for ( i=0; i<(int)timers.count(); i++ ) {
			t = timers.at(i);
			if ( d->liveIsRecording() && t->running && curchan.name == t->channel ) {
				d->updateTimer( t, 0 );
				emit showOSD( i18n("Recording successfully stopped"), 5000, 3 );
				return;
			}
		}
		showTimers();
	}
}



void DvbPanel::setRecordLed( bool on )
{
	int i;
	bool rec=false;

	if ( on ) {
		recordLed->on();
		fillChannelList();
	}
	else {
		for ( i=0; i<(int)dvb.count(); i++ ) {
			if ( dvb.at(i)->hasRec() ) {
				rec = true;
				break;
			}
		}
		if ( !rec )
			recordLed->off();
		else
			recordLed->on();
		fillChannelList();
	}

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->liveIsRecording() ) {
			recordBtn->setOn( true );
			return;
		}
	}
	recordBtn->setOn( false );
}


void DvbPanel::setBroadcastLed( bool on )
{
	int i;
	bool broad=false;

	if ( on ) {
		broadcastLed->on();
		fillChannelList();
	}
	else {
		for ( i=0; i<(int)dvb.count(); i++ ) {
			if ( dvb.at(i)->hasBroadcast() ) {
				broad = true;
				break;
			}
		}
		if ( !broad )
			broadcastLed->off();
		else
			broadcastLed->on();
		fillChannelList();
	}
}



void DvbPanel::killTimer( RecTimer *t )
{
	int i;

	for ( i=0; i<(int)timers.count(); i++ ) {
		if ( timers.at(i)==t ) {
			KProcess proc;
			proc << QDir::homeDirPath()+"/bin/kaffeine_recording";
			if ( recordLed->state()==KLed::On )
				proc << "On";
			else
				proc << "Off";
			proc << KProcess::quote( t->fullPath );
			proc.start( KProcess::DontCare );

			if ( t->mode )
				updateModeTimer( t );
			else
				timers.remove( t );
			if ( timersDialog )
				emit timersChanged();
			return;
		}
	}
}



void DvbPanel::updateModeTimer( RecTimer *t )
{
	int stop=0, r=0, i;

	if ( t->mode==CronTimer::Daily )
		t->begin = t->begin.addDays(1);
	else if ( t->mode==CronTimer::Weekly )
		t->begin = t->begin.addDays(7);
	else if ( t->mode==CronTimer::Monthly )
		t->begin = t->begin.addMonths(1);
	else while ( !stop ) {
		t->begin = t->begin.addDays(1);
		if ( (t->begin.date().dayOfWeek()==1) && (t->mode&CronTimer::Monday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==2) && (t->mode&CronTimer::Tuesday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==3) && (t->mode&CronTimer::Wednesday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==4) && (t->mode&CronTimer::Thursday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==5) && (t->mode&CronTimer::Friday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==6) && (t->mode&CronTimer::Saturday) ) stop++;
		else if ( (t->begin.date().dayOfWeek()==7) && (t->mode&CronTimer::Sunday) ) stop++;
	}

	RecTimer *nt = new RecTimer();
	nt->name = t->name;
	nt->channel = t->channel;
	nt->begin = t->begin;
	nt->duration = t->duration;
	nt->running = 0;
	nt->mode = t->mode;
	timers.remove( t );
	for ( i=0; i<(int)timers.count(); i++ ) {
		t = timers.at(i);
		if ( nt->begin>t->begin )
			r=i+1;
	}
	timers.insert( r, nt );
}



void DvbPanel::showTimers()
{
	int i;
	QStringList list;
	QMap<QString,QString> map;

	if ( channels.count()==0 ) {
		KMessageBox::sorry( 0, i18n("You may want to define some channel first!") );
		return;
	}
	for ( QPtrListIterator<ChannelDesc> it(channels); it.current(); ++it )
		map[it.current()->name] = it.current()->name;
	QMap<QString,QString>::Iterator it;
	QMap<QString,QString>::Iterator end(map.end());
        for ( it = map.begin(); it != end; ++it )
        	list.append( it.data() );

	timersDialog = new KRecord( list, &timers, mainWidget, dvbConfig->timerSize );
	for ( i=0; i<(int)dvb.count(); i++ ) {
		connect( timersDialog, SIGNAL(updateTimer(RecTimer*,int)), dvb.at(i), SLOT(updateTimer(RecTimer*,int)) );
	}
	connect( this, SIGNAL(timersChanged()), timersDialog, SLOT(refresh()) );
	timersDialog->exec();
	for ( i=0; i<(int)dvb.count(); i++ ) {
		disconnect( timersDialog, SIGNAL(updateTimer(RecTimer*,int)), dvb.at(i), SLOT(updateTimer(RecTimer*,int)) );
	}
	disconnect( this, SIGNAL(timersChanged()), timersDialog, SLOT(refresh()) );
	dvbConfig->timerSize = timersDialog->size();
	delete timersDialog;
	timersDialog = 0;
	saveTimerList();
}



int DvbPanel::getSNR( int device )
{
	if ( device<0 || device>=(int)dvb.count() )
		return -1;
	return dvb.at(device)->getSNR();
}


void DvbPanel::dvbNewTimer( QString name, QString channel, QString datetime, QString duration )
{
	newTimer( channel, name, QDateTime::fromString( datetime, Qt::ISODate ), QTime::fromString( duration ), false );
}



void DvbPanel::newTimer( QString channel, QString name, QDateTime begin, QTime duration, bool warn )
{
	int i, r=0;
	RecTimer *t;
	RecTimer *rt = new RecTimer();

	rt->name = name.replace("/","_").replace(">","_").replace("<","_").replace(":","_").replace('"',"_").replace("\\","_").replace("|","_");
	rt->channel = channel;
	rt->begin = begin.addSecs( -(dvbConfig->beginMargin*60) );
	rt->duration = duration.addSecs( (dvbConfig->beginMargin+dvbConfig->endMargin)*60 ) ;
	rt->running = 0;
	rt->mode = 0;

	for ( i=0; i<(int)timers.count(); i++ ) {
		t = timers.at(i);
		if ( rt->begin>t->begin )
			r=i+1;
	}
	timers.insert( r, rt );
	if ( warn )
		KMessageBox::information( 0, i18n("Timer successfully created.") );
}



void DvbPanel::scanner()
{
	int ret;

loop:
	if ( !dvbConfig->haveData() ) {
		ret = KMessageBox::questionYesNo( mainWidget, i18n("<qt>Can't get DVB data from http://hftom.free.fr/kaxtv/dvbdata.tar.gz!<br>\
			Check your internet connection, and say Yes to try again.<br>\
			Or say No to cancel.<br>\
			If you already have this archive, copy it to ~/.kde/share/apps/kaffeine/dvbdata.tar.gz and say Yes.<br><br>Should I try again?</qt>") );
		if ( ret==KMessageBox::Yes )
			goto loop;
		return;
	}

	if ( !dvbConfig->haveData() )
		return;
	ScanDialog dlg( &dvb, &channels, dvbConfig->scanSize, dvbConfig->dvbConfigDir, dvbConfig->defaultCharset );
	dlg.exec();
	dvbConfig->scanSize = dlg.size();
	fillChannelList();
	saveChannelList();
}



void DvbPanel::showEvents()
{
	events.doClean( false );
	KEvents dlg( &channels, &dvb, &events, mainWidget, dvbConfig->epgSize );
	connect( &dlg, SIGNAL(addTimer(QString,QString,QDateTime,QTime)), this, SLOT(newTimer(QString,QString,QDateTime,QTime)) );
	connect( &dlg, SIGNAL(zapTo(const QString &)), this, SLOT(channelSelected(const QString &)) );
	dlg.exec();
	dvbConfig->epgSize = dlg.size();
	disconnect( &dlg, SIGNAL(addTimer(QString,QString,QDateTime,QTime)), this, SLOT(newTimer(QString,QString,QDateTime,QTime)) );
	disconnect( &dlg, SIGNAL(zapTo(const QString &)), this, SLOT(channelSelected(const QString &)) );
	events.doClean( true );
}



void DvbPanel::setShiftLed( bool on )
{
	if ( on ) {
		shiftLed->on();
		diskTimer.start( 300000 );
	}
	else {
		shiftLed->off();
		diskTimer.stop();
	}
}



void DvbPanel::channelSelected( const QString &name )
{
	QPtrListIterator<ChannelDesc> iter( channels );
	ChannelDesc *itdesc;

	iter.toFirst();
	while ( (itdesc=iter.current())!=0 ) {
		if ( itdesc->name==name ) {
			dvbZap( itdesc );
			break;
		}
		++iter;
	}
}



void DvbPanel::channelSelected( QListViewItem *it )
{
	QPtrListIterator<ChannelDesc> iter( channels );
	ChannelDesc *itdesc;

	if ( !it )
		return;

	iter.toFirst();
	while ( (itdesc=iter.current())!=0 ) {
		if ( itdesc->name==it->text(1) ) {
			dvbZap( itdesc );
			break;
		}
		++iter;
	}
}



bool DvbPanel::nextTrack( MRL& )
{
	next();
	return false;
}



bool DvbPanel::previousTrack( MRL& )
{
	previous();
	return false;
}



bool DvbPanel::currentTrack( MRL& )
{
	playLastChannel();
	return false;
}



bool DvbPanel::trackNumber( int num, MRL& )
{
	playNumChannel( num );
	return false;
}



void DvbPanel::recallZap()
{
	emit showMe( this );
	playNumChannel( recallChannel );
}



void DvbPanel::playNumChannel( int num )
{
	int j;
	bool ok=false;

	if ( !channelsCb->isEnabled() )
		return;

	for ( j=0; j<(int)channels.count(); j++ ) {
		if ( (int)channels.at(j)->num==num ) {
			ok = true;
			break;
		}
	}

	if ( ok )
		dvbZap( channels.at( j ) );
}



bool DvbPanel::playbackFinished( MRL& )
{
	return false;
}



void DvbPanel::playLastChannel()
{
	/*int ret, i, j;
	QPtrList<ChannelDesc> list;
	bool live=false, ok=false;
	DvbStream *d;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasBroadcast() )
			dvb.at(i)->stopBroadcast();
	}
	++dvbConfig->lastChannel;
	if ( dvbConfig->lastChannel>channels.count() )
		dvbConfig->lastChannel = 1;
	for ( j=0; j<(int)channels.count(); j++ ) {
		if ( (int)channels.at(j)->num==dvbConfig->lastChannel ) {
			ok = true;
			list.append( channels.at(j) );
			break;
		}
	}
	if ( ! list.count() ) {
		QTimer::singleShot( 2000, this, SLOT(playLastChannel()) );
		return;
	}
	d = getWorkingDvb( MODE_BROADCAST, list.at(0) );
	if ( d )
		ret = d->canStartBroadcast( live, list.at(0) );
	else
		ret = -1;
	if ( ret==0 ) {
		if ( live ) {
			stopLive();
			emit dvbStop();
		}
		if ( !d->startBroadcast( &list, rtp ) ) {
			fprintf( stderr, "Broadcasting failed.\n" );
		}
		else {
			fprintf( stderr, "Tuning to: %s / autocount: %lu\n", channels.at(j)->name.ascii(), autocount );
			++autocount;
		}
	}
	else
		fprintf( stderr, "Can't start broadcasting.\n");

	QTimer::singleShot( 2000, this, SLOT(playLastChannel()) );
	return;*/




	int j;
	bool ok=false;

	if ( !channelsCb->isEnabled() )
		return;

	for ( j=0; j<(int)channels.count(); j++ ) {
		if ( (int)channels.at(j)->num==dvbConfig->lastChannel ) {
			ok = true;
			break;
		}
	}
	if ( !ok )
		return;

	dvbZap( channels.at(j) );
}



void DvbPanel::next()
{
	if ( !channelsCb->isEnabled() )
		return;

	QListViewItem* nextItem;

	QListViewItem* playingItem = channelsCb->findItem( QString().sprintf("%05d", dvbConfig->lastChannel), 0 );

	if ( !playingItem == 0 ) // yes, it's in the current category
	{
		if ( playingItem == channelsCb->lastItem() )
			nextItem = channelsCb->firstChild();  // wrap around
		else
			nextItem = playingItem->itemBelow();
	}
	else	// not in current category, user has switched category, use first in current cat
		 nextItem = channelsCb->firstChild();

	channelsCb->setSelected( nextItem, true );
	channelSelected( nextItem );

}



void DvbPanel::previous()
{
	if ( !channelsCb->isEnabled() )
		return;

	QListViewItem* prevItem;

	QListViewItem* playingItem = channelsCb->findItem( QString().sprintf("%05d", dvbConfig->lastChannel), 0 );

	if ( !playingItem == 0 ) // yes, it's in the current category
	{
		if ( playingItem == channelsCb->firstChild() )
			prevItem = channelsCb->lastItem();  // wrap around
		else
			prevItem = playingItem->itemAbove();
	}
	else	// not in current category, user has switched category, use first in current cat
		 prevItem = channelsCb->firstChild();

	channelsCb->setSelected( prevItem, true );
	channelSelected( prevItem );
}



void DvbPanel::dvbZap( ChannelDesc *chan )
{
	int i;
	DvbStream *d;

	if ( currentFifo.isEmpty() || isTuning )
		return;

	isTuning = true;
	emit setTimeShiftFilename( "" );
	d = getWorkingDvb( MODE_LIVE, chan );
	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			dvb.at(i)->preStopLive();
			emit dvbPause( false );
			dvb.at(i)->stopLive( chan );
			break;
		}
	}
	finalZap( d, chan );
}



void DvbPanel::pipeOpened()
{
	tuningTimer.start( 1000, true );
}



void DvbPanel::setTuning()
{
	tuningTimer.stop();
	stopTuningTimer.stop();
	isTuning = false;
}



void DvbPanel::finalZap( DvbStream *d, ChannelDesc *chan )
{
	QString s, t;
	int i;

	emit setCurrentPlugin( this );

	fprintf( stderr, "Tuning to: %s / autocount: %lu\n", chan->name.latin1(), autocount );
	QTime tm;
	tm.start();

	if ( !d ) {
		emit dvbStop();
		isTuning = false;
		return;
	}

	int ret = d->goLive( chan, currentFifo, dvbConfig->ringBufSize );

	switch ( ret ) {
		case DvbStream::ErrIsRecording :
			emit showOSD( i18n("Still recording."), 5000, 3 );
			break;
		case DvbStream::ErrIsBroadcasting :
			emit showOSD( i18n("Still broadcasting."), 5000, 3 );
			break;
		case DvbStream::ErrCantTune :
			emit showOSD( i18n("Can't tune dvb!"), 5000, 3 );
			break;
		case DvbStream::ErrCantSetPids :
			emit showOSD( i18n("Can't set pid(s)"), 5000, 3 );
			break;
		case DvbStream::ErrCamUsed :
			emit showOSD( i18n("No CAM free"), 5000, 3 );
			break;
	}

	fprintf( stderr, "Tuning delay: %d ms\n", tm.elapsed() );
	osdMode = 0;
	if ( ret<1 ) {
		emit dvbOpen( currentFifo, s.setNum( chan->num)+" - "+chan->name, chan->vpid );
		showOsdTimer.start( 2000, true );
		++autocount;
		stopTuningTimer.start( 5000, true );
		if ( currentFifo==fifoName ) {
			if ( !fifoName1.isEmpty() )
				currentFifo = fifoName1;
		}
		else {
			if ( !fifoName.isEmpty() )
				currentFifo = fifoName;
		}
		if ( channelsCb->visibleItems<5 ) {
			resetSearch();
		}

		if ( d->liveIsRecording() )
			recordBtn->setOn( true );
		else
			recordBtn->setOn( false );
	}
	else {
		recordBtn->setOn( false );
		isTuning = false;
		emit dvbStop();
	}
	if ( dvbConfig->lastChannel!=(int)chan->num ) {
		recallChannel = dvbConfig->lastChannel;
		dvbConfig->lastChannel = chan->num;
	}
	//QTimer::singleShot( 3000, this, SLOT(playLastChannel()) );
}



void DvbPanel::pauseLiveTV()
{
	int i;
	DvbStream *d=0;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			d = dvb.at(i);
			break;
		}
	}
	if ( !d )
		return;

	timeShiftFileName = dvbConfig->shiftDir+"DVBLive-"+QDateTime::currentDateTime().toString("yyyyMMddThhmmss")+".m2t";
	if ( d->doPause( timeShiftFileName ) )
		emit setTimeShiftFilename( timeShiftFileName );
}



bool DvbPanel::timeShiftMode()
{
	int i;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			return dvb.at(i)->timeShiftMode();
			break;
		}
	}
	return false;
}



void DvbPanel::stopLive()
{
	int i;
	ChannelDesc chan;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasLive() ) {
			recallChannel = dvb.at(i)->getLiveChannel().num;
			dvb.at(i)->preStopLive();
			dvb.at(i)->stopLive( &chan );
			break;
		}
	}
	recordBtn->setOn( false );
	emit setTimeShiftFilename( "" );
}


//   TV | vpid(stream_type) | apid1(lang)(ac3),apid2, | ttpid | sid | tsid | type(S,C,T)source | freq | sr | pol | fecH | inv | mod | fecL | bw | trans | guard | hier | number / subpid1(type)(pageid)(ancid)(lang),subpid2..., | category | nid |
bool DvbPanel::getChannelList()
{
	bool ret=false;
	QString s, c, t, u, type;
	int pos, tpos;
	ChannelDesc *chan;
	QString src="";
	int ns;
	KListViewItem *it;
	QPixmap pix;

	maxChannelNumber = 0;
	minChannelNumber = -1;

	int sort = dvbConfig->readDvbChanOrder();
	if ( sort ) {
		int column = sort&1;
		int order = sort>>1;
		switch (order){
			case 1:
				channelsCb->setSorting ( column, FALSE );
				break;
			case 0:
				channelsCb->setSorting ( column, TRUE );
				break;
		}
	}

	QFile f( locateLocal("appdata", "channels.dvb" ) );
	if ( f.open(IO_ReadOnly) ) {
		QTextStream tt( &f );
		while ( !tt.eof() ) {
			s = tt.readLine();
			if ( s.startsWith("#") ) {
				if ( s.contains("KaxTV") )
					break;
				continue;
			}
			chan = new ChannelDesc();
			pos = s.find("|");
			c = s.left( pos );
			if ( c=="TV" || c=="TVC" )
				chan->type=1;
			else
				chan->type=2;
			if ( c=="TVC" || c=="RAC" )
				chan->fta=1;
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->name = s.left( pos );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			c = s.left( pos );
			s = s.right( s.length()-pos-1 );
			tpos = c.find("(");
			if ( tpos>0 )
				chan->vpid = c.left(tpos).toUShort();
			else
				chan->vpid = c.toUShort();
			if( tpos>0 ) {
				t = c.right( c.length()-tpos-1);
				t.remove(")");
				chan->vType = t.toUShort();
			}
			else
				chan->vType = 2;
			if ( !chan->vpid )
				chan->vType = 0;
			pos = s.find("|");
			c = s.left( pos );
			s = s.right( s.length()-pos-1 );
			while ( (pos=c.find(","))!=-1 ) {
				t = c.left(pos);
				chan->napid++;
				if ( t.contains("(ac3)") ) {
					chan->apid[chan->napid-1].ac3=1;
					t.remove("(ac3)");
				}
				if( (tpos=t.find("("))!=-1 ) {
					t.remove(")");
					chan->apid[chan->napid-1].lang=t.right( t.length()-tpos-1 );
					t = t.left( tpos );
				}
				chan->apid[chan->napid-1].pid=t.toUShort();
				c = c.right( c.length()-pos-1 );
			}
			pos = s.find("|");
			chan->ttpid = s.left(pos).toUShort();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->sid = s.left(pos).toUShort();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->tp.tsid = s.left(pos).toUShort();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			c = s.left(pos);
			if ( c.startsWith("T") ) {
				chan->tp.type=FE_OFDM;
				chan->tp.source = "Terrestrial";
			}
			else if ( c.startsWith("C") ) {
				chan->tp.type=FE_QAM;
				chan->tp.source = "Cable";
			}
			else if ( c.startsWith("S") ) {
				chan->tp.type=FE_QPSK;
				c.remove( 0, 1 );
				chan->tp.source = c;
			}
			else {
				chan->tp.type=FE_ATSC;
				chan->tp.source = "Atsc";
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->tp.freq = s.left(pos).toULong();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->tp.sr = s.left(pos).toULong();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			c = s.left( pos );
			chan->tp.pol = c[0].latin1();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 0 : chan->tp.coderateH = FEC_NONE; break;
				case 12 : chan->tp.coderateH = FEC_1_2; break;
				case 23 : chan->tp.coderateH = FEC_2_3; break;
				case 34 : chan->tp.coderateH = FEC_3_4; break;
				case 45 : chan->tp.coderateH = FEC_4_5; break;
				case 56 : chan->tp.coderateH = FEC_5_6; break;
				case 67 : chan->tp.coderateH = FEC_6_7; break;
				case 78 : chan->tp.coderateH = FEC_7_8; break;
				case 89 : chan->tp.coderateH = FEC_8_9; break;
				case 35 : chan->tp.coderateH = FEC_3_5; break;
				case 910 : chan->tp.coderateH = FEC_9_10; break;
				case -1 : chan->tp.coderateH = FEC_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 0 : chan->tp.inversion = INVERSION_OFF; break;
				case 1 : chan->tp.inversion = INVERSION_ON; break;
				case -1 : chan->tp.inversion = INVERSION_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 8 : chan->tp.modulation = QPSK; break;
				case 16 : chan->tp.modulation = QAM_16; break;
				case 32 : chan->tp.modulation = QAM_32; break;
				case 64 : chan->tp.modulation = QAM_64; break;
				case 128 : chan->tp.modulation = QAM_128; break;
				case 256 : chan->tp.modulation = QAM_256; break;
				case 108 : chan->tp.modulation = VSB_8; break;
				case 116 : chan->tp.modulation = VSB_16; break;
				case 1000 : chan->tp.modulation = PSK_8; break;
				case 1001 : chan->tp.modulation = APSK_16; break;
				case 1003 : chan->tp.modulation = DQPSK; break;
				case -1 : chan->tp.modulation = QAM_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 0 : chan->tp.coderateL = FEC_NONE; break;
				case 12 : chan->tp.coderateL = FEC_1_2; break;
				case 23 : chan->tp.coderateL = FEC_2_3; break;
				case 34 : chan->tp.coderateL = FEC_3_4; break;
				case 45 : chan->tp.coderateL = FEC_4_5; break;
				case 56 : chan->tp.coderateL = FEC_5_6; break;
				case 67 : chan->tp.coderateL = FEC_6_7; break;
				case 78 : chan->tp.coderateL = FEC_7_8; break;
				case 89 : chan->tp.coderateL = FEC_8_9; break;
				case 35 : chan->tp.coderateH = FEC_3_5; break;
				case 910 : chan->tp.coderateH = FEC_9_10; break;
				case -1 : chan->tp.coderateL = FEC_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 8 : chan->tp.bandwidth = BANDWIDTH_8_MHZ; break;
				case 7 : chan->tp.bandwidth = BANDWIDTH_7_MHZ; break;
				case 6 : chan->tp.bandwidth = BANDWIDTH_6_MHZ; break;
				case -1 : chan->tp.bandwidth = BANDWIDTH_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 2 : chan->tp.transmission = TRANSMISSION_MODE_2K; break;
				case 8 : chan->tp.transmission = TRANSMISSION_MODE_8K; break;
				case -1 : chan->tp.transmission = TRANSMISSION_MODE_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 32 : chan->tp.guard = GUARD_INTERVAL_1_32; break;
				case 16 : chan->tp.guard = GUARD_INTERVAL_1_16; break;
				case 8 : chan->tp.guard = GUARD_INTERVAL_1_8; break;
				case 4 : chan->tp.guard = GUARD_INTERVAL_1_4; break;
				case -1 : chan->tp.guard = GUARD_INTERVAL_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 0 : chan->tp.hierarchy = HIERARCHY_NONE; break;
				case 1 : chan->tp.hierarchy = HIERARCHY_1; break;
				case 2 : chan->tp.hierarchy = HIERARCHY_2; break;
				case 4 : chan->tp.hierarchy = HIERARCHY_4; break;
				case -1 : chan->tp.hierarchy = HIERARCHY_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->num = s.left(pos).toUInt();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			c = s.left( pos );
			s = s.right( s.length()-pos-1 );
			while ( (pos=c.find(","))!=-1 ) {
				t = c.left(pos);
				tpos=t.find("(");
				ns = (int)chan->nsubpid;
				chan->subpid[ns].pid = t.left(tpos).toUShort();
				t = t.right( t.length()-tpos-1 );
				tpos=t.find(")");
				chan->subpid[ns].type = t.left(tpos).toUShort();
				t = t.right( t.length()-tpos-2 );
				tpos=t.find(")");
				chan->subpid[ns].page = t.left(tpos).toUShort();
				t = t.right( t.length()-tpos-2 );
				tpos=t.find(")");
				chan->subpid[ns].id = t.left(tpos).toUShort();
				t = t.right( t.length()-tpos-2 );
				tpos=t.find(")");
				chan->subpid[ns].lang = t.left(tpos);
				c = c.right( c.length()-pos-1 );
				chan->nsubpid++;
			}
			pos = s.find("|");
			chan->category = s.left( pos );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->tp.nid = s.left(pos).toUShort();
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			switch ( s.left(pos).toInt() ) {
				case 20 : chan->tp.rolloff = ROLLOFF_20; break;
				case 25 : chan->tp.rolloff = ROLLOFF_25; break;
				case 35 : chan->tp.rolloff = ROLLOFF_35; break;
				case -1 : chan->tp.rolloff = ROLLOFF_AUTO;
			}
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			chan->tp.S2 = s.left(pos).toInt();

			if ( chan->tp.source.isEmpty() ) {
				delete chan;
				continue;
			}

			chan->pix.load( dvbConfig->dvbConfigIconsDir+chan->name );
			it = new KListViewItem( channelsCb, QString().sprintf("%05d", chan->num), chan->name, chan->tp.source );
			it->setDragEnabled( true );
			if ( !chan->pix.isNull() )
				it->setPixmap( 1, chan->pix );
			else {
				if ( chan->type==1 ) {
					if ( chan->fta )
						it->setPixmap( 1, tvcPix );
					else
						it->setPixmap( 1, tvPix );
				}
				else {
					if ( chan->fta )
						it->setPixmap( 1, racPix );
					else
						it->setPixmap( 1, raPix );
				}
			}
			channels.append( chan );
			if ( chan->num<=0 )
				chan->num = channels.count();

			if ( (int)chan->num > maxChannelNumber )
				maxChannelNumber = chan->num;
			if ( minChannelNumber == -1 || (int)chan->num < minChannelNumber )
				minChannelNumber = chan->num;

		}
		ret = true;
		f.close();
	}
	return ret;
}



bool DvbPanel::saveChannelList()
{
	bool ret=false;
	int i, k;
	QString s;
	ChannelDesc *chan=0;

	QFile f( locateLocal("appdata", "channels.dvb" ) );
	if ( f.open(IO_WriteOnly|IO_Truncate) ) {
		QTextStream tt( &f );
		tt<<"#Generated by Kaffeine 0.5\n";
		for( i=0; i<(int)channels.count(); i++ ) {
			chan = channels.at(i);
			if ( chan->type==1 ) {
				if ( chan->fta )
					tt<<"TVC|";
				else
					tt<<"TV|";
			}
			else {
				if ( chan->fta )
					tt<<"RAC|";
				else
					tt<<"RA|";
			}
			tt<< chan->name+"|";
			tt<< s.setNum(chan->vpid);
			if ( chan->vType ) {
				tt<< "(";
				tt<< s.setNum(chan->vType);
				tt<< ")";
			}
			tt<< "|";
			for ( k=0; k<chan->napid; k++ ) {
				tt<< s.setNum(chan->apid[k].pid);
				if ( !chan->apid[k].lang.isEmpty() )
					tt<< "("+chan->apid[k].lang+")";
				if ( chan->apid[k].ac3 )
					tt<< "(ac3)";
				tt<< ",";
			}
			tt<< "|";
			tt<< s.setNum(chan->ttpid)+"|";
			tt<< s.setNum(chan->sid)+"|";
			tt<< s.setNum(chan->tp.tsid)+"|";
			switch ( chan->tp.type ) {
				case FE_QPSK : tt<< "S"; tt<< chan->tp.source; break;
				case FE_QAM : tt<< "Cable"; break;
				case FE_OFDM : tt<< "Terrestrial"; break;
				case FE_ATSC : tt<< "Atsc"; break;
			}
			tt<< "|";
			tt<< s.setNum(chan->tp.freq)+"|";
			tt<< s.setNum(chan->tp.sr)+"|";
			if ( chan->tp.pol=='h'  )
				tt<< "h|";
			else
				tt<< "v|";
			switch ( chan->tp.coderateH ) {
				case FEC_NONE : tt<< "0|"; break;
				case FEC_1_2 : tt<< "12|"; break;
				case FEC_2_3 : tt<< "23|"; break;
				case FEC_3_4 : tt<< "34|"; break;
				case FEC_4_5 : tt<< "45|"; break;
				case FEC_5_6 : tt<< "56|"; break;
				case FEC_6_7 : tt<< "67|"; break;
				case FEC_7_8 : tt<< "78|"; break;
				case FEC_8_9 : tt<< "89|"; break;
				case FEC_3_5 : tt<< "35|"; break;
				case FEC_9_10 : tt<< "910|"; break;
				case FEC_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.inversion ) {
				case INVERSION_OFF : tt<< "0|"; break;
				case INVERSION_ON : tt<< "1|"; break;
				case INVERSION_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.modulation ) {
				case QPSK : tt<< "8|"; break;
				case QAM_16 : tt<< "16|"; break;
				case QAM_32 : tt<< "32|"; break;
				case QAM_64 : tt<< "64|"; break;
				case QAM_128 : tt<< "128|"; break;
				case QAM_256 : tt<< "256|"; break;
				case VSB_8 : tt<< "108|"; break;
				case VSB_16 : tt<< "116|"; break;
				case PSK_8 : tt<< "1000|"; break;
				case APSK_16 : tt<< "1001|"; break;
				case DQPSK : tt<< "1003|"; break;
				case QAM_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.coderateL ) {
				case FEC_NONE : tt<< "0|"; break;
				case FEC_1_2 : tt<< "12|"; break;
				case FEC_2_3 : tt<< "23|"; break;
				case FEC_3_4 : tt<< "34|"; break;
				case FEC_4_5 : tt<< "45|"; break;
				case FEC_5_6 : tt<< "56|"; break;
				case FEC_6_7 : tt<< "67|"; break;
				case FEC_7_8 : tt<< "78|"; break;
				case FEC_8_9 : tt<< "89|"; break;
				case FEC_3_5 : tt<< "35|"; break;
				case FEC_9_10 : tt<< "910|"; break;
				case FEC_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.bandwidth ) {
				case BANDWIDTH_8_MHZ : tt<< "8|"; break;
				case BANDWIDTH_7_MHZ : tt<< "7|"; break;
				case BANDWIDTH_6_MHZ : tt<< "6|"; break;
				case BANDWIDTH_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.transmission ) {
				case TRANSMISSION_MODE_8K : tt<< "8|"; break;
				case TRANSMISSION_MODE_2K : tt<< "2|"; break;
				case TRANSMISSION_MODE_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.guard ) {
				case GUARD_INTERVAL_1_32 : tt<< "32|"; break;
				case GUARD_INTERVAL_1_16 : tt<< "16|"; break;
				case GUARD_INTERVAL_1_8 : tt<< "8|"; break;
				case GUARD_INTERVAL_1_4 : tt<< "4|"; break;
				case GUARD_INTERVAL_AUTO : tt<< "-1|";
			}
			switch ( chan->tp.hierarchy ) {
				case HIERARCHY_NONE : tt<< "0|"; break;
				case HIERARCHY_1 : tt<< "1|"; break;
				case HIERARCHY_2 : tt<< "2|"; break;
				case HIERARCHY_4 : tt<< "4|"; break;
				case HIERARCHY_AUTO : tt<< "-1|";
			}
			tt<< s.setNum(chan->num)+"|";
			for ( k=0; k<chan->nsubpid; k++ ) {
				tt<< s.setNum(chan->subpid[k].pid);
				tt<< "("+s.setNum(chan->subpid[k].type)+")";
				tt<< "("+s.setNum(chan->subpid[k].page)+")";
				tt<< "("+s.setNum(chan->subpid[k].id)+")";
				tt<< "("+chan->subpid[k].lang+")";
				tt<< ",";
			}
			tt<< "|";
			tt<< chan->category+"|";
			tt<< s.setNum(chan->tp.nid)+"|";
			switch ( chan->tp.rolloff ) {
				case ROLLOFF_20 : tt<< "20|"; break;
				case ROLLOFF_25 : tt<< "25|"; break;
				case ROLLOFF_35 : tt<< "35|"; break;
				case ROLLOFF_AUTO : tt<< "-1|";
			}
			tt<< s.setNum(chan->tp.S2)+"|";
			tt<< "\n";
		}
		ret = true;
		f.close();
	}
	return ret;
}



bool DvbPanel::getTimerList()
{
	bool ret=false;
	QString s;
	int pos;
	RecTimer *t;

	QFile f( locateLocal("appdata", "timers.dvb" ) );
	if ( f.open(IO_ReadOnly) ) {
		QTextStream tt( &f );
		while ( !tt.eof() ) {
			s = tt.readLine();
			if ( s.startsWith("#") )
				continue;
			t = new RecTimer();
			pos = s.find("|");
			t->name = s.left( pos );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			t->channel = s.left( pos );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			t->begin = QDateTime::fromString( s.left( pos ), Qt::ISODate );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			t->duration = QTime::fromString( s.left( pos ) );
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			//t->filetype = s.left( pos ).toInt();
			t->mode = 0;
			s = s.right( s.length()-pos-1 );
			pos = s.find("|");
			t->mode = s.left( pos ).toInt();
			t->running = 0;
			timers.append( t );
		}
		ret = true;
		f.close();
	}
	return ret;
}



bool DvbPanel::saveTimerList()
{
	bool ret=false;
	int i;
	QString s;
	RecTimer *t;

	QFile f( locateLocal("appdata", "timers.dvb" ) );
	if ( f.open(IO_WriteOnly|IO_Truncate) ) {
		QTextStream tt( &f );
		tt<<"#Generated by Kaffeine 0.5\n";
		for( i=0; i<(int)timers.count(); i++ ) {
			t = timers.at(i);
			if ( t->running && !t->mode )
				continue;
			tt<< t->name+"|";
			tt<< t->channel+"|";
			tt<< t->begin.toString("yyyy-MM-ddThh:mm:ss")+"|";
			tt<< t->duration.toString()+"|";
			tt<< s.setNum(0/*t->filetype*/)+"|";
			tt<< s.setNum(t->mode)+"|";
			tt<< "\n";
		}
		ret = true;
		f.close();
	}
	return ret;
}



bool DvbPanel::close()
{
	int ret=0, i;
	bool rec = false;

	for ( i=0; i<(int)dvb.count(); i++ ) {
		if ( dvb.at(i)->hasRec() ) {
			rec = true;
			break;
		}
	}

	if ( rec ) {
		ret = KMessageBox::questionYesNo( 0, i18n("Kaffeine is still recording. Do you really want to quit?") );
		if ( ret!=KMessageBox::Yes )
			return false;
	}
	else if ( timers.count() ) {
		ret = KMessageBox::questionYesNo( 0, i18n("Kaffeine has queued timers. Do you really want to quit?") );
		if ( ret!=KMessageBox::Yes )
			return false;
	}
	emit dvbStop();
	stopLive();
	return true;
}



void DvbPanel::saveConfig()
{
	saveTimerList();
	saveChannelList();
	dvbConfig->splitSizes = split->sizes();
	dvbConfig->saveDvbChanOrder( channelsCb->sortOrder(), channelsCb->sortColumn() );
	dvbConfig->saveConfig();
	//delete dvbConfig;
}


DvbPanel::~DvbPanel()
{
	dvb.clear();
	delete rtp;
	delete cleaner;
	if ( plug ) {
		KService::Ptr service = KService::serviceByDesktopName( plugName );
		KLibLoader::self()->unloadLibrary( service->library().ascii() );
	}
	events.saveEpg();
}

#include "dvbpanel.moc"
