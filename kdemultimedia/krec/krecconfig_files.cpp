/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#include "krecconfig_files.h"
#include "krecconfig_files.moc"

#include "krecconfig_fileswidget.h"

#include <kgenericfactory.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qhbox.h>
#include <qlabel.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>

typedef KGenericFactory<KRecConfigFiles, QWidget> KRecConfigFilesFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_krec_files, KRecConfigFilesFactory( "krec" ) )

KRecConfigFiles::KRecConfigFiles( QWidget* p, const char*, const QStringList& s )
 : KCModule( KRecConfigFilesFactory::instance(), p, s )
 , _layout( 0 )
 , _filewidget( 0 )
{
	_layout = new QBoxLayout( this, QBoxLayout::TopToBottom );

	_layout->addSpacing( 10 );

	_filewidget = new KRecConfigFilesWidget( this );
	connect( _filewidget, SIGNAL( sRateChanged( int ) ), this, SLOT( ratechanged( int ) ) );
	connect( _filewidget, SIGNAL( sChannelsChanged( int ) ), this, SLOT( channelschanged( int ) ) );
	connect( _filewidget, SIGNAL( sBitsChanged( int ) ), this, SLOT( bitschanged( int ) ) );
	connect( _filewidget, SIGNAL( sUseDefaultsChanged( bool ) ), this, SLOT( usedefaultschanged( bool ) ) );
	_layout->addWidget( _filewidget );

	_layout->addStretch( 100 );

	load();
}

KRecConfigFiles::~KRecConfigFiles() {
}

void KRecConfigFiles::load() {
kdDebug( 60005 ) << k_funcinfo << endl;
	defaults();
	_filewidget->load();
}

void KRecConfigFiles::save() {
	_filewidget->save();
	emit changed( false );
}

void KRecConfigFiles::defaults() {
	_filewidget->defaults();
}

void KRecConfigFiles::ratechanged( int ) {
	emit changed( true );
}
void KRecConfigFiles::channelschanged( int ) {
	emit changed( true );
}
void KRecConfigFiles::bitschanged( int ) {
	emit changed( true );
}
void KRecConfigFiles::usedefaultschanged( bool ) {
	emit changed( true );
}

