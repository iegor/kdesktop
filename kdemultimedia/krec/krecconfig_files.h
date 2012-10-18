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

#ifndef KREC_CONFIGURE_FILES_H
#define KREC_CONFIGURE_FILES_H

#include <kcmodule.h>
#include <qstringlist.h>
#include <qstring.h>

class QBoxLayout;
class QRadioButton;
class QButtonGroup;
class QLineEdit;
class QHBox;
class QLabel;

class KRecConfigFilesWidget;

class KRecConfigFiles : public KCModule {
   Q_OBJECT
public:
	KRecConfigFiles( QWidget*, const char* =0, const QStringList& = QStringList() );
	~KRecConfigFiles();

	void load();
	void save();
	void defaults();
private slots:
	void ratechanged( int );
	void channelschanged( int );
	void bitschanged( int );
	void usedefaultschanged( bool );

private:
	QBoxLayout *_layout;
	KRecConfigFilesWidget *_filewidget;
};

#endif

// vim:sw=4:ts=4
