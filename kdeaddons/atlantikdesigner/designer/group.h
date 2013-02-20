#ifndef GROUP_H
#define GROUP_H

#include <qcolor.h>
#include <qptrlist.h>

#include <kdialogbase.h>

#include <atlantic/estategroup.h>

class KColorButton;
class KLineEdit;
class KListBox;
class KPushButton;

class QComboBox;
class QListBoxItem;
class QSpinBox;
class QVGroupBox;

class ConfigEstateGroup
{
	public:
	ConfigEstateGroup() { init(); }
	ConfigEstateGroup(const QString &name) { setName(name); init(); }
	void init() { setHousePrice(0); setGlobalPrice(0); }

	void setHousePrice(int newPrice) { m_housePrice = newPrice; }
	int housePrice()const { return m_housePrice; }
	void setGlobalPrice(int newGlobalPrice) { m_globalPrice = newGlobalPrice; }
	int globalPrice()const { return m_globalPrice; }
	const QString &rentMath() { return m_rentMath; }
	void setRentMath(const QString &newMath) { m_rentMath = newMath; }
	bool dynamicRent()const { return !m_rentMath.isEmpty(); }
	void setName(const QString &name) { m_name = name; }
	const QString &name() { return m_name; }

	const QColor &fgColor() { return m_fgColor; }
	void setFgColor(const QColor &color) { m_fgColor = color; }
	const QColor &bgColor() { return m_bgColor; }
	void setBgColor(const QColor &color) { m_bgColor = color; }

	private:
	QColor m_fgColor;
	QColor m_bgColor;
	QString m_rentMath;
	int m_housePrice;
	int m_globalPrice;
	QString m_name;
};
typedef QValueList<ConfigEstateGroup> ConfigEstateGroupList;

class GroupEditor : public KDialogBase
{
	Q_OBJECT

	public:
	GroupEditor(ConfigEstateGroupList *, QWidget *parent=0);

	signals:
	void changed();
	void update();

	protected slots:
	virtual void slotOk();
	virtual void slotApply();

	private slots:
	void updateSettings(QListBoxItem *item);
	void fgChanged(const QColor &);
	void bgChanged(const QColor &);
	void housePriceChanged(int);
	void globalPriceChanged(int);
	void rentMathChanged(const QString &);
	void add();
	void remove();
	void selectionChanged();

	private:
	KListBox *groups;
	KLineEdit *rentMathEdit;
	KColorButton *fgButton;
	KColorButton *bgButton;
	QWidget *pricesWidget;
	QWidget *mathWidget;
	QSpinBox *housePrice;
	QSpinBox *globalPrice;
	QVGroupBox *colorGroupBox;
	QVGroupBox *pricesGroupBox;
	QVGroupBox *dynamicGroupBox;
	KPushButton *removeB;

	ConfigEstateGroupList *list;
	ConfigEstateGroupList mylist;

	ConfigEstateGroup *currentGroup();
};

#endif
