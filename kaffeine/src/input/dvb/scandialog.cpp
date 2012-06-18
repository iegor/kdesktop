/*
 * scandialog.cpp
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
#include <qdir.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlistview.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include <qaccel.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#include "scandialog.h"
#include "channeleditor.h"
#include "dvbsi.h"



DListViewItem::DListViewItem( QListView *parent, ChannelDesc *desc, QString label1, QString label2 )
	: QListViewItem( parent, label1, label2 )
{
	chan = desc;
}



ScanDialog::ScanDialog( QPtrList<DvbStream> *d, QPtrList<ChannelDesc> *ch, QSize size, QString src, const QString &charset )
{
	QString s;
	int i, j;
	QStringList list, tmp;

	KIconLoader *icon = new KIconLoader();
	tvPix = icon->loadIcon( "kdvbtv", KIcon::Small );
	tvcPix = icon->loadIcon( "kdvbtvc", KIcon::Small );
	raPix = icon->loadIcon( "kdvbra", KIcon::Small );
	racPix = icon->loadIcon( "kdvbrac", KIcon::Small );

	chandesc = ch;
	dvbsi = 0;
	dvb = d;
	sourcesPath = src;
	defaultCharset = charset;

	for ( i=0; i<(int)dvb->count(); i++ ) {
		tmp = dvb->at(i)->getSources( true );
		for ( j=0; j<(int)tmp.count(); j++ ) {
			if ( tmp[j].startsWith( "AUTO" ) )
				list.append( QString("AUTO(%1:%2)").arg(dvb->at(i)->getAdapter()).arg(dvb->at(i)->getTuner()) );
			else
				list.append( tmp[j] );
		}
	}
	searchComb->insertStringList( list );

	setCaption( i18n("Channels") );

	transponders.setAutoDelete( true );

	QGridLayout *statLayout = new QGridLayout( statusFrame, 1, 1, 11, 6 );

	QLabel *lab = new QLabel( i18n("Signal:"), statusFrame );
	statLayout->addWidget( lab, 0, 0 );
	signal = new KGradProgress( statusFrame );
	statLayout->addWidget( signal, 0, 1 );
	lab = new QLabel( i18n("SNR:"), statusFrame );
	statLayout->addWidget( lab, 1, 0 );
	snr = new KGradProgress( statusFrame );
	statLayout->addWidget( snr, 1, 1 );
	lab = new QLabel( i18n("Tuned:"), statusFrame );
	statLayout->addWidget( lab, 2, 0 );
	lock = new KLed( statusFrame, "lockLed" );
	lock->setState( KLed::Off );
	lock->setDarkFactor( 500 );
	statLayout->addWidget( lock, 2, 1 );
	progress = new QProgressBar( 100, statusFrame );
	progress->setIndicatorFollowsStyle( false );
	progress->setProgress( 0 );
	statLayout->addMultiCellWidget( progress, 3, 3, 0, 1 );
	progressLab = new QLabel( statusFrame );
	statLayout->addMultiCellWidget( progressLab, 4, 4, 0, 1 );

	channelsList->clear();
	foundList->clear();
	foundList->setAllColumnsShowFocus( true );
	foundList->setSelectionMode( QListView::Extended );
	channelsList->setSorting( 1 ); // sort by source 1st than channel name 
	channelsList->setAllColumnsShowFocus( true ); 
	channelsList->setSelectionMode( QListView::Extended );

	ChannelDesc *chan;
	QListViewItem *it;
	for ( int i=0; i<(int)chandesc->count(); i++ ) {
		chan = chandesc->at(i);
	it = new QListViewItem( channelsList, chan->name, chan->tp.source );
		if ( !chan->pix.isNull() )
			it->setPixmap( 0, chan->pix );
		else {
			if ( chan->type==1 ) {
				if ( chan->fta )
					it->setPixmap( 0, tvcPix );
				else
					it->setPixmap( 0, tvPix );
			}
			else {
				if ( chan->fta )
					it->setPixmap( 0, racPix );
				else
					it->setPixmap( 0, raPix );
			}
		}
	}

	startBtn->setPaletteForegroundColor( QColor(255,0,0) );

	connect( searchComb, SIGNAL(activated(int)), this, SLOT(setDvb(int)) );

	ds = dvb->at(0);
	ds->setScanning( true );
	if ( searchComb->currentText().startsWith( "AUTO" ) )
		offsetGroup->show();
	else
		offsetGroup->hide();
	bool ok=false;
	dvbsi = new DVBsi( &ok, ds->getAdapter(), ds->getTuner(), ds, defaultCharset );
	connect( ds, SIGNAL(snrStatus(int)), snr, SLOT(setProgress(int)) );
	connect( ds, SIGNAL(signalStatus(int)), signal, SLOT(setProgress(int)) );
	connect( ds, SIGNAL(lockStatus(bool)), this, SLOT(setLock(bool)) );
	connect( dvbsi, SIGNAL(end(bool)), this, SLOT(siEnded(bool)) );

	connect( &checkTimer, SIGNAL( timeout() ), this, SLOT( checkNewChannel() ) );
	connect( &progressTimer, SIGNAL( timeout() ), this, SLOT( setProgress() ) );
	connect( startBtn, SIGNAL(toggled(bool)), this, SLOT(scan(bool)) );
	connect( addfilteredBtn, SIGNAL(clicked()), this, SLOT(addFiltered()) );
	connect( addselectedBtn, SIGNAL(clicked()), this, SLOT(addSelected()) );
	connect( channelsList, SIGNAL(contextMenuRequested(QListViewItem*,const QPoint&,int)), this, SLOT(pop(QListViewItem*,const QPoint&,int)) );
	connect( channelsList, SIGNAL(doubleClicked(QListViewItem*,const QPoint &,int)), this, SLOT( slotChannelChanged(QListViewItem*,const QPoint &,int) ) );
	connect( newBtn, SIGNAL(clicked()), this, SLOT(newChannel()) );
	connect( delBtn, SIGNAL(clicked()), this, SLOT(deleteAll()) );
	connect( selectallBtn, SIGNAL(clicked()), this, SLOT(selectAll()) );

	QAccel *ac = new QAccel( channelsList );
	ac->insertItem( Key_Delete, 100 );
	ac->connectItem( 100, this, SLOT(deleteChannel()) );

	resize( size );
        delete icon;
}



void ScanDialog::selectAll()
{
	foundList->selectAll( true );
}



void ScanDialog::slotChannelChanged( QListViewItem *_channel, const QPoint &, int )
{
	if ( !_channel )
		return;
	QPixmap pix;
	QString s = _channel->text(0);
	if ( edit( s, pix ) ) {
		_channel->setText( 0, s );
		_channel->setPixmap( 0, pix );
	}
}



void ScanDialog::setDvb( int index )
{
	int i, pos=0;
	QStringList list;
	bool ok=false;

	for ( i=0; i<(int)dvb->count(); i++ ) {
		list = dvb->at(i)->getSources( true );
		pos+= list.count();
		if ( index<pos ) {
			if ( checkTimer.isActive() )
				checkTimer.stop();
			if ( progressTimer.isActive() )
				progressTimer.stop();
			disconnect( ds, SIGNAL(snrStatus(int)), snr, SLOT(setProgress(int)) );
			disconnect( ds, SIGNAL(signalStatus(int)), signal, SLOT(setProgress(int)) );
			disconnect( ds, SIGNAL(lockStatus(bool)), this, SLOT(setLock(bool)) );
			disconnect( dvbsi, SIGNAL(end(bool)), this, SLOT(siEnded(bool)) );
			ds->setScanning( false );
			dvbsi->stop();
			delete dvbsi;
			ds = dvb->at(i);
			ds->setScanning( true );
			dvbsi = new DVBsi( &ok, ds->getAdapter(), ds->getTuner(), ds, defaultCharset );
			connect( ds, SIGNAL(snrStatus(int)), snr, SLOT(setProgress(int)) );
			connect( ds, SIGNAL(signalStatus(int)), signal, SLOT(setProgress(int)) );
			connect( ds, SIGNAL(lockStatus(bool)), this, SLOT(setLock(bool)) );
			connect( dvbsi, SIGNAL(end(bool)), this, SLOT(siEnded(bool)) );
			break;
		}
	}
	if ( searchComb->currentText().startsWith( "AUTO" ) )
		offsetGroup->show();
	else
		offsetGroup->hide();
}



void ScanDialog::pop( QListViewItem *it, const QPoint &pos, int col )
{
	QPoint p=pos;

	if ( !it )
		return;

	int i=col;
	QPixmap pix;
	QString s = it->text(0);

	QPopupMenu *pop = new QPopupMenu();
	pop->insertItem( i18n("Edit..."), 1 );
	pop->insertItem( i18n("Delete"), 2 );
	i = 0;
	i = pop->exec( QCursor::pos() );
	switch ( i ) {
		case 0 :
			break;
		case 1 :
			if ( edit( s, pix ) ) {
				it->setText( 0, s );
				it->setPixmap( 0, pix );
			}
			break;
		case 2 :
			deleteChannel( it->text(0) );
			break;
	}
	delete pop;
}



void ScanDialog::deleteChannel()
{
	QListViewItem *it = channelsList->currentItem();
	if ( it )
		deleteChannel( it->text(0) );
}



void ScanDialog::deleteChannel( QString name )
{
	int j, i;
	QListViewItem *it;

	for ( j=0; j<(int)chandesc->count(); j++ ) {
		if ( chandesc->at(j)->name==name ) {
			for ( i=0; i<(int)chandesc->count(); i++ ) {
				if ( chandesc->at(i)->num==chandesc->count() ) {
					chandesc->at(i)->num = chandesc->at(j)->num;
					break;
				}
			}
			dvbsi->channels.append( new ChannelDesc( *chandesc->at(j) ) );
			addFound( dvbsi->channels.getLast(), false );
			chandesc->remove(j);
			it = channelsList->firstChild();
			while ( it!=0 ) {
				if ( name==it->text(0) ) {
					channelsList->removeItem( it );
					break;
				}
				it = it->nextSibling();
			}
			break;
		}
	}
}



void ScanDialog::newChannel()
{
	ChannelDesc chan;
	QStringList list;
	//int i;

	//for ( i=0; i<(int)searchComb->count(); i++ ) list.append( searchComb->text(i) );
	if ( ds->getType()==FE_QPSK )
		list.append( searchComb->currentText() );
	else
		list = ds->getSources();

	chandesc->append( new ChannelDesc( chan ) );
	chan.tp.type = ds->getType();
	chan.num = chandesc->count();

	ChannelEditor dlg( list, false, &chan, chandesc, this );

	int ret = dlg.exec();
	chandesc->remove();
	if ( ret==ChannelEditor::Accepted ) {
		chandesc->append( new ChannelDesc( chan ) );
		QListViewItem *it = new QListViewItem( channelsList, chan.name );
		if ( chan.type==1 ) {
			if ( chan.fta )
				it->setPixmap( 0, tvcPix );
			else
				it->setPixmap( 0, tvPix );
		}
		else {
			if ( chan.fta )
				it->setPixmap( 0, racPix );
			else
				it->setPixmap( 0, raPix );
		}
	}
}



void ScanDialog::deleteAll()
{
	int i, ret;

	ret = KMessageBox::questionYesNo( this, i18n("Do you really want to delete all channels?") );
	if ( ret!=KMessageBox::Yes )
		return;
	for ( i=0; i<(int)chandesc->count(); i++ ) {
		dvbsi->channels.append( new ChannelDesc( *chandesc->at(i) ) );
		addFound( dvbsi->channels.getLast(), false );
	}
	channelsList->clear();
	chandesc->clear();
}



bool ScanDialog::edit( QString &name, QPixmap &pix )
{
	int j;
	ChannelDesc *chan=0;
	QStringList list, tmp;
	int i;

	for ( i=0; i<(int)dvb->count(); i++ ) {
		tmp = dvb->at(i)->getSources();
		for ( j=0; j<(int)tmp.count(); j++ )
			list.append( tmp[j] );
	}

	for ( j=0; j<(int)chandesc->count(); j++ ) {
		chan = chandesc->at(j);
		if ( chan->name==name ) {
			j = -1;
			break;
		}
	}
	if ( j==-1 ) {
		ChannelEditor dlg( list, false, chan, chandesc, this );
		int ret = dlg.exec();
		if ( ret==ChannelEditor::Accepted ) {
			name = chan->name;
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
			return true;
		}
	}
	return false;
}



void ScanDialog::parseTp( QString s, fe_type_t type, QString src )
{
	Transponder *trans;
	QString t;
	int pos;

	s = s.stripWhiteSpace();
	trans = new Transponder();
	pos = s.find(" ");
	if ( s.left(pos)=="T" ) {
		trans->type=FE_OFDM;
		trans->source = "Terrestrial";
	}
	else if ( s.left(pos)=="C" ) {
		trans->type=FE_QAM;
		trans->source = "Cable";
	}
	else if ( s.left(pos)=="A" ) {
		trans->type=FE_ATSC;
		trans->source = "ATSC Terrestrial";
	}
	else if ( s.left(pos)=="S" ) {
		trans->type=FE_QPSK;
		trans->source = src;
	}
	if ( trans->type!=type ) {
		delete trans;
		return;
	}
	s = s.right( s.length()-pos-1 );
	s = s.stripWhiteSpace();
	pos = s.find(" ");
	trans->freq = s.left(pos).toULong()/1000;
	s = s.right( s.length()-pos-1 );
	s = s.stripWhiteSpace();
	if ( trans->type!=FE_ATSC )
		pos = s.find(" ");
	if ( trans->type==FE_QPSK ) {
		trans->pol =  s.left(pos).lower()[0].latin1();
		s = s.right( s.length()-pos-1 );
		s = s.stripWhiteSpace();
		pos = s.find(" ");
	}
	if ( trans->type!=FE_OFDM && trans->type!=FE_ATSC ) {
		trans->sr = s.left(pos).toULong()/1000;
	}
	else {
		if ( s.left(pos)=="8MHz" )
			trans->bandwidth = BANDWIDTH_8_MHZ;
		else if ( s.left(pos)=="7MHz" )
			trans->bandwidth = BANDWIDTH_7_MHZ;
		else if ( s.left(pos)=="6MHz" )
			trans->bandwidth = BANDWIDTH_6_MHZ;
		else
			trans->bandwidth = BANDWIDTH_AUTO;
	}
	if ( trans->type==FE_ATSC ) {
		if ( s.left(pos)=="8VSB" )
			trans->modulation = VSB_8;
		else if ( s.left(pos)=="16VSB" )
			trans->modulation = VSB_16;
		else if ( s.left(pos)=="QAM16" )
			trans->modulation = QAM_16;
		else if ( s.left(pos)=="QAM32" )
			trans->modulation = QAM_32;
		else if ( s.left(pos)=="QAM64" )
			trans->modulation = QAM_64;
		else if ( s.left(pos)=="QAM128" )
			trans->modulation = QAM_128;
		else if ( s.left(pos)=="QAM256" )
			trans->modulation = QAM_256;
		else
			trans->modulation = QAM_AUTO;
		transponders.append( trans );
		return;
	}

	s = s.right( s.length()-pos-1 );
	s = s.stripWhiteSpace();
	pos = s.find(" ");
	if ( s.left(pos)=="1/2" )
		trans->coderateH = FEC_1_2;
	else if ( s.left(pos)=="2/3" )
		trans->coderateH = FEC_2_3;
	else if ( s.left(pos)=="3/4" )
		trans->coderateH = FEC_3_4;
	else if ( s.left(pos)=="4/5" )
		trans->coderateH = FEC_4_5;
	else if ( s.left(pos)=="5/6" )
		trans->coderateH = FEC_5_6;
	else if ( s.left(pos)=="6/7" )
		trans->coderateH = FEC_6_7;
	else if ( s.left(pos)=="7/8" )
		trans->coderateH = FEC_7_8;
	else if ( s.left(pos)=="8/9" )
		trans->coderateH = FEC_8_9;
	else if ( s.left(pos)=="NONE" )
		trans->coderateH = FEC_NONE;
	else
		trans->coderateH = FEC_AUTO;
	s = s.right( s.length()-pos-1 );
	s = s.stripWhiteSpace();
	pos = s.find(" ");
	if ( trans->type==FE_OFDM ) {
		if ( s.left(pos)=="1/2" )
			trans->coderateL = FEC_1_2;
		else if ( s.left(pos)=="2/3" )
			trans->coderateL = FEC_2_3;
		else if ( s.left(pos)=="3/4" )
			trans->coderateL = FEC_3_4;
		else if ( s.left(pos)=="4/5" )
			trans->coderateL = FEC_4_5;
		else if ( s.left(pos)=="5/6" )
			trans->coderateL = FEC_5_6;
		else if ( s.left(pos)=="6/7" )
			trans->coderateL = FEC_6_7;
		else if ( s.left(pos)=="7/8" )
			trans->coderateL = FEC_7_8;
		else if ( s.left(pos)=="8/9" )
			trans->coderateL = FEC_8_9;
		else if ( s.left(pos)=="NONE" )
			trans->coderateL = FEC_NONE;
		else
			trans->coderateL = FEC_AUTO;

		if ( trans->coderateH==FEC_NONE )
			trans->coderateH = FEC_AUTO;
		if ( trans->coderateL==FEC_NONE )
			trans->coderateL = FEC_AUTO;

		s = s.right( s.length()-pos-1 );
		s = s.stripWhiteSpace();
		pos = s.find(" ");
	}
	if ( trans->type!=FE_QPSK ) {
		if ( s.left(pos)=="QPSK" )
			trans->modulation = QPSK;
		else if ( s.left(pos)=="QAM16" )
			trans->modulation = QAM_16;
		else if ( s.left(pos)=="QAM32" )
			trans->modulation = QAM_32;
		else if ( s.left(pos)=="QAM64" )
			trans->modulation = QAM_64;
		else if ( s.left(pos)=="QAM128" )
			trans->modulation = QAM_128;
		else if ( s.left(pos)=="QAM256" )
			trans->modulation = QAM_256;
		else
			trans->modulation = QAM_AUTO;
	}
	if ( trans->type==FE_OFDM ) {
		s = s.right( s.length()-pos-1 );
		s = s.stripWhiteSpace();
		pos = s.find(" ");
		if ( s.left(pos)=="8k" )
			trans->transmission = TRANSMISSION_MODE_8K;
		else if ( s.left(pos)=="2k" )
			trans->transmission = TRANSMISSION_MODE_2K;
		else
			trans->transmission = TRANSMISSION_MODE_AUTO;

		s = s.right( s.length()-pos-1 );
		s = s.stripWhiteSpace();
		pos = s.find(" ");
		if ( s.left(pos)=="1/32" )
			trans->guard = GUARD_INTERVAL_1_32;
		else if ( s.left(pos)=="1/16" )
			trans->guard = GUARD_INTERVAL_1_16;
		else if ( s.left(pos)=="1/8" )
			trans->guard = GUARD_INTERVAL_1_8;
		else if ( s.left(pos)=="1/4" )
			trans->guard = GUARD_INTERVAL_1_4;
		else
			trans->guard = GUARD_INTERVAL_AUTO;

		s = s.right( s.length()-pos-1 );
		s = s.stripWhiteSpace();
		pos = s.find(" ");
		if ( s.left(pos)=="1" )
			trans->hierarchy = HIERARCHY_1;
		else if ( s.left(pos)=="2" )
			trans->hierarchy = HIERARCHY_2;
		else if ( s.left(pos)=="4" )
			trans->hierarchy = HIERARCHY_4;
		else if ( s.left(pos)=="NONE" )
			trans->hierarchy = HIERARCHY_NONE;
		else
			trans->hierarchy = HIERARCHY_AUTO;
	}
	transponders.append( trans );
}



bool ScanDialog::getTransData()
{
	QString s=sourcesPath;

	transponders.clear();

	if ( searchComb->currentText().startsWith("AUTO") ) {
		int i;
		for ( i=177; i<227; i+=7 ) {
			if ( offset07->isChecked() ) {
				s = QString("T %1 7MHz AUTO AUTO AUTO AUTO AUTO AUTO").arg( (i*1000000)+500000 );
				parseTp( s, ds->getType(), "" );
			}
			if ( offset125p->isChecked() ) {
				s = QString("T %1 7MHz AUTO AUTO AUTO AUTO AUTO AUTO").arg( (i*1000000)+500000+125000 );
				parseTp( s, ds->getType(), "" );
			}
		}
		for ( i=474; i<859; i+=8 ) {
			if ( offset167m->isChecked() ) {
				s = QString("T %1 8MHz AUTO AUTO AUTO AUTO AUTO AUTO").arg( (i*1000000)-167000 );
				parseTp( s, ds->getType(), "" );
			}
			if ( offset0->isChecked() ) {
				s = QString("T %1 8MHz AUTO AUTO AUTO AUTO AUTO AUTO").arg( i*1000000 );
				parseTp( s, ds->getType(), "" );
			}
			if ( offset167p->isChecked() ) {
				s = QString("T %1 8MHz AUTO AUTO AUTO AUTO AUTO AUTO").arg( (i*1000000)+167000 );
				parseTp( s, ds->getType(), "" );
			}
		}
		return true;
	}

	switch ( ds->getType() ) {
		case FE_QPSK : s += "dvb-s/"; break;
		case FE_QAM : s += "dvb-c/"; break;
		case FE_OFDM : s += "dvb-t/"; break;
		case FE_ATSC : s += "atsc/"; break;
		default:
			return false;
	}
	s += searchComb->currentText();
	QFile f( s );
	if ( f.open(IO_ReadOnly) ) {
		QTextStream tt( &f );
		while ( !tt.eof() ) {
			s = tt.readLine();
			if ( s.startsWith("#") )
				continue;
			if ( s.length()==0 )
				continue;
			parseTp( s, ds->getType(), searchComb->currentText() );
		}
		f.close();
		return true;
	}
	else
		return false;
}



void ScanDialog::checkDuplicateName( ChannelDesc *chan )
{
	QString org, name;
	int i, j=1;
	bool loop;

	org = name = chan->name;

	do {
		loop = false;
		for ( i=0; i<(int)chandesc->count(); i++ ) {
			if ( name==chandesc->at(i)->name ) {
				name = org+"-"+QString().setNum(j);
				loop = true;
				++j;
				break;
			}
		}
	} while ( loop );
	chan->name = name;
}

bool ScanDialog::checkChannUpdate( ChannelDesc *chan )
{
	int i, j;
	AudioPid a;
	SubPid s;
	ChannelDesc *desc;

	for ( i=0; i<(int)chandesc->count(); i++ ) {
		desc = chandesc->at(i);
		if ( chan->tp.source==desc->tp.source && chan->tp.freq==desc->tp.freq && chan->sid==desc->sid ) {
			chan->category=desc->category;
			chan->num=desc->num;
			chan->name=desc->name;
			chan->fta=desc->fta;
			for ( j=0; j<chan->napid; j++ ) {
				if ( desc->apid[0].pid==chan->apid[j].pid ) {
					if ( j>0 ) {
						a = chan->apid[0];
						chan->apid[0]=chan->apid[j];
						chan->apid[j]=a;
					}
					break;
				}
			}
			for ( j=0; j<chan->nsubpid; j++ ) {
				if ( desc->subpid[0].pid==chan->subpid[j].pid ) {
					if ( j>0 ) {
						s = chan->subpid[0];
						chan->subpid[0]=chan->subpid[j];
						chan->subpid[j]=s;
					}
					break;
				}
			}
			chandesc->replace(i,new ChannelDesc(*chan));
			return false;
		}
	}
	return true;
}


void ScanDialog::addSelected()
{
	ChannelDesc *chan;
	DListViewItem *dit, *odit;
	QListViewItem *it;

	dit = (DListViewItem*)foundList->firstChild();
	while( dit ) {
		chan = 0;
		if ( !dit->isSelected( ) ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		chan = dit->chan;
		if ( !chan ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if(checkChannUpdate(chan)){
			checkDuplicateName( chan );

			if (chan->num == 0) {
				chan->num = chandesc->count()+1;
			}
			chandesc->append( new ChannelDesc( *chan ) );
			it = new QListViewItem( channelsList, chan->name );
			if ( chan->type==1 ) {
				if ( chan->fta )
					it->setPixmap( 0, tvcPix );
				else
					it->setPixmap( 0, tvPix );
			}
			else {
				if ( chan->fta )
					it->setPixmap( 0, racPix );
				else
					it->setPixmap( 0, raPix );
			}
		}
		odit = dit;
		dit = (DListViewItem*)dit->nextSibling();
		delete odit;
		dvbsi->channels.remove( chan );
	}
}



void ScanDialog::working( bool b )
{
	offsetGroup->setEnabled( !b );
	addfilteredBtn->setEnabled( !b );
	addselectedBtn->setEnabled( !b );
	//if ( !isTuned ) searchGroup->setEnabled( !b );
	newBtn->setEnabled( !b );
	delBtn->setEnabled( !b );
}



void ScanDialog::addFiltered()
{
	ChannelDesc *chan;
	QListViewItem *it;
	DListViewItem *dit, *odit;
	int m = dvbsi->channels.count();
	int t=0;

	working( true );
	startBtn->setEnabled( false );
	progress->setProgress(0);
	qApp->processEvents();

	dit = (DListViewItem*)foundList->firstChild();
	while( dit ) {
		t++;
		progress->setProgress( t*100/m );
		chan = dit->chan;
		if ( !chan->completed ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->name.isEmpty() ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type<1 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type>2 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type==1 && chan->vpid==0 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type==1 && chan->napid==0 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type==2 && chan->napid==0 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( chan->type==2 && chan->vpid!=0 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( ftaCb->isChecked() && chan->fta ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( !tvCb->isChecked() && chan->type==1 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( !radioCb->isChecked() && chan->type==2 ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if ( providerCb->isChecked() && chan->provider!=providerComb->currentText() ) {
			dit = (DListViewItem*)dit->nextSibling();
			continue;
		}
		if(checkChannUpdate(chan)){
			checkDuplicateName( chan );

			chan->num = chandesc->count()+1;
			chandesc->append( new ChannelDesc( *chan ) );
			it = new QListViewItem( channelsList, chan->name );
			if ( chan->type==1 ) {
				if ( chan->fta )
					it->setPixmap( 0, tvcPix );
				else
					it->setPixmap( 0, tvPix );
			}
			else {
				if ( chan->fta )
					it->setPixmap( 0, racPix );
				else
					it->setPixmap( 0, raPix );
			}
		}
		odit = dit;
		dit = (DListViewItem*)dit->nextSibling();
		delete odit;
		dvbsi->channels.remove( chan );
	}

	progress->setProgress(0);
	startBtn->setEnabled( true );
	working( false );
}



void ScanDialog::siEnded( bool b )
{
	checkTimer.stop();
	progressTimer.stop();
	checkNewChannel();
	snr->setProgress( 0 );
	signal->setProgress( 0 );
	setLock( false );
	progress->setProgress( 0 );
	startBtn->setOn( !b );
	working( false );
	startBtn->setText( i18n("START scan") );
	startBtn->setPaletteForegroundColor( QColor(255,0,0) );
}



void ScanDialog::scan( bool b )
{
	if ( b ) {
		if ( checkTimer.isActive() )
			checkTimer.stop();
		foundList->clear();
		nChannels = 0;
		ntv = nradio = 0;
		if ( ds->isTuned() )
			dvbsi->go( transponders, 2 );
		else {
			if ( !getTransData() )
				return;
			if ( searchComb->currentText().startsWith("AUTO") )
				dvbsi->go( transponders, 0 ); // no NIT
			else
				dvbsi->go( transponders );
		}
		checkTimer.start( 100 );
		progressTimer.start( 1000 );
		working( true );
		startBtn->setText( i18n("STOP scan") );
		startBtn->setPaletteForegroundColor( QColor(0,255,0) );
		searchComb->setEnabled( false );
	}
	else {
		//setCursor(QCursor(Qt::WaitCursor));
		startBtn->setText( i18n("Stopping...") );
		startBtn->setEnabled( false );
		qApp->processEvents();
		dvbsi->stop();
		checkTimer.stop();
		progressTimer.stop();
		checkNewChannel();
		snr->setProgress( 0 );
		signal->setProgress( 0 );
		setLock( false );
		progress->setProgress( 0 );
		working( false );
		//setCursor(QCursor(Qt::ArrowCursor));
		startBtn->setText( i18n("START scan") );
		startBtn->setEnabled( true );
		startBtn->setPaletteForegroundColor( QColor(255,0,0) );
		searchComb->setEnabled( true );
	}
}



void ScanDialog::setProgress()
{
	if ( !dvbsi->transponders.count() )
		return;
	progress->setProgress( dvbsi->progressTransponder*100/dvbsi->transponders.count() );
}



void ScanDialog::setLock( bool on )
{
	if ( on )
		lock->on();
	else
		lock->off();
}



void ScanDialog::checkNewChannel()
{
	ChannelDesc *chan;
	int j;

	for ( j=nChannels; j<(int)dvbsi->channels.count(); j++ ) {
		chan = dvbsi->channels.at(j);
		if ( chan->completed==0 )
			break;
		nChannels++;
		if ( chan->name.isEmpty() )
			continue;
		if ( chan->type<1 )
			continue;
		if ( chan->type>2 )
			continue;
		if ( chan->type==1 && chan->vpid==0 )
			continue;
		if ( chan->type==1 && chan->napid==0 )
			continue;
		if ( chan->type==2 && chan->napid==0 )
			continue;
		if ( chan->type==2 && chan->vpid!=0 )
			continue;
		addFound( chan, true );
	}
}



void ScanDialog::addFound( ChannelDesc *chan, bool scan )
{
	int i;
	DListViewItem *it;

	it = new DListViewItem( foundList, chan, QString( "%1").arg( chan->tp.snr ), chan->name );

	if ( chan->type==1 ) {
		if ( scan )
			ntv++;
		if ( chan->fta )
			it->setPixmap( 1, tvcPix );
		else
			it->setPixmap( 1, tvPix );
	}
	else {
		if ( scan )
			nradio++;
		if ( chan->fta )
			it->setPixmap( 1, racPix );
		else
			it->setPixmap( 1, raPix );
	}

	if ( scan )
		progressLab->setText( QString( i18n("Found: %1 TV - %2 radio") ).arg(ntv).arg(nradio) );
	if ( chan->provider.isEmpty() )
		return;
	for ( i=0; i<providerComb->count(); i++ ) {
		if ( chan->provider==providerComb->text(i) ) {
			i = -1;
			break;
		}
	}
	if ( i>-1 )
		providerComb->insertItem( chan->provider );
}



ScanDialog::~ScanDialog()
{
	if ( checkTimer.isActive() )
		checkTimer.stop();
	if ( progressTimer.isActive() )
		progressTimer.stop();
	dvbsi->stop();
        delete dvbsi;

        ds->setScanning( false );
	transponders.clear();
}

#include "scandialog.moc"
