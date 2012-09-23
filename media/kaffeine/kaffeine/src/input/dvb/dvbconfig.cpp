/*
 * dvbconfig.cpp
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

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include <qdir.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qregexp.h>
#include <qradiobutton.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <ktar.h>
#include <kstandarddirs.h>

#include "dvbconfig.h"
#include "kaffeinedvbplugin.h"
#include "dvbpanel.h"



MSpinBox::MSpinBox( QWidget *parent, int devNum ) : QSpinBox( 1, 4, 1, parent )
{
	deviceNumber = devNum;
	connect( this, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged(int)) );
}



void MSpinBox::slotValueChanged( int value )
{
	emit signalValueChanged( value, deviceNumber );
}



MPushButton::MPushButton( QWidget *parent, int devNum, int lnbNum ) : KPushButton( parent )
{
	deviceNumber = devNum;
	lnbNumber = lnbNum;
	connect( this, SIGNAL(clicked()), this, SLOT(isClicked()) );
}



void MPushButton::isClicked()
{
	emit clicked( deviceNumber, lnbNumber );
}



MCAMButton::MCAMButton( QWidget *parent, int devNum ) : QPushButton( i18n("CAM"), parent )
{
	deviceNumber = devNum;
	connect( this, SIGNAL(clicked()), this, SLOT(isClicked()) );
}



void MCAMButton::isClicked()
{
	emit clicked( deviceNumber );
}



MComboBox::MComboBox( QWidget *parent, int devNum, int lnbNum ) : QComboBox( parent )
{
	deviceNumber = devNum;
	lnbNumber = lnbNum;
	connect( this, SIGNAL(activated(int)), this, SLOT(isActivated(int)) );
}



void MComboBox::isActivated( int index )
{
	emit activated( index, deviceNumber, lnbNumber );
}



LNB::LNB()
{
	switchFreq = 11700;
	loFreq = 9750;
	hiFreq = 10600;
	rotorType = 0;
	speed13v = 2.5;
	speed18v = 1.5;
}



Device::Device( int anum, int tnum, fe_type_t t, const QString &n, bool as )
{
	adapter = anum;
	tuner = tnum;
	type = t;
	name = n;
	source = "";
	canAutoscan= as;
	tuningTimeout = 1500;
	hasCAM = false;
	camMaxService = 1;
	secMini = 0;
	secTwice = 0;
	priority = 10;
	doS2 = 0;
}



Category::Category( const QString &tname, const QString &ticon )
{
	name = tname;
	icon = ticon;
}



DVBconfig::DVBconfig( const QString &dvbConf )
{
	dvbConfigDir = dvbConf;
	dvbConfigIconsDir = dvbConf+"icons/";
	QDir dir;
	dir.setPath( dvbConfigIconsDir );
	if ( !dir.exists() )
		dir.mkdir( dvbConfigIconsDir );
	config = new KConfig( dvbConfigDir+"dvbrc" );
	downProgress = 0;
	sizeFile = 0;
	categories.setAutoDelete( true );
	devList.setAutoDelete( true );
	readFirst();
	startup();
	readConfig();
}



DVBconfig::~DVBconfig()
{
	saveConfig();
	delete config;
	categories.clear();
	devList.clear();
}



bool DVBconfig::haveDvbDevice()
{
	int i, j, res, fdFrontend=0;
	struct dvb_frontend_info info;

	QStringList list, flist;
	QDir d;
	d.setPath( "/dev/dvb/" );
	list = d.entryList( "adapter*", QDir::Dirs, QDir::Name );

	for ( i=0; i<(int)list.count(); i++ ) {
		d.setPath( "/dev/dvb/"+list[i]+"/" );
		flist = d.entryList( "frontend*", QDir::System, QDir::Name );
		for ( j=0; j<(int)flist.count(); j++ ) {
			fdFrontend = open( QString("/dev/dvb/%1/%2").arg( list[i] ).arg( flist[j] ).ascii(), O_RDWR);
			if ( fdFrontend>0 ) {
				if ( (res = ioctl( fdFrontend, FE_GET_INFO, &info ) < 0) )
					perror( QString("/dev/dvb/%1/%2 FE_GET_INFO: ").arg( list[i] ).arg( flist[j] ).ascii() );
				else {
					close( fdFrontend );
					return true;
				}
				close( fdFrontend );
			}
		}
	}

	return false;
}



void DVBconfig::startup()
{
	int i=0, j=0, res, fdFrontend=0;
	struct dvb_frontend_info info;
	bool as;
	QTime t1;

	QStringList list, flist;
	QString s, t;
	QDir d;
	d.setPath( "/dev/dvb/" );
	list = d.entryList( "adapter*", QDir::Dirs, QDir::Name );

	for ( i=0; i<(int)list.count(); i++ ) {
		d.setPath( "/dev/dvb/"+list[i]+"/" );
		flist = d.entryList( "frontend*", QDir::System, QDir::Name );
		for ( j=0; j<(int)flist.count(); j++ ) {
			s = list[i];
			t = flist[j];
			if ( devList.count()==MAX_DEVICES )
				break;
			if ( !probeMfe && t!="frontend0" )
				continue;
			t1 = QTime::currentTime();
			fdFrontend = open( QString("/dev/dvb/%1/%2").arg( s ).arg( t ).ascii(), O_RDWR);
			if ( fdFrontend>0 ) {
				if ( !(res = ioctl( fdFrontend, FE_GET_INFO, &info ) < 0) ) {
					if ( (info.type==FE_OFDM)
						&& (info.caps & FE_CAN_QAM_AUTO)
						&& (info.caps & FE_CAN_TRANSMISSION_MODE_AUTO)
						&& (info.caps & FE_CAN_GUARD_INTERVAL_AUTO)
						&& (info.caps & FE_CAN_HIERARCHY_AUTO)
						&& (info.caps & FE_CAN_FEC_AUTO) )
						as = true;
					else
						as = false;
					fprintf(stderr,"/dev/dvb/%s/%s : opened ( %s ) (%dms)\n", s.ascii(), t.ascii(), info.name, t1.msecsTo(QTime::currentTime()) );
					devList.append( new Device( s.replace("adapter","").toInt(), t.replace("frontend","").toInt(), info.type, info.name, as ) );
				}
				close( fdFrontend );
			}
			else {
				perror( QString("/dev/dvb/%1/%2  %3/%4").arg( s ).arg( t ).arg( errno ).arg( -EBUSY ).ascii() );
			}
		}
	}

	//devList.append( new Device( 3, 0, FE_QPSK, "Dummy S", false ) );
}



void DVBconfig::setDownloadResult( Job *job )
{
	if ( downProgress && job ) {
		delete downProgress;
		downProgress = 0;
	}
}



void DVBconfig::setDownloadPercent( Job *job, unsigned long percent )
{
	if ( downProgress && job )
		downProgress->progressBar()->setProgress( percent );
}



bool DVBconfig::loadDvbData( QWidget *parent )
{
	QString s="";
	FileCopyJob *job;
	QFile f( dvbConfigDir+"dvbdata.tar.gz" );

	//if ( f.exists() ) f.remove();
	downProgress = new KProgressDialog( parent, "progress", i18n("Downloading... "), i18n("Copying data files..."), true );
	downProgress->progressBar()->setTotalSteps( 100 );
	//job = file_copy( KURL( "http://hftom.free.fr/kaxtv/dvbdata.tar.gz" ), KURL( dvbConfigDir+"dvbdata.tar.gz" ), -1, true, false, false );
	job = file_copy( KURL( "http://hftom.free.fr/kaxtv/dvbdata.tar.gz" ), KURL( dvbConfigDir+"dvbdata.tar.gz" ), -1, true, false, false );
	connect( job, SIGNAL(result(KIO::Job*)), this, SLOT(setDownloadResult(KIO::Job*)) );
	connect( job, SIGNAL(percent(KIO::Job*,unsigned long)), this, SLOT(setDownloadPercent(KIO::Job*,unsigned long)) );
	downProgress->exec();
	disconnect( job, SIGNAL(result(KIO::Job*)), this, SLOT(setDownloadResult(KIO::Job*)) );
	disconnect( job, SIGNAL(percent(KIO::Job*,unsigned long)), this, SLOT(setDownloadPercent(KIO::Job*,unsigned long)) );
	if ( downProgress ) {
		delete downProgress;
		downProgress = 0;
	}
	KTar tar( dvbConfigDir+"dvbdata.tar.gz");
	if ( tar.open( IO_ReadOnly ) ) {
		tar.directory()->copyTo( dvbConfigDir );
		return true;
	}
	else
		return false;
}



bool DVBconfig::localData()
{
	QString s = locate("data","kaffeine/dvbdata.tar.gz");
	KTar tar( s );
	if ( tar.open( IO_ReadOnly ) ) {
		tar.directory()->copyTo( dvbConfigDir );
		return true;
	}
	else
		return false;
}



bool DVBconfig::haveData()
{
	if ( !QDir( dvbConfigDir+"dvb-s" ).exists() || !QDir( dvbConfigDir+"dvb-c" ).exists() || !QDir( dvbConfigDir+"dvb-t" ).exists() || !QDir( dvbConfigDir+"atsc" ).exists()) {
		loadDvbData(0);
		if ( !QDir( dvbConfigDir+"dvb-s" ).exists() || !QDir( dvbConfigDir+"dvb-c" ).exists() || !QDir( dvbConfigDir+"dvb-t" ).exists() || !QDir( dvbConfigDir+"atsc" ).exists() ) {
			if ( !localData() )
				return false;
		}
	}
	return true;
}



QStringList DVBconfig::getSourcesList( fe_type_t type )
{
	QString s;
	QStringList list;

	switch ( type ) {
		case FE_QPSK : s = "dvb-s"; break;
		case FE_QAM : s = "dvb-c"; break;
		case FE_OFDM : s = "dvb-t"; break;
		case FE_ATSC : s = "atsc"; break;
		default : return list;
	}
	list = QDir( dvbConfigDir+s ).entryList( QDir::Files, QDir::Name );
	return list;
}



void DVBconfig::addCategory( const QString &name, const QString &icon )
{
	categories.append( new Category( name, icon ) );
}



void DVBconfig::removeCategory( const QString &name )
{
	int i;

	for ( i=0; i<(int)categories.count(); i++ ) {
		if ( categories.at(i)->name==name ) {
			categories.remove( i );
			break;
		}
	}
}



void DVBconfig::changeIconCategory( const QString &name, const QString &icon )
{
	int i;

	if ( name==i18n("All") )
		allIcon = icon;
	else if ( name==i18n("TV") )
		tvIcon = icon;
	else if ( name==i18n("Radio") )
		radioIcon = icon;
	else {
		for ( i=0; i<(int)categories.count(); i++ ) {
			if ( categories.at(i)->name==name ) {
				categories.at(i)->icon = icon;
				break;
			}
		}
	}
}



int DVBconfig::readDvbChanOrder()
{
	config->setGroup( "DVB Options" );
	int sort = config->readNumEntry("ChannelsSorting", 0);
	return sort;
}



void DVBconfig::saveDvbChanOrder( int s, int col )
{
	int sort = (s<<1) | col;
	config->setGroup( "DVB Options" );
	config->writeEntry("ChannelsSorting", sort);
	config->sync();
}



void DVBconfig::readFirst()
{
	config->setGroup( "DVB Options" );
	probeMfe = config->readNumEntry( "ProbeMFE", 1 );
}



void DVBconfig::readConfig()
{
	QSize size;
	QString s;
	int i, j;

	config->setGroup( "DVB Options" );
	broadcastAddress = config->readEntry( "BroadcastAddress", "192.168.0.255" );
	broadcastPort = config->readNumEntry( "BroadcastPort", 1234 );
	senderPort = config->readNumEntry( "SenderPort", 1235 );
	size = QSize(600, 350);
	epgSize = config->readSizeEntry( "EPG Geometry", &size );
	size = QSize(600, 300);
	timerSize = config->readSizeEntry( "Timers Geometry", &size );
	size = QSize(300, 300);
	scanSize = config->readSizeEntry( "Scan Geometry", &size );
	beginMargin = config->readNumEntry( "BeginMargin", 5 );
	endMargin = config->readNumEntry( "EndMargin", 10 );
	instantDuration = config->readNumEntry( "InstantDuration", 120 );
	recordDir = config->readEntry( "RecordDir", QDir::homeDirPath() );
	if ( !recordDir.endsWith("/") )
		recordDir+= "/";
	sizeFile = config->readNumEntry("SizeFile",0);
	shiftDir = config->readEntry( "ShiftDir", QDir::homeDirPath() );
	if ( !shiftDir.endsWith("/") )
		shiftDir+= "/";
	filenameFormat = config->readEntry( "filenameFormat", "%name" );
	for ( i=0; i<(int)devList.count(); i++ ) {
		devList.at(i)->source = config->readEntry( QString("DVB%1").arg(i), "" );
		devList.at(i)->tuningTimeout = config->readNumEntry( QString("DVB%1_TIMEOUT").arg(i), 1500 );
		devList.at(i)->camMaxService = config->readNumEntry( QString("DVB%1_CAM_MAX").arg(i), 1 );
		devList.at(i)->priority = config->readNumEntry( QString("DVB%1_PRIORITY").arg(i), 10 );
		if ( devList.at(i)->type!=FE_QPSK )
			continue;
		devList.at(i)->numLnb = config->readNumEntry( QString("DVB%1_NLNB").arg(i), 1 );
		for ( j=0; j<devList.at(i)->numLnb; j++ ) {
			devList.at(i)->lnb[j].switchFreq = config->readNumEntry( QString("DVB%1_LNB%2_switch").arg(i).arg(j), 11700 );
			devList.at(i)->lnb[j].loFreq = config->readNumEntry( QString("DVB%1_LNB%2_lo").arg(i).arg(j), 9750 );
			devList.at(i)->lnb[j].hiFreq = config->readNumEntry( QString("DVB%1_LNB%2_hi").arg(i).arg(j), 10600 );
			devList.at(i)->lnb[j].rotorType = config->readNumEntry( QString("DVB%1_LNB%2_rotor").arg(i).arg(j), 0 );
			devList.at(i)->lnb[j].source = config->readListEntry( QString("DVB%1_LNB%2_source").arg(i).arg(j) );
			devList.at(i)->lnb[j].position = config->readIntListEntry( QString("DVB%1_LNB%2_position").arg(i).arg(j) );
			devList.at(i)->lnb[j].speed13v = config->readDoubleNumEntry( QString("DVB%1_LNB%2_speed13v").arg(i).arg(j), 2.5 );
			devList.at(i)->lnb[j].speed18v = config->readDoubleNumEntry( QString("DVB%1_LNB%2_speed18v").arg(i).arg(j), 1.5 );
		}
		devList.at(i)->secMini = config->readNumEntry( QString("DVB%1_SEC_MINI").arg(i), 0 );
		devList.at(i)->secTwice = config->readNumEntry( QString("DVB%1_SEC_TWICE").arg(i), 0 );
		devList.at(i)->doS2 = config->readNumEntry( QString("DVB%1_DOS2").arg(i), 0 );
	}
	j = config->readNumEntry( "NumCategories", 0 );
	for ( i=0; i<j; i++ )
		categories.append( new Category( config->readEntry( QString("CategoryName%1").arg(i), "" ), config->readEntry( QString("CategoryIcon%1").arg(i), "kaffeine" ) ) );
	allIcon = config->readEntry( "AllIcon", "kaffeine" );
	tvIcon = config->readEntry( "TvIcon", "kdvbtv" );
	radioIcon = config->readEntry( "RadioIcon", "kdvbra" );
	lastChannel = config->readNumEntry( "LastChannel", 1 );
	splitSizes = config->readIntListEntry("SplitSizes");
	defaultCharset = config->readEntry( "DefaultCharset", "ISO8859-1" );
	usalsLatitude = config->readDoubleNumEntry( "UsalsLatitude", 0.0 );
	usalsLongitude = config->readDoubleNumEntry( "UsalsLongitude", 0.0 );
	for ( i=0; i<(int)devList.count(); i++ ) {
		devList.at(i)->usalsLatitude = usalsLatitude;
		devList.at(i)->usalsLongitude = usalsLongitude;
	}
	ringBufSize = config->readNumEntry( "RingBufSize", 2 );
	if ( ringBufSize<2 )
		ringBufSize = 2;
}



void DVBconfig::saveConfig()
{
	int i, j;

	config->setGroup( "DVB Options" );
	config->writeEntry( "EPG Geometry", epgSize );
	config->writeEntry( "Timers Geometry", timerSize );
	config->writeEntry( "Scan Geometry", scanSize );
	config->writeEntry( "BeginMargin", beginMargin );
	config->writeEntry( "EndMargin", endMargin );
	config->writeEntry( "InstantDuration", instantDuration );
	config->writeEntry( "RecordDir", recordDir );
	config->writeEntry( "ShiftDir", shiftDir );
	config->writeEntry( "filenameFormat", filenameFormat );
	config->writeEntry( "BroadcastAddress", broadcastAddress );
	config->writeEntry( "BroadcastPort", broadcastPort );
	config->writeEntry( "SenderPort", senderPort );
	config->writeEntry( "ProbeMFE", probeMfe );
	for ( i=0; i<(int)devList.count(); i++ ) {
		config->writeEntry( QString("DVB%1").arg(i), devList.at(i)->source );
		config->writeEntry( QString("DVB%1_TIMEOUT").arg(i), devList.at(i)->tuningTimeout );
		config->writeEntry( QString("DVB%1_PRIORITY").arg(i), devList.at(i)->priority );
		config->writeEntry( QString("DVB%1_CAM_MAX").arg(i), devList.at(i)->camMaxService );
		if ( devList.at(i)->type!=FE_QPSK )
			continue;
		config->writeEntry( QString("DVB%1_NLNB").arg(i), devList.at(i)->numLnb );
		for ( j=0; j<devList.at(i)->numLnb; j++ ) {
			config->writeEntry( QString("DVB%1_LNB%2_switch").arg(i).arg(j), devList.at(i)->lnb[j].switchFreq );
			config->writeEntry( QString("DVB%1_LNB%2_lo").arg(i).arg(j), devList.at(i)->lnb[j].loFreq );
			config->writeEntry( QString("DVB%1_LNB%2_hi").arg(i).arg(j), devList.at(i)->lnb[j].hiFreq );
			config->writeEntry( QString("DVB%1_LNB%2_rotor").arg(i).arg(j), devList.at(i)->lnb[j].rotorType );
			config->writeEntry( QString("DVB%1_LNB%2_source").arg(i).arg(j), devList.at(i)->lnb[j].source );
			config->writeEntry( QString("DVB%1_LNB%2_position").arg(i).arg(j), devList.at(i)->lnb[j].position );
			config->writeEntry( QString("DVB%1_LNB%2_speed13v").arg(i).arg(j), devList.at(i)->lnb[j].speed13v );
			config->writeEntry( QString("DVB%1_LNB%2_speed18v").arg(i).arg(j), devList.at(i)->lnb[j].speed18v );
		}
		config->writeEntry( QString("DVB%1_SEC_MINI").arg(i), devList.at(i)->secMini );
		config->writeEntry( QString("DVB%1_SEC_TWICE").arg(i), devList.at(i)->secTwice );
		config->writeEntry( QString("DVB%1_DOS2").arg(i), devList.at(i)->doS2 );
	}
	config->writeEntry( "NumCategories", categories.count() );
	for ( i=0; i<(int)categories.count(); i++ ) {
		config->writeEntry( QString("CategoryName%1").arg(i), categories.at(i)->name );
		config->writeEntry( QString("CategoryIcon%1").arg(i), categories.at(i)->icon );
	}
	config->writeEntry( "AllIcon", allIcon );
	config->writeEntry( "TvIcon", tvIcon );
	config->writeEntry( "RadioIcon", radioIcon );
	config->writeEntry( "LastChannel", lastChannel );
	config->writeEntry( "SplitSizes", splitSizes );
	config->writeEntry( "DefaultCharset", defaultCharset );
	config->writeEntry( "UsalsLatitude", usalsLatitude );
	config->writeEntry( "UsalsLongitude", usalsLongitude );
	config->writeEntry( "SizeFile", sizeFile );
	config->writeEntry( "RingBufSize", ringBufSize );
	config->sync();
}



bool DVBconfig::firstRun()
{
	config->setGroup( "DVB Options" );
	if ( config->readNumEntry( "FirstRun", 0 )<3 ) {
		config->writeEntry( "FirstRun", 3 );
		return true;
	}
	return false;
}



DvbConfigDialog::DvbConfigDialog( DvbPanel *pan, DVBconfig *dc, QWidget *parent, KaffeineDvbPlugin *p ) :
	KDialogBase ( IconList, i18n("DVB Settings"), Ok|Cancel, Ok, parent, "dvbConfigDialog", true, true )
{
	QLabel *lab;
	KIconLoader *icon = new KIconLoader();
	QHBoxLayout *h1;
	QString s;
	int i;
	QVBoxLayout *vb;
	QGroupBox *gb;
	QGridLayout *grid, *sgrid;
	QLabel *ident;
	QLabel *dvbType;
	int gridLine;
	QFrame *page;
	QSpinBox *spin;
	KPushButton *usals;
	QWidget *swidg;
	QStringList rotorList; rotorList<<i18n("No rotor")<<i18n("USALS rotor")<<i18n("Positions rotor")<<i18n("External positionner");

	dvbConfig = dc;
	timeoutSpin.setAutoDelete( true );

	for ( i=0; i<(int)dvbConfig->devList.count(); i++ ) {
		page = addPage( i18n("DVB Device")+" "+QString("%1:%2").arg(dvbConfig->devList.at(i)->adapter).arg(dvbConfig->devList.at(i)->tuner), i18n("Device Settings"),
			KGlobal::instance()->iconLoader()->loadIcon( "hwinfo", KIcon::NoGroup, KIcon::SizeMedium ) );
		vb = new QVBoxLayout( page, 6, 6 );
		gb = new QGroupBox( "", page );
		grid = new QGridLayout( gb, 1, 1, 20, 6 );
		gridLine = 0;

		lab = new QLabel( i18n("<qt><b>Name:</b></qt>"), gb );
		grid->addWidget( lab, gridLine, 0 );
		ident = new QLabel( dvbConfig->devList.at(i)->name, gb );
		grid->addMultiCellWidget( ident, gridLine, gridLine, 1, 3 );
		++gridLine;

		lab = new QLabel( i18n("<qt><b>Type:</b></qt>"), gb );
		grid->addWidget( lab, gridLine, 0 );
		dvbType = new QLabel( gb );
		switch ( dvbConfig->devList.at(i)->type ) {
			case FE_QAM : dvbType->setText( i18n("Cable") ); break;
			case FE_OFDM : dvbType->setText( i18n("Terrestrial") ); break;
			case FE_QPSK : dvbType->setText( i18n("Satellite") ); break;
			case FE_ATSC : dvbType->setText( i18n("Atsc") ); break;
			default : dvbType->setText( i18n("Unknown") );
		}
		if ( dvbConfig->devList.at(i)->hasCAM ) {
			grid->addWidget( dvbType, gridLine, 1 );
			MCAMButton *camb = new MCAMButton( gb, i );
			connect( camb, SIGNAL(clicked(int)), pan, SLOT(camClicked(int)) );
			grid->addWidget( camb, gridLine, 2 );
		}
		else
			grid->addMultiCellWidget( dvbType, gridLine, gridLine, 1, 3 );
		++gridLine;
		
		lab = new QLabel( i18n("Tuner priority (0=Don't use):"), gb );
		grid->addWidget( lab, gridLine, 0 );
		spin = new QSpinBox( 0, 99, 1, gb );
		spin->setValue( dvbConfig->devList.at(i)->priority );
		priority.append( spin );
		grid->addWidget( spin, gridLine, 1 );
		++gridLine;

		lab = new QLabel( i18n("Tuner timeout :"), gb );
		grid->addWidget( lab, gridLine, 0 );
		spin = new QSpinBox( 500, 5000, 100, gb );
		spin->setValue( dvbConfig->devList.at(i)->tuningTimeout );
		timeoutSpin.append( spin );
		grid->addWidget( spin, gridLine, 1 );
		lab = new QLabel( i18n("(ms)"), gb );
		grid->addWidget( lab, gridLine, 2 );
		++gridLine;

		if ( dvbConfig->devList.at(i)->type==FE_QPSK ) {
			doS2[i] = new QCheckBox( i18n("S2 capable device"), gb );
			doS2[i]->setChecked( dvbConfig->devList.at(i)->doS2 );
			grid->addWidget( doS2[i], gridLine, 0 );
			++gridLine;
			
			lab = new QLabel( i18n("Number of LNBs:"), gb );
			grid->addWidget( lab, gridLine, 0 );
			satNumber[i] = new MSpinBox( gb, i );
			connect( satNumber[i], SIGNAL(signalValueChanged(int,int)), this, SLOT(satNumberChanged(int,int)));
			grid->addWidget( satNumber[i], gridLine, 1 );
			usals = new KPushButton( gb );
			usals->setGuiItem( KGuiItem(i18n("Set rotor coordinates..."), icon->loadIconSet("move", KIcon::Small) ) );
			connect( usals, SIGNAL(clicked()), this, SLOT(setUsals()));
			grid->addWidget( usals, gridLine, 2 );

			++gridLine;
			
			secMini[i] = new QCheckBox( i18n("Mini DiSEqC (A-B)."), gb );
			secMini[i]->setChecked( dvbConfig->devList.at(i)->secMini );
			secMini[i]->setEnabled( false );
			grid->addWidget( secMini[i], gridLine, 1 );
			secTwice[i] = new QCheckBox( i18n("Send DiSEqC commands twice."), gb );
			secTwice[i]->setChecked( dvbConfig->devList.at(i)->secTwice );
			grid->addWidget( secTwice[i], gridLine, 0 );
			
			++gridLine;

			lnb0[i] = new MPushButton( gb, i, 0 );
			lnb0[i]->setGuiItem( KGuiItem(i18n("LNB 1 settings..."), icon->loadIconSet("hwinfo", KIcon::Small) ) );
			lnb0[i]->setEnabled(true);
			grid->addWidget( lnb0[i], gridLine, 0 );
			connect( lnb0[i], SIGNAL(clicked(int,int)), this, SLOT(setLnb(int,int)));
			rotor0[i] = new MComboBox( gb, i, 0 );
			rotor0[i]->insertStringList( rotorList );
			rotor0[i]->setCurrentItem( dvbConfig->devList.at(i)->lnb[0].rotorType );
			grid->addWidget( rotor0[i], gridLine, 1 );
			connect( rotor0[i], SIGNAL(activated(int,int,int)), this, SLOT(setRotor(int,int,int)));
			swidg = new QWidget( gb );
			sgrid = new QGridLayout( swidg, 1, 1, 0, 0 );
			sat0[i] = new QComboBox( swidg );
			sat0[i]->insertStringList( dvbConfig->getSourcesList(dvbConfig->devList.at(i)->type) );
			sgrid->addWidget( sat0[i], 0, 0 );
			src0[i] = new MPushButton( swidg, i, 0 );
			src0[i]->setGuiItem( KGuiItem(i18n("Sources list...") ) );
			connect( src0[i], SIGNAL(clicked(int,int)), this, SLOT(setRotorSources(int,int)) );
			sgrid->addWidget( src0[i], 1, 0 );
			if ( dvbConfig->devList.at(i)->lnb[0].rotorType==0 ) {
				setSource( sat0[i], dvbConfig->devList.at(i)->lnb[0].source[0] );
				src0[i]->hide();
			}
			else {
				sat0[i]->hide();
			}
			grid->addWidget( swidg, gridLine, 2 );
			++gridLine;

			lnb1[i] = new MPushButton( gb, i, 1 );
			lnb1[i]->setGuiItem( KGuiItem(i18n("LNB 2 settings..."), icon->loadIconSet("hwinfo", KIcon::Small) ) );
			lnb1[i]->setEnabled(false);
			grid->addWidget( lnb1[i], gridLine, 0 );
			connect( lnb1[i], SIGNAL(clicked(int,int)), this, SLOT(setLnb(int,int)));
			rotor1[i] = new MComboBox( gb, i, 1 );
			rotor1[i]->setEnabled( false );
			rotor1[i]->insertStringList( rotorList );
			rotor1[i]->setCurrentItem( dvbConfig->devList.at(i)->lnb[1].rotorType );
			grid->addWidget( rotor1[i], gridLine, 1 );
			connect( rotor1[i], SIGNAL(activated(int,int,int)), this, SLOT(setRotor(int,int,int)));
			swidg = new QWidget( gb );
			sgrid = new QGridLayout( swidg, 1, 1, 0, 0 );
			sat1[i] = new QComboBox( swidg );
			sat1[i]->setEnabled(false);
			sat1[i]->insertStringList( dvbConfig->getSourcesList(dvbConfig->devList.at(i)->type) );
			sgrid->addWidget( sat1[i], 0, 0 );
			src1[i] = new MPushButton( swidg, i, 1 );
			src1[i]->setEnabled(false);
			src1[i]->setGuiItem( KGuiItem(i18n("Sources list...") ) );
			connect( src1[i], SIGNAL(clicked(int,int)), this, SLOT(setRotorSources(int,int)) );
			sgrid->addWidget( src1[i], 1, 0 );
			if ( dvbConfig->devList.at(i)->lnb[1].rotorType==0 ) {
				setSource( sat1[i], dvbConfig->devList.at(i)->lnb[1].source[0] );
				src1[i]->hide();
			}
			else {
				sat1[i]->hide();
			}
			grid->addWidget( swidg, gridLine, 2 );
			++gridLine;

			lnb2[i] = new MPushButton( gb, i, 2 );
			lnb2[i]->setGuiItem( KGuiItem(i18n("LNB 3 settings..."), icon->loadIconSet("hwinfo", KIcon::Small) ) );
			lnb2[i]->setEnabled(false);
			grid->addWidget( lnb2[i], gridLine, 0 );
			connect( lnb2[i], SIGNAL(clicked(int,int)), this, SLOT(setLnb(int,int)));
			rotor2[i] = new MComboBox( gb, i, 2 );
			rotor2[i]->setEnabled(false);
			rotor2[i]->insertStringList( rotorList );
			rotor2[i]->setCurrentItem( dvbConfig->devList.at(i)->lnb[2].rotorType );
			grid->addWidget( rotor2[i], gridLine, 1 );
			connect( rotor2[i], SIGNAL(activated(int,int,int)), this, SLOT(setRotor(int,int,int)));
			swidg = new QWidget( gb );
			sgrid = new QGridLayout( swidg, 1, 1, 0, 0 );
			sat2[i] = new QComboBox( swidg );
			sat2[i]->setEnabled(false);
			sat2[i]->insertStringList( dvbConfig->getSourcesList(dvbConfig->devList.at(i)->type) );
			sgrid->addWidget( sat2[i], 0, 0 );
			src2[i] = new MPushButton( swidg, i, 2 );
			src2[i]->setEnabled(false);
			src2[i]->setGuiItem( KGuiItem(i18n("Sources list...") ) );
			connect( src2[i], SIGNAL(clicked(int,int)), this, SLOT(setRotorSources(int,int)) );
			sgrid->addWidget( src2[i], 1, 0 );
			if ( dvbConfig->devList.at(i)->lnb[2].rotorType==0 ) {
				setSource( sat2[i], dvbConfig->devList.at(i)->lnb[2].source[0] );
				src2[i]->hide();
			}
			else {
				sat2[i]->hide();
			}
			grid->addWidget( swidg, gridLine, 2 );
			++gridLine;

			lnb3[i] = new MPushButton( gb, i, 3 );
			lnb3[i]->setGuiItem( KGuiItem(i18n("LNB 4 settings..."), icon->loadIconSet("hwinfo", KIcon::Small) ) );
			lnb3[i]->setEnabled(false);
			grid->addWidget( lnb3[i], gridLine, 0 );
			connect( lnb3[i], SIGNAL(clicked(int,int)), this, SLOT(setLnb(int,int)));
			rotor3[i] = new MComboBox( gb, i, 3 );
			rotor3[i]->setEnabled(false);
			rotor3[i]->insertStringList( rotorList );
			rotor3[i]->setCurrentItem( dvbConfig->devList.at(i)->lnb[3].rotorType );
			grid->addWidget( rotor3[i], gridLine, 1 );
			connect( rotor3[i], SIGNAL(activated(int,int,int)), this, SLOT(setRotor(int,int,int)));
			swidg = new QWidget( gb );
			sgrid = new QGridLayout( swidg, 1, 1, 0, 0 );
			sat3[i] = new QComboBox( swidg );
			sat3[i]->setEnabled(false);
			sat3[i]->insertStringList( dvbConfig->getSourcesList(dvbConfig->devList.at(i)->type) );
			sgrid->addWidget( sat3[i], 0, 0 );
			src3[i] = new MPushButton( swidg, i, 3 );
			src3[i]->setEnabled(false);
			src3[i]->setGuiItem( KGuiItem(i18n("Sources list...") ) );
			connect( src3[i], SIGNAL(clicked(int,int)), this, SLOT(setRotorSources(int,int)) );
			sgrid->addWidget( src3[i], 1, 0 );
			if ( dvbConfig->devList.at(i)->lnb[3].rotorType==0 ) {
				setSource( sat3[i], dvbConfig->devList.at(i)->lnb[3].source[0] );
				src3[i]->hide();
			}
			else {
				sat3[i]->hide();
			}
			grid->addWidget( swidg, gridLine, 2 );
			++gridLine;

			satNumber[i]->setValue( dvbConfig->devList.at(i)->numLnb );
		}
		else {
			lab = new QLabel( i18n("Source:"), gb );
			grid->addWidget( lab, gridLine, 0 );
			sat0[i] = new QComboBox( gb );
			if ( dvbConfig->devList.at(i)->canAutoscan )
				sat0[i]->insertItem( "AUTO" );
			sat0[i]->insertStringList( dvbConfig->getSourcesList(dvbConfig->devList.at(i)->type) );
			setSource( sat0[i], dvbConfig->devList.at(i)->source );
			grid->addWidget( sat0[i], gridLine, 1 );
			++gridLine;

			if ( dvbConfig->devList.at(i)->canAutoscan ) {
				lab = new QLabel( i18n("<qt>This device seems to support the <b><i>autoscan</i></b> feature. "
					"You can choose <b>AUTO</b> in Source list to let Kaffeine "
					"search for a range of frequencies.<br>"
					"If <b><i>autoscan</i></b> fails to find your channels, choose a real Source in list.</qt>"), gb );
				grid->addMultiCellWidget( lab, gridLine, gridLine, 0, 3 );
				++gridLine;
			}
		}
		lab = new QLabel( i18n("<qt><i>If you can't find your network/location in the list, you'll have to create one. "
			"Look in $HOME/.kde/share/apps/kaffeine/dvb-x/ and take an existing file as start point. "
			"Fill in with the values for your network/location and give it a sensible name "
			"(follow the naming convention). If you think your new file could be usefull for others, send it to "
			"kaffeine-user(AT)lists.sf.net.</i></qt>"), gb );
		grid->addMultiCellWidget( lab, gridLine, gridLine, 0, 3 );

		vb->addWidget( gb );
		vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );
	}

	page = addPage(i18n("Recording"),i18n("DVB Recording Options"),
		KGlobal::instance()->iconLoader()->loadIcon( "hdd_unmount", KIcon::NoGroup, KIcon::SizeMedium ) );
	vb = new QVBoxLayout( page, 6, 6 );
	gb = new QGroupBox( "", page );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );

	lab = new QLabel( i18n("Records directory:"), gb );
	grid->addWidget( lab, 0, 0 );
	recordDirLe = new QLineEdit( gb );
	recordDirLe->setReadOnly( true );
	grid->addWidget( recordDirLe, 0, 1 );
	recordDirBtn = new QToolButton( gb );
	recordDirBtn->setIconSet( icon->loadIconSet("fileopen", KIcon::Small) );
	grid->addWidget( recordDirBtn, 0, 2 );

	lab = new QLabel( i18n("Time shifting directory:"), gb );
	grid->addWidget( lab, 1, 0 );
	shiftDirLe = new QLineEdit( gb );
	shiftDirLe->setReadOnly( true );
	grid->addWidget( shiftDirLe, 1, 1 );
	shiftDirBtn = new QToolButton( gb );
	shiftDirBtn->setIconSet( icon->loadIconSet("fileopen", KIcon::Small) );
	grid->addWidget( shiftDirBtn, 1, 2 );

	lab = new QLabel( i18n("Begin margin:"), gb );
	grid->addWidget( lab, 2, 0 );
	beginSpin = new QSpinBox( gb );
	h1 = new QHBoxLayout();
	h1->addWidget( beginSpin );
	lab = new QLabel( i18n("(minutes)"), gb );
	h1->addWidget( lab );
	grid->addLayout( h1, 2, 1 );

	lab = new QLabel( i18n("End margin:"), gb );
	grid->addWidget( lab, 3, 0 );
	endSpin = new QSpinBox( gb );
	h1 = new QHBoxLayout();
	h1->addWidget( endSpin );
	lab = new QLabel( i18n("(minutes)"), gb );
	h1->addWidget( lab );
	grid->addLayout( h1, 3, 1 );

	lab = new QLabel( i18n("Instant record duration:"), gb );
	grid->addWidget( lab, 4, 0 );
	instantDurationSpin = new QSpinBox( 1, 1440, 1, gb );
	h1 = new QHBoxLayout();
	h1->addWidget( instantDurationSpin );
	lab = new QLabel( i18n("(minutes)"), gb );
	h1->addWidget( lab );
	grid->addLayout( h1, 4, 1 );

	lab = new QLabel( i18n("Max file size (0=Unlimited):"), gb );
	grid->addWidget( lab, 5, 0 );
	sizeFileSpin = new QSpinBox( 1, 9999, 1, gb );
	sizeFileSpin->setMinValue(0);
	h1 = new QHBoxLayout();
	h1->addWidget( sizeFileSpin );
	lab = new QLabel( i18n("(MB)"), gb );
	h1->addWidget( lab );
	grid->addLayout( h1, 5, 1 );

	lab = new QLabel( i18n("Filename Format:"), gb );
	grid->addWidget( lab, 7, 0 );
	filenameFormatLe = new QLineEdit( gb );
	grid->addWidget( filenameFormatLe, 7, 1 );
	helpNameBtn = new QToolButton( gb );
	helpNameBtn->setIconSet( icon->loadIconSet("help", KIcon::Small) );
	grid->addWidget( helpNameBtn, 7, 2 );

	vb->addWidget( gb );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	recordDirLe->setText( dvbConfig->recordDir );
	shiftDirLe->setText( dvbConfig->shiftDir );
	beginSpin->setValue( dvbConfig->beginMargin );
	endSpin->setValue( dvbConfig->endMargin );
	instantDurationSpin->setValue( dvbConfig->instantDuration );
	sizeFileSpin->setValue( dvbConfig->sizeFile );
	filenameFormatLe->setText( dvbConfig->filenameFormat );

	page = addPage(i18n("Broadcasting"),i18n("DVB Broadcasting"),
		KGlobal::instance()->iconLoader()->loadIcon( "network_local", KIcon::NoGroup, KIcon::SizeMedium ) );
	vb = new QVBoxLayout( page, 6, 6 );
	gb = new QGroupBox( "", page );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );

	lab = new QLabel( i18n("Broadcast address:"), gb );
	grid->addWidget( lab, 0, 0 );
	broadcastLe = new QLineEdit( gb );
	grid->addWidget( broadcastLe, 0, 1 );
	lab = new QLabel( i18n("Broadcast port:"), gb );
	grid->addWidget( lab, 1, 0 );
	bportSpin = new QSpinBox( 1, 65535, 1, gb );
	grid->addWidget( bportSpin, 1, 1 );
	lab = new QLabel( i18n("Info port:"), gb );
	grid->addWidget( lab, 2, 0 );
	sportSpin = new QSpinBox( 1, 65535, 1, gb );
	grid->addWidget( sportSpin, 2, 1 );

	vb->addWidget( gb );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	broadcastLe->setText( dvbConfig->broadcastAddress );
	bportSpin->setValue( dvbConfig->broadcastPort );
	sportSpin->setValue( dvbConfig->senderPort );

	page = addPage(i18n("Misc"),i18n("Misc"),
		KGlobal::instance()->iconLoader()->loadIcon( "misc", KIcon::NoGroup, KIcon::SizeMedium ) );
	vb = new QVBoxLayout( page, 6, 6 );
	gb = new QGroupBox( "", page );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );
	
	probeMfe = new QCheckBox( i18n("Probe Multiple-Frontends (Restart required)."), gb );
	probeMfe->setChecked( dvbConfig->probeMfe );
	grid->addWidget( probeMfe, 0, 0 );
	
	lab = new QLabel( i18n("LiveShow ringbuffer size (MB) :"), gb );
	grid->addWidget( lab, 1, 0 );
	ringBufSize = new QSpinBox( 2, 99, 1, gb );
	ringBufSize->setValue( dvbConfig->ringBufSize );
	grid->addWidget( ringBufSize, 1, 1 );

	lab = new QLabel( i18n("Default charset (restart needed):"), gb );
	grid->addWidget( lab, 2, 0 );
	charsetComb = new QComboBox( gb );
	charsetComb->insertItem( "ISO8859-1" );
	charsetComb->insertItem( "ISO6937" );
	if ( dvbConfig->defaultCharset=="ISO8859-1" )
		charsetComb->setCurrentItem( 0 );
	else if ( dvbConfig->defaultCharset=="ISO6937" )
		charsetComb->setCurrentItem( 1 );
	grid->addWidget( charsetComb, 2, 1 );

	lab = new QLabel( i18n("Update scan data:"), gb );
	grid->addWidget( lab, 3, 0 );
	updateBtn = new KPushButton( gb );
	updateBtn->setGuiItem( KGuiItem(i18n("Download"), icon->loadIconSet("khtml_kget", KIcon::Small) ) );
	grid->addWidget( updateBtn, 3, 1 );

	lab = new QLabel( i18n("Dump epg's events to \n~/kaffeine_dvb_events.tx:"), gb );
	grid->addWidget( lab, 4, 0 );
	dumpBtn = new KPushButton( gb );
	dumpBtn->setGuiItem( KGuiItem(i18n("Dump"), icon->loadIconSet("filesave", KIcon::Small) ) );
	grid->addWidget( dumpBtn, 4, 1 );

	vb->addWidget( gb );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	if ( p ) {
		page = addPage(i18n("DVB plugins"),i18n("DVB plugins"),
			KGlobal::instance()->iconLoader()->loadIcon( "misc", KIcon::NoGroup, KIcon::SizeMedium ) );
		vb = new QVBoxLayout( page, 6, 6 );
		gb = new QGroupBox( "", page );
		grid = new QGridLayout( gb, 1, 1, 20, 6 );

		KPushButton *btn = new KPushButton( p->pluginName(), gb );
		grid->addWidget( btn, 0, 0 );
		connect( btn, SIGNAL(clicked()), p, SLOT(configDialog()) );
		vb->addWidget( gb );
		vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );
	}

	connect( recordDirBtn, SIGNAL(clicked()), this, SLOT(setRecordDir()) );
	connect( shiftDirBtn, SIGNAL(clicked()), this, SLOT(setShiftDir()) );
	connect( updateBtn, SIGNAL(clicked()), this, SLOT(downloadData()) );
	connect( helpNameBtn, SIGNAL(clicked()), this, SLOT(fileTemplateHelp()) );
	delete icon;
}



void DvbConfigDialog::setUsals()
{
	KDialogBase *dlg = new KDialogBase( this, "usalsConfigDialog", true, i18n("Rotors settings"), Ok|Cancel, Ok, true );
	QGridLayout *grid;
	QWidget *page = new QWidget( dlg );
	dlg->setMainWidget( page );

	QVBoxLayout *vb = new QVBoxLayout( page, 6, 6 );
	QLabel *lab = new QLabel( i18n("Set your position coordinates for rotors:"), page );
	vb->addWidget( lab );
	lab = new QLabel( "(Sydney: -33.8916, 151.2417 - New York: 40.7139, -74.0063)", page );
	vb->addWidget( lab );
	grid = new QGridLayout( 0, 1, 1 );
	lab = new QLabel( i18n("Latitude:"), page );
	grid->addWidget( lab, 0, 0 );
	QLineEdit *latitude = new QLineEdit( page );
	latitude->setText( QString().setNum( dvbConfig->usalsLatitude ) );
	grid->addWidget( latitude, 0, 1 );
	lab = new QLabel( i18n("Longitude:"), page );
	grid->addWidget( lab, 1, 0 );
	QLineEdit *longitude = new QLineEdit( page );
	longitude->setText( QString().setNum( dvbConfig->usalsLongitude ) );
	grid->addWidget( longitude, 1, 1 );
	vb->addLayout( grid );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	if ( dlg->exec()==QDialog::Accepted ) {
		dvbConfig->usalsLatitude = latitude->text().toDouble();
		dvbConfig->usalsLongitude = longitude->text().toDouble();
		for ( int i=0; i<(int)dvbConfig->devList.count(); i++ ) {
			dvbConfig->devList.at(i)->usalsLatitude = dvbConfig->usalsLatitude;
			dvbConfig->devList.at(i)->usalsLongitude = dvbConfig->usalsLongitude;
		}
	}
}



void DvbConfigDialog::setRotorSources( int devNum, int lnbNum )
{
	RotorConfig rotor( dvbConfig->devList.at(devNum), dvbConfig, lnbNum, this );
	rotor.exec();
}



void DvbConfigDialog::setRotor( int index, int devNum, int lnbNum )
{
	QComboBox *comb;
	QPushButton *btn;

	switch ( lnbNum ) {
		case 0 : comb=sat0[devNum]; btn=src0[devNum]; break;
		case 1 : comb=sat1[devNum]; btn=src1[devNum]; break;
		case 2 : comb=sat2[devNum]; btn=src2[devNum]; break;
		case 3 : comb=sat3[devNum]; btn=src3[devNum]; break;
		default : return;
	}

	dvbConfig->devList.at(devNum)->lnb[lnbNum].rotorType=index;

	if ( index==0 ) {
		btn->hide();
		comb->show();
	}
	else {
		comb->hide();
		btn->show();
	}
}



void DvbConfigDialog::setLnb( int devNum, int lnbNum )
{
	LnbConfig lnb( &dvbConfig->devList.at(devNum)->lnb[lnbNum], this );
	lnb.exec();
}



void DvbConfigDialog::downloadData()
{
	int ret;

loop:
	if ( !dvbConfig->loadDvbData(0) ) {
		ret = KMessageBox::questionYesNo( this, i18n("<qt>Can't get DVB data from http://hftom.free.fr/kaxtv/dvbdata.tar.gz!<br>\
			Check your internet connection, and say Yes to try again.<br>\
			Or say No to cancel.<br> Should I try again?</qt>") );
		if ( ret==KMessageBox::Yes )
			goto loop;
		return;
	}
}



void DvbConfigDialog::setSource( QComboBox *box, QString s )
{
	int pos, i;

	pos = s.find("|");
	if ( pos>=0 )
		s = s.right( s.length()-pos-1 );
	for ( i=0; i<(int)box->count(); i++ ) {
		if ( box->text(i)==s ) {
			box->setCurrentItem(i);
			break;
		}
	}
}



void DvbConfigDialog::satNumberChanged( int value, int devNum )
{
	sat0[devNum]->setEnabled( value > 0 );
	sat1[devNum]->setEnabled( value > 1 );
	sat2[devNum]->setEnabled( value > 2 );
	sat3[devNum]->setEnabled( value > 3 );

	src0[devNum]->setEnabled( value > 0 );
	src1[devNum]->setEnabled( value > 1 );
	src2[devNum]->setEnabled( value > 2 );
	src3[devNum]->setEnabled( value > 3 );

	lnb0[devNum]->setEnabled( value > 0 );
	lnb1[devNum]->setEnabled( value > 1 );
	lnb2[devNum]->setEnabled( value > 2 );
	lnb3[devNum]->setEnabled( value > 3 );

	rotor0[devNum]->setEnabled( value > 0 );
	rotor1[devNum]->setEnabled( value > 1 );
	rotor2[devNum]->setEnabled( value > 2 );
	rotor3[devNum]->setEnabled( value > 3 );

	secMini[devNum]->setEnabled( value==2 );
}



void DvbConfigDialog::fileTemplateHelp()
{
	KMessageBox::information( this, i18n("Special strings are:\n- %chan (channel's name)\n- %date (the starting date, YYMMdd-hhmmss)\n- %name (the name given in timer editor or the program name from EPG)\nSo, if you set template to '%chan-%date-%name', the resulting filename will be, for example, BBC-20070919-210000-News.m2t") );
}



void DvbConfigDialog::setRecordDir()
{
	QString s = KFileDialog::getExistingDirectory( recordDirLe->text().stripWhiteSpace() );
	if ( !s.isEmpty() )
		recordDirLe->setText( s );
}



void DvbConfigDialog::setShiftDir()
{
	QString s = KFileDialog::getExistingDirectory( shiftDirLe->text().stripWhiteSpace() );
	if ( !s.isEmpty() )
		shiftDirLe->setText( s );
}



void DvbConfigDialog::accept()
{
	QString s;
	int i;

	if ( recordDirLe->text().stripWhiteSpace().isEmpty() ) {
		KMessageBox::sorry( this, i18n("Invalid records directory.") );
		recordDirLe->setFocus();
		return;
	}
	if ( shiftDirLe->text().stripWhiteSpace().isEmpty() ) {
		KMessageBox::sorry( this, i18n("Invalid time shifting directory.") );
		shiftDirLe->setFocus();
		return;
	}
	if ( bportSpin->value()==sportSpin->value() ) {
		KMessageBox::sorry( this, i18n("Broadcast and Info ports must be different.") );
		sportSpin->setFocus();
		return;
	}
	if ( !QRegExp("(\\d{1,3}\\.){3}\\d{1,3}").exactMatch( broadcastLe->text().stripWhiteSpace() ) ) {
		KMessageBox::sorry( this, i18n("Invalid broadcast address.") );
		broadcastLe->setFocus();
		return;
	}
	if ( !filenameFormatLe->text().contains("%chan") && !filenameFormatLe->text().contains("%date") && !filenameFormatLe->text().contains("%name") ) {
		KMessageBox::sorry( this, i18n("Invalid filename format.") );
		filenameFormatLe->setFocus();
		return;
	}

	for ( i=0; i<(int)dvbConfig->devList.count(); i++ ) {
		switch (dvbConfig->devList.at(i)->type) {
		case FE_QPSK: {
			dvbConfig->devList.at(i)->numLnb = satNumber[i]->value();
			dvbConfig->devList.at(i)->secMini = secMini[i]->isChecked();
			dvbConfig->devList.at(i)->secTwice = secTwice[i]->isChecked();
			dvbConfig->devList.at(i)->doS2 = doS2[i]->isChecked();
			if ( dvbConfig->devList.at(i)->lnb[3].rotorType==0 ) {
				dvbConfig->devList.at(i)->lnb[3].source.clear();
				dvbConfig->devList.at(i)->lnb[3].source.append(sat3[i]->currentText());
			}
			if ( dvbConfig->devList.at(i)->lnb[2].rotorType==0 ) {
				dvbConfig->devList.at(i)->lnb[2].source.clear();
				dvbConfig->devList.at(i)->lnb[2].source.append(sat2[i]->currentText());
			}
			if ( dvbConfig->devList.at(i)->lnb[1].rotorType==0 ) {
				dvbConfig->devList.at(i)->lnb[1].source.clear();
				dvbConfig->devList.at(i)->lnb[1].source.append(sat1[i]->currentText());
			}
			if ( dvbConfig->devList.at(i)->lnb[0].rotorType==0 ) {
				dvbConfig->devList.at(i)->lnb[0].source.clear();
				dvbConfig->devList.at(i)->lnb[0].source.append(sat0[i]->currentText());
			}
			s = "S";
			break;
			}
		case FE_QAM: {
			s = "C";
			s+="|"+sat0[i]->currentText();
			break;
			}
		case FE_OFDM: {
			s = "T";
			s+="|"+sat0[i]->currentText();
			break;
			}
		case FE_ATSC: {
			s = "A";
			s+="|"+sat0[i]->currentText();
			break;
			}
		}
		dvbConfig->devList.at(i)->source = s;
		dvbConfig->devList.at(i)->tuningTimeout = timeoutSpin.at(i)->value();
		dvbConfig->devList.at(i)->priority = priority.at(i)->value();
	}

	dvbConfig->recordDir = recordDirLe->text();
	if ( !dvbConfig->recordDir.endsWith("/") )
		dvbConfig->recordDir+= "/";
	dvbConfig->shiftDir = shiftDirLe->text();
	if ( !dvbConfig->shiftDir.endsWith("/") )
		dvbConfig->shiftDir+= "/";
	dvbConfig->beginMargin = beginSpin->value();
	dvbConfig->endMargin = endSpin->value();
	dvbConfig->instantDuration = instantDurationSpin->value();
	dvbConfig->sizeFile = sizeFileSpin->value();
	dvbConfig->filenameFormat = filenameFormatLe->text();
	dvbConfig->defaultCharset = charsetComb->currentText();
	dvbConfig->broadcastAddress = broadcastLe->text().stripWhiteSpace();
	dvbConfig->broadcastPort = bportSpin->value();
	dvbConfig->senderPort = sportSpin->value();
	dvbConfig->probeMfe = probeMfe->isChecked();
	dvbConfig->ringBufSize = ringBufSize->value();
	dvbConfig->saveConfig();
	done( Accepted );
}



DvbConfigDialog::~DvbConfigDialog()
{
}



LnbConfig::LnbConfig( LNB *b, QWidget *parent ) :
	KDialogBase ( parent, "lnbConfigDialog", true, i18n("LNB Settings"), Ok|Cancel, Ok, true )
{
	QGridLayout *grid;

	QWidget *page = new QWidget( this );
	setMainWidget( page );

	lnb = b;

	QVBoxLayout *vb = new QVBoxLayout( page, 6, 6 );
	univ = new QPushButton( i18n("Universal LNB"), page );
	connect( univ, SIGNAL(clicked()), this, SLOT(setUniversal()) );
	vb->addWidget( univ );
	cmono = new QPushButton( i18n("C-Band LNB"), page );
	connect( cmono, SIGNAL(clicked()), this, SLOT(setCBandMono()) );
	vb->addWidget( cmono );
	cmulti = new QPushButton( i18n("C-Band Multipoint LNB"), page );
	connect( cmulti, SIGNAL(clicked()), this, SLOT(setCBandMulti()) );
	vb->addWidget( cmulti );

	grid = new QGridLayout( 0, 1, 1 );
	nLO = new QButtonGroup( 3, Qt::Horizontal, page );
	connect( nLO, SIGNAL(clicked(int)), this, SLOT(setDual(int)) );
	new QRadioButton( i18n("Dual LO"), nLO );
	new QRadioButton( i18n("Single LO"), nLO );
	new QRadioButton( i18n("H/V LO"), nLO );
	grid->addMultiCellWidget( nLO, 0, 0, 0, 1 );
	slofLab = new QLabel( i18n("Dual LO switch frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( slofLab, 1, 0 );
	slof = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( slof, 1, 1 );
	loLab = new QLabel( i18n("Lo-band frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( loLab, 2, 0 );
	lo = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( lo, 2, 1 );
	hiLab = new QLabel( i18n("Hi-band frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( hiLab, 3, 0 );
	hi = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( hi, 3, 1 );
	singleLab = new QLabel( i18n("Single LO frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( singleLab, 4, 0 );
	single = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( single, 4, 1 );
	verticalLab = new QLabel( i18n("Vertical pol. LO frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( verticalLab, 5, 0 );
	vertical = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( vertical, 5, 1 );
	horizontalLab = new QLabel( i18n("Horizontal pol. LO frequency:")+i18n(" (MHz)"), page );
	grid->addWidget( horizontalLab, 6, 0 );
	horizontal = new QSpinBox( 0, 13000, 1, page );
	grid->addWidget( horizontal, 6, 1 );

	vb->addLayout( grid );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );


	slof->setValue( lnb->switchFreq );
	lo->setValue( lnb->loFreq );
	hi->setValue( lnb->hiFreq );
	single->setValue( lnb->loFreq );
	vertical->setValue( lnb->loFreq );
	horizontal->setValue( lnb->hiFreq );

	if ( lnb->switchFreq ) {
		nLO->setButton( 0 );
		setDual( 0 );
	}
	else {
		if ( lnb->hiFreq ) {
			nLO->setButton( 2 );
			setDual( 2 );
		}
		else {
			nLO->setButton( 1 );
			setDual( 1 );
		}
	}
}



void LnbConfig::setDual( int id )
{
	switch ( id ) {
		case 0: {
			singleLab->hide();
			single->hide();
			verticalLab->hide();
			vertical->hide();
			horizontalLab->hide();
			horizontal->hide();
			slofLab->show();
			slof->show();
			loLab->show();
			lo->show();
			hiLab->show();
			hi->show();
			break;
		}
		case 1: {
			slofLab->hide();
			slof->hide();
			loLab->hide();
			lo->hide();
			hiLab->hide();
			hi->hide();
			verticalLab->hide();
			vertical->hide();
			horizontalLab->hide();
			horizontal->hide();
			singleLab->show();
			single->show();
			break;
		}
		case 2: {
			slofLab->hide();
			slof->hide();
			loLab->hide();
			lo->hide();
			hiLab->hide();
			hi->hide();
			singleLab->hide();
			single->hide();
			verticalLab->show();
			vertical->show();
			horizontalLab->show();
			horizontal->show();
			break;
		}
	}
}



void LnbConfig::setUniversal()
{
	nLO->setButton( 0 );
	setDual( 0 );
	slof->setValue( 11700 );
	lo->setValue( 9750 );
	hi->setValue( 10600 );
}



void LnbConfig::setCBandMono()
{
	nLO->setButton( 1 );
	setDual( 1 );
	single->setValue( 5150 );
}



void LnbConfig::setCBandMulti()
{
	nLO->setButton( 2 );
	setDual( 2 );
	vertical->setValue( 5150 );
	horizontal->setValue( 5750 );
}



void LnbConfig::accept()
{
	switch ( nLO->selectedId() ) {
		case 0: {
			lnb->switchFreq = slof->value();
			lnb->loFreq = lo->value();
			lnb->hiFreq = hi->value();
			break;
		}
		case 1: {
			lnb->switchFreq = 0;
			lnb->hiFreq = 0;
			lnb->loFreq = single->value();
			break;
		}
		case 2: {
			lnb->switchFreq = 0;
			lnb->loFreq = vertical->value();
			lnb->hiFreq = horizontal->value();
			break;
		}
	}
	done( Accepted );
}



RotorConfig::RotorConfig( Device *d, DVBconfig *c, int lnb, QWidget *parent ) :
	KDialogBase ( parent, "rotorConfigDialog", true, i18n("Rotor Settings"), Ok|Cancel, Ok, true )
{
	int i;

	QWidget *page = new QWidget( this );
	setMainWidget( page );

	dev = d;
	config = c;
	lnbNum = lnb;

	QVBoxLayout *vb = new QVBoxLayout( page, 6, 6 );
	QGridLayout *grid = new QGridLayout( 0, 1, 1 );
	QLabel *lab = new QLabel( i18n("Sattelite:"), page );
	grid->addWidget( lab, 0, 0 );
	if ( dev->lnb[lnbNum].rotorType==2 ) {
		lab = new QLabel( i18n("Position:"), page );
		grid->addWidget( lab, 0, 1 );
	}
	srcComb = new QComboBox( page );
	srcComb->insertStringList( config->getSourcesList( dev->type ) );
	grid->addWidget( srcComb, 1, 0 );
	position = new QSpinBox( 0, 500, 1, page );
	grid->addWidget( position, 1, 1 );
	vb->addLayout( grid );
	vb->addItem( new QSpacerItem( 20, 10, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

	addBtn = new QPushButton( i18n("Add to list"), page );
	connect( addBtn, SIGNAL(clicked()), this, SLOT(add()) );
	vb->addWidget( addBtn );
	vb->addItem( new QSpacerItem( 20, 10, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

	listView = new QListView( page );
	listView->addColumn( i18n("Sattelite:") );
	vb->addWidget( listView );

	resetBtn = new QPushButton( i18n("Clear list"), page );
	connect( resetBtn, SIGNAL(clicked()), this, SLOT(reset()) );
	vb->addWidget( resetBtn );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

	if ( dev->lnb[lnbNum].rotorType!=3 ) {
		grid = new QGridLayout( 0, 1, 1 );
		lab = new QLabel( i18n("13V rotor speed:"), page );
		grid->addWidget( lab, 0, 0 );
		speed13 = new QLineEdit( page );
		speed13->setText( QString().setNum( dev->lnb[lnbNum].speed13v ) );
		grid->addWidget( speed13, 0, 1 );
		lab = new QLabel( i18n("sec./ °"), page );
		grid->addWidget( lab, 0, 2 );
		lab = new QLabel( i18n("18V rotor speed:"), page );
		grid->addWidget( lab, 1, 0 );
		speed18 = new QLineEdit( page );
		speed18->setText( QString().setNum( dev->lnb[lnbNum].speed18v ) );
		grid->addWidget( speed18, 1, 1 );
		lab = new QLabel( i18n("sec./ °"), page );
		grid->addWidget( lab, 1, 2 );
		vb->addLayout( grid );
	}

	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	if ( dev->lnb[lnbNum].rotorType==2 ) {
		listView->addColumn( i18n("Position:") );
		for ( i=0; i<(int)dev->lnb[lnbNum].source.count(); i++ )
			new QListViewItem( listView, dev->lnb[lnbNum].source[i], QString().setNum(dev->lnb[lnbNum].position[i])  );
	}
	else {
		position->hide();
		for ( i=0; i<(int)dev->lnb[lnbNum].source.count(); i++ )
			new QListViewItem( listView, dev->lnb[lnbNum].source[i] );
	}

}



void RotorConfig::reset()
{
	listView->clear();
	position->setValue( 1 );
}



void RotorConfig::add()
{
	if ( position->isHidden() ) {
		new QListViewItem( listView, srcComb->currentText() );
	}
	else {
		new QListViewItem( listView, srcComb->currentText(), QString().setNum(position->value()) );
	}

}



void RotorConfig::accept()
{
	QListViewItem *it;

	if ( dev->lnb[lnbNum].rotorType!=3 ) {
		dev->lnb[lnbNum].speed18v = speed18->text().toDouble();
		dev->lnb[lnbNum].speed13v = speed13->text().toDouble();
	}
	else {
		dev->lnb[lnbNum].speed18v = 0;
		dev->lnb[lnbNum].speed13v = 0;
	}
	dev->lnb[lnbNum].source.clear();
	dev->lnb[lnbNum].position.clear();
	for ( it=listView->firstChild(); it; it=it->nextSibling() ) {
		dev->lnb[lnbNum].source.append( it->text(0) );
		if ( dev->lnb[lnbNum].rotorType==2 )
			dev->lnb[lnbNum].position.append( it->text(1).toInt() );
	}
	done( Accepted );
}


#include "dvbconfig.moc"
