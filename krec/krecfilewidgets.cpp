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

#include "krecfilewidgets.h"
#include "krecfilewidgets.moc"

#include "krecfile.h"
#include "krecglobal.h"

#include <kdebug.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qlineedit.h>
#include <kinputdialog.h>
#include <qregion.h>
#include <qrect.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <kpopupmenu.h>
#include <kactionclasses.h>
#include <kaction.h>

KRecFileWidget::KRecFileWidget( KRecFile* file, QWidget* p, const char* n )
  : QFrame( p,n )
  , _file( 0 )
{
	kdDebug( 60005 ) << k_funcinfo << file << endl;
	setFrameStyle( QFrame::Panel|QFrame::Sunken );
	setLineWidth( 1 );
	setMinimumHeight( 20 );

	setFile( file );
}
KRecFileWidget::~KRecFileWidget() {
	kdDebug( 60005 ) << k_funcinfo << endl;
}

void KRecFileWidget::setFile( KRecFile* file ) {
kdDebug( 60005 ) << k_funcinfo << file << endl;
	if ( _file != file ) {
		_file = file;
			//kdDebug( 60005 ) << "Removing widgets" << endl;
			QValueList<KRecBufferWidget*>::iterator it = bufferwidgets.begin();
			while ( it != bufferwidgets.end() ) {
				delete ( *it );
				++it;
			}
			bufferwidgets.clear();
			resizeEvent();
		if ( _file ) {
			for ( QValueList<KRecBuffer*>::iterator it = _file->_buffers.begin(); it != _file->_buffers.end(); ++it ) {
				newBuffer( ( *it ) );
			}
			connect( _file, SIGNAL( sNewBuffer( KRecBuffer* ) ), this, SLOT( newBuffer( KRecBuffer* ) ) );
			connect( _file, SIGNAL( sDeleteBuffer( KRecBuffer* ) ), this, SLOT( deleteBuffer( KRecBuffer* ) ) );
			//kdDebug( 60005 ) << _file->_currentBuffer << endl;
		}
	}
}

void KRecFileWidget::resizeEvent( QResizeEvent* /*qre*/ ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _file ) {
		int w = contentsRect().width();
		int h = contentsRect().height();
		QValueList<KRecBufferWidget*>::iterator it;
		for ( it = bufferwidgets.begin(); it != bufferwidgets.end(); ++it ) {
			int width, x;
			if ( !_file->offsetSize()==0 && !( *it )->buffer()->size()==0 ) {
				width = int ( float( ( *it )->buffer()->size() ) / _file->offsetSize() * w );
				x = int( float( ( *it )->buffer()->startpos() ) / _file->size() * w ) + contentsRect().left() ;
			} else {
				width = 5;
				x = contentsRect().left();
			}
			( *it )->setGeometry( x, contentsRect().top(), width, h );
		}
	}
}

void KRecFileWidget::mouseReleaseEvent( QMouseEvent* qme ) {
	kdDebug( 60005 ) << k_funcinfo << "(" << qme->x() << "|" << qme->y() << ")" << endl;
}

void KRecFileWidget::newBuffer( KRecBuffer* buffer ) {
	//kdDebug( 60005 ) << k_funcinfo << buffer << endl;
	KRecBufferWidget *tmp = new KRecBufferWidget( buffer, this );
	connect( tmp, SIGNAL( popupMenu( KRecBufferWidget*, QPoint ) ), this, SLOT( popupMenu( KRecBufferWidget*, QPoint ) ) );
	bufferwidgets.append( tmp );
	tmp->show();
	resizeEvent();
}
void KRecFileWidget::deleteBuffer( KRecBuffer* buffer ) {
	//kdDebug( 60005 ) << k_funcinfo << buffer << endl;
	QValueList<KRecBufferWidget*>::iterator it = bufferwidgets.begin();
	KRecBufferWidget* tmp = 0;
	while ( it != bufferwidgets.end() ) {
		if ( ( *it )->buffer() == buffer ) tmp = ( *it );
		++it;
	}
	if ( tmp != 0 ) {
		delete tmp;
		bufferwidgets.remove( tmp );
	}
}

void KRecFileWidget::popupMenu( KRecBufferWidget* bw, QPoint pos ) {
	KPopupMenu tmp( this );
	KToggleAction* _activeaction = new KToggleAction( i18n( "Toggle Active/Disabled State" ), KShortcut(), this );
	_activeaction->setChecked( bw->buffer()->active() );
	connect( _activeaction, SIGNAL( toggled( bool ) ), bw->buffer(), SLOT( setActive( bool ) ) );
	KAction* _removeaction = new KAction( i18n( "Remove This Part" ), "fileremove", KShortcut(), bw->buffer(), SLOT( deleteBuffer() ), this );
	KAction* _changetitle = new KAction( i18n( "Change Title of This Part" ), KShortcut(), bw, SLOT( changeTitle() ), this );
	KAction* _changecomment = new KAction( i18n( "Change Comment of This Part" ), KShortcut(), bw, SLOT( changeComment() ), this );
	_activeaction->plug( &tmp );
	_changetitle->plug( &tmp );
	_changecomment->plug( &tmp );
	tmp.insertSeparator();
	_removeaction->plug( &tmp );

	tmp.exec( pos );

	delete _removeaction;
	delete _changecomment;
	delete _changetitle;
	delete _activeaction;
}



KRecBufferWidget::KRecBufferWidget( KRecBuffer* buffer, QWidget* p, const char* n )
  : QFrame( p,n )
  , _buffer( buffer )
  , _main_region( 0 ), _title_region( 0 ), _fileend_region( 0 )
  , alreadyreadsize( 0 )
{
	connect( _buffer, SIGNAL( somethingChanged() ), this, SLOT( update() ) );
	kdDebug( 60005 ) << k_funcinfo << endl;
}
KRecBufferWidget::~KRecBufferWidget() {
	kdDebug( 60005 ) << k_funcinfo << endl;
}

void KRecBufferWidget::resizeEvent( QResizeEvent* ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
}

void KRecBufferWidget::initLayout() {
	_title_height = fontMetrics().boundingRect( _buffer->filename() ).height() + 4;
	if( height()/5 > _title_height ) _title_height = height()/5;
	int _title_width = fontMetrics().boundingRect( _buffer->filename() ).width() + 10;
	if ( _title_width > width() ) _title_width = width();
	if ( _main_region ) delete _main_region;
	_main_region = new QRegion( QRect( 0, _title_height, width(), height()-_title_height ) );
	if ( _title_region ) delete _title_region;
	if ( _buffer->active() )
		_title_region = new QRegion( QRect( 0, 0, _title_width, _title_height ) );
	else
		_title_region = new QRegion( QRect( 0, _title_height/2, _title_width, _title_height/2 ) );
	if ( _fileend_region ) { delete _fileend_region; _fileend_region=0; }
	if ( _buffer->active() )
		_fileend_region = new QRegion( QRect( width()-4, _title_height/2, 4, _title_height/2 ) );
	else
		_fileend_region = new QRegion( QRect( width()-4, _title_height/4*3, 4, _title_height/4 ) );

	setMask( _main_region->unite( *_title_region ).unite( *_fileend_region ) );
	
	_topleft = _title_region->boundingRect().topLeft();
	_bottomleft = _main_region->boundingRect().bottomLeft();
	_bottomright = _main_region->boundingRect().bottomRight();
	_topright = _main_region->boundingRect().topRight();
	_bottommiddle = _title_region->boundingRect().bottomRight();
	_bottommiddle += QPoint( 0, 1 );
	_topmiddle = _title_region->boundingRect().topRight();
}
void KRecBufferWidget::drawFrame( QPainter* p ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _buffer->active() )
		p->setBrush( colorGroup().highlight() );
	else
		p->setBrush( colorGroup().highlight().dark() );
	p->setPen( QPen( colorGroup().dark(), 1 ) );
	p->drawRect( _title_region->boundingRect() );
	p->drawRect( _fileend_region->boundingRect() );

	p->setBrush( QBrush() );
	p->setPen( QPen( colorGroup().dark(), 1 ) );
	p->drawRect( _main_region->boundingRect() );

	p->setPen( QPen( colorGroup().highlightedText() ) );
	p->drawText( _title_region->boundingRect(), Qt::AlignCenter, _buffer->title() );

}
void KRecBufferWidget::drawContents( QPainter* p ) {
	//kdDebug( 60005 ) << k_funcinfo << endl;
	initSamples();
	int space = ( _main_region->boundingRect().height() - 2 ) / 2;
	int xoffset = _main_region->boundingRect().top() + space + 1;
	p->setPen( QPen( QColor( 0,0,255 ) ) );
	for ( uint i=0; i<samples1.count(); i++ )
		p->drawPoint( i, int( samples1[ i ]->getMax() * space + xoffset ) );
	for ( uint i=0; i<samples1.count(); i++ )
		p->drawPoint( i, int( samples1[ i ]->getMin() * space + xoffset ) );
	p->setPen( QPen( QColor( 255,0,0 ) ) );
	for ( uint i=0; i<samples1.count(); i++ )
		p->drawPoint( i, int( samples1[ i ]->getValue() * space + xoffset ) );
	p->setPen( QPen( QColor( 0,0,0 ) ) );
	QString comment = _buffer->comment();
	if ( comment.isNull() ) comment = i18n( "Lots of Data" );
	p->drawText( _main_region->boundingRect(), Qt::AlignCenter, comment );
}
void KRecBufferWidget::paintEvent( QPaintEvent* ) {
	initLayout();
	QPainter *p = new QPainter( this );
	drawFrame( p );
	drawContents( p );
	delete p;
}

void KRecBufferWidget::mousePressEvent( QMouseEvent* qme ) {
	kdDebug( 60005 ) << k_funcinfo << endl;
	if ( _main_region->contains( qme->pos() ) || _title_region->contains( qme->pos() ) )
		if ( qme->button() == Qt::RightButton )
			emit popupMenu( this, qme->globalPos() );
}
void KRecBufferWidget::mouseDoubleClickEvent( QMouseEvent* qme ) {
	if ( _title_region->contains( qme->pos() ) )
		_buffer->setActive( !_buffer->active() );
}

void KRecBufferWidget::initSamples() {
#if 0
	/**
	 * Experimental.
	 *
	 * Doesn't really work since the conversion from char8/char16 to float in
	 * krecfile.cpp isn't working.
	 */
	//kdDebug( 60005 ) << k_funcinfo << endl;
	if ( alreadyreadsize != _buffer->size() ) {
		kdDebug( 60005 ) << k_funcinfo << "something to do" << endl;
		while ( samples1.count() > 0 ) {
			delete samples1.first();
			samples1.pop_front();
		}
		while ( samples2.count() > 0 ) {
			delete samples2.first();
			samples2.pop_front();
		}
		Sample* tmp = new Sample();
		samples1.append( tmp );
		int j=0;
		for ( int i=0; i<= _buffer->sizeInSamples(); ++i ) {
		//for ( int i=0; i<= 10000; ++i ) {
			if ( j/128 ) {
				//kdDebug(60005) << "sample " << i << ": creating new Sample" << endl;
				tmp = new Sample();
				samples1.append( tmp );
				j=0;
			}
			tmp->addValue( _buffer->getSample( i, 0 ) );
			//kdDebug(60005) << "Getting value " << _buffer->getSample( i,0 ) << endl;
			++j;
		}

		tmp = new Sample;
		samples2.append( tmp );
		j=0;
		for ( uint i=0; i<samples1.count(); ++i ) {
			if ( j/128 ) {
				tmp = new Sample();
				samples2.append( tmp );
				j=0;
			}
			tmp->addValue( samples1[ i ]->getValue() );
			++j;
		}

//		for ( uint i=0; i<samples1.count(); ++i )
//			kdDebug(60005) << i << ": " << samples1[ i ]->getMin() << " < " << samples1[ i ]->getValue() << " < " << samples1[ i ]->getMax() << endl;
		for ( uint i=0; i<samples2.count(); ++i )
			kdDebug(60005) << i << ": " << samples2[ i ]->getMin() << " < " << samples2[ i ]->getValue() << " < " << samples2[ i ]->getMax() << endl;

		alreadyreadsize = _buffer->size();
	}
#endif
}


void KRecBufferWidget::changeTitle() {
	QString tmp = KInputDialog::getText( i18n( "New Title" ), i18n( "Enter new part title:" ), _buffer->title() );
	if ( !tmp.isNull() )
		_buffer->setTitle( tmp );
}
void KRecBufferWidget::changeComment() {
	QString tmp = KInputDialog::getText( i18n( "New Comment" ), i18n( "Enter new part comment:" ), _buffer->comment() );
	if ( !tmp.isNull() )
		_buffer->setComment( tmp );
}

