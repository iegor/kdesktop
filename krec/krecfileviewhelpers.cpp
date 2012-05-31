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

#include "krecfileviewhelpers.h"
#include "krecfileviewhelpers.moc"

#include "krecglobal.h"

#include <kdebug.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qpopupmenu.h>
#include <kpopupmenu.h>

KRecTimeBar::KRecTimeBar( QWidget* p, const char* n ) : QFrame( p,n )
 ,_size(10), _pos(0)
 {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	setMinimumSize( 10,10 );
	setFrameStyle( QFrame::Panel|QFrame::Plain );
	setLineWidth( 1 );
}

KRecTimeBar::~KRecTimeBar() {
	//kdDebug( 60005 ) << k_funcinfo << endl;
}

void KRecTimeBar::drawContents( QPainter* p ) {
	int w = contentsRect().width();
	int h = contentsRect().height();
	int t = contentsRect().top();
	int l = contentsRect().left();
	p->setPen( QColor( 255,0,0 ) );
	if ( _pos < _size ) {
		int x = int( l + w * float( _pos ) / _size );
		p->drawLine( x, t, x, int( t+h/2 ) );
	} else {
		QPointArray tmp;
		tmp.putPoints( 0, 4, l+w-3,t+h/4, l+w-3,t+h/4*3, l+w,t+h/2, l+w-3,t+h/4 );
		p->drawPolyline( tmp );
	}
}

void KRecTimeBar::mouseReleaseEvent( QMouseEvent* qme ) {
	QIODevice::Offset pos = 0;
	if ( _size > 0 )
		pos = int( float( qme->x() - contentsRect().left() ) / contentsRect().width() * _size );
	//kdDebug( 60005 ) << k_funcinfo << "New Position: " << pos << endl;
	emit sNewPos( pos );
}

void KRecTimeBar::newPos( int n ) {
	_pos = n;
	repaint();
}

void KRecTimeBar::newSize( int n ) {
	_size = n;
	repaint();
}


KRecTimeDisplay::KRecTimeDisplay( QWidget* p, const char* n )
  : QFrame( p,n )
  , _posmenu( 0 ), _sizemenu( 0 )
  , _sizevalue( 0 ), _posvalue( 0 )
  , _samplingRate( 44100 ), _bits( 16 ), _channels( 2 )
{
	_position = new AKLabel( this );
	connect( _position, SIGNAL( showContextMenu( const QPoint & ) ), this, SLOT( timeContextMenu( const QPoint &) ) );
	_size = new AKLabel( this );
	connect( _size, SIGNAL( showContextMenu( const QPoint &) ), this, SLOT( sizeContextMenu( const QPoint &) ) );

	_layout = new QBoxLayout( this, QBoxLayout::LeftToRight, 0, 2 );
	_layout->addStretch( 100 );
	_layout->addWidget( _position );
	_layout->addSpacing( 2 );
	_layout->addWidget( _size );

	reset();
}
KRecTimeDisplay::~KRecTimeDisplay() {
}

void KRecTimeDisplay::reset() {
	_position->setText( positionText( 0, 0 ) );
	_size->setText( sizeText( 0, 0 ) );
	_position->setFrameStyle( QFrame::Panel|QFrame::Sunken );
	_position->setLineWidth( 1 );
	_position->setMargin( 2 );
	_size->setFrameStyle( QFrame::Panel|QFrame::Sunken );
	_size->setLineWidth( 1 );
	_size->setMargin( 2 );
}

void KRecTimeDisplay::newPos( int n ) {
	_posvalue = n;
	_position->setText( positionText( KRecGlobal::the()->timeFormatMode(), _posvalue ) );
	static int timeformat = KRecGlobal::the()->timeFormatMode();
	if ( timeformat != KRecGlobal::the()->timeFormatMode() ) {
		timeformat = KRecGlobal::the()->timeFormatMode();
		newSize( _sizevalue );
	}
}
void KRecTimeDisplay::newSize( int n ) {
	_sizevalue = n;
	_size->setText( sizeText( KRecGlobal::the()->timeFormatMode(), n ) );
}
void KRecTimeDisplay::newFilename( const QString &n ) {
	_filename = n;
	if ( _filename.isNull() ) {
		newPos( 0 );
		newSize( 0 );
	}
}

QString KRecTimeDisplay::formatTime( const int mode, const int sample ) const {
	QString text;
	bool verbose = mode / 100;
	switch ( mode % 100 ) {
		case 3:
			{
			int bytes = sample * ( _bits/8 ) * _channels / 1024;
			int kbyte = bytes % 1024;
			bytes /= 1024;
			int mbyte = bytes % 1024;
			text += QString::number( mbyte );
			if ( verbose ) text += i18n( "MB" );
			text += ".";
			if ( kbyte < 1000 ) text += "0";
			if ( kbyte < 100 ) text += "0";
			if ( kbyte < 10 ) text += "0";
			text += QString::number( kbyte );
			if ( verbose ) text += i18n( "kB" );
			}
			break;
		case 2:
			{
			int value = sample / ( _samplingRate / KRecGlobal::the()->frameBase() ); // overall frames
			int frames = value % KRecGlobal::the()->frameBase();
			value = value / KRecGlobal::the()->frameBase();
			int secs = value % 60;
			value = value / 60;
			int mins = value % 60;
			value = value / 60;
			if ( value ) {
				text += QString::number( value );
				if ( verbose ) text += i18n( "hours" );
				text += ":";
			}
			if ( mins<10 ) text += "0";
			text += QString::number( mins );
			if ( verbose ) text += i18n( "mins" );
			text += ":";
			if ( secs<10 ) text += "0";
			text += QString::number( secs );
			if ( verbose ) text += i18n( "secs" );
			text += ".";
			if ( frames < 10 ) text += "0";
			text += QString::number( frames );
			if ( verbose ) text += i18n( "frames" );
			}
			break;
		case 1:
			{
			int value = sample; // overall samples
			int samples = value % ( _samplingRate / 60 );
			value = value / ( _samplingRate / 60 );
			int secs = value % 60;
			value = value / 60;
			int mins = value % 60;
			value = value / 60;
			if ( value ) {
				text += QString::number( value );
				if ( verbose ) text += i18n( "hours" );
				text += ":";
			}
			if ( mins<10 ) text += "0";
			text += QString::number( mins );
			if ( verbose ) text += i18n( "mins" );
			text += ":";
			if ( secs<10 ) text += "0";
			text += QString::number( secs );
			if ( verbose ) text += i18n( "secs" );
			text += ".";
			if ( samples < 10000 && ( _samplingRate / 60 > 10000 ) ) text += "0";
			if ( samples < 1000 ) text += "0";
			if ( samples < 100 ) text += "0";
			if ( samples < 10 ) text += "0";
			text += QString::number( samples );
			if ( verbose ) text += i18n( "samples" );
			}
			break;
		case 0:
		default:
			text = QString::number( sample );
			if ( verbose ) text += i18n( "samples" );
			break;
	};
	return text;
}

void KRecTimeDisplay::timeContextMenu( QPopupMenu* menu ) {
	if ( !_filename.isNull() ) {
		menu->insertSeparator( 0 );
		menu->insertItem( i18n( "kByte: %1" ).arg( formatTime( 3, _posvalue ) ), -1, 0 );
		menu->insertItem( i18n( "[h:]m:s.f %1" ).arg( formatTime( 2, _posvalue ) ), -1, 0 );
		menu->insertItem( i18n( "[h:]m:s.s %1" ).arg( formatTime( 1, _posvalue ) ), -1, 0 );
		menu->insertItem( i18n( "%1 Samples" ).arg( formatTime( 0, _posvalue ) ), -1, 0 );
		KPopupTitle *tmp = new KPopupTitle( menu );
		tmp->setTitle( i18n( "Position" ) );
		menu->insertItem( tmp, -1, 0 );
	} else
		menu->insertItem( i18n( "<no file>" ), -1, 0 );
}
void KRecTimeDisplay::timeContextMenu( const QPoint &point ) {
	if ( _posmenu ) delete _posmenu;
	_posmenu = new KPopupMenu( this );
	timeContextMenu( _posmenu );
	_posmenu->exec( point );
}
void KRecTimeDisplay::sizeContextMenu( QPopupMenu* menu ) {
	if ( !_filename.isNull() ) {
		menu->insertSeparator( 0 );
		menu->insertItem( i18n( "kByte: %1" ).arg( formatTime( 3, _sizevalue ) ), -1, 0 );
		menu->insertItem( i18n( "[h:]m:s.f %1" ).arg( formatTime( 2, _sizevalue ) ), -1, 0 );
		menu->insertItem( i18n( "[h:]m:s.s %1" ).arg( formatTime( 1, _sizevalue ) ), -1, 0 );
		menu->insertItem( i18n( "%1 Samples" ).arg( formatTime( 0, _sizevalue ) ), -1, 0 );
		KPopupTitle *tmp = new KPopupTitle( menu );
		tmp->setTitle( i18n( "Size" ) );
		menu->insertItem( tmp, -1, 0 );
	} else
		menu->insertItem( i18n( "<no file>" ), -1, 0 );
}
void KRecTimeDisplay::sizeContextMenu( const QPoint &point ) {
	if ( _sizemenu ) delete _sizemenu;
	_sizemenu = new KPopupMenu( this );
	sizeContextMenu( _sizemenu );
	_sizemenu->exec( point );
}
void KRecTimeDisplay::jumpToTime() {
}

QString KRecTimeDisplay::positionText( int m, int n ) {
	return i18n( "Position: %1" ).arg( formatTime( m,n ) );
}
QString KRecTimeDisplay::sizeText( int m, int n ) {
	return i18n( "Size: %1" ).arg( formatTime( m,n ) );
}

void AKLabel::mousePressEvent( QMouseEvent* qme ) {
	if ( qme->button() == Qt::RightButton )
		emit showContextMenu( qme->globalPos() );
}

