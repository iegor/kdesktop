/*
 * krecord.cpp
 *
 * Copyright (C) 2004-2005 Christophe Thommeret <hftom@free.fr>
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
#include <qlabel.h>
#include <qfile.h>
#include <qdir.h>
#include <qframe.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "krecord.h"



KRecord::KRecord( QStringList chanList, QPtrList<RecTimer> *t, QWidget *parent, QSize size ) : QDialog( parent )
{
	timers = t;
	channelsList = chanList;

	QVBoxLayout *vb = new QVBoxLayout( 0, 0, 6 );
	QLabel *lab = new QLabel( i18n("Timers list:"), this );
	vb->addWidget( lab );

	list = new KListView( this );
	list->addColumn( "" );
	list->addColumn( i18n( "Name" ) );
	list->addColumn( i18n( "Channel" ) );
	list->addColumn( i18n( "Begin" ) );
	list->addColumn( i18n( "Duration" ) );
	list->setResizePolicy( KListView::AutoOneFit );
	list->setAllColumnsShowFocus( true );
	list->setFullWidth( true );
	QListViewItem * item = new QListViewItem( list, 0 );
	item->setText( 0, "00" );
	item->setText( 1, "Un nom assez long mais pas trop" );
	item->setText( 2, "Une chaine du meme calibre" );
	item->setText( 3, "00:00 00/00/00mm" );
	item->setText( 4, "00:00mm" );
	vb->addWidget( list );

	QVBoxLayout *vb1 = new QVBoxLayout( 0, 0, 6 );
	newBtn = new KPushButton(i18n("New"), this);
	vb1->addWidget( newBtn );
	editBtn = new KPushButton(i18n("Edit"), this);
	vb1->addWidget( editBtn );
	deleteBtn = new KPushButton(i18n("Delete"), this);
	vb1->addWidget( deleteBtn );
	vb1->addItem( new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding ) );

	QHBoxLayout *hb = new QHBoxLayout( 0, 0, 6 );
	hb->addLayout( vb );
	hb->addLayout( vb1 );

	QHBoxLayout *hb1 = new QHBoxLayout( 0 );
	hb1->addItem( new QSpacerItem( 20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum ) );
	okBtn = new KPushButton(i18n("Close"), this);
	hb1->addWidget( okBtn );

	QVBoxLayout *vb2 = new QVBoxLayout( this, 6, 6 );
	vb2->addLayout( hb );
	QFrame *line = new QFrame( this, "line1" );
	line->setFrameStyle( QFrame::HLine );
	line->setFrameShadow( QFrame::Sunken );
	line->setFrameShape( QFrame::HLine );
	vb2->addWidget( line );
	vb2->addLayout( hb1 );

	KIconLoader *icon = new KIconLoader();
	newBtn->setGuiItem( KGuiItem(i18n("New"), icon->loadIconSet("filenew", KIcon::Small) ) );
	editBtn->setGuiItem( KGuiItem(i18n("Edit"), icon->loadIconSet("edit", KIcon::Small) ) );
	//editBtn->setEnabled( false );
	deleteBtn->setGuiItem( KGuiItem(i18n("Stop/Delete"), icon->loadIconSet("editcut", KIcon::Small) ) );
	okBtn->setGuiItem( KGuiItem(i18n("Close"), icon->loadIconSet("exit", KIcon::Small) ) );

	isRecording = icon->loadIcon("player_record", KIcon::Small);
	yesRepeat = icon->loadIcon("reload", KIcon::Small);

	setCaption( i18n("Timers") );

	connect( okBtn, SIGNAL(clicked()), this, SLOT(accept()) );
	connect( newBtn, SIGNAL(clicked()), this, SLOT(newTimer()) );
	connect( deleteBtn, SIGNAL(clicked()), this, SLOT(deleteTimer()) );
	connect( editBtn, SIGNAL(clicked()), this, SLOT(editTimer()) );

	refresh();
	resize( size );
        delete icon;
}



void KRecord::refresh()
{
	QListViewItem *after, *itt;
	RecTimer *ti;
	int i;
	QString s;

	list->clear();
	list->setSorting( -1 );
	for ( i=0; i<(int)timers->count(); i++ ) {
		ti = timers->at(i);
		after = where( ti );
		/*if ( ti->running ) s = ">";
		else s = "";
		s = s+ti->name;*/
		s = ti->name;
		itt = new KListViewItem( (QListView*)list, "", s, ti->channel, KGlobal::locale()->formatDateTime( ti->begin ), ti->duration.toString("hh:mm") );
		if ( ti->mode )
			itt->setPixmap( 0, yesRepeat );
		if ( ti->running )
			itt->setPixmap( 1, isRecording );
		if ( after )
			itt->moveItem( after );
	}
}



void KRecord::newTimer()
{
	QListViewItem *itt, *after;
	RecTimer *t = new RecTimer();

	t->running = 0;
	t->mode = 0;

	KTimerEditor dlg( true, channelsList, *t, this );
	int ret=dlg.exec();
	if ( ret==KTimerEditor::Accepted ) {
		*t = dlg.timer;
		after = where( t, true );
		itt = new KListViewItem( (QListView*)list, "", t->name, t->channel, KGlobal::locale()->formatDateTime( t->begin ), t->duration.toString("hh:mm") );
		if ( t->mode )
			itt->setPixmap( 0, yesRepeat );
		if ( t->running )
			itt->setPixmap( 1, isRecording );
		if ( after )
			itt->moveItem( after );
	}
	else delete t;
}



void KRecord::editTimer()
{
	QListViewItem *it, *after;
	int i=0;
	RecTimer *t, *tn=0;

	it = list->firstChild();
	while ( it!=0 ) {
		if ( it->isSelected() )
			break;
		it = it->nextSibling();
	}
	if ( !it )
		return;

	for ( i=0; i<(int)timers->count(); i++ ) {
		t = timers->at(i);
		if ( t->name==it->text(1) && t->channel==it->text(2) ) {
			tn = t;
			break;
		}
	}

	if ( !tn )
		return;

	KTimerEditor dlg( false, channelsList, *tn, this );
	int ret=dlg.exec();
	if ( ret==KTimerEditor::Accepted ) {
		for ( i=0; i<(int)timers->count(); i++ ) {
			if ( timers->at(i)==tn ) {
				if ( tn->running ) {
					int ms = tn->begin.time().secsTo( QTime::currentTime() );
					ms = ((dlg.timer.duration.hour()*3600+dlg.timer.duration.minute()*60)-ms)*1000;
					if ( ms<2000 )
						ms = 0;
					tn->duration = dlg.timer.duration;
					emit updateTimer( tn, ms );
					if ( ms )
						refresh();
				}
				else {
					timers->remove( tn );
					t = new RecTimer();
					*t = dlg.timer;
					after = where( t, true );
					refresh();
				}
				break;
			}
		}
	}
}



void KRecord::deleteTimer()
{
	QListViewItem *it;
	int i=0, ret;
	KIconLoader *icon = new KIconLoader();

	it = list->firstChild();
       while ( it!=0 ) {
		if ( it->isSelected() )  {
			if ( timers->at(i)->mode )
				ret = KMessageBox::questionYesNoCancel( this, i18n("This timer is repeated. Do you want to skip the current job or delete the timer?"), i18n("Warning"), KGuiItem(i18n("Skip Current"), icon->loadIconSet("next", KIcon::Small) ), KGuiItem(i18n("Delete"), icon->loadIconSet("editcut", KIcon::Small) ) );
			else
				ret = KMessageBox::questionYesNo( this, i18n("Delete the selected timer?") );
			if ( ret==KMessageBox::Yes ) {
				emit updateTimer( timers->at(i), 0 );
			}
			else if ( ret==KMessageBox::No && timers->at(i)->mode ) {
				timers->at(i)->mode=0;
				emit updateTimer( timers->at(i), 0 );
			}
			break;
		}
		i++;
		it = it->nextSibling();
	}

	delete icon;
}



QListViewItem* KRecord::where( RecTimer *rt, bool add )
{
	QListViewItem *it, *ret=0;
	int i, r=0;
	RecTimer *t;

	for ( i=0; i<(int)timers->count(); i++ ) {
		t = timers->at(i);
		if ( rt->begin>t->begin )
			r=i+1;
	}
	if ( add )
		timers->insert( r, rt );
	if ( !r )
		return ret;

	it = list->firstChild();
	i=0;
	while ( it!=0 ) {
		if ( i==(r-1) ) {
			ret = it;
			break;
		}
		i++;
		it = it->nextSibling();
	}
	return ret;
}



void KRecord::accept()
{
	done( Accepted );
}



KRecord::~KRecord()
{
}

#include "krecord.moc"
