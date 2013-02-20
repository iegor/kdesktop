#ifndef ATLANTIK_BOARDINFO_H
#define ATLANTIK_BOARDINFO_H

#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qwidget.h>

#include <kdialogbase.h>

class KColorButton;
class KLineEdit;
class KURLLabel;

class QVBoxLayout;

struct BoardInfo
{
	BoardInfo() { bgColor = Qt::lightGray; }
	QString name;
	QStringList authors;
	QStringList credits;
	QString description;
	QString url;
	QString version;
	QColor bgColor;
};

class LotsaEdits : public QWidget
{
	Q_OBJECT

	public:
	LotsaEdits(bool, QStringList=QStringList(), QWidget *parent=0, const char *name=0);

	public slots:
	QStringList save();

	private slots:
	void more();
	void less();

	private:
	bool editable;
	QPtrList<QWidget> list;
	QVBoxLayout *layout;
};

class BoardInfoDlg : public KDialogBase
{
	Q_OBJECT
	
	public:
	BoardInfoDlg(bool editable, BoardInfo *, QWidget *parent=0, const char *_name=0, bool = true);

	protected slots:
	void slotOk();
	void slotApply();

	private:
	KLineEdit *name;
	KLineEdit *url;
	KLineEdit *version;
	KLineEdit *description;
	KColorButton *bgColor;

	LotsaEdits *authors;
	LotsaEdits *credits;

	BoardInfo *info;
};

#endif
