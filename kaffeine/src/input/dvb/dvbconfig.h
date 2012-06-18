/*
 * dvbconfig.h
 *
 * Copyright (C) 2004-2007 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef DVBCONFIG_H
#define DVBCONFIG_H

#include <qspinbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qlistview.h>
#include <qcheckbox.h>

#include <kdialogbase.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kio/job.h>
#include <kprogress.h>

#include <linux/dvb/frontend.h>

#define MAX_DEVICES 8

using namespace KIO;

class MSpinBox : public QSpinBox
{
	Q_OBJECT
public:
	MSpinBox( QWidget *parent, int devNum );
public slots:
	void slotValueChanged( int value );
signals:
	void signalValueChanged( int value, int devNum );
private:
	int deviceNumber;
};



class MPushButton : public KPushButton
{
	Q_OBJECT
public:
	MPushButton( QWidget *parent, int devNum, int lnbNum );
private slots:
	void isClicked();
signals:
	void clicked( int devnum, int lnbnum );
private:
	int deviceNumber;
	int lnbNumber;
};



class MCAMButton : public QPushButton
{
	Q_OBJECT
public:
	MCAMButton( QWidget *parent, int devNum );
private slots:
	void isClicked();
signals:
	void clicked( int devnum );
private:
	int deviceNumber;
};



class MComboBox : public QComboBox
{
	Q_OBJECT
public:
	MComboBox( QWidget *parent, int devNum, int lnbNum );
private slots:
	void isActivated( int index );
signals:
	void activated( int index, int devnum, int lnbnum );
private:
	int deviceNumber;
	int lnbNumber;
};



class LNB
{
public:
	LNB();

	unsigned int switchFreq;
	unsigned int loFreq;
	unsigned int hiFreq;
	int rotorType;
	double speed13v, speed18v;
	QStringList source;
	QValueList<int> position;
	QString currentSource;
};



class Device
{
public:
	Device( int anum, int tnum, fe_type_t t, const QString &n, bool as );
	int adapter, tuner;
	fe_type_t type;
	QString name;
	QString source;
	int numLnb;
	LNB lnb[4];
	bool canAutoscan;
	int tuningTimeout;
	double usalsLatitude, usalsLongitude;
	bool hasCAM;
	int camMaxService;
	int secMini;
	int secTwice;
	int priority;
	int doS2;
};



class Category
{
public:
	Category( const QString &tname, const QString &ticon );
	QString name;
	QString icon;
};



class DVBconfig : public QObject
{
	Q_OBJECT

public:

	DVBconfig( const QString &dvbConf );
	~DVBconfig();
	void readFirst();
	void readConfig();
	void saveConfig();
	int readDvbChanOrder();
	void saveDvbChanOrder( int s, int col );
	void startup();
	static bool haveDvbDevice();
	bool loadDvbData( QWidget *parent );
	bool localData();
	QStringList getSourcesList( fe_type_t type );
	bool haveData();
	bool firstRun();
	void addCategory( const QString &name, const QString &icon );
	void removeCategory( const QString &name );
	void changeIconCategory( const QString &name, const QString &icon );

	KConfig *config;
	QString recordDir, shiftDir, filenameFormat;
	int beginMargin, endMargin, instantDuration, sizeFile;
	QSize epgSize, scanSize, timerSize;
	QPtrList<Device> devList;
	QPtrList<Category> categories;
	QString allIcon, tvIcon, radioIcon;
	QString dvbConfigDir;
	QString dvbConfigIconsDir;
	QString broadcastAddress;
	int broadcastPort, senderPort;
	int lastChannel;
	QValueList<int> splitSizes;
	QString defaultCharset;
	double usalsLatitude, usalsLongitude;
	int probeMfe;
	int ringBufSize;

private:

	KProgressDialog *downProgress;
	bool downloadFinished;

private slots:

	void setDownloadResult( KIO::Job *job );
	void setDownloadPercent( KIO::Job *job, unsigned long percent );
};



class KaffeineDvbPlugin;
class DvbPanel;

class DvbConfigDialog : public KDialogBase
{
	Q_OBJECT

public:

	DvbConfigDialog( DvbPanel *pan, DVBconfig *dc, QWidget *parent, KaffeineDvbPlugin *p );
	~DvbConfigDialog();
	void setSource( QComboBox *box, QString s );

	QLineEdit *recordDirLe, *shiftDirLe, *broadcastLe, *filenameFormatLe;
	QSpinBox *beginSpin, *endSpin, *instantDurationSpin, *bportSpin, *sportSpin, *sizeFileSpin;
	MSpinBox *satNumber[MAX_DEVICES];
	QCheckBox *secMini[MAX_DEVICES], *secTwice[MAX_DEVICES];
	QCheckBox *doS2[MAX_DEVICES];
	QComboBox *sat0[MAX_DEVICES];
	QComboBox *sat1[MAX_DEVICES];
	QComboBox *sat2[MAX_DEVICES];
	QComboBox *sat3[MAX_DEVICES];
	MPushButton *src0[MAX_DEVICES];
	MPushButton *src1[MAX_DEVICES];
	MPushButton *src2[MAX_DEVICES];
	MPushButton *src3[MAX_DEVICES];
	MComboBox *rotor0[MAX_DEVICES];
	MComboBox *rotor1[MAX_DEVICES];
	MComboBox *rotor2[MAX_DEVICES];
	MComboBox *rotor3[MAX_DEVICES];
	MPushButton *lnb0[MAX_DEVICES];
	MPushButton *lnb1[MAX_DEVICES];
	MPushButton *lnb2[MAX_DEVICES];
	MPushButton *lnb3[MAX_DEVICES];
	KPushButton *updateBtn, *dumpBtn;
	QToolButton *recordDirBtn, *shiftDirBtn, *helpNameBtn;
	DVBconfig *dvbConfig;
	QComboBox *charsetComb;
	QPtrList<QSpinBox> timeoutSpin;
	QPtrList<QSpinBox> priority;
	QCheckBox *probeMfe;
	QSpinBox *ringBufSize;

private slots:

	void fileTemplateHelp();
	void setRecordDir();
	void setShiftDir();
	void satNumberChanged( int value, int devNum );
	void downloadData();
	void setLnb( int devNum, int lnbNum );
	void setRotor( int index, int devNum, int lnbNum );
	void setRotorSources( int devNum, int lnbNum );
	void setUsals();

protected slots:

	virtual void accept();
};



class LnbConfig : public KDialogBase
{
	Q_OBJECT

public:

	LnbConfig( LNB *b, QWidget *parent );

protected slots:

	virtual void accept();
	void setDual( int id );
	void setUniversal();
	void setCBandMono();
	void setCBandMulti();

private:

	QSpinBox *slof;
	QSpinBox *lo, *hi, *single, *vertical, *horizontal;
	QLabel *slofLab, *loLab, *hiLab, *singleLab, *verticalLab, *horizontalLab;
	QButtonGroup *nLO;
	QPushButton *univ, *cmono, *cmulti;

	LNB *lnb;
};



class RotorConfig : public KDialogBase
{
	Q_OBJECT

public:

	RotorConfig( Device *d, DVBconfig *c, int lnb, QWidget *parent );

protected slots:

	virtual void accept();

private slots:

	void reset();
	void add();

private:

	QSpinBox *position;
	QComboBox *srcComb;
	QListView *listView;
	QPushButton *addBtn, *resetBtn;
	QLineEdit *speed13, *speed18;

	Device *dev;
	int lnbNum;
	DVBconfig *config;
};


#endif /* DVBCONFIG_H */
