/*
 * klameenc.cpp
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

#include <qcombobox.h>
#include <qcheckbox.h>

#include <kparts/genericfactory.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>

#include "klameenc.h"
#include "klameenc.moc"

LameSettings::LameSettings( QWidget *parent, KConfig *confile ) : LameConfig( parent )
{
	KIconLoader *icon = new KIconLoader();
	okBtn->setGuiItem( KGuiItem(i18n("OK"), icon->loadIconSet("ok", KIcon::Small) ) );
	cancelBtn->setGuiItem( KGuiItem(i18n("Cancel"), icon->loadIconSet("cancel", KIcon::Small) ) );
	delete icon;
	brateComb->insertItem( "32" );
	brateComb->insertItem( "40" );
	brateComb->insertItem( "48" );
	brateComb->insertItem( "56" );
	brateComb->insertItem( "64" );
	brateComb->insertItem( "80" );
	brateComb->insertItem( "96" );
	brateComb->insertItem( "112" );
	brateComb->insertItem( "128" );
	brateComb->insertItem( "160" );
	brateComb->insertItem( "192" );
	brateComb->insertItem( "224" );
	brateComb->insertItem( "256" );
	brateComb->insertItem( "320" );
	Conf = confile;
	Conf->setGroup("LameMp3");
	brateComb->setCurrentText( Conf->readEntry( "BitRate", "128" ) );
	vbrCb->setChecked( Conf->readBoolEntry( "VBR", false ) );
}

LameSettings::~LameSettings()
{
}

void LameSettings::accept()
{
	Conf->setGroup("LameMp3");
	Conf->writeEntry( "BitRate", brateComb->currentText() );
	Conf->writeEntry( "VBR", vbrCb->isChecked() );
	done( Accepted );
}

int LameSettings::getBitrate()
{
	return brateComb->currentText().toInt();
}

bool LameSettings::isVBR()
{
	return vbrCb->isChecked();
}

K_EXPORT_COMPONENT_FACTORY (libkaffeinemp3lame, KParts::GenericFactory<KLameEnc>)

KLameEnc::KLameEnc( QWidget*, const char*, QObject* parent, const char* name, const QStringList& )
	: KaffeineAudioEncoder(parent,name)
{
	setInstance(KParts::GenericFactory<KLameEnc>::instance());
}

KAboutData *KLameEnc::createAboutData()
{
	KAboutData* aboutData = new KAboutData( "kaffeinemp3lame", I18N_NOOP("KaffeineMp3Lame"),
	                                        "0.1", I18N_NOOP("A Lame mp3 encoder plugin for Kaffeine."),
	                                        KAboutData::License_GPL,
	                                        "(c) 2006, Christophe Thommeret.", 0, "http://kaffeine.sourceforge.net");
	aboutData->addAuthor("Christophe Thommeret.",0, "hftom@free.fr");

	return aboutData;
}

QString KLameEnc::getExtension()
{
	return QString(".mp3");
}

bool KLameEnc::options( QWidget *parent, KConfig *conf )
{
	LameSettings dlg( parent, conf );
	int ret = dlg.exec();
	if ( ret!=QDialog::Accepted )
		return false;
	bitrate = dlg.getBitrate();
	vbr = dlg.isVBR();
	return true;
}

void KLameEnc::start( QString title, QString artist, QString album, QString tracknumber, QString genre )
{
	flags = lame_init();
	lame_set_mode( flags, STEREO );
	if ( vbr ) {
		lame_set_VBR( flags, vbr_abr );
		lame_set_VBR_mean_bitrate_kbps( flags, bitrate );
	}
	else {
		lame_set_VBR( flags, vbr_off );
		lame_set_brate( flags, bitrate );
	}
	lame_init_params( flags );

	id3tag_init( flags );
	id3tag_v2_only( flags );
	if ( !title.isNull() )
		id3tag_set_title( flags, title.latin1() );
	if ( !artist.isNull() )
		id3tag_set_artist( flags, artist.latin1() );
	if ( !album.isNull() )
		id3tag_set_album( flags, album.latin1() );
	if ( !tracknumber.isNull() )
		id3tag_set_track( flags, tracknumber.latin1() );
	if ( !genre.isNull() )
		id3tag_set_genre( flags, genre.latin1() );
	id3tag_set_comment( flags, "Encoded by Kaffeine" );
	lame_init_params( flags );
}

char* KLameEnc::getHeader( int &len )
{
	len = 0;
	return NULL;
}

char* KLameEnc::encode( char *data, int datalen, int &len )
{
	len = lame_encode_buffer_interleaved( flags, (short int*)data, datalen/4, (unsigned char*)bufEncode, 8000 );

	if ( len>0 )
		return bufEncode;
	else
		return NULL;
}

char* KLameEnc::stop( int &len )
{
	len = lame_encode_flush( flags, (unsigned char*)bufEncode, 8000 );

	lame_close( flags );
	flags = 0;

	if ( len>0 )
		return bufEncode;
	else
		return NULL;
}

KLameEnc::~KLameEnc()
{
}
