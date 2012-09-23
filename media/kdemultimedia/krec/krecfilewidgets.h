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

#ifndef KRECFILEWIDGETS_H
#define KRECFILEWIDGETS_H

#include <qframe.h>
#include <qvaluelist.h>
#include <qpoint.h>

class KRecFile;
class KRecBuffer;
class TimeDisplay;
class QBoxLayout;
class QLabel;
class QLineEdit;

class KRecFileWidget;
class KRecBufferWidget;
class KRecTimeBar;
class KRecTimeDisplay;

class QRegion;
class QPainter;
class KAction;
class KToggleAction;

class KRecFileWidget : public QFrame {
   Q_OBJECT
public:
	KRecFileWidget( KRecFile*, QWidget*, const char* =0 );
	~KRecFileWidget();

	void setFile( KRecFile* );

	void resizeEvent( QResizeEvent* =0 );
	void mouseReleaseEvent( QMouseEvent* );
public slots:
	void newBuffer( KRecBuffer* );
	void deleteBuffer( KRecBuffer* );
private slots:
	void popupMenu( KRecBufferWidget*, QPoint );
private:
	KRecFile *_file;
	QValueList<KRecBufferWidget*> bufferwidgets;
};

class Sample : public QObject {
   Q_OBJECT
public:
	Sample() : _values( 0 ), _min( 0 ), _max( 0 ) {}

	void addValue( float n ) { _values += n; ++_count; addMin( n ); addMax( n ); }
	void addMin( float n ) { if ( n < _min ) _min = n; }
	void addMax( float n ) { if ( n > _max ) _max = n; }

	float getValue() const { return _values / _count; }
	float getMax() const { return ( _max>1 )?1:_max; }
	float getMin() const { return ( _min<-1 )?-1:_min; }
	int getCount() const { return _count; }
private:
	float _values, _min, _max;
	int _count;
};

class KRecBufferWidget : public QFrame {
   Q_OBJECT
public:
	KRecBufferWidget( KRecBuffer*, QWidget*, const char* =0 );
	~KRecBufferWidget();

	const KRecBuffer* buffer() { return _buffer; }

	void resizeEvent( QResizeEvent* );

	void drawFrame( QPainter* );
	void drawContents( QPainter* );
	void paintEvent( QPaintEvent* );

	void mousePressEvent( QMouseEvent* );
	void mouseDoubleClickEvent( QMouseEvent* );
signals:
	void popupMenu( KRecBufferWidget*, QPoint );
private slots:
	void initSamples();
	void changeTitle();
	void changeComment();
private:
	void initLayout();
	KRecBuffer *_buffer;
	QRegion *_main_region, *_title_region, *_fileend_region;
	QPoint _topleft, _bottomleft, _bottomright, _topright, _topmiddle, _bottommiddle;
	int _title_height;
	QValueList <Sample*> samples1, samples2;
	uint alreadyreadsize;
};

#endif

// vim:sw=4:ts=4
