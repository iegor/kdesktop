/*

    Copyright ( C ) 2000-2001 Stefan Westerfeld
                              <stefan@space.twc.de>
                    2003 Arnold Krille <arnold@arnoldarts.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    ( at your option ) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "fftscopeview.h"

#include <qlayout.h>
#include <qcursor.h>
#include <qtimer.h>
#include <kaction.h>
#include <kpopupmenu.h>
#include <kartswidget.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>

#include "artsactions.h"

using namespace std;
using namespace Arts;

FFTScopeView::FFTScopeView( SimpleSoundServer server, QWidget* parent )
	: Template_ArtsView( parent )
	, server( server )
	, scopeData( 0 )
{
kdDebug()<<k_funcinfo<<endl;
	this->setCaption( i18n( "FFT Scope View" ) );
	this->setIcon( MainBarIcon( "artsfftscope", 32 ) );
	/*
	   create a stereo fft scope on the server and push it into the
	   effect chain
	*/
	{
		scopefx = DynamicCast( server.createObject( "Arts::StereoFFTScope" ) );
		assert( !scopefx.isNull() );
		scopefx.start();

		// put it into the effect chain
		effectID = server.outstack().insertBottom( scopefx,"FFT Scope" );
	}

	updateScopeData();
	QBoxLayout * l = new QHBoxLayout(  this );
	l->setAutoAdd( TRUE );

	for ( unsigned int i=0;i<scopeData->size();i++ )
	{
		LevelMeter tmp;
		tmp.count( 20 );
		scopeScales.push_back( tmp );
		scopeDraw.push_back( 0.0 );
		KArtsWidget *w = new KArtsWidget( tmp, this );
		aw.push_back( w );
	}

	l->activate();
	show();

	updatetimer = new QTimer( this );
	updatetimer->start( 100 );
	connect( updatetimer,SIGNAL( timeout() ),this,SLOT( updateScope() ) );

	_artsactions = new ArtsActions( 0, 0, this );
	_moreBars = ArtsActions::actionMoreBars( this, SLOT( moreBars() ), 0 );
	_lessBars = ArtsActions::actionLessBars( this, SLOT( lessBars() ), 0 );
	_menu = new KPopupMenu( 0 );
	_moreBars->plug( _menu ); _lessBars->plug( _menu );
	_substyle = new KAction( i18n( "Substyle" ), "", KShortcut(), this, SLOT( substyle() ), this );
	_substyle->plug( _menu );
	_menu->insertItem( i18n("VU-Style"), _artsactions->stylemenu() );

	connect( _artsactions, SIGNAL( styleNormal() ), this, SLOT( styleNormalBars() ) );
	connect( _artsactions, SIGNAL( styleFire() ), this, SLOT( styleFireBars() ) );
	connect( _artsactions, SIGNAL( styleLine() ), this, SLOT( styleLineBars() ) );
	connect( _artsactions, SIGNAL( styleLED() ), this, SLOT( styleLEDs() ) );
	connect( _artsactions, SIGNAL( styleAnalog() ), this, SLOT( styleAnalog() ) );
	connect( _artsactions, SIGNAL( styleSmall() ), this, SLOT( styleSmall() ) );
}

FFTScopeView::~FFTScopeView() {
kdDebug()<<"FFTScopeView::~FFTScopeView()"<<endl;
	updatetimer->stop();
	for ( int i=int( aw.size() )-1; i>=0; i-- ) { scopeScales[ i ].hide(); delete aw[ i ]; aw.pop_back(); scopeScales.pop_back(); }
	server.outstack().remove( effectID );
kdDebug()<<"FFTScopeView is gone..."<<endl;
}

void FFTScopeView::updateScopeData() {
	if ( scopeData ) delete scopeData;
	scopeData = scopefx.scope();
}

void FFTScopeView::updateScope() {
	updateScopeData();

	for ( unsigned int i=0;i<scopeData->size();i++ )
	{
//		scopeDraw[ i ] /= 1.25;
//		if ( ( *scopeData )[ i ] > scopeDraw[ i ] ) scopeDraw[ i ] = ( *scopeData )[ i ];
		scopeDraw[ i ] = ( *scopeData )[ i ];
		scopeScales[ i ].invalue( scopeDraw[ i ] );
	}
}

void FFTScopeView::mousePressEvent( QMouseEvent* ev ) {
	if ( Qt::RightButton == ev->button() /*|| Qt::LeftButton == ev->button()*/ )
		_menu->exec(  QCursor::pos() );
}

void FFTScopeView::moreBars() {
	int bars = scopeScales[ 0 ].count() + 10;
	for ( unsigned int i=0;i<scopeData->size();i++ )
		scopeScales[ i ].count( bars );
}

void FFTScopeView::lessBars() {
	int bars = scopeScales[ 0 ].count() - 10;
	for ( unsigned int i=0;i<scopeData->size();i++ )
		scopeScales[ i ].count( bars );
}

void FFTScopeView::setStyle( Arts::LevelMeterStyle style ) {
	for ( uint i=0; i<scopeScales.size(); i++ )
		scopeScales[ i ].style( style );
}

void FFTScopeView::styleNormalBars() { setStyle( Arts::lmNormalBars ); }
void FFTScopeView::styleFireBars() { setStyle( Arts::lmFireBars ); }
void FFTScopeView::styleLineBars() { setStyle( Arts::lmLineBars ); }
void FFTScopeView::styleLEDs() { setStyle( Arts::lmLEDs ); }
void FFTScopeView::styleAnalog() { setStyle( Arts::lmAnalog ); }
void FFTScopeView::styleSmall() { setStyle( Arts::lmSmall ); }

#include <kinputdialog.h>

void FFTScopeView::substyle() {
	int _substyle = KInputDialog::getInteger( i18n("Substyle"), i18n("Please enter substyle:"), 0, 0, 10, 1, 0, this );
	for ( unsigned int i=0; i<scopeData->size(); i++ )
		scopeScales[ i ].substyle( _substyle );
}

#include "fftscopeview.moc"
// vim: sw=4 ts=4

