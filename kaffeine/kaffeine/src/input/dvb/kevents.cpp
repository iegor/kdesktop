/*
 * kevents.cpp
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

#include <qlayout.h>
#include <qfile.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qtooltip.h>

#include <klocale.h>
#include <kiconloader.h>

#include "kevents.h"
#include "channeldesc.h"
#include "dvbevents.h"
#include "dvbstream.h"



EListViewItem::EListViewItem( QListView *parent, QString chanName, QString eBegin, QString eDuration, QString eTitle, EventDesc *desc ) : KListViewItem( parent, chanName, eBegin, eDuration, eTitle )
{
	event = desc;
}



int EListViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
	EListViewItem *ei = (EListViewItem*)i;

	if ( col != 1 )
		return QListViewItem::compare( i, col, ascending );

	if ( event->startDateTime<ei->event->startDateTime )
		return -1;
	if ( event->startDateTime==ei->event->startDateTime )
		return 0;
	return 1;
}



KEvents::KEvents( QPtrList<ChannelDesc> *chans, QPtrList<DvbStream> *d, EventTable *t, QWidget *parent, QSize size ) : QDialog( parent )
{
	dvb = d;
	events = t;
	channels = chans;

	setCaption( i18n("Electronic Program Guide") );

	QGridLayout *grid = new QGridLayout( this, 1, 1, 11, 6, "grid");
	QHBoxLayout *hbox = new QHBoxLayout( 0, 0, 6, "hbox" );
	QVBoxLayout *vbox = new QVBoxLayout( 0, 0, 6 , "vbox");

	resetBtn = new KPushButton( i18n("Refresh"), this );
	hbox->addWidget( resetBtn );
	allBtn = new KPushButton( i18n("Scheduled"), this );
	hbox->addWidget( allBtn );
	currentNextBtn = new KPushButton( i18n("Current/Next"), this );
	hbox->addWidget( currentNextBtn );
	currentChannelEpgBtn = new KPushButton( i18n("Current Channel"), this );
	hbox->addWidget( currentChannelEpgBtn );

	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	hbox->addItem( spacer );

	vbox->addLayout( hbox);

	hbox = new QHBoxLayout();

	searchBtn = new QToolButton( this );
	searchBtn->setAutoRaise( true );
	QToolTip::add( searchBtn, i18n("Electronic Program Guide Search"));
	hbox->addWidget( searchBtn );

	QLabel* filterLabel = new QLabel( i18n("Search") + ":", this );
	hbox->addWidget( filterLabel );

	searchLineEdit = new KLineEdit(this);
	hbox->addWidget( searchLineEdit );

	tvradioCb  = new QCheckBox(i18n("TV "), this );
	QToolTip::add( tvradioCb, i18n("Search TV Channels only (omit Radio)"));
	tvradioCb->setChecked(true);
	hbox->addWidget( tvradioCb);

	titleCb  = new QCheckBox(i18n("Titles "), this );
	QToolTip::add( titleCb, i18n("Search Event Titles only (omit Description)"));
	titleCb->setChecked(true);
	hbox->addWidget( titleCb);

	ftaCb  = new QCheckBox(i18n("FTA "), this );
	QToolTip::add( ftaCb, i18n("Search Free to Air Channels only (omit PayTV)"));
	ftaCb->setChecked(false);
	hbox->addWidget( ftaCb);

	vbox->addLayout( hbox );

	grid->addLayout( vbox, 0, 0 );

	listView = new KListView( this, "listView" );
	listView->addColumn( i18n( "Channel" ) );
	listView->addColumn( i18n( "Begin" ) );
	listView->addColumn( i18n( "Duration" ) );
	listView->addColumn( i18n( "Title" ) );
	listView->setResizePolicy( KListView::AutoOneFit );
	listView->setAllColumnsShowFocus( TRUE );
	listView->setFullWidth( TRUE );
	grid->addWidget( listView, 1, 0 );
	grid->setRowStretch( 1, 4 );

	textBrow = new QTextBrowser( this );
	grid->addWidget( textBrow, 2, 0 );
	grid->setRowStretch( 2, 2 );

	KIconLoader *icon = new KIconLoader();
	resetBtn->setGuiItem( KGuiItem(i18n("Refresh"), icon->loadIconSet("reload", KIcon::Small) ) );
	allBtn->setGuiItem( KGuiItem(i18n("Scheduled"), icon->loadIconSet("date", KIcon::Small) ) );
	currentNextBtn->setGuiItem( KGuiItem(i18n("Current/Next"), icon->loadIconSet("toggle_log", KIcon::Small) ) );
	currentChannelEpgBtn->setGuiItem( KGuiItem(i18n("Current Channel"), icon->loadIconSet("date", KIcon::Small) ) );
	searchBtn->setIconSet( icon->loadIconSet("locationbar_erase", KIcon::Small) );

	new EListViewItem( (QListView*)listView, "Une chaine", "99/99/99  99:99 9999", "99:99 99", "un titre de programme", 0 );

	resize( size );
	connect( resetBtn			, SIGNAL( pressed() ), this, SLOT( reset() ) );
	connect( allBtn				, SIGNAL( clicked() ), this, SLOT( setScheduled() ) );
	connect( currentNextBtn		, SIGNAL( clicked() ), this, SLOT( setCurrentNext() ) );
	connect( currentChannelEpgBtn, SIGNAL( clicked() ), this, SLOT( setCurrentChannelEpg() ) );
	connect( searchBtn, 		  SIGNAL( clicked() ), this,	 SLOT(resetSearch()) );
	connect( searchLineEdit, SIGNAL( returnPressed() ), this, SLOT( epgSearch() ) );

	connect( listView, SIGNAL( mouseButtonClicked(int,QListViewItem*,const QPoint&,int) ),
		this, SLOT(mouseClickedSlot(int,QListViewItem*,const QPoint&,int)) );

	connect( listView, SIGNAL(doubleClicked(QListViewItem*,const QPoint &,int)),
		this, SLOT(zap(QListViewItem*,const QPoint &,int)) );

	setMode( 1 );
        delete icon;
}

void KEvents::resetSearch()
{
	searchLineEdit->clear();
}

void KEvents::checkEpgSearch(QString searchword)
{
	int i, j, k, m, n, l;
	EventSource *esrc;
	EventTsid *et;
	EventSid *es;
	EventDesc *desc;
	EListViewItem *itt=0;
	ChannelDesc *ch;
	QString s, begin, duration, title;
	bool found;

	for( k=0; k<events->getNSource(); k++ ) {
		if ( !(esrc=events->getNEventSource( k )) )
			continue;
		for ( m=0; m<esrc->getNTsid(); m++ ) {
			if ( !(et=esrc->getNEventTsid( m )) )
				continue;
			for ( n=0; n<et->getNSid(); n++ ) {
				if ( !(es=et->getNEventSid( n )) )
					continue;
				for ( j=0; j<es->getNDesc(); j++ ) {
					if ( !(desc=es->getEventDesc( j )) )
						continue;

					if ( desc->title.isEmpty() )
						continue;

					for ( i=0; i<(int)channels->count(); i++ ) {
						ch = channels->at(i);
						if ( desc->source==ch->tp.source && desc->nid==ch->tp.nid && desc->sid==ch->sid && desc->tsid==ch->tp.tsid ) {
							if(tvradioCb->isChecked() && ch->type==2)
								break;

							if(ftaCb->isChecked() && ch->fta==1)
								break;

							found=false;
							if(desc->title.upper().find(searchword.upper())!=-1)
								found=true;

							if(!desc->subtitle.isEmpty()) {
								if(desc->subtitle.upper().find(searchword.upper())!=-1)
									found=true;
							}

							if(!titleCb->isChecked()) {
								if(!found) {
									for ( l=0; l<(int)desc->extEvents.count(); l++ ) {
										s = *desc->extEvents.at(l);
										if(!s.isEmpty()) {
											if(s.upper().find(searchword.upper())!=-1) {
												found=true;
												s="";
												break;
											}
										}
									}
								}
								if(!found) {
									for ( l=0; l<(int)desc->shortEvents.count(); l++ ) {
										s = desc->shortEvents.at(l)->name;
										if(!s.isEmpty()) {
											if(s.upper().find(searchword.upper())!=-1) {
												found=true;
												s="";
												break;
											}
										}
										s = desc->shortEvents.at(l)->text;
										if(!s.isEmpty()) {
											if(s.upper().find(searchword.upper())!=-1) {
												found=true;
												s="";
												break;
											}
										}
									}
								}
							}
							if(!found)
								break;

							begin = KGlobal::locale()->formatDateTime( desc->startDateTime );
							duration = desc->duration.toString("hh:mm");
							title = desc->title;

							itt = new EListViewItem( (QListView*)listView, ch->name, begin, duration, title, desc );

							if ( !ch->pix.isNull() )
								itt->setPixmap( 0, ch->pix );
						}
					}
				}
			}
		}
	}
}

void KEvents::epgSearch()
{

	QString text = searchLineEdit->text();
	if (text.stripWhiteSpace().isEmpty()) return;

	listView->clear();
	checkEpgSearch(text);
}

void KEvents::setCurrentChannelEpg()
{
	ChannelDesc curchan;
	int i;
	for ( i=0; i<(int)dvb->count(); i++ ) {
		if ( dvb->at(i)->hasLive() ) {
			curchan = dvb->at(i)->getLiveChannel();
			break;
		}
	}
	if ( !curchan.name.isEmpty() )
			setMode( 2, curchan.name );

}

void KEvents::zap( QListViewItem* it, const QPoint &p, int col )
{
	QPoint pt=p;
	int c=col;
	c++;

	if ( it->text(0) != QString::null )
		emit zapTo( it->text(0) );
}



void KEvents::mouseClickedSlot( int btn, QListViewItem *it, const QPoint &p, int c )
{
	int i=c;
	QPoint pt=p;
	QString s;

	if ( !it )
		return;
	EListViewItem *ei = (EListViewItem*)it;

	switch ( btn ) {
		case Qt::RightButton : {
			QPopupMenu *pop = new QPopupMenu();
			pop->insertItem( i18n("View All Programs"), 1 );
			pop->insertSeparator();
			pop->insertItem( i18n("Add to Timers"), 2 );
			if ( ei->event->running==4 )
				pop->setItemEnabled( 2, false );
			i = 0;
			i = pop->exec( QCursor::pos() );
			switch ( i ) {
				case 0 :
					break;
				case 1 :
					setMode( 2, it->text(0) );
					break;
				case 2 :
					emit addTimer( it->text(0), it->text(3), ei->event->startDateTime, ei->event->duration );
					break;
			}
			delete pop;
			break;
		}
		case Qt::LeftButton : {
			s = "<qt><h3><font color=\"darkblue\">";
			s = s+it->text(0)+"</font></h3>";
			if ( !ei->event->title.isEmpty() ) {
				s = s+"<b><font color=\"darkgreen\"><big>";
				s = s+ei->event->title;
				s = s+"</big></font></b><br>";
			}
			if ( !ei->event->subtitle.isEmpty() ) {
				s = s+"<b><font color=\"#A69631\">";
				s = s+ei->event->subtitle;
				s = s+"</font></b><br>";
			}
			s = s+"<br><font color=\"darkred\">";
			s = s+KGlobal::locale()->formatDateTime( ei->event->startDateTime, false )+"<br>";
			s = s+ei->event->duration.toString("hh:mm");
			s = s+"</font><br><br>";
			for ( i=0; i<(int)ei->event->shortEvents.count(); i++ ) {
				s = s+"<p>";
				s = s + ei->event->shortEvents.at(i)->name;
				if (!ei->event->shortEvents.at(i)->name.isEmpty() && !ei->event->shortEvents.at(i)->text.isEmpty()) {
					s = s + " : ";
				}
				s = s + ei->event->shortEvents.at(i)->text;
				s = s + "</p>";
			}
			for ( i=0; i<(int)ei->event->extEvents.count(); i++ ) {
				s = s+ *ei->event->extEvents.at(i);
			}
			s = s+"</qt>";
			textBrow->setText( s );
			break;
		}
	}
}



void KEvents::setCurrentNext()
{
	setMode( 1 );
}



void KEvents::setScheduled()
{
	QListViewItem *it = listView->currentItem();
	if ( !it )
		return;
	setMode( 2, it->text(0) );
}



void KEvents::setMode( int m, QString name )
{
	int i;

	mode = m;
	if ( !name.isEmpty() ) {
		currentNextBtn->show();
		allBtn->hide();
		for ( i=0; i<(int)channels->count(); i++ ) {
			if ( channels->at(i)->name==name ) {
				chan = channels->at(i);
				break;
			}
		}
	}
	else {
		chan = 0;
		currentNextBtn->hide();
  		allBtn->show();
	}
	reset();
}



void KEvents::reset()
{
	listView->clear();
	checkNewEvent();
}



void KEvents::checkNewEvent()
{
	int i;
	EventSource *esrc;
	EventSid *es;
	EventDesc *desc;
	EListViewItem *itt=0;
	ChannelDesc *ch;
	QString s, t, channel, begin, duration, title;

	if ( mode==2 ) {
		if ( !chan )
			return;
		if ( !(esrc=events->getEventSource( chan->tp.source )) )
			return;
		if ( !(es=esrc->getEventSid( chan->tp.nid, chan->tp.tsid, chan->sid )) )
			return;
		i = 0;
		while ( (desc=es->getEventDesc(i)) ) {
			++i;
			begin = KGlobal::locale()->formatDateTime( desc->startDateTime );
			duration = desc->duration.toString("hh:mm");
			title = desc->title;
			if ( title.isEmpty() )
				continue;
			itt = new EListViewItem( (QListView*)listView, chan->name, begin, duration, title, desc );
			if ( !chan->pix.isNull() )
				itt->setPixmap( 0, chan->pix );
		}
		return;
	}

	for ( i=0; i<(int)channels->count(); i++ ) {
		ch = channels->at(i);
		if ( (desc=events->getEventDesc( ch->tp.source, ch->tp.nid, ch->tp.tsid, ch->sid, 0 )) ) {
			begin = KGlobal::locale()->formatDateTime( desc->startDateTime );
			duration = desc->duration.toString("hh:mm");
			title = desc->title;
			if ( title.isEmpty() )
				continue;
			itt = new EListViewItem( (QListView*)listView, ch->name, begin, duration, title, desc );
			if ( !ch->pix.isNull() )
				itt->setPixmap( 0, ch->pix );
		}
	}
}



KEvents::~KEvents()
{
}

#include "kevents.moc"
