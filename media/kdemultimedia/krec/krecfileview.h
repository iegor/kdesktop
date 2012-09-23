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

#ifndef KRECFILEVIEW_H
#define KRECFILEVIEW_H

#include <qwidget.h>

class KRecFile;
class KRecBuffer;
class QBoxLayout;
class QLabel;
class QLineEdit;

class KRecFileWidget;
class KRecBufferWidget;
class KRecTimeBar;
class KRecTimeDisplay;

class KRecFileView : virtual public QWidget {
   Q_OBJECT
public:
	KRecFileView( QWidget*, const char* =0 );
	~KRecFileView();

	void setFile( KRecFile* );
public slots:
	/// Mainly calls resize() on the KRecFileWidget.
	void updateGUI();
private slots:
	void setPos( int );
	void setSize( int );
	void setFilename( const QString & );
private:
	QBoxLayout *_layout_td, *_layout_lr;
	QLabel *_filename;

	KRecFile *_file;
	KRecFileWidget *_fileview;
	KRecTimeBar *_timebar;
	KRecTimeDisplay *_timedisplay;
};

#endif

// vim:sw=4:ts=4
