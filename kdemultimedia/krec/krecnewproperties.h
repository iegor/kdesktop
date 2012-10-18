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

#ifndef KREC_NEW_PROPERTIES_H
#define KREC_NEW_PROPERTIES_H

#include <qdialog.h>

class KConfig;
class QBoxLayout;
class QButtonGroup;
class QRadioButton;
class KPushButton;
class QHBox;
class QVBox;

class KRecConfigFilesWidget;

class KRecNewProperties : public QDialog {
   Q_OBJECT
public:
	KRecNewProperties( QWidget*, const char* =0 );
	~KRecNewProperties();

	QString filename();
	int samplerate();
	int channels();
	int bits();
	bool usedefaults();

	void done( int );
private slots:
	void ratechanged( int );
	void channelschanged( int );
	void bitschanged( int );
	void usedefaultschanged( bool );
private:
	QString _filename;
	KRecConfigFilesWidget *_filewidget;

	int _samplerate, _channels, _bits;
	bool _usedefaults;

	QBoxLayout *_layout, *_layoutbuttons;
	KPushButton *_btnok;
};

#endif

// vim:sw=4:ts=4
