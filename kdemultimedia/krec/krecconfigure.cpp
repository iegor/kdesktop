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

#include "krecconfigure.h"
#include "krecconfigure.moc"

#include "krecglobal.h"

#include <kgenericfactory.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qhbox.h>
#include <qlabel.h>
#include <kapplication.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

typedef KGenericFactory<KRecConfigGeneral, QWidget> KRecConfigGeneralFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_krec, KRecConfigGeneralFactory( "krec" ) )

KRecConfigGeneral::KRecConfigGeneral( QWidget* p, const char*, const QStringList& s )
 : KCModule( KRecConfigGeneralFactory::instance(), p, s )
 , _layout( 0 ), _layout_display( 0 )
 , _display_title( 0 )
 , _displaybox( 0 ), _framebasebox( 0 )
 , _display0( 0 ), _display1( 0 ), _display2( 0 ), _display3( 0 )
 , _framebase30( 0 ), _framebase25( 0 ), _framebase75( 0 ), _framebaseother( 0 )
 , _framebaseotherbox( 0 ), _framebaseotherlabel( 0 ), _framebaseotherline( 0 )
 , _displaymode( 0 ), _framebase( 25 )
{
	_layout = new QBoxLayout( this, QBoxLayout::TopToBottom );

	_layout->addSpacing( 10 );
	_display_title = new QLabel( i18n( "<qt><b>Timedisplay Related Settings</b></qt>" ), this );
	_layout->addWidget( _display_title, -100 );

	_layout_display = new QBoxLayout( _layout, QBoxLayout::LeftToRight );
	_layout->setStretchFactor( _layout_display, -100 );

	_displaybox = new QButtonGroup( 1, Qt::Horizontal, i18n( "Timedisplay Style" ), this );
	_layout_display->addWidget( _displaybox, 100 );
	connect( _displaybox, SIGNAL( clicked( int ) ), this, SLOT( displaychanged( int ) ) );
	_display0 = new QRadioButton( i18n( "Plain samples" ), _displaybox );
	_display1 = new QRadioButton( i18n( "[hours:]mins:secs:samples" ), _displaybox );
	_display2 = new QRadioButton( i18n( "[hours:]mins:secs:frames" ), _displaybox );
	_display3 = new QRadioButton( i18n( "MByte.KByte" ), _displaybox );
	_framebasebox = new QButtonGroup( 1, Qt::Horizontal, i18n( "Framebase" ), this );
	_layout_display->addWidget( _framebasebox, 100 );
	connect( _framebasebox, SIGNAL( clicked( int ) ), this, SLOT( framebasechanged( int ) ) );
	_framebase30 = new QRadioButton( i18n( "30 frames per second (American TV)" ), _framebasebox );
	_framebase25 = new QRadioButton( i18n( "25 frames per second (European TV)" ), _framebasebox );
	_framebase75 = new QRadioButton( i18n( "75 frames per second (CD)" ), _framebasebox );
	_framebaseother = new QRadioButton( i18n( "Other" ), _framebasebox );
	_framebaseotherbox = new QHBox( _framebasebox );
	_framebaseotherbox->setSpacing( 2 );
	_framebaseotherlabel = new QLabel( i18n( "Other" ), _framebaseotherbox );
	_framebaseotherline = new QLineEdit( _framebaseotherbox );
	_framebaseotherline->setMaxLength( 10 );
	_framebaseotherbox->setEnabled( false );
	connect( _framebaseotherline, SIGNAL( textChanged( const QString& ) ), this, SLOT( framebaseotherchanged( const QString& ) ) );

	_layout->addSpacing( 5 );
	_verboseDisplayMode = new QCheckBox( i18n( "Show verbose times ( XXmins:XXsecs:XXframes instead of XX:XX::XX )" ), this );
	connect( _verboseDisplayMode, SIGNAL( toggled( bool ) ), this, SLOT( verboseDisplayChanged( bool ) ) );
	_layout->addWidget( _verboseDisplayMode );

	_layout->addSpacing( 10 );
	_other_title = new QLabel( i18n( "<qt><b>Miscellaneous Settings</b></qt>" ), this );
	_layout->addWidget( _other_title );

	_tipofday = new QCheckBox( i18n( "Show tip of the day at startup" ), this );
	connect( _tipofday, SIGNAL( toggled( bool ) ), this, SLOT( tipofdaychanged( bool ) ) );
	_layout->addWidget( _tipofday );
	QBoxLayout* _tmplayout = new QBoxLayout( this, QBoxLayout::LeftToRight );
	_enableAllMessages = new KPushButton( i18n( "Enable All Hidden Messages" ), this );
	connect( _enableAllMessages, SIGNAL( clicked() ), this, SLOT( enableallmessagesclicked() ) );
	_tmplayout->addWidget( _enableAllMessages );
	QLabel* _tmplbl = new QLabel( i18n( "<qt><i>All messages with the \"Don't show this message again\" option are shown again after selecting this button.</i></qt>" ), this );
	_tmplayout->addWidget( _tmplbl );
	_layout->addLayout( _tmplayout );

	_layout->addStretch( 100 );

	load();
}

KRecConfigGeneral::~KRecConfigGeneral() {
}

void KRecConfigGeneral::load() {
kdDebug( 60005 ) << k_funcinfo << endl;
	defaults();
	_displaymode = KRecGlobal::the()->timeFormatMode();
	switch ( _displaymode % 100 ) {
		default:
		case 0: _display0->setChecked( true ); break;
		case 1: _display1->setChecked( true ); break;
		case 2: _display2->setChecked( true ); break;
		case 3: _display3->setChecked( true ); break;
	};
	_verboseDisplayMode->setChecked( ( _displaymode / 100 == 1 ) );

	_framebase = KRecGlobal::the()->frameBase();
	switch ( _framebase ) {
		case 30: _framebase30->setChecked( true ); break;
		case 25: _framebase25->setChecked( true ); break;
		case 75: _framebase75->setChecked( true ); break;
		default:
			_framebaseother->setChecked( true );
			_framebaseotherbox->setEnabled( true );
			_framebaseotherline->setText( QString::number( _framebase ) );
			break;
	};
	kapp->config()->setGroup( "TipOfDay" );
	_tip = kapp->config()->readBoolEntry( "RunOnStart", true );
	_tipofday->setChecked( _tip );
}

void KRecConfigGeneral::save() {
	KRecGlobal::the()->setTimeFormatMode( _displaymode );
	KRecGlobal::the()->setFrameBase( _framebase );
kdDebug(60005) << k_funcinfo << "Framebase=" << _framebase << endl;

	kapp->config()->setGroup( "TipOfDay" );
	kapp->config()->writeEntry( "RunOnStart", _tip );

	kapp->config()->sync();
	emit changed( false );
}

void KRecConfigGeneral::defaults() {
	_display0->setChecked( true );
	_framebase25->setChecked( true );
}

void KRecConfigGeneral::displaychanged( int index ) {
	int verbose = _displaymode / 100;
	if ( _displaybox->find( index ) == _display0 ) _displaymode = 0 + verbose * 100;
	if ( _displaybox->find( index ) == _display1 ) _displaymode = 1 + verbose * 100;
	if ( _displaybox->find( index ) == _display2 ) _displaymode = 2 + verbose * 100;
	if ( _displaybox->find( index ) == _display3 ) _displaymode = 3 + verbose * 100;
	emit changed( true );
}
void KRecConfigGeneral::framebasechanged( int index ) {
	if ( _framebasebox->find( index ) == _framebase30 ) _framebase = 30;
	if ( _framebasebox->find( index ) == _framebase25 ) _framebase = 25;
	if ( _framebasebox->find( index ) == _framebase75 ) _framebase = 75;
	if ( _framebasebox->find( index ) == _framebaseother ) {
		_framebaseotherbox->setEnabled( true );
		_framebase = _framebaseotherline->text().toInt();
kdDebug(60005) << k_funcinfo << "Framebase=" << _framebase << endl;
	} else
		_framebaseotherbox->setEnabled( false );
	emit changed( true );
}
void KRecConfigGeneral::framebaseotherchanged( const QString& text ) {
	_framebase = text.toInt();
kdDebug(60005) << k_funcinfo << "Framebase=" << _framebase << endl;
	emit changed( true );
}
void KRecConfigGeneral::verboseDisplayChanged( bool n ) {
	if ( n && _displaymode < 100 ) _displaymode += 100;
	if ( !n && _displaymode >= 100 ) _displaymode -= 100;
	emit changed( true );
}

void KRecConfigGeneral::tipofdaychanged( bool n ) {
	_tip = n;
	emit changed( true );
}

void KRecConfigGeneral::enableallmessagesclicked() {
kdDebug(60005) << k_funcinfo << endl;
	KMessageBox::enableAllMessages();
}

