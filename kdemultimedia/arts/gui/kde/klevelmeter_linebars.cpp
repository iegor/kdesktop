/*
    Copyright (  C ) 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include "klevelmeter_linebars.h"

#include <kdebug.h>
#include <qpainter.h>
#include <qpixmap.h>

KLevelMeter_LineBars::KLevelMeter_LineBars( Arts::KLevelMeter_impl* impl, QWidget* parent, long substyle, long count, Arts::Direction dir, float _dbmin, float _dbmax ) : KLevelMeter_Template( impl, parent, substyle, count, dir, _dbmin, _dbmax ) 
 , _value( 0.0 )
 , _peak( 0.0 )
{
//kdDebug()<<"KLevelMeter_LineBars::KLevelMeter_LineBars( Arts::KLevelMeter_impl* "<<impl<<", QWidget* "<<parent<<", long "<<substyle<<", long "<<count<<", Arts::Direction "<<dir<<", float "<<_dbmin<<", float "<<_dbmax<<" )"<<endl;
	this->setMinimumSize( 5, 5 );
	this->substyle( substyle );
	_stdcolors = colorGroup();
	setBackgroundMode( Qt::NoBackground );
}

void KLevelMeter_LineBars::invalue( float n, float p ) {
	_value = amptondb( n );
	_peak  = amptondb( p );
	repaint();
}

void KLevelMeter_LineBars::substyle( long n ) {
	//kdDebug() << k_funcinfo << n << endl;
	_substyle = n;
}
long KLevelMeter_LineBars::substyle() { return _substyle; }

void KLevelMeter_LineBars::paintEvent( QPaintEvent* ) {
	QPixmap pm( size() );
	QPainter p( &pm );

	switch ( _dir ) {
		case Arts::BottomToTop:
			break;
		case Arts::TopToBottom:
			p.rotate( 180.0 );
			p.translate( -width() + 1, -height() + 1 );
			break;
		case Arts::LeftToRight:
			p.rotate( 90.0 );
			p.translate( 0, -width() + 1 );
			break;
		case Arts::RightToLeft:
			p.rotate( 270.0 );
			p.translate( -height() + 1, 0 );
			break;
	}

	if ( _substyle & 1 )
		p.setBrush( ( _peak<1 )?QColor( 0,0,255 ):QColor( 255,0,0 ) );
	else
		p.setBrush( ( _peak<1 )?color( _value ):QColor( 255,0,0 ) );

	QColor bgcolor = ( _substyle & 2 ) ? p.brush().color().dark() : _stdcolors.background();
	pm.fill( bgcolor );

	p.setPen( NoPen );

	QSize s = size();
	if ( Arts::LeftToRight == _dir || Arts::RightToLeft == _dir )
		s.transpose();

	// Value
	int h = int( s.height() * _value );
	int top = s.height() - h;
	int w = s.width();
	p.drawRect( 0, top, w, h );
	// PeakBar
	if ( _peak > 1.0/1000 && _peak <= 1.0 ) {
		p.setPen( QColor( 255-bgcolor.red(), 255-bgcolor.green(), 255-bgcolor.blue() ) );
		top = int( s.height() * ( 1 - _peak ) );
		p.drawLine( 0, top, w, top );
	}

	bitBlt( this, 0, 0, &pm, 0, 0, pm.width(), pm.height(), CopyROP, true );
}

/**
	Planned feature: a little Tooltip showing the actual value of the volume in deziBel and perhaps as linear scaleFactor
*/
void KLevelMeter_LineBars::mouseMoveEvent( QMouseEvent* /*qme*/ ) {
//kdDebug()<<"KLevelMeter_LineBars::mouseMoveEvent(QMouseEvent* "<<qme<<" )"<<endl;
//kdDebug()<<"qme.y()="<<this->height()-qme->y()<<" db="<<db<<" dbtoamp(db)="<<dbtoamp( db )<<endl;
}

#include <klevelmeter_linebars.moc>
// vim: sw=4 ts=4
