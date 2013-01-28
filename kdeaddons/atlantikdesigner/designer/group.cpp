#include <qcombobox.h>
#include <qframe.h>
#include <qvgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kcolorbutton.h>
#include <kdebug.h>
#include <klineedit.h>
#include <klineeditdlg.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kseparator.h>

#include "group.h"

GroupEditor::GroupEditor(ConfigEstateGroupList *newList, QWidget *parent)
	: KDialogBase(KDialogBase::Plain, i18n("Group Editor"), Ok|Apply|Cancel, Ok, parent, "Group Editor", false, true), mylist(*newList)
{
	setWFlags(WDestructiveClose);
	list = newList;

	QFrame *page = plainPage();
	QHBoxLayout *hlayout = new QHBoxLayout(page, marginHint(), spacingHint());

	groups = new KListBox(page);
	hlayout->addWidget(groups);
	connect(groups, SIGNAL(highlighted(QListBoxItem *)), this, SLOT(updateSettings(QListBoxItem *)));
	QStringList newgroups;
	for (ConfigEstateGroupList::Iterator it = list->begin(); it != list->end(); ++it)
		newgroups.append((*it).name());
	groups->insertStringList(newgroups);
	connect(groups, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

	QVBoxLayout *vlayout = new QVBoxLayout(hlayout, spacingHint());
	colorGroupBox = new QVGroupBox(i18n("&Colors"), page);
	vlayout->addWidget(colorGroupBox);

	(void) new QLabel(i18n("Foreground:"), colorGroupBox);
	fgButton = new KColorButton(colorGroupBox, "Foreground Button");
	connect(fgButton, SIGNAL(changed(const QColor &)), this, SLOT(fgChanged(const QColor &)));
	connect(fgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(changed()));

	(void) new QLabel(i18n("Background:"), colorGroupBox);
	bgButton = new KColorButton(colorGroupBox, "Background Button");
	connect(bgButton, SIGNAL(changed(const QColor &)), this, SLOT(bgChanged(const QColor &)));
	connect(bgButton, SIGNAL(changed(const QColor &)), this, SIGNAL(changed()));

	pricesGroupBox = new QVGroupBox(i18n("&Prices"), page);
	vlayout->addWidget(pricesGroupBox);

	pricesWidget = new QWidget(pricesGroupBox);
	QGridLayout *pricesLayout = new QGridLayout(pricesWidget, 2, 2, 0, spacingHint());
	pricesLayout->addWidget(new QLabel(i18n("House price:"), pricesWidget), 0, 0);
	pricesLayout->addWidget(housePrice = new QSpinBox(0, 3000, 25, pricesWidget), 0, 1);
	housePrice->setSpecialValueText(i18n("None"));
	housePrice->setSuffix(i18n("$"));
	connect(housePrice, SIGNAL(valueChanged(int)), this, SLOT(housePriceChanged(int)));

	pricesLayout->addWidget(new QLabel(i18n("Global price:"), pricesWidget), 1, 0);
	pricesLayout->addWidget(globalPrice = new QSpinBox(0, 3000, 25, pricesWidget), 1, 1);
	globalPrice->setSpecialValueText(i18n("None"));
	globalPrice->setSuffix(i18n("$"));
	connect(globalPrice, SIGNAL(valueChanged(int)), this, SLOT(globalPriceChanged(int)));

	dynamicGroupBox = new QVGroupBox(i18n("&Dynamic Rent"), page);
	vlayout->addWidget(dynamicGroupBox);

	mathWidget = new QWidget(dynamicGroupBox);
	QGridLayout *mathLayout = new QGridLayout(mathWidget, 2, 2, 0, spacingHint());
	mathLayout->addWidget(new QLabel(i18n("Add rent variable:"), mathWidget), 0, 0);
	mathLayout->addWidget(new QLabel(i18n("Expression:"), mathWidget), 1, 0);

	QComboBox *rentVarCombo = new QComboBox(mathWidget);
	QStringList vars;
	vars << "DICE";
	vars << "HOUSES";
	vars << "GROUPOWNED";
	rentVarCombo->insertStringList(vars);
	mathLayout->addWidget(rentVarCombo, 0, 1);

	rentMathEdit = new KLineEdit(mathWidget);
	connect(rentMathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(rentMathChanged(const QString &)));
	connect(rentVarCombo, SIGNAL(activated(const QString &)), rentMathEdit, SLOT(insert(const QString &)));
	mathLayout->addWidget(rentMathEdit, 1, 1);

	QHBoxLayout *buttonlayout = new QHBoxLayout(vlayout, spacingHint());
	KPushButton *addB = new KPushButton(i18n("&Add..."), page);
	buttonlayout->addWidget(addB);
	connect(addB, SIGNAL(clicked()), this, SLOT(add()));

	removeB = new KPushButton(i18n("&Remove"), page);
	buttonlayout->addWidget(removeB);
	connect(removeB, SIGNAL(clicked()), this, SLOT(remove()));

	selectionChanged();
}

void GroupEditor::add()
{
	bool ok;
	QString name = KLineEditDlg::getText(i18n("Add Group"), i18n("Enter the name of the new group:"), QString::null, &ok, this);
	if (ok)
	{
		for (ConfigEstateGroupList::Iterator it =  mylist.begin(); it != mylist.end(); ++it)
		{
			if ((*it).name() == name)
			{
				KMessageBox::information(this, i18n("That group is already on the list."));
				return;
			}
		}

		mylist.append(ConfigEstateGroup(name));
		groups->insertItem(name);

		emit changed();
	}
}

void GroupEditor::remove()
{
	QString curText = groups->currentText();
	if (!curText.isNull())
	{
		groups->removeItem(groups->currentItem());
		for (ConfigEstateGroupList::Iterator it =  mylist.begin(); it != mylist.end(); ++it)
		{
			if ((*it).name() == curText)
			{
				mylist.remove(it);
				break;
			}
		}

		emit changed();
	}
}

void GroupEditor::updateSettings(QListBoxItem *item)
{
	if (!mylist.size())
		return;

	if (!item)
		return;

	for (ConfigEstateGroupList::Iterator it =  mylist.begin(); it != mylist.end() ; ++it)
	{
		if ((*it).name() == item->text())
		{
			fgButton->setColor((*it).fgColor());
			bgButton->setColor((*it).bgColor());
			housePrice->setValue((*it).housePrice());
			globalPrice->setValue((*it).globalPrice());
			rentMathEdit->setText((*it).rentMath());
			break;
		}
	}
}

ConfigEstateGroup *GroupEditor::currentGroup()
{
	QListBoxItem *item = groups->item(groups->currentItem());
	if (!item)
		return 0;

	for (ConfigEstateGroupList::Iterator it =  mylist.begin(); it != mylist.end(); ++it)
		if ((*it).name() == item->text())
			return &(*it);

	return 0;
}

void GroupEditor::fgChanged(const QColor &color)
{
	ConfigEstateGroup *group = currentGroup();
	if (group)
		group->setFgColor(color);
}

void GroupEditor::bgChanged(const QColor &color)
{
	ConfigEstateGroup *group = currentGroup();
	if (group)
		group->setBgColor(color);
}

void GroupEditor::housePriceChanged(int newValue)
{
	ConfigEstateGroup *group = currentGroup();
	if (group)
		group->setHousePrice(newValue);
}

void GroupEditor::globalPriceChanged(int newValue)
{
	ConfigEstateGroup *group = currentGroup();
	if (group)
		group->setGlobalPrice(newValue);
}

void GroupEditor::rentMathChanged(const QString &newValue)
{
	ConfigEstateGroup *group = currentGroup();
	if (group)
	{
		group->setRentMath(newValue);
	}
}

void GroupEditor::slotApply()
{
	*list = mylist;

	KDialogBase::slotApply();

	emit update();
}

void GroupEditor::slotOk()
{
	slotApply();

	KDialogBase::slotOk();
}

void GroupEditor::selectionChanged()
{
	bool issel = groups->currentItem() >= 0;
	colorGroupBox->setEnabled(issel);
	pricesGroupBox->setEnabled(issel);
	dynamicGroupBox->setEnabled(issel);
	removeB->setEnabled(issel);
}

#include "group.moc"
