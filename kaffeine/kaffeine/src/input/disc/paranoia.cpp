/*
 * paranoia.cpp
 *
 * Copyright (C) 2002-2006 Christophe Thommeret <hftom@free.fr>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <math.h>

#include <qfile.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qdir.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>

#include <qcombobox.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktrader.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kparts/componentfactory.h>

#include "paranoia.h"
#include "paranoia.moc"

#define DEFAULT_DRIVE "/dev/cdrom"

KiloConfig::KiloConfig( QWidget *parent, KConfig *confile, const QStringList &encoders ) : ParanoiaSettings( parent )
{
	int i;

	KIconLoader *icon = new KIconLoader();
	okBtn->setGuiItem( KGuiItem(i18n("OK"), icon->loadIconSet("ok", KIcon::Small) ) );
	cancelBtn->setGuiItem( KGuiItem(i18n("Cancel"), icon->loadIconSet("cancel", KIcon::Small) ) );
	baseDirBtn->setIconSet( icon->loadIconSet("fileopen", KIcon::Small) );
	delete icon;
	connect( baseDirBtn, SIGNAL( clicked() ), this, SLOT( setBaseDir() ) );
	Conf = confile;
	Conf->setGroup( "Paranoia" );
	baseDirLineEdit->setText( Conf->readEntry( "Basedir", QDir::homeDirPath() ) );
	paranoiaGroup->setButton( Conf->readNumEntry( "Mode", 0 ) );
	normCb->setChecked( Conf->readBoolEntry( "Normalize", false ) );
	encoderComb->insertStringList( encoders );
	QString s = Conf->readEntry( "CurrentEncoder", "" );
	if ( !s.isEmpty() ) {
		for ( i=0; i<(int)encoders.count(); i++ ) {
        		if ( encoders[i]==s ) {
        			encoderComb->setCurrentText( s );
        		}
    		}
    	}
}

void KiloConfig::setBaseDir()
{
	Conf->setGroup( "Paranoia" );
	QString d = Conf->readEntry( "Basedir", QDir::homeDirPath() );
	QString u = KFileDialog::getExistingDirectory( d );
	if ( u!="" ) {
		baseDirLineEdit->setText( u );
		Conf->writeEntry( "Basedir", u );
	}
}

QString KiloConfig::getEncoder()
{
	return encoderComb->currentText();
}

bool KiloConfig::getNormalize()
{
	return normCb->isChecked();
}

QString KiloConfig::getBaseDir()
{
	return baseDirLineEdit->text().stripWhiteSpace();
}

int KiloConfig::getParanoiaMode()
{
	return paranoiaGroup->selectedId();
}

void KiloConfig::accept()
{
	Conf->setGroup( "Paranoia" );
	Conf->writeEntry( "Mode", paranoiaGroup->id( paranoiaGroup->selected() ) );
	Conf->writeEntry( "CurrentEncoder", encoderComb->currentText() );
	Conf->writeEntry( "Normalize", normCb->isChecked() );
	done(Accepted);
}

KiloConfig::~KiloConfig()
{
}

void paranoiaCallback( long int, paranoia_cb_mode_t )
{
}

Paranoia::Paranoia()
{
	d = 0;
	p = 0;
	isRunning = false;
}

bool Paranoia::init( QString dev )
{
	QString s;
	QFile f;

       if ( p!=0 ) paranoia_free( p );
	if (d!=0 ) cdda_close( d );
	nTracks = 0;

	dev = dev.stripWhiteSpace();
	f.setName( dev );
	if ( !f.exists() ) {
		/*if ( !findCdrom() ) {
			d = cdda_find_a_cdrom( CDDA_MESSAGE_PRINTIT, 0 );
			if ( cdda_open( d )!=0 )
				return false;
		}*/
		return false;
	}
       else {
		d = cdda_identify( dev.ascii(), CDDA_MESSAGE_PRINTIT, 0 );
		if ( cdda_open( d )!=0 )
			return false;
	}
	p = paranoia_init( d );
	nTracks = cdda_tracks( d );
	return true;
}

bool Paranoia::findCdrom()
{
	QFile *f;
	QString c;
	QString s="";
	int pos, i;
	bool stop=false;
	char dev[4][4]={"","","",""};

	f = new QFile( "/proc/sys/dev/cdrom/info" );
	if ( !f->open(IO_ReadOnly) )
		return false;

	QTextStream t( f );
	while ( !t.eof() && !stop ) {
		s = t.readLine();
		if ( s.contains("drive name:") )
			stop = true;
	}
	if ( !stop )
		return false;

	pos = s.find(":");
	c = s.right( s.length()-pos-1 );
	sscanf( c.latin1(), "%s %s %s %s", dev[0], dev[1], dev[2], dev[3] );

	for ( i=0; i<4; i++ )
		if ( procCdrom( dev[i] ) )
			return true;

	f->close();
	return false;
}

bool Paranoia::procCdrom( QString name )
{
	int pos;

	if ( name.contains("sr") ) {
		pos = name.find("r");
		name = name.right( name.length()-pos-1 );
		name = "/dev/scd"+name;
		d = cdda_identify( name.ascii(), CDDA_MESSAGE_PRINTIT, 0 );
		if ( cdda_open( d )==0 )
			return true;
	}
	else if ( name.contains("hd") ) {
		name = "/dev/"+name;
		d = cdda_identify( name.ascii(), CDDA_MESSAGE_PRINTIT, 0 );
		if ( cdda_open( d )==0 )
			return true;
	}
	return false;
}

void Paranoia::setMode( int mode )
{
	switch ( mode ) {
		case 0 : mode = PARANOIA_MODE_DISABLE;
				break;
		case 1 : mode = PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP;
				break;
		case 2 : mode = PARANOIA_MODE_FULL;
	}
	paranoia_modeset( p, mode );
}

bool Paranoia::encode( const QStringList &list, QWidget *parent )
{
	QStringList desktop;
	QStringList encoderName;

	encodingList.clear();
	encodingList = list;
	myParent = parent;

	// check for encoders
	KTrader::OfferList offers = KTrader::self()->query("KaffeineAudioEncoder");
	KTrader::OfferList::Iterator end(offers.end());
	for(KTrader::OfferList::Iterator it = offers.begin(); it != end; ++it) {
		KService::Ptr ptr = (*it);
		desktop.append( ptr->desktopEntryName() );
		encoderName.append( ptr->name() );
	}

	if ( !encoderName.count() ) {
		KMessageBox::error( myParent, i18n("No audio encoders could be found."), i18n("Warning") );
		return false;
	}

	KiloConfig dlg( myParent, KGlobal::config(), encoderName );
	int ret = dlg.exec();
	if ( ret!=QDialog::Accepted )
		return false;
	normalize = dlg.getNormalize();
	baseDir = dlg.getBaseDir();
	paraMode = dlg.getParanoiaMode();

	QString s = dlg.getEncoder();
	for ( ret=0; ret<(int)encoderName.count(); ++ret ) {
		if ( encoderName[ret]==s ) {
			encoderDesktop = desktop[ret];
			break;
		}
	}

	if  ( !loadEncoder( myParent ) )
		return false;

	if ( !currentEncoder->options( myParent, KGlobal::config() ) ) {
		unloadEncoder();
		return false;
	}

	if ( !setPath( baseDir, QString(encodingList[0]).replace("/","_"), QString(encodingList[1]).replace("/","_") ) ) {
		return false;
	}
	isRunning = true;
	start();
	return true;
}

bool Paranoia::loadEncoder( QWidget *parent )
{
	int error = 0;

	KService::Ptr service = KService::serviceByDesktopName( encoderDesktop );
	if (!service) {
		KMessageBox::error( parent, i18n("Loading of encoder '%1' failed.").arg(encoderDesktop) );
		return false;
	}

	if ( service->serviceTypes().contains("KaffeineAudioEncoder") ) {
		currentEncoder = KParts::ComponentFactory::createPartInstanceFromService<KaffeineAudioEncoder>(service, 0, service->name().ascii(), 0, 0, 0, &error);
		if (error > 0) {
			KMessageBox::error( parent, i18n("Loading of encoder '%1' failed.").arg(encoderDesktop) );
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

void Paranoia::unloadEncoder()
{
	//kdDebug()<<"Unload encoder ..."<<endl;
	KService::Ptr service = KService::serviceByDesktopName( encoderDesktop );
	KLibLoader::self()->unloadLibrary( service->library().ascii() );
	//kdDebug()<<"... encoder unloaded."<<endl;
}

bool Paranoia::validPath( QString path )
{
	QDir dir;

	dir.setPath( path );
	if ( !dir.exists() ) {
		if ( !dir.mkdir( path ) ) {
			KMessageBox::error( 0, i18n("Unable to create folder: ")+path );
			return false;
		}
	}
	return true;
}

bool Paranoia::setPath( QString &path, const QString &artist, const QString &album )
{
	QString s;

	if ( !path.endsWith("/") )
		path = path+"/";
	if ( !validPath( path ) )
		return false;

	s = artist;
	if ( s!="" )
		path = path+s+"/";
	if ( !validPath( path ) )
		return false;

	s = album;
	if ( s!="" )
		path = path+s+"/";
	if ( !validPath( path ) )
		return false;
	return true;
}

bool Paranoia::initTrack( int t )
{
	currentSector = cdda_track_firstsector( d, t );
	endOfTrack = cdda_track_lastsector( d, t );
	paranoia_seek( p, currentSector, SEEK_SET );
	return true;
}

static inline signed short paraSwap16( signed short x ) {
	return ((((unsigned short)x & 0x00ffU) <<  8) |
		(((unsigned short )x & 0xff00U) >>  8));
}

void Paranoia::run()
{
	signed short *buf;
	int i, n, len, retlen;
	long curpos, endpos;
	QFile f, fn;
	float max;
	float factor;
	QString s;
	char *encoded;
	int overallSectors=0;
	int sectorCount=0;

	progress = 0;
	sleep(2); // give some time for the player to be stopped

	setMode( paraMode );
	for ( i=2; i<(int)encodingList.count(); ++i ) {
		n = encodingList[i].left(2).toInt();
		overallSectors+= trackSectorSize( n );
	}

	fn.setName( baseDir+".temp" );

	for ( i=2; i<(int)encodingList.count(); ++i ) {
		n = encodingList[i].left(2).toInt();
		s = QString(encodingList[i]).replace("/","_")+currentEncoder->getExtension();
		f.setName( baseDir+s );
		initTrack( n );
		max = 0;
		curpos = currentSector;
		endpos = endOfTrack;
		if ( normalize ) {
			len = CDIO_CD_FRAMESIZE_RAW;
			fn.open( IO_ReadWrite | IO_Truncate );
			do {
				buf = paranoia_read_limited( p, paranoiaCallback, 3 );
				if ( Q_BYTE_ORDER == Q_BIG_ENDIAN ) {
					for ( i=0; i<len/2; i++)
						buf[i] = paraSwap16(buf[i]);
				}
				++curpos;
				if ( len>0 ) {
					for ( n=0; n<len/2; ++n )
						if ( fabs(buf[n])>max )
							max = fabs(buf[n]);
					fn.writeBlock( (char*)buf, len );
					++sectorCount;
					progress = sectorCount*50/overallSectors;
				}
				if ( !isRunning )
					len=0;
			}
			while ( curpos<endpos && len!=0 );

			factor = 32767.0/max;
			buf = new signed short[CDIO_CD_FRAMESIZE_RAW];
			fn.at( 0 );
			f.open( IO_ReadWrite | IO_Truncate );
			currentEncoder->start( encodingList[i].remove(0,3), encodingList[0], encodingList[1], encodingList[i].left(2) );
			encoded = currentEncoder->getHeader( len );
			if ( encoded )
				f.writeBlock( encoded, len );

			do {
				len = fn.readBlock( (char*)buf, CDIO_CD_FRAMESIZE_RAW );
				if ( len>0 ) {
					if ( max<32760 )
						for ( n=0; n<len/2; ++n )
							buf[n] = (float)buf[n]*factor;
					encoded = currentEncoder->encode( (char*)buf, len, retlen );
					if ( encoded )
						f.writeBlock( encoded, retlen );
					++sectorCount;
					progress = sectorCount*50/overallSectors;
					if ( !isRunning )
						len=0;
				}
			}
			while ( len>0 );
			encoded = currentEncoder->stop( len );
			if ( encoded )
				f.writeBlock( encoded, len );
			delete [] buf;
			fn.remove();
		}
		else {
			f.open( IO_ReadWrite | IO_Truncate );
			currentEncoder->start( encodingList[i].remove(0,3), encodingList[0], encodingList[1], encodingList[i].left(2) );
			encoded = currentEncoder->getHeader( len );
			if ( encoded )
				f.writeBlock( encoded, len );
			len = CDIO_CD_FRAMESIZE_RAW;
			do {
				buf = paranoia_read_limited( p, paranoiaCallback, 3 );
				if ( Q_BYTE_ORDER == Q_BIG_ENDIAN ) {
					for ( i=0; i<len/2; i++) {
						buf[i] = paraSwap16(buf[i]);
					}
				}
				++curpos;
				if ( len>0 ) {
					encoded = currentEncoder->encode( (char*)buf, len, retlen );
					if ( encoded )
						f.writeBlock( encoded, retlen );
					++sectorCount;
					progress = sectorCount*100/overallSectors;
				}
				if ( !isRunning )
					len=0;
			}
			while ( curpos<endpos && len!=0 );
			encoded = currentEncoder->stop( len );
			if ( encoded )
				f.writeBlock( encoded, len );
			sleep(1); // cdparanoia seems to like that.
		}
		f.flush();
		f.close();
	}

	unloadEncoder();
	isRunning = false;
}

int Paranoia::trackFirstSector( int t )
{
	return cdda_track_firstsector( d, t );
}

int Paranoia::discFirstSector()
{
	return cdda_disc_firstsector( d );
}

int Paranoia::discLastSector()
{
	return cdda_disc_lastsector( d );
}

bool Paranoia::isAudio( int t )
{
	if ( cdda_track_audiop( d, t+1 ) ) return true;
	else return false;
}

QString Paranoia::trackSize( int t )
{
	QString s, c;
	long total;

	total = CDIO_CD_FRAMESIZE_RAW * (cdda_track_lastsector( d, t+1 )-cdda_track_firstsector( d, t+1 ) );
	if ( total>(1048576 ) ) s = c.setNum(total/1048576.0, 'f', 2)+" "+i18n("MB");
	else if ( total>1024 ) s = c.setNum(total/1024.0, 'f', 2)+" "+i18n("KB");
	else s = c.setNum(total*1.0, 'f', 2)+" "+i18n("Bytes");
	return s;
}

long Paranoia::trackSectorSize( int t )
{
	return cdda_track_lastsector( d, t )-cdda_track_firstsector( d, t );
}

QString Paranoia::trackTime( int t )
{
	QString c;
	long total, time;
	int m, s;

	if ( t<0 ) total = CDIO_CD_FRAMESIZE_RAW * (cdda_disc_lastsector( d )-cdda_disc_firstsector( d ) );
	else total = CDIO_CD_FRAMESIZE_RAW * (cdda_track_lastsector( d, t+1 )-cdda_track_firstsector( d, t+1 ) );
	time = (8 * total) / (44100 * 2 * 16);
	m = time/60;
	s = time%60;
	c.sprintf( "%.2i:%.2i", m, s );
	return c;
}

Paranoia::~Paranoia()
{
	if ( p!=0 ) paranoia_free( p );
	if (d!=0 ) cdda_close( d );
}

long Paranoia::getTracks()
{
	return nTracks;
}
