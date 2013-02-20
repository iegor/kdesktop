#include <qcheckbox.h>
#include <qevent.h>
#include <qframe.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qvgroupbox.h>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kdialogbase.h>
#include <klineeditdlg.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kseparator.h>

#include <atlantic/estate.h>

#include "editor.h"

ConfigEstate::ConfigEstate(int estateId) : Estate(estateId)
{
	m_rent[0] = m_rent[1] = m_rent[2] = m_rent[3] = m_rent[4] = m_rent[5] = 0;
	m_type = m_price = m_tax = m_taxPercentage = m_passMoney = 0;
	m_go = false;
	m_group = "Default";
	m_changed = false;
}

void ConfigEstate::setChanged(bool b)
{
	m_changed = b;
}

void ConfigEstate::setEstateId(const int estateId)
{
	if (m_id != estateId)
	{
		m_id = estateId;
		m_changed = true;
	}
}

void ConfigEstate::setType(const int type)
{
	if (m_type != type)
	{
		m_type = type;
		m_changed = true;
	}
}

void ConfigEstate::setGroup(const QString &group)
{
	if (m_group != group)
	{
		m_group = group;
		m_changed = true;
	}
}

void ConfigEstate::setPrice(int price)
{
	if (m_price != price)
	{
		m_price = price;
		m_changed = true;
	}
}

void ConfigEstate::setRent(const int houses, const int rent)
{
	if (m_rent[houses] != rent)
	{
		m_rent[houses] = rent;
		m_changed = true;
	}
}

void ConfigEstate::setTax(const int tax)
{
	if (m_tax != tax)
	{
		m_tax = tax;
		m_changed = true;
	}
}

void ConfigEstate::setTaxPercentage(const int taxPercentage)
{
	if (m_taxPercentage != taxPercentage)
	{
		m_taxPercentage = taxPercentage;
		m_changed = true;
	}
}

void ConfigEstate::setTakeCard(const QString &takeCard)
{
	if (m_takeCard != takeCard)
	{
		m_takeCard = takeCard;
		m_changed = true;
	}
}

void ConfigEstate::setForceBg(const QColor &forceBg)
{
	if (m_forceBg != forceBg)
	{
		m_forceBg = forceBg;
		m_changed = true;
	}
}

void ConfigEstate::setGo(const bool go)
{
	if (m_go != go)
	{
		m_go = go;
		m_changed = true;
	}
}

void ConfigEstate::setPassMoney(const int passMoney)
{
	if (m_passMoney != passMoney)
	{
		m_passMoney = passMoney;
		m_changed = true;
	}
}

///////////////////////////

QStringList types;

EstateEdit::EstateEdit(ConfigEstateGroupList *newGroups, EstateList *estates, QValueList<CardStack> *cards, QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	groups = newGroups;

	types.append("pay");
	types.append("payeach");
	types.append("collect");
	types.append("collecteach");
	types.append("advanceto");
	types.append("advance");
	types.append("goback");
	types.append("tojail");
	types.append("outofjail");
	types.append("nextutil");
	types.append("nextrr");
	types.append("payhouse");
	types.append("payhotel");

	oldType = -1;
	estate = 0;
	this->cards = cards;
	this->estates = estates;
	ready = false;
	locked = false;

	connect(this, SIGNAL(somethingChanged()), this, SLOT(saveEstate()));

	layout = new QGridLayout(this, 7, 1, KDialog::marginHint(), KDialog::spacingHint());
	nameEdit = new QLineEdit(this, "Name Edit");
	layout->addWidget(nameEdit, 0, 0);
	connect(nameEdit, SIGNAL(returnPressed()), this, SIGNAL(somethingChanged()));

	confDlg = 0;

	layout->setRowStretch(2, 2);

	layout->addWidget(new KSeparator(this), 3, 0);

	QHBoxLayout *typeLayout = new QHBoxLayout(KDialog::spacingHint());
	layout->addLayout(typeLayout, 4, 0);

	QLabel *typeLabel = new QLabel(i18n("Type:"), this);
	typeLayout->addWidget(typeLabel);
	typeCombo = new KComboBox(false, this, "Type Combo");
	typeLayout->addWidget(typeCombo);
	connect(typeCombo, SIGNAL(activated(int)), this, SIGNAL(somethingChanged()));
	connect(typeCombo, SIGNAL(activated(int)), this, SIGNAL(updateBackground()));

	goCheck = new QCheckBox(i18n("This estate is 'Go'"), this);
	connect(goCheck, SIGNAL(toggled(bool)), this, SIGNAL(somethingChanged()));
	connect(goCheck, SIGNAL(toggled(bool)), this, SLOT(goToggled(bool)));
	layout->addWidget(goCheck, 5, 0);

	QHBoxLayout *passMoneyLayout = new QHBoxLayout(KDialog::spacingHint());
	layout->addLayout(passMoneyLayout, 6, 0);
	QLabel *passMoneyLabel = new QLabel(i18n("Pass money:"), this);
	passMoneyLayout->addWidget(passMoneyLabel);
	passMoney = new QSpinBox(0, 3000, 10, this);
	passMoney->setSpecialValueText(i18n("None"));
	passMoney->setSuffix("$");
	passMoneyLayout->addWidget(passMoney);

	QStringList estateTypes(i18n("Street"));
	estateTypes.append(i18n("Cards"));
	estateTypes.append(i18n("Free Parking"));
	estateTypes.append(i18n("Go to Jail"));
	estateTypes.append(i18n("Tax"));
	estateTypes.append(i18n("Jail"));
	typeCombo->insertStringList(estateTypes);
}

void EstateEdit::aboutToDie()
{
	delete confDlg;
	confDlg = 0;
}

void EstateEdit::setReady(bool ready)
{
	this->ready = ready;
}

void EstateEdit::resizeEvent(QResizeEvent *)
{
	emit resized();
}

void EstateEdit::setEstate(ConfigEstate *_estate)
{
	if (!_estate)
		return;

	// why the hell is this here? :-)
	while (1)
		if (!locked)
			break;

	estate = _estate;

	ready = false;
	nameEdit->setText(estate->name());
	typeCombo->setCurrentItem(estate->type());
	goCheck->setChecked(estate->go());
	passMoney->setValue(estate->passMoney());

	ready = true;

	saveEstate(true);
}

ConfigEstate *EstateEdit::saveEstate(bool superficial)
{
	if (!estate || !ready)
		return 0;

	locked = true;

	EstateType curType = (EstateType)typeCombo->currentItem();

	if (!superficial)
	{
		confDlg->slotOk();

		estate->setType(curType);
		estate->setName(nameEdit->text());
		estate->setGo(goCheck->isChecked());
		estate->setPassMoney(passMoney->value());
	}

	if (curType != Street)
	{
	}

	if (!superficial)
		estate->update();

	configure();

	locked = false;

	return estate;
}

void EstateEdit::configure()
{
	if (oldType == typeCombo->currentItem())
	{
		confDlg->slotUpdate(estate);
		return;
	}

	delete confDlg;

	switch (estate->type())
	{
	case Street:
		confDlg = new StreetDlg(groups, this);
		break;

	case Tax:
		confDlg = new TaxDlg(this);
		break;

	case Cards:
		confDlg = new CardsDlg(estates, cards, this);
		break;

	case Jail:
	case ToJail:
	case FreeParking:
		confDlg = new GenericDlg(this);
		break;

	default:
		confDlg = new EstateDlg(this);
		break;
	}

	confDlg->slotUpdate(estate);

	connect(confDlg, SIGNAL(updateBackground()), this, SIGNAL(updateBackground()));

	layout->addWidget(confDlg, 1, 0);
	confDlg->show();

	oldType = typeCombo->currentItem();
}

void EstateEdit::groupsChanged()
{
	confDlg->groupsChanged();
}

void EstateEdit::goToggled(bool on)
{
	if (on)
	{
		emit goChecked(estate->id());
	}
}

bool EstateEdit::upArrow()const
{
	return (nameEdit->hasFocus() || typeCombo->hasFocus());
}

bool EstateEdit::downArrow()const
{
	return upArrow();
}

bool EstateEdit::leftArrow()const
{
	if (nameEdit->hasFocus())
	{
		nameEdit->setCursorPosition(nameEdit->cursorPosition() - 1);
		return true;
	}
	return false;
}

bool EstateEdit::rightArrow()const
{
	if (nameEdit->hasFocus())
	{
		nameEdit->setCursorPosition(nameEdit->cursorPosition() + 1);
		return true;
	}
	return false;
}

/////////////////////////////////

TaxDlg::TaxDlg(QWidget *parent, char *name)
	: EstateDlg(parent, name)
{
	QGridLayout *taxBox = new QGridLayout(this, 2, 2, KDialog::marginHint(), KDialog::spacingHint());
	taxBox->addWidget(new QLabel(i18n("Fixed tax:"), this), 0, 0);
	taxBox->addWidget(tax = new QSpinBox(0, 3000, 1, this), 0, 1);
	tax->setSpecialValueText(i18n("None"));
	tax->setSuffix("$");
	taxBox->addWidget(new QLabel(i18n("Percentage tax:"), this), 1, 0);
	taxBox->addWidget(taxPercentage = new QSpinBox(0, 100, 1, this), 1, 1);
	taxPercentage->setSpecialValueText(i18n("None"));
	taxPercentage->setSuffix("%");
}

void TaxDlg::save()
{
	estate->setTax(tax->value());
	estate->setTaxPercentage(taxPercentage->value());
}

void TaxDlg::update()
{
	tax->setValue(estate->tax());
	taxPercentage->setValue(estate->taxPercentage());
}

/////////////////////////////////

GenericDlg::GenericDlg(QWidget *parent, char *name)
	: EstateDlg(parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this, KDialog::spacingHint());
	layout->addWidget(new QLabel(i18n("Background:"), this));

	col = new KColorButton(this);
	layout->addWidget(col);
}

void GenericDlg::save()
{
	estate->setForceBg(col->color());
	emit updateBackground();
}

void GenericDlg::update()
{
	col->setColor(estate->forceBg());
}

/////////////////////////////////

CardsDlg::CardsDlg(EstateList *newEstates, QValueList<CardStack> *newCards, QWidget *parent, char *name)
	: EstateDlg(parent, name)
{
	estates = newEstates;
	stacks = newCards;

	view = 0;

	vlayout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

	QHBoxLayout *layout = new QHBoxLayout(vlayout, KDialog::spacingHint());

	KPushButton *addB = new KPushButton(i18n("&New Stack"), this);
	connect(addB, SIGNAL(clicked()), this, SLOT(addStack()));
	layout->addWidget(addB);
	layout->addStretch();

	layout->addWidget(new QLabel(i18n("Cards from"), this));
	QStringList cardNames;
	for (QValueList<CardStack>::Iterator it = stacks->begin(); it != stacks->end(); ++it)
		cardNames.append((*it).name());

	cards = new QComboBox(this);
	cards->insertStringList(cardNames);
	layout->addWidget(cards);
	connect(cards, SIGNAL(activated(const QString &)), this, SLOT(updateView(const QString &)));

	vlayout->addWidget(new KSeparator(this));
}

void CardsDlg::addStack()
{
	bool ok;
	QString name = KLineEditDlg::getText(i18n("Add Stack"), i18n("Enter the name of the new stack:"), QString::null, &ok, this);
	if (ok)
	{
		for (QValueList<CardStack>::Iterator it = stacks->begin(); it != stacks->end(); ++it)
		{
			if ((*it).name() == name)
			{
				KMessageBox::information(this, i18n("That name is already on the list."));
				return;
			}
		}

		stacks->append(CardStack(name));
		cards->insertItem(name);
		cards->setCurrentText(name);
		updateView(name);
	}
}

void CardsDlg::updateView(const QString &curName)
{
	CardStack *curStack = 0;
	for (QValueList<CardStack>::Iterator it = stacks->begin(); it != stacks->end(); ++it)
		if ((*it).name() == curName)
			curStack = &(*it);

	if (!curStack)
		return;

	delete view;
	view = new CardView(estates, curStack, this);
	vlayout->addWidget(view);
	view->show();
}

void CardsDlg::save()
{
	estate->setTakeCard(cards->currentText());
}

void CardsDlg::update()
{
	if (!estate->takeCard().isNull())
	{
		cards->setCurrentText(estate->takeCard());
		updateView(estate->takeCard());
	}
	else
		cards->setCurrentItem(-1);
}

/////////////////////////////////

ChooseWidget::ChooseWidget(EstateList *estates, int id, Card *card, QWidget *parent, char *name)
	: QWidget (parent, name)
{
	this->id = id;
	this->card = card;
	this->estates = estates;

	value = 0;
	estate = 0;
	number = true;
	prevNumber = true;
	init = true;

	value = 0;

	hlayout = new QHBoxLayout(this, KDialog::spacingHint());
	typeCombo = new KComboBox(this);
	QStringList _types(i18n("Pay"));
	_types.append(i18n("Pay Each Player"));
	_types.append(i18n("Collect"));
	_types.append(i18n("Collect From Each Player"));
	_types.append(i18n("Advance To"));
	_types.append(i18n("Advance"));
	_types.append(i18n("Go Back"));
	_types.append(i18n("Go to Jail"));
	_types.append(i18n("Get out of Jail Free Card"));
	_types.append(i18n("Advance to Nearest Utility"));
	_types.append(i18n("Advance to Nearest Railroad"));
	_types.append(i18n("Pay for Each House"));
	_types.append(i18n("Pay for Each Hotel"));
	typeCombo->insertStringList(_types);
	hlayout->addWidget(typeCombo);
	connect(typeCombo, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));

	hlayout->addStretch();
}

void ChooseWidget::valueChanged(int i)
{
	if (!value)
		return;
	(*card->values.at(id)) = i;
	value->setValue(i);
}

void ChooseWidget::estateChanged(int i)
{
	if (!estate)
		return;

	(*card->values.at(id)) = i;
	estate->setCurrentItem(i);
}

void ChooseWidget::typeChanged(int i)
{
	QString key = (*types.at(i));

	(*card->keys.at(id)) = key;
	typeCombo->setCurrentItem(i);

	number = key != "advanceto";

	if (prevNumber == number && !init)
		goto Skipped;

	if (number)
	{
		delete estate;
		estate = 0;

		value = new QSpinBox(0, 2000, (key == "advance" || key == "goback")? 1 : 5, this);

		hlayout->addWidget(value);
		connect(value, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

		value->show();
	}
	else
	{
		delete value;
		value = 0;
		estate = new KComboBox(this);
		ConfigEstate *curestate = 0;
		QStringList estateStrings;
		for (curestate = estates->first(); curestate; curestate = estates->next())
			estateStrings.append(curestate->name());
		estate->insertStringList(estateStrings);
		connect(estate, SIGNAL(activated(int)), this, SLOT(estateChanged(int)));

		hlayout->addWidget(estate);
		estate->show();
	}

	prevNumber = number;

Skipped:
	init = false;

	if (!number)
		return;

	bool boolean = (key == "outofjail" || key == "tojail" || key == "nextrr" || key == "nextutil");
	if (boolean)
	{
		value->setValue(1);
		valueChanged(1);
	}
	value->setEnabled(!boolean);

	QString suffix = "";
	QString prefix = "";

	// first four types are money, pay, payeach, collect, collecteach
	if (i < 4 || key == "payhouse" || key == "payhotel")
		suffix = "$";
	else if (key == "advance" || key == "goback")
		suffix = i18n("Estate(s)").prepend(" ");

	value->setPrefix(prefix);
	value->setSuffix(suffix);
}

/////////////////////////////////

CardView::CardView(EstateList *estates, CardStack *stack, QWidget *parent, char *name) : QWidget(parent, name)
{
	card = 0;

	this->stack = stack;
	this->estates = estates;

	choosies.setAutoDelete(true);

	layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	QHBoxLayout *hlayout = new QHBoxLayout(layout, KDialog::spacingHint());

	addButton = new KPushButton(i18n("&Add Card..."), this);
	connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
	hlayout->addWidget(addButton);
	hlayout->addStretch();
	renameButton = new KPushButton(i18n("&Rename..."), this);
	connect(renameButton, SIGNAL(clicked()), this, SLOT(rename()));
	hlayout->addWidget(renameButton);
	hlayout->addStretch();
	delButton = new KPushButton(i18n("&Delete"), this);
	connect(delButton, SIGNAL(clicked()), this, SLOT(del()));
	hlayout->addWidget(delButton);

	List = new KListBox(this);
	layout->addWidget(List);
	connect(List, SIGNAL(highlighted(int)), this, SLOT(selected(int)));

	// it gets very big (and won't shrink) otherwise
	List->setMaximumHeight(90);

	hlayout = new QHBoxLayout(layout, KDialog::spacingHint());
	moreButton = new KPushButton(i18n("&More Properties"), this);
	connect(moreButton, SIGNAL(clicked()), this, SLOT(more()));
	hlayout->addWidget(moreButton);
	hlayout->addStretch();
	lessButton = new KPushButton(i18n("&Fewer Properties"), this);
	connect(lessButton, SIGNAL(clicked()), this, SLOT(less()));
	hlayout->addWidget(lessButton);

	for (CardStack::Iterator it = stack->begin(); it != stack->end(); ++it)
		List->insertItem((*it).name);

	updateButtonsEnabled();
}

void CardView::more()
{
	if (!card)
		return;

	card->keys.append("pay");
	card->values.append(0);
	ChooseWidget *newChooseWidget = new ChooseWidget(estates, choosies.count(), card, this);
	newChooseWidget->typeChanged(0);
	newChooseWidget->valueChanged(0);

	choosies.append(newChooseWidget);
	layout->addWidget(newChooseWidget);

	newChooseWidget->show();

	updateButtonsEnabled();
}

void CardView::less()
{
	if (List->count() <= 0 || choosies.count() <= 0)
		return;

	choosies.removeLast();
	card->keys.pop_back();
	card->values.pop_back();

	updateButtonsEnabled();
}

void CardView::add()
{
	bool ok = false;
	QString name = KLineEditDlg::getText(i18n("Add Card"), i18n("Enter the name of the new card:"), QString::null, &ok, this);
	if (ok)
		List->insertItem(name, 0);

	choosies.clear();

	stack->prepend(Card(name));

	List->setCurrentItem(0);

	more();

	updateButtonsEnabled();
}

void CardView::rename()
{
	int curItem = List->currentItem();
	if (curItem < 0)
		return;

	bool ok = false;;
	QString name = KLineEditDlg::getText(i18n("Add Card"), i18n("Enter the name of the new card:"), (*stack->at(curItem)).name, &ok, this);
	if (ok)
	{
		(*stack->at(curItem)).name = name;
		List->changeItem(name, curItem);
	}

	updateButtonsEnabled();
}

void CardView::del()
{
	int curItem = List->currentItem();

	// for some reason, crashes if count == 0
	if (curItem < 0 || List->count() <= 1)
		return;

	List->removeItem(curItem);
	stack->remove(stack->at(curItem));
	choosies.clear();

	updateButtonsEnabled();
}

void CardView::selected(int i)
{
	choosies.clear();

	card = &(*stack->at(i));
	unsigned int num = card->keys.count();

	QValueList<int>::Iterator vit = card->values.begin();
	for (QStringList::Iterator it = card->keys.begin(); it != card->keys.end(); ++it)
	{
		ChooseWidget *newChooseWidget = new ChooseWidget(estates, choosies.count(), card, this);

		choosies.append(newChooseWidget);
		layout->addWidget(newChooseWidget);

		newChooseWidget->show();

		newChooseWidget->typeChanged(types.findIndex(*it));
		newChooseWidget->valueChanged(*vit);
		newChooseWidget->estateChanged(*vit);

		++vit;
	}

	if (num == 0)
	{
		card->values.clear();
		more();
	}

	updateButtonsEnabled();
}

void CardView::updateButtonsEnabled()
{
	int curItem = List->currentItem();
	int count = List->count();
	delButton->setEnabled(!(curItem < 0 || count <= 1));
	renameButton->setEnabled(!(curItem < 0));
	moreButton->setEnabled(card);
	lessButton->setEnabled(!(count <= 0 || choosies.count() <= 0));
}

/////////////////////////////////

StreetDlg::StreetDlg(ConfigEstateGroupList *newGroups, QWidget *parent, char *name)
	: EstateDlg(parent, name)
{
	groups = newGroups;

	QVBoxLayout *bigbox = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());

	QVGroupBox *RentPage = new QVGroupBox(i18n("&Rent by Number of Houses"), this);
	RentPage->setInsideSpacing(KDialog::spacingHint());
	RentPage->setInsideMargin(KDialog::marginHint());
	bigbox->addWidget(RentPage);
	QWidget *topRent = new QWidget(RentPage);
	QGridLayout *rentBox = new QGridLayout(topRent, 3, 3, KDialog::spacingHint());
	rentBox->addWidget(new QLabel(i18n("None:"), topRent), 0, 0);
	rentBox->addWidget(new QLabel(i18n("One:"), topRent), 0, 1);
	rentBox->addWidget(new QLabel(i18n("Two:"), topRent), 0, 2);
	rentBox->addWidget(new QLabel(i18n("Three:"), topRent), 2, 0);
	rentBox->addWidget(new QLabel(i18n("Four:"), topRent), 2, 1);
	rentBox->addWidget(new QLabel(i18n("Hotel:"), topRent), 2, 2);

	rentBox->addWidget(houses0 = new QSpinBox(0, 3000, 1, topRent), 1, 0);
	rentBox->addWidget(houses1 = new QSpinBox(0, 3000, 1, topRent), 1, 1);
	rentBox->addWidget(houses2 = new QSpinBox(0, 3000, 1, topRent), 1, 2);
	rentBox->addWidget(houses3 = new QSpinBox(0, 3000, 1, topRent), 3, 0);
	rentBox->addWidget(houses4 = new QSpinBox(0, 3000, 1, topRent), 3, 1);
	rentBox->addWidget(houses5 = new QSpinBox(0, 3000, 1, topRent), 3, 2);
	houses0->setSuffix(i18n("$"));
	houses0->setSpecialValueText(i18n("None"));
	houses1->setSuffix(i18n("$"));
	houses1->setSpecialValueText(i18n("None"));
	houses2->setSuffix(i18n("$"));
	houses2->setSpecialValueText(i18n("None"));
	houses3->setSuffix(i18n("$"));
	houses3->setSpecialValueText(i18n("None"));
	houses4->setSuffix(i18n("$"));
	houses4->setSpecialValueText(i18n("None"));
	houses5->setSuffix(i18n("$"));
	houses5->setSpecialValueText(i18n("None"));

	QGridLayout *pricesBox = new QGridLayout(bigbox, 2, 2, KDialog::spacingHint());
	pricesBox->addWidget(new QLabel(i18n("Price:"), this), 0, 0);
	pricesBox->addWidget(price = new QSpinBox(0, 3000, 25, this), 0, 1);
	price->setSpecialValueText(i18n("None"));
	price->setSuffix(i18n("$"));

	QLabel *groupLabel = new QLabel(i18n("Group:"), this);
	pricesBox->addWidget(groupLabel, 1, 0);
	groupCombo = new KComboBox(this, "Group Combo");
	groupUpdate();
	connect(groupCombo, SIGNAL(activated(const QString &)), this, SLOT(groupChanged(const QString &)));

	pricesBox->addWidget(groupCombo, 1, 1);
}

void StreetDlg::groupChanged(const QString &groupName)
{
	for (ConfigEstateGroupList::Iterator it =  groups->begin(); it != groups->end(); ++it)
	{
		if ((*it).name() == groupName)
		{
			if ( (*it).fgColor().isValid() )
				estate->setColor((*it).fgColor());
			if ( (*it).bgColor().isValid() )
				estate->setBgColor((*it).bgColor());
			estate->update();

			break;
		}
	}
}

void StreetDlg::groupUpdate()
{
	QString curGroupName = groupCombo->currentText();

	QStringList newGroups;
	for (ConfigEstateGroupList::Iterator it =  groups->begin(); it != groups->end(); ++it)
		newGroups.append((*it).name());

	groupCombo->clear();
	groupCombo->insertStringList(newGroups);

	if (!curGroupName.isNull())
	{
		groupCombo->setCurrentText(curGroupName);
		groupChanged(curGroupName);
	}
}

void StreetDlg::save()
{
	estate->setRent(0, houses0->value());
	estate->setRent(1, houses1->value());
	estate->setRent(2, houses2->value());
	estate->setRent(3, houses3->value());
	estate->setRent(4, houses4->value());
	estate->setRent(5, houses5->value());
	estate->setPrice(price->value());
	estate->setGroup(groupCombo->currentText());
}

void StreetDlg::update()
{
	houses0->setValue(estate->rent(0));
	houses1->setValue(estate->rent(1));
	houses2->setValue(estate->rent(2));
	houses3->setValue(estate->rent(3));
	houses4->setValue(estate->rent(4));
	houses5->setValue(estate->rent(5));

	price->setValue(estate->price());

	if (estate->group().isEmpty())
	{
		groupCombo->setCurrentText("Default");
		estate->setGroup("Default");
	}
	else
		groupCombo->setCurrentText(estate->group());

	groupChanged(estate->group());
}

#include "editor.moc"
