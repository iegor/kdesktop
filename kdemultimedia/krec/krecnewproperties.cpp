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

#include "krecnewproperties.h"
#include "krecnewproperties.moc"

#include "krecglobal.h"
#include "krecconfig_fileswidget.h"

#include <kconfig.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <kseparator.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qfont.h>

#include <kdebug.h>
#include <klocale.h>

KRecNewProperties::KRecNewProperties( QWidget* p, const char* n )
  : QDialog( p,n, 0, Qt::WType_Dialog|Qt::WStyle_Customize|Qt::WStyle_DialogBorder )
  , _filename( QString::null )
  , _samplerate( 44100 ), _channels( 2 ), _bits( 16 )
{
kdDebug( 60005 ) << k_funcinfo << endl;
	KConfig *config = KRecGlobal::kconfig();
	config->setGroup( "FileDefaults" );
	_samplerate = config->readNumEntry( "SamplingRate", 44100 );
	_channels = config->readNumEntry( "Channels", 2 );
	_bits = config->readNumEntry( "Bits", 16 );
	_usedefaults = config->readBoolEntry( "UseDefaults", false );

	_layout = new QVBoxLayout( this, 5, 5 );

	QLabel *captionlabel = new QLabel( this );
	QFont labelfont( captionlabel->font() );
	labelfont.setPointSize( labelfont.pointSize()*3/2 );
	captionlabel->setFont( labelfont );
	captionlabel->setText( i18n( "Properties for the new File" ) );
	captionlabel->setAlignment( AlignCenter );
	_layout->addWidget( captionlabel );

	_filewidget = new KRecConfigFilesWidget( this );
	connect( _filewidget, SIGNAL( sRateChanged( int ) ), this, SLOT( ratechanged( int ) ) );
	connect( _filewidget, SIGNAL( sChannelsChanged( int ) ), this, SLOT( channelschanged( int ) ) );
	connect( _filewidget, SIGNAL( sBitsChanged( int ) ), this, SLOT( bitschanged( int ) ) );
	connect( _filewidget, SIGNAL( sUseDefaultsChanged( bool ) ), this, SLOT( usedefaultschanged( bool ) ) );

	QWidget *_btnWidget = new QWidget( this );
	_layoutbuttons = new QHBoxLayout( _btnWidget );
	_layoutbuttons->addStretch( 100 );
	_btnok = new KPushButton( KStdGuiItem::ok(), _btnWidget );
	connect( _btnok, SIGNAL( clicked() ), this, SLOT( accept() ) );
	_layoutbuttons->addWidget( _btnok, 0 );

	_layout->addWidget( new KSeparator( KSeparator::HLine, this ) );
	_layout->addWidget( _filewidget );
	_layout->addWidget( new KSeparator( KSeparator::HLine, this ) );
	_layout->addWidget( _btnWidget );

	setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
}
KRecNewProperties::~KRecNewProperties() {
kdDebug( 60005 ) << k_funcinfo << endl;
}

QString KRecNewProperties::filename() { return _filename; }
int KRecNewProperties::samplerate() { return _samplerate; }
int KRecNewProperties::channels() { return _channels; }
int KRecNewProperties::bits() { return _bits; }
bool KRecNewProperties::usedefaults() { return _usedefaults; }

void KRecNewProperties::ratechanged( int rate ) { _samplerate = rate; }
void KRecNewProperties::channelschanged( int channels ) { _channels = channels; }
void KRecNewProperties::bitschanged( int bits ) { _bits = bits; }
void KRecNewProperties::usedefaultschanged( bool n ) {
	_usedefaults = n;
	KRecGlobal::kconfig()->setGroup( "FileDefaults" );
	KRecGlobal::kconfig()->writeEntry( "UseDefaults", _usedefaults );
}

void KRecNewProperties::done( int r ) {
kdDebug( 60005 ) << k_funcinfo << endl;
	QDialog::done( r );
}

