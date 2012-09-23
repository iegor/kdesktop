/*
 * cdwidget.cpp
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
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

#include <qapplication.h>
#include <qlayout.h>
#include <qdir.h>

#include <kiconloader.h>
#include <klocale.h>

#include "cdwidget.h"



CdWidget::CdWidget( const QString &ad, int port, int info, const QString &tspath, QWidget *parent, QObject *objParent, const char *name )
	: KaffeineInput( objParent, name )
{
	mainWidget = new QVBox( parent );
	mainWidget->setSizePolicy( QSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred) );
	split = new QSplitter( mainWidget );
	split->setOpaqueResize( true );
	playerBox = new QVBox( split );
	playerBox->setMinimumWidth( 200 );
	channelsLb = new QListBox( split );
	split->moveToFirst( channelsLb );
	channelsLb->setSizePolicy( QSizePolicy (QSizePolicy::Preferred, QSizePolicy::MinimumExpanding) );
	split->setResizeMode( channelsLb, QSplitter::KeepSize );

	cdAddress = ad;
	cdPort = port;
	cdInfo = info;
	cdShiftDir = tspath;
	if ( !cdShiftDir.endsWith("/") ) cdShiftDir+= "/";

	cleaner = new CdCleaner( cdShiftDir );

	chan.setAutoDelete( true );

	KIconLoader *icon = new KIconLoader();

	tvPix = icon->loadIcon( "kdvbtv", KIcon::Small );
	raPix = icon->loadIcon( "kdvbra", KIcon::Small );
        delete icon;

	listen = new CdListen();
	listen->go( cdAddress, cdInfo );

	fifoName = QDir::homeDirPath()+"/.kaxclient.ts";
	QFile f( fifoName );
	if ( f.exists() ) f.remove();
	if ( (mkfifo( fifoName.ascii(), 0644 ))<0 ) {
		perror( fifoName.latin1() );
		fifoName = "";
		dump = 0;
	}
	else {
		dump = new CdDump( fifoName );
		connect( channelsLb, SIGNAL(selected(const QString &)), this, SLOT(channelSelected(const QString &)) );
		connect( listen, SIGNAL(listChanged(const QString&)), this, SLOT(updateList(const QString&)) );
	}

	lastChannel = 0;
	enableLive( false );
	loadConfig( KGlobal::config() );
}



void CdWidget::togglePanel()
{
	if ( channelsLb->isHidden() )
		channelsLb->show();
	else
		channelsLb->hide();
}



void CdWidget::getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames )
{
	uiNames.append( i18n("DVB Client") );
	iconNames.append( "network" );
	targetNames.append( "DVBCLIENT" );
}



bool CdWidget::execTarget( const QString &target )
{
	if ( target=="DVBCLIENT" ) {
		emit showMe( this );
		QTimer::singleShot( 100, this, SLOT(playLastChannel()) );
		return true;
	}
	return false;
}



QWidget* CdWidget::wantPlayerWindow()
{
	return playerBox;
}



QWidget* CdWidget::inputMainWidget()
{
	return mainWidget;
}



CdWidget::~CdWidget()
{
	if ( dump->running() )
		emit dvbStop();
	stopLive();
	delete dump;
	delete listen;
	delete cleaner;
}



void CdWidget::loadConfig( KConfig* config )
{
	QValueList<int> sl;

	config->setGroup("DVBClient");
	sl = config->readIntListEntry("SplitSizes");
	if ( !sl.count() ) {
		sl.append( 200 );
		sl.append( 200 );
	}
	split->setSizes( sl );
}



void CdWidget::saveConfig()
{
	KConfig* config = KGlobal::config();
	config->setGroup("DVBClient");
	config->writeEntry( "SplitSizes", split->sizes() );
}



void CdWidget::updateList( const QString &list )
{
	QString c;
	QString s = list;
	int pos;
	QString name, lang;
	int apid, vpid, ac3, subpid, page, id, type;

	if ( dump->running() )
		emit dvbStop();
	dump->stop();

	channelsLb->clear();
	chan.clear();
	if ( list=="quit" ) return;

	while ( (pos = s.find("|"))!=-1 ) {
		name = s.left( pos );
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		vpid = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		apid = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		if ( s.left( pos )=="n" ) ac3 = 0;
		else ac3 = 1;
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		subpid = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		page = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		id = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		type = s.left( pos ).toInt();
		s = s.right( s.length()-pos-1 );
		pos  = s.find("|");
		lang = s.left( pos );
		s = s.right( s.length()-pos-1 );
		chan.append(  new CdChannel( name, vpid, apid, ac3, subpid, page, id, type, lang ) );
		if ( vpid ) channelsLb->insertItem( tvPix, name );
		else channelsLb->insertItem( raPix, name );
	}
}



bool CdWidget::nextTrack( MRL& )
{
	next();
	return false;
}



bool CdWidget::previousTrack( MRL& )
{
	previous();
	return false;
}



bool CdWidget::currentTrack( MRL& )
{
	playLastChannel();
	return false;
}



bool CdWidget::trackNumber( int num, MRL& )
{
	playNumChannel( num );
	return false;
}



bool CdWidget::playbackFinished( MRL& )
{
	return false;
}



void CdWidget::playLastChannel()
{
	if ( !channelsLb->count() )
		return;
	if ( !lastChannel ) {
		lastChannel++;
	}
	else if ( lastChannel>(int)channelsLb->count() ) {
		lastChannel = 1;
	}
	channelSelected( channelsLb->text( lastChannel-1 ) );
}



void CdWidget::playNumChannel( int num )
{
	if ( num>0 && num<=(int)channelsLb->count() )
		channelSelected( channelsLb->text( num-1 ) );
}



void CdWidget::next()
{
	if ( !channelsLb->count() )
		return;
	if ( (lastChannel+1)>(int)channelsLb->count() )
		return;
	lastChannel++;
	channelSelected( channelsLb->text( lastChannel-1 ) );
}



void CdWidget::previous()
{
	if ( !channelsLb->count() )
		return;
	if ( (lastChannel-1)<1 )
		return;
	lastChannel--;
	channelSelected( channelsLb->text( lastChannel-1 ) );
}



void CdWidget::channelSelected( const QString &name )
{
	int i;

	for ( i=0; i<(int)chan.count(); i++ ) {
		if ( chan.at(i)->name==name ) {
			emit setCurrentPlugin( this );
			dump->stop();
			dump->go( cdAddress, cdPort, *chan.at(i) );
			emit dvbOpen( fifoName, chan.at(i)->name, chan.at(i)->vpid );
			break;
		}
	}
}



void CdWidget::pauseLiveTV()
{
	if ( !dump )
		return;
	if ( dump->running() ) {
		timeShiftFileName = cdShiftDir+"DVBClient-"+QDateTime::currentDateTime().toString( Qt::ISODate )+".ts";
		if ( dump->doPause( timeShiftFileName ) ) emit setTimeShiftFilename( timeShiftFileName );
	}
}



void CdWidget::stopLive()
{
	if ( !dump )
		return;
	dump->stop();
	emit setTimeShiftFilename( "" );
}



void CdWidget::enableLive( bool b )
{
	channelsLb->setEnabled( b );
}



void CdWidget::setParam( const QString &ad, int port, int info, const QString &tspath )
{
	cdAddress = ad;
	cdPort = port;
	cdInfo = info;
	cdShiftDir = tspath;
	if ( !cdShiftDir.endsWith("/") ) cdShiftDir+= "/";
	cleaner->setPath( cdShiftDir );

	channelsLb->clear();
	chan.clear();
	if ( dump ) {
		if ( dump->running() )
			emit dvbStop();
		dump->stop();
	}
	listen->stop();
	listen->go( cdAddress, cdInfo );
}

#include "cdwidget.moc"
