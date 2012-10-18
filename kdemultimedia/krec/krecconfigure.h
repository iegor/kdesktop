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

#ifndef KREC_CONFIGURE_H
#define KREC_CONFIGURE_H

#include <kcmodule.h>
#include <qstringlist.h>
#include <qstring.h>

class QBoxLayout;
class QRadioButton;
class QButtonGroup;
class QLineEdit;
class QHBox;
class QLabel;
class QCheckBox;
class KPushButton;

class KRecConfigGeneral : public KCModule {
   Q_OBJECT
public:
	KRecConfigGeneral( QWidget*, const char* =0, const QStringList& = QStringList() );
	~KRecConfigGeneral();

	void load();
	void save();
	void defaults();
private slots:

	void displaychanged( int );
	void framebasechanged( int );
	void framebaseotherchanged( const QString& );
	void verboseDisplayChanged( bool );

	void tipofdaychanged( bool );
	void enableallmessagesclicked();
private:
	QBoxLayout *_layout, *_layout_display;
	QLabel *_display_title, *_other_title;
	QButtonGroup *_displaybox, *_framebasebox;
	QRadioButton *_display0, *_display1, *_display2, *_display3;
	QRadioButton *_framebase30, *_framebase25, *_framebase75, *_framebaseother;
	QHBox *_framebaseotherbox;
	QLabel *_framebaseotherlabel;
	QLineEdit *_framebaseotherline;

	QCheckBox *_tipofday, *_verboseDisplayMode;
	KPushButton *_enableAllMessages;

	int _displaymode, _framebase;
	bool _tip;
};

#endif

// vim:sw=4:ts=4
