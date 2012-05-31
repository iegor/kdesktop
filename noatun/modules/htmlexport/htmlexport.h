
#ifndef _HTMLEXPORT_H_
#define _HTMLEXPORT_H_

#include <qfile.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qhgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kpopupmenu.h>
#include <kfiledialog.h>
#include <ktempfile.h>
#include <kcolorbutton.h>
#include <kio/netaccess.h>
#include <kconfig.h>
#include <kurlrequester.h>

//#include <kdebug.h>

#include <noatun/app.h>
#include <noatun/playlist.h>
#include <noatun/pref.h>
#include <noatun/plugin.h>

class KAction;

class HTMLExport : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND
public:
	HTMLExport();
	~HTMLExport();


private:
	QString htmlEscape(const QString &source);
	QString getColorByEntry(QString s);
	KConfig *config;
	KAction *mAction;

private slots:
	void slotExport();

};

class Prefs : public CModule
{
Q_OBJECT
public:
	Prefs(QObject *parent);
	virtual void save();
	virtual void reopen();

	QGroupBox* colorBox;

	KColorButton* headColorSelect;
	KColorButton* hoverColorSelect;
	KColorButton* bgColorSelect;
	KColorButton* txtColorSelect;

	QLabel* bgColorLabel;
	QLabel* txtColorLabel;
	QLabel* headColorLabel;
	QLabel* hoverColorLabel;

	QCheckBox* linkEntries;
	QCheckBox* numberEntries;

	QGroupBox* bgPicBox;
	KURLRequester* bgPicPath;

protected:
	QGridLayout* bgcolorLabel;


signals:
	void saved();
};

#endif

