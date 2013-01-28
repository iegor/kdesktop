#ifndef ATLANTIK_EDITOR_H
#define ATLANTIK_EDITOR_H

#include <kmainwindow.h>
#include <qstring.h>
#include <qcolor.h>
#include <qstringlist.h>

#include <atlantic/estate.h>

#include "group.h"

class QCheckBox;
class QGridLayout;
class QLineEdit;
class QSpinBox;

class KColorButton;
class KComboBox;
class KListBox;
class KPushButton;

enum EstateType { Street = 0, Cards, FreeParking, ToJail, Tax, Jail };

class ConfigEstate : public Estate
{
	public:
	ConfigEstate(int estateId);
	void setEstateId(const int estateId);
	int type()const { return m_type; }
	void setType(const int type);
	const QString &group() { return m_group; }
	void setGroup(const QString &group);
	int price()const { return m_price; }
	void setPrice(int);
	int rent(int _h)const { return m_rent[_h]; }
	void setRent(const int, const int);
	int tax()const { return m_tax; }
	void setTax(const int);
	int taxPercentage() { return m_taxPercentage; }
	void setTaxPercentage(const int);
	const QString &takeCard() { return m_takeCard; }
	void setTakeCard(const QString &);
	const QColor &forceBg() { return m_forceBg; }
	void setForceBg(const QColor &);
	bool go()const { return m_go; }
	void setGo(const bool);
	int passMoney()const { return m_passMoney; }
	void setPassMoney(const int);

	void setChanged(bool);

	private:
	int m_type;
	QString m_group;
	int m_rent[6];
	int m_price;
	int m_tax;
	int m_taxPercentage;
	QString m_takeCard;
	QColor m_forceBg;
	bool m_go;
	int m_passMoney;
};
typedef QPtrList<ConfigEstate> EstateList;

struct Card
{
	Card() {}
	Card(const QString &newName) { name = newName; }
	QString name;
	QStringList keys;
	QValueList<int> values;
};
class CardStack : public QValueList<Card>
{
	public:
	CardStack() {}
	CardStack(const QString &newName) { setName(newName); }
	const QString &name() { return m_name; }
	void setName(const QString &newName) { m_name = newName; }

	private:
	QString m_name;
};

class EstateDlg : public QWidget
{
	Q_OBJECT

	public:
	EstateDlg(QWidget *parent = 0, char *name = 0) : QWidget(parent, name) { estate = 0; }

	signals:
	void updateBackground();

	public slots:
	virtual void slotOk() { save(); }
	void slotUpdate(ConfigEstate *newEstate) { estate = newEstate; if (estate) update(); }
	void groupsChanged() { groupUpdate(); }

	protected:
	virtual void update() {}
	virtual void save() {}
	virtual void groupUpdate() {}
	ConfigEstate *estate;
};

class EstateEdit : public QWidget
{
	Q_OBJECT

	public:
	EstateEdit(ConfigEstateGroupList *, EstateList *, QValueList<CardStack> *, QWidget *parent = 0, const char *name = 0);
	ConfigEstate *theEstate() { return estate; }
	bool upArrow()const;
	bool downArrow()const;
	bool leftArrow()const;
	bool rightArrow()const;
	void aboutToDie();
	void setReady(bool);

	public slots:
	void setEstate(ConfigEstate *);
	ConfigEstate *saveEstate(bool superficial = false);
	void groupsChanged();

	signals:
	void somethingChanged();
	void modified();
	void updateBackground();
	void goChecked(int id);
	void resized();

	protected:
    void resizeEvent(QResizeEvent *);

	private slots:
	void configure();
	void goToggled(bool);

	private:
	KComboBox *typeCombo;
	QLineEdit *nameEdit;
	QCheckBox *goCheck;
	QSpinBox *passMoney;
	QWidget *centerWidget;
	QGridLayout *layout;

	EstateList *estates;
	QValueList<CardStack> *cards;
	ConfigEstateGroupList *groups;

	ConfigEstate *estate;

	EstateDlg *confDlg;

	int oldType;

	bool ready;
	bool locked;
};

class ChooseWidget : public QWidget
{
	Q_OBJECT

	public:
	ChooseWidget(EstateList *, int id, Card *, QWidget *parent = 0, char *name = 0);

	public slots:
	void typeChanged(int);
	void valueChanged(int);
	void estateChanged(int);

	private:
	Card *card;
	KComboBox *typeCombo;
	QSpinBox *value;
	KComboBox *estate;
	bool number;
	bool prevNumber;
	bool init;
	QHBoxLayout *hlayout;

	EstateList *estates;

	int id;
};

class CardView : public QWidget
{
	Q_OBJECT

	public:
	CardView(EstateList *, CardStack *, QWidget *parent = 0, char *name = 0);

	private slots:
	void selected(int);
	void add();
	void del();
	void rename();
	void more();
	void less();
	void updateButtonsEnabled();

	private:
	KListBox *List;
	KPushButton *addButton;
	KPushButton *renameButton;
	KPushButton *delButton;
	KPushButton *moreButton;
	KPushButton *lessButton;

	EstateList *estates;

	QVBoxLayout *layout;

	Card *card;

	CardStack *stack;
	QPtrList<ChooseWidget> choosies;
};

class TaxDlg : public EstateDlg
{
	Q_OBJECT

	public:
	TaxDlg(QWidget *parent = 0, char *name = 0);

	protected:
	virtual void save();
	virtual void update();

	private:
	QSpinBox *tax;
	QSpinBox *taxPercentage;
};

class GenericDlg : public EstateDlg
{
	Q_OBJECT

	public:
	GenericDlg(QWidget *parent = 0, char *name = 0);

	protected:
	virtual void save();
	virtual void update();

	private:
	KColorButton *col;
};

class CardsDlg : public EstateDlg
{
	Q_OBJECT

	public:
	CardsDlg(EstateList *, QValueList<CardStack> *, QWidget *parent = 0, char *name = 0);

	protected:
	virtual void save();
	virtual void update();

	private slots:
	void addStack();
	void updateView(const QString &);

	private:
	QValueList<CardStack> *stacks;
	EstateList *estates;

	QComboBox *cards;
	CardView *view;
	QVBoxLayout *vlayout;
};

class StreetDlg : public EstateDlg
{
	Q_OBJECT

	public:
	StreetDlg(ConfigEstateGroupList *, QWidget *parent = 0, char *name = 0);

	protected:
	virtual void save();
	virtual void update();
	virtual void groupUpdate();

	private slots:
	void groupChanged(const QString &);

	private:
	QSpinBox *houses0;
	QSpinBox *houses1;
	QSpinBox *houses2;
	QSpinBox *houses3;
	QSpinBox *houses4;
	QSpinBox *houses5;
	QSpinBox *price;
	KComboBox *groupCombo;

	ConfigEstateGroupList *groups;

	ConfigEstateGroup *curGroup();
};

#endif
