/***************************************************************************
                          krecconfig_fileswidget.h  -  description
                             -------------------
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

#ifndef KREC_CONFIGURE_FILESWIDGET_H
#define KREC_CONFIGURE_FILESWIDGET_H

#include <qhbox.h>
#include <qvbox.h>
#include <qstringlist.h>
#include <qstring.h>

class QRadioButton;
class QCheckBox;
class QButtonGroup;
class QLineEdit;
class QLabel;

class KRecConfigFilesWidget : public QVBox {
   Q_OBJECT
public:
	KRecConfigFilesWidget( QWidget*, const char* =0 );
	~KRecConfigFilesWidget();

	void load();
	void save();
	void defaults();
signals:
	void sRateChanged( int );
	void sChannelsChanged( int );
	void sBitsChanged( int );
	void sUseDefaultsChanged( bool );
private slots:
	void ratechanged( int );
	void rateotherchanged( const QString& );
	void channelschanged( int );
	void bitschanged( int );
	void usedefaultschanged( bool );

private:
	QHBox *_hbox;
	QButtonGroup *_ratebox, *_channelsbox, *_bitsbox;
	QRadioButton *_rate48, *_rate44, *_rate22, *_rate11, *_rateother;
	QHBox *_rateotherbox;
	QLabel *_rateotherlabel;
	QLineEdit *_rateotherline;
	QRadioButton *_channels2, *_channels1;
	QRadioButton *_bits16, *_bits8;

	QCheckBox *_usedefaults;

	int _samplingRate, _channels, _bits;
};

#endif

// vim:sw=4:ts=4
