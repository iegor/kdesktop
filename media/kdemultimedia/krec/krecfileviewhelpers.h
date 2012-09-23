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

#ifndef KRECFILEVIEWHELPERS_H
#define KRECFILEVIEWHELPERS_H

#include <qframe.h>
#include <qlabel.h>
#include <qstring.h>
#include <qpoint.h>
#include <kglobalsettings.h>

class QBoxLayout;
class AKLabel;
class QPopupMenu;
class KPopupMenu;

class KRecTimeBar : public QFrame {
   Q_OBJECT
public:
	KRecTimeBar( QWidget*, const char* =0 );
	~KRecTimeBar();

	void drawContents( QPainter* );

	void mouseReleaseEvent( QMouseEvent* );

public slots:
	void newPos( int );
	void newSize( int );
signals:
	void sNewPos( int );
private:
	int _pos, _size;
};

/**
 * Modes are:
 *  0 - Just samples
 *  1 - [hours:]mins:secs:samples
 *  2 - [hours:]mins:secs:frames ( framebase from global )
 *  3 - [hours:]mins:secs:msecs
 *
 * +100 - verbose ( [XXhours:]XXmins:XXsecs:... )
*/

class KRecTimeDisplay : public QFrame {
   Q_OBJECT
public:
	KRecTimeDisplay( QWidget*, const char* =0 );
	~KRecTimeDisplay();

	/// Resets the display to its defaultvalues
	void reset();
public slots:
	void newPos( int );
	void newSize( int );
	void newFilename( const QString & );
	void newSamplingRate( int n ) { _samplingRate = n; }
	void newChannels( int n ) { _channels = n; }
	void newBits( int n ) { _bits = n; }
signals:
	void sNewPos( int );
private slots:
	void timeContextMenu( QPopupMenu* );
	void timeContextMenu( const QPoint &);
	void sizeContextMenu( QPopupMenu* );
	void sizeContextMenu( const QPoint &);
	void jumpToTime();
private:
	QString positionText( int, int );
	QString sizeText( int, int );
	QString formatTime( const int mode, const int samples ) const;
	QString _filename;
	QBoxLayout *_layout;
	AKLabel *_position, *_size;
	KPopupMenu *_posmenu, *_sizemenu;
	int _sizevalue, _posvalue;
	int _samplingRate, _bits, _channels;
};


class AKLabel : public QLabel {
  Q_OBJECT
public:
	AKLabel( QWidget* p, const char* n=0, WFlags f=0 ) : QLabel( p, n, f ) { init(); }
	AKLabel( const QString& s, QWidget* p, const char* n=0, WFlags f=0 ) : QLabel( s, p, n, f ) { init(); }
	AKLabel( QWidget* w, const QString& s, QWidget* p, const char* n=0, WFlags f=0 ) : QLabel( w,s,p,n,f ) { init(); }

	void mousePressEvent( QMouseEvent* );
signals:
	void showContextMenu( const QPoint & );
private:
	void init() {
		setFont( KGlobalSettings::fixedFont() );
	}
};
#endif

// vim:sw=4:ts=4
