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

#include "krecconfig_fileswidget.h"
#include "krecconfig_fileswidget.moc"

#include "krecglobal.h"

#include <kdebug.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <qlabel.h>

KRecConfigFilesWidget::KRecConfigFilesWidget( QWidget* p, const char* n )
 : QVBox( p,n )
 , _hbox( new QHBox( this ) )
 , _ratebox( 0 ), _channelsbox( 0 ), _bitsbox( 0 )
 , _rate48( 0 ), _rate44( 0 ), _rate22( 0 ), _rate11( 0 ), _rateother( 0 )
 , _rateotherbox( 0 ), _rateotherlabel( 0 ), _rateotherline( 0 )
 , _channels2( 0 ), _channels1( 0 )
 , _bits16( 0 ), _bits8( 0 )
 , _samplingRate( 44100 ), _channels( 2 ), _bits( 16 )
{
	_ratebox = new QButtonGroup( 1, Qt::Horizontal, i18n( "Sampling Rate" ), _hbox );
	connect( _ratebox, SIGNAL( clicked( int ) ), this, SLOT( ratechanged( int ) ) );
	_rate48 = new QRadioButton( i18n( "48000 Hz" ), _ratebox );
	_rate44 = new QRadioButton( i18n( "44100 Hz" ), _ratebox );
	_rate22 = new QRadioButton( i18n( "22050 Hz" ), _ratebox );
	_rate11 = new QRadioButton( i18n( "11025 Hz" ), _ratebox );
	_rateother = new QRadioButton( i18n( "Other" ), _ratebox );
	_rateotherbox = new QHBox( _ratebox );
	_rateotherbox->setSpacing( 2 );
	_rateotherlabel = new QLabel( i18n( "Other:" ), _rateotherbox );
	_rateotherline = new QLineEdit( _rateotherbox );
	_rateotherline->setMaxLength( 10 );
	_rateotherline->setFrame( true );
	_rateotherbox->setEnabled( false );
	connect( _rateotherline, SIGNAL( textChanged( const QString& ) ), this, SLOT( rateotherchanged( const QString& ) ) );
	_channelsbox = new QButtonGroup( 1, Qt::Horizontal, i18n( "Channels" ), _hbox );
	connect( _channelsbox, SIGNAL( clicked( int ) ), this, SLOT( channelschanged( int ) ) );
	_channels2 = new QRadioButton( i18n( "Stereo (2 channels)" ), _channelsbox );
	_channels1 = new QRadioButton( i18n( "Mono (1 channel)" ), _channelsbox );
	_bitsbox = new QButtonGroup( 1, Qt::Horizontal, i18n( "Bits" ), _hbox );
	connect( _bitsbox, SIGNAL( clicked( int ) ), this, SLOT( bitschanged( int ) ) );
	_bits16 = new QRadioButton( i18n( "16 bit" ), _bitsbox );
	_bits8 = new QRadioButton( i18n( "8 bit" ), _bitsbox );

	_usedefaults = new QCheckBox( i18n( "Use defaults for creating new files" ), this );
	connect( _usedefaults, SIGNAL( toggled( bool ) ), this, SLOT( usedefaultschanged( bool ) ) );

	setSpacing( 5 );

	load();
}

KRecConfigFilesWidget::~KRecConfigFilesWidget() {
}

void KRecConfigFilesWidget::load() {
kdDebug( 60005 ) << k_funcinfo << endl;
	defaults();
	kapp->config()->setGroup( "FileDefaults" );
	_samplingRate = kapp->config()->readNumEntry( "SamplingRate", 44100 );
	switch ( _samplingRate ) {
		case 48000: _rate48->setChecked( true ); break;
		case 44100: _rate44->setChecked( true ); break;
		case 22050: _rate22->setChecked( true ); break;
		case 11025: _rate11->setChecked( true ); break;
		default:
			_rateother->setChecked( true );
			_rateotherbox->setEnabled( true );
			_rateotherline->setText( QString::number( _samplingRate ) );
			break;
	};
	_channels = kapp->config()->readNumEntry( "Channels", 2 );
	switch ( _channels ) {
		default:
		case 2: _channels2->setChecked( true ); break;
		case 1: _channels1->setChecked( true ); break;
	};
	_bits = kapp->config()->readNumEntry( "Bits", 16 );
	switch ( _bits ) {
		default:
		case 16: _bits16->setChecked( true ); break;
		case 8: _bits8->setChecked( true ); break;
	};
	_usedefaults->setChecked( kapp->config()->readBoolEntry( "UseDefaults", false ) );
}

void KRecConfigFilesWidget::save() {
	kapp->config()->setGroup( "FileDefaults" );
	kapp->config()->writeEntry( "SamplingRate", _samplingRate );
	kapp->config()->writeEntry( "Channels", _channels );
	kapp->config()->writeEntry( "Bits", _bits );
	kapp->config()->writeEntry( "UseDefaults", _usedefaults->isOn() );

	kapp->config()->sync();
}

void KRecConfigFilesWidget::defaults() {
	_rate44->setChecked( true );
	_channels2->setChecked( true );
	_bits16->setChecked( true );
}

void KRecConfigFilesWidget::ratechanged( int index ) {
	if ( _ratebox->find( index ) == _rateother ) _rateotherbox->setEnabled( true );
		else _rateotherbox->setEnabled( false );
	if ( _ratebox->find( index ) == _rate48 ) _samplingRate = 48000;
	if ( _ratebox->find( index ) == _rate44 ) _samplingRate = 44100;
	if ( _ratebox->find( index ) == _rate22 ) _samplingRate = 22050;
	if ( _ratebox->find( index ) == _rate11 ) _samplingRate = 11025;
	emit sRateChanged( _samplingRate );
}
void KRecConfigFilesWidget::rateotherchanged( const QString& text ) {
	_samplingRate = text.toInt();
	emit sRateChanged( _samplingRate );
}
void KRecConfigFilesWidget::channelschanged( int index ) {
	if ( _channelsbox->find( index ) == _channels2 ) _channels = 2;
	if ( _channelsbox->find( index ) == _channels1 ) _channels = 1;
	emit sChannelsChanged( _channels );
}
void KRecConfigFilesWidget::bitschanged( int index ) {
	if ( _bitsbox->find( index ) == _bits16 ) _bits = 16;
	if ( _bitsbox->find( index ) == _bits8 ) _bits = 8;
	emit sBitsChanged( _bits );
}

void KRecConfigFilesWidget::usedefaultschanged( bool n ) {
	emit sUseDefaultsChanged( n );
}
