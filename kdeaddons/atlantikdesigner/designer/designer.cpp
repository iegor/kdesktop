#include <qcolor.h>
#include <qpalette.h>
#include <qevent.h>
#include <qfile.h>
#include <qguardedptr.h>
#include <qlayout.h>
#include <qpoint.h>
#include <qpopupmenu.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qwidget.h>

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <kurl.h>

#include <atlantic/estate.h>
#include <atlantic/player.h>
#include <atlantik/ui/board.h>

#include "boardinfo.h"
#include "editor.h"
#include "designer.h"

AtlanticDesigner::AtlanticDesigner(QWidget *parent, const char *name)
	: KMainWindow(parent, name)
{
	firstBoard = true;
	estates.setAutoDelete(true);

	isMod = false;
	m_player = 0;
	copiedEstate = 0;
	editor = 0;
	board = 0;
	layout = 0;

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::open(this, SLOT(open()), actionCollection());
	(void) KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	(void) KStdAction::save(this, SLOT(save()), actionCollection());
	(void) KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
	(void) new KAction(i18n("&Edit Gameboard Info..."), CTRL+Key_I, this, SLOT(info()), actionCollection(), "boardinfo");
	(void) new KAction(i18n("&Edit Groups..."), CTRL+Key_G, this, SLOT(editGroups()), actionCollection(), "groupeditor");

	(void) new KAction(i18n("&Add 4 Squares"), "viewmag+", 0, this, SLOT(larger()), actionCollection(), "larger");
	(void) new KAction(i18n("&Remove 4 Squares"), "viewmag-", 0, this, SLOT(smaller()), actionCollection(), "smaller");
	(void) KStdAction::copy(this, SLOT(copy()), actionCollection());
	(void) KStdAction::paste(this, SLOT(paste()), actionCollection());
	(void) new KAction(i18n("&Up"), Key_Up, this, SLOT(up()), actionCollection(), "up");
	(void) new KAction(i18n("&Down"), Key_Down, this, SLOT(down()), actionCollection(), "down");
	(void) new KAction(i18n("&Left"), Key_Left, this, SLOT(left()), actionCollection(), "left");
	(void) new KAction(i18n("&Right"), Key_Right, this, SLOT(right()), actionCollection(), "right");

	recentAct = KStdAction::openRecent(0, 0, actionCollection());
	connect(recentAct, SIGNAL(urlSelected(const KURL &)), this, SLOT(openRecent(const KURL &)));
	recentAct->loadEntries(KGlobal::config(), "Designer recent files");

	estateAct = new KListAction(i18n("Change Estate"), 0, 0, 0, actionCollection(), "estate_num");
	connect(estateAct, SIGNAL(activated(int)), SLOT(changeEstate(int)));

	createGUI("atlantikdesignerui.rc");

	KConfig *config = kapp->config();
	config->setGroup("General");
	QColor defaultColor = colorGroup().background();
	defaultBg = config->readColorEntry("alternateBackground", &defaultColor);
	config->setGroup("WM");
	defaultColor = colorGroup().dark();
	defaultFg = config->readColorEntry("activeBackground", &defaultColor);

	// these must match up to the ones in editor.cpp!
	// I don't think these are used anymore.
	types.append("street");
	types.append("cards");
	types.append("freeparking");
	types.append("tojail");
	types.append("tax");
	types.append("jail");

	openNew();

	resize(350, 400);
	applyMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
}

AtlanticDesigner::~AtlanticDesigner()
{
	delete m_player;
}

void AtlanticDesigner::initMembers()
{
	estates.clear();
	cards.clear();
	groups.clear();

	boardInfo = BoardInfo();

	for (ConfigEstateGroupList::Iterator it =  groups.begin(); it != groups.end(); ++it)
		if ((*it).name() == "Default")
			return;

	ConfigEstateGroup defaultGroup("Default");
	defaultGroup.setBgColor(defaultBg);
	defaultGroup.setFgColor(defaultFg);
	groups.append(defaultGroup);
}

void AtlanticDesigner::initBoard()
{
	// let her say her prayers (if she's alive)
	if (!firstBoard)
		editor->aboutToDie();

	firstBoard = false;

	delete editor;
	delete board;
	delete layout;

	board = new AtlantikBoard(0, max, AtlantikBoard::Edit, this, "Board");

	setCentralWidget(board);
	layout = new QVBoxLayout(board->centerWidget());
	editor = new EstateEdit(&groups, &estates, &cards, board->centerWidget(), "Estate Editor");
	connect(editor, SIGNAL(goChecked(int)), this, SLOT(goChanged(int)));
	connect(editor, SIGNAL(updateBackground()), this, SLOT(updateBackground()));
	connect(editor, SIGNAL(resized()), board, SLOT(slotResizeAftermath()));
	layout->addWidget(editor);

	editor->setReady(false);

	board->show();
	editor->show();

	initToken();
}

void AtlanticDesigner::initToken()
{
	if (!m_player)
		m_player = new Player(0);
	else
		m_player->setLocation(0);
	
	board->addToken(m_player);

	connect(m_player, SIGNAL(changed(Player *)), board, SLOT(playerChanged(Player *)));
}

void AtlanticDesigner::openNew()
{
	if (warnClose())
		return;
	filename = QString::null;

	max = 40;

	initMembers();
	initBoard();

	for (int i = 0; i < 40; ++i)
	{
		(void) newEstate(i);
	}

	doCaption(false);
	updateJumpMenu();

	groupsChanged();
	updateBackground();

	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));
	doCaption(false);
}

ConfigEstate *AtlanticDesigner::newEstate(int i)
{
	ConfigEstate *estate = new ConfigEstate(i);
	estate->setName(i18n("New Estate"));
	estate->setPrice(100);
	for (int j = 0; j < 6; ++j)
		estate->setRent(j, 10 * (j + 1));
	estate->setChanged(false);
	estates.insert(i, estate);

	connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
	connect(estate, SIGNAL(changed()), this, SLOT(modified()));

	board->addEstateView(estate);

	return estate;
}

bool AtlanticDesigner::warnClose()
{
	if (isMod)
	{
		int result = KMessageBox::warningYesNoCancel(this, i18n("There are unsaved changes to gameboard. Save them?"), i18n("Unsaved Changes"), KStdGuiItem::save(), KStdGuiItem::discard(), "DiscardAsk", true);
		switch(result)
		{
		case KMessageBox::Yes:
			save();
			// fallthrough
		case KMessageBox::No:
			return false;
		case KMessageBox::Cancel:
			return true;
		}
	}
	return false;
}

void AtlanticDesigner::open()
{
	if (warnClose())
		return;

	filename = KFileDialog::getOpenFileName();

	if (filename.isNull())
		return;

	openFile(filename);
	recentAct->addURL(KURL::fromPathOrURL( filename ));
}

void AtlanticDesigner::openRecent(const KURL &url)
{
	if (url.isLocalFile())
	{
		filename = url.path();
		openFile(filename);

		recentAct->popupMenu()->setItemChecked(recentAct->currentItem(), false);
	}
	else
	{
		recentAct->removeURL(url);
	}
}

void AtlanticDesigner::openFile(const QString &filename)
{
	enum ParseMode { Parse_Nothing, Parse_General, Parse_Board, Parse_Cards, Parse_EstateGroups, Parse_Estates };
	QFile f(filename);
	if (!f.open(IO_ReadOnly))
		return;

	initMembers();

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("utf8"));

	QString s = t.readLine();
	int parseMode = Parse_Nothing;

	CardStack *curCardStack = 0;

	int goEstate = 0;
	int i;
	for (i = 0; !t.atEnd();)
	{
		s = s.stripWhiteSpace();

		if (s.isEmpty())
		{
			s = t.readLine();
			continue;
		}

		QString name;

		if (s.left(1) == "<")
		{
			if (s == "<General>")
				parseMode = Parse_General;
			else if (s == "<Board>")
				parseMode = Parse_Board;
			else if (s == "<EstateGroups>")
				parseMode = Parse_EstateGroups;
			else if (s == "<Cards>")
				parseMode = Parse_Cards;
			else if (s == "<Estates>")
				parseMode = Parse_Estates;

			s = t.readLine();
			continue;
		}

		// this for outside-of-[]-settings
		int eqSign = s.find("=");
		if (eqSign >= 0)
		{
			QString key = s.left(eqSign);
			QString value = s.right(s.length() - eqSign - 1);

			if (parseMode == Parse_Cards)
			{
				if (key == "groupname")
				{
					cards.append(CardStack(value));
					curCardStack = &cards.last();
				}
			}
			else if (parseMode == Parse_General)
			{
				if (key == "name")
					boardInfo.name = value;
				else if (key == "description")
					boardInfo.description = value;
				else if (key == "version")
					boardInfo.version = value;
				else if (key == "url")
					boardInfo.url = value;
				else if (key == "authors")
					boardInfo.authors = QStringList::split(",", value);
				else if (key == "credits")
					boardInfo.credits = QStringList::split(",", value);
			}
			else if (parseMode == Parse_Board)
			{
				if (key == "go")
					goEstate = value.toInt();
				else if (key == "bgcolor")
					boardInfo.bgColor.setNamedColor(value);
			}

			s = t.readLine();
			continue;
		}

		name = s.left(s.find("]"));
		name = name.right(name.length() - name.find("[") - 1);

		if (name.isEmpty())
			name = i18n("No Name");

		// for estates
		int type = FreeParking;
		QString group, takeCard;
		int price = -1;
		int rent[6] = {-1, -1, -1, -1, -1, -1};
		int tax = -1;
		int taxPercentage = -1;
		int passMoney = -1;
		QColor forceBg;

		// for groups
		QColor color, bgColor;
		QString rentMath;
		int housePrice = -1;
		int globalPrice = -1;

		// for cards
		QStringList keys;
		QValueList<int> values;

		while (true)
		{
			if (t.atEnd())
				break;

			s = t.readLine().stripWhiteSpace();

			if (s.left(1) == "[" || s.left(1) == "<")
				break;

			int eqSign = s.find("=");
			if (eqSign < 0)
				continue;

			QString key = s.left(eqSign);
			QString value = s.right(s.length() - eqSign - 1);

			//////////////////////////////// ESTATES
			if (parseMode == Parse_Estates)
			{
				if (key == "price")
					price = value.toInt();
				else if (key.left(4) == "rent")
				{
					int houses = key.right(1).toInt();
					if (houses < 0 || houses > 5)
						continue;

					rent[houses] = value.toInt();
				}
				else if (key == "tax")
				{
					tax = value.toInt();
					type = Tax;
				}
				else if (key == "taxpercentage")
					taxPercentage = value.toInt();
				else if (key == "group")
				{
					group = value;
					type = Street;
				}
				else if (key == "tojail")
					type = ToJail;
				else if (key == "jail")
					type = Jail;
				else if (key == "freeparking")
					type = FreeParking;
				else if (key == "takecard")
				{
					takeCard = value;
					type = Cards;
				}
				else if (key == "bgcolor")
					forceBg = QColor(value);
				else if (key == "passmoney")
					passMoney = value.toInt();
			}
			else if (parseMode == Parse_EstateGroups)
			{
				if (key == "color")
					color.setNamedColor(value);
				else if (key == "bgcolor")
					bgColor.setNamedColor(value);
				else if (key == "houseprice")
					housePrice = value.toInt();
				else if (key == "price")
					globalPrice = value.toInt();
				else if (key == "rentmath")
					rentMath = value;
			}
			else if (parseMode == Parse_Cards)
			{
				bool ok;
				int v = value.toInt(&ok);
				if (!ok)
					continue;

				if (key == "pay" && v < 0)
				{
					v *= -1;
					key = "collect";
				}
				else if (key == "payeach" && v < 0)
				{
					v *= -1;
					key = "collecteach";
				}
				else if (key == "advance" && v < 0)
				{
					v *= -1;
					key = "goback";
				}

				keys.append(key);
				values.append(v);
			}
		}

		if (parseMode == Parse_Estates)
		{
			ConfigEstate *estate = new ConfigEstate(i);
			estate->setName(name);
			estate->setType(type);
			if (!group.isEmpty())
				estate->setGroup(group);
			estate->setTakeCard(takeCard);
			estate->setPrice(price);
			for (int j = 0; j < 6; j++)
				estate->setRent(j, rent[j]);
			estate->setTax(tax);
			estate->setTaxPercentage(taxPercentage);
			estate->setForceBg(forceBg);
			estate->setPassMoney(passMoney);
			estates.append(estate);

			connect(estate, SIGNAL(LMBClicked(Estate *)), this, SLOT(changeEstate(Estate *)));
			connect(estate, SIGNAL(changed()), this, SLOT(modified()));
			i++;
		}
		else if (parseMode == Parse_Cards)
		{
			Card card;
			card.name = name;
			card.keys = keys;
			card.values = values;
			if (curCardStack)
				curCardStack->append(card);
		}
		else if (parseMode == Parse_EstateGroups)
		{
			bool found = false;
			for (ConfigEstateGroupList::Iterator it =  groups.begin(); it != groups.end(); ++it)
			{
				if ((*it).name() == name)
				{
					found = true;
					break;
				}
			}
			
			if (found)
				continue;

			ConfigEstateGroup group(name);
			group.setHousePrice(housePrice);
			group.setGlobalPrice(globalPrice);
			group.setFgColor(color);
			group.setBgColor(bgColor);
			group.setRentMath(rentMath);
			groups.append(group);
		}
	}

	if (i < 8)
	{
		KMessageBox::detailedSorry(this, i18n("This board file is invalid; cannot open."), i18n("There are only %1 estates specified in this file.").arg(i));
		if (this->filename.isNull())
			close();
		return;
	}
	max = i;

	initBoard();
	ConfigEstate *estate = 0;
	for (estate = estates.first(); estate; estate = estates.next())
		board->addEstateView(estate);

	if (goEstate >= 0 && goEstate < max - 1)
		estates.at(goEstate)->setGo(true);

	groupsChanged();
	updateBackground();
	updateJumpMenu();

	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));
	doCaption(false);
}

void AtlanticDesigner::updateJumpMenu()
{
	QStringList estates;
	for (int i = 1; i <= max; i++)
		estates.append(i18n("Jump to Estate %1").arg(QString::number(i)));
	estateAct->setItems(estates);
}

void AtlanticDesigner::setPlayerAtBeginning()
{
	ConfigEstate *go = 0;
	for (go = estates.first(); go; go = estates.next())
		if (go->go())
			break;

	if (!go)
		go = estates.first();

	movePlayer(go);
	editor->setEstate(go);
}

void AtlanticDesigner::saveAs()
{
	QString oldfilename = filename;
	filename = QString::null;

	save();

	if (filename.isNull())
	{
		filename = oldfilename;
		return;
	}
	else
		recentAct->addURL(KURL::fromPathOrURL( filename ));
}

void AtlanticDesigner::save()
{
	(void) editor->saveEstate();
	QString oldfilename = filename;
	if (filename.isNull())
		filename = KFileDialog::getOpenFileName();

	if (filename.isNull())
	{
		filename = oldfilename;
		return;
	}

	QFile f(filename);
	if (!f.open(IO_WriteOnly))
		return;

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("utf8"));

	t << "<General>" << endl << endl;
	if (!boardInfo.name.isEmpty())
		t << "name=" << boardInfo.name << endl;
	if (!boardInfo.description.isEmpty())
		t << "description=" << boardInfo.description << endl;
	if (!boardInfo.url.isEmpty())
		t << "url=" << boardInfo.url << endl;
	if (!boardInfo.version.isEmpty())
		t << "version=" << boardInfo.version << endl;
	if (!boardInfo.authors.isEmpty())
		t << "authors=" << boardInfo.authors.join(",") << endl;
	if (!boardInfo.credits.isEmpty())
		t << "credits=" << boardInfo.credits.join(",") << endl;

	// FIXME: make configurable. Hardcoded defaults are still better than
	// non-existant entries that would make the saved configs useless.
	t << "minplayers=2" << endl;
	t << "maxplayers=6" << endl;
	t << "houses=32" << endl;
	t << "hotels=16" << endl;
	t << "startmoney=1500" << endl;

	t << endl;
	t << "<Board>" << endl << endl;

	ConfigEstate *estate = 0;
	bool hasGo = false;
	for (estate = estates.first(); estate; estate = estates.next())
	{
		if (estate->go())
		{
			t << "go=" << estate->id() << endl;
			hasGo = true;
			break;
		}
	}
	if (!hasGo)
		t << "go=" << 0 << endl;

	if (boardInfo.bgColor.isValid())
		t << "bgcolor=" << boardInfo.bgColor.name() << endl;

	t << endl << endl;

	t << "<EstateGroups>" << endl;

	// double-prevent duplicates
	QStringList writtenGroups;

	for (ConfigEstateGroupList::Iterator it =  groups.begin(); it != groups.end(); ++it)
	{
		if (writtenGroups.contains((*it).name()) > 0)
			continue;

		if ((*it).name() == "Default")
		{
			bool containsDefault = false;
			for (estate = estates.first(); estate; estate = estates.next())
			{
				if (estate->group() == "Default")
				{
					containsDefault = true;
					break;
				}
			}

			if (!containsDefault)
				continue;
		}

		writtenGroups.append((*it).name());

		t << endl << QString("[%1]").arg((*it).name()) << endl;

		if ((*it).globalPrice() > 0)
			t << "price=" << (*it).globalPrice() << endl;

		if ((*it).fgColor().isValid())
			t << "color=" << (*it).fgColor().name() << endl;
		if ((*it).bgColor().isValid())
			t << "bgcolor=" << (*it).bgColor().name() << endl;

		if ((*it).housePrice() > 0)
			t << "houseprice=" << (*it).housePrice() << endl;

		if (!(*it).rentMath().isEmpty())
			t << "rentmath=" << (*it).rentMath() << endl;
	}

	// now do the cards
	
	for (QValueList<CardStack>::Iterator it = cards.begin(); it != cards.end(); ++it)
	{
		t << endl << endl;
		t << "<Cards>" << endl << endl;
		t << "groupname=" << (*it).name() << endl;

		for (CardStack::Iterator cit = (*it).begin(); cit != (*it).end(); ++cit)
		{
			t << endl << "[" << (*cit).name << "]" << endl;
			QValueList<int>::Iterator vit = (*cit).values.begin();
			for (QStringList::Iterator it = (*cit).keys.begin(); it != (*cit).keys.end(); ++it, ++vit)
			{
				QString key = (*it);
				int value = (*vit);

				if (key == "collect")
				{
					value *= -1;
					key = "pay";
				}
				else if (key == "collecteach")
				{
					value *= -1;
					key = "payeach";
				}
				else if (key == "goback")
				{
					value *= -1;
					key = "advance";
				}

				if (key == "outofjail" || key == "tojail" || key == "nextrr" || key == "nextutil")
					value = 1;

				if (key == "outofjail")
				{
					t << "canbeowned=" << 1 << endl;
				}
				
				t << key << "=" << value << endl;
			}
		}
	}

	t << endl << endl;

	t << "<Estates>" << endl;

	for (estate = estates.first(); estate; estate = estates.next())
	{
		t << endl << QString("[%1]").arg(estate->name()) << endl;

		switch (estate->type())
		{
			case Street:
				if (!estate->group().isNull())
					t << "group=" << estate->group() << endl;

				if (estate->price() > 0)
					t << "price=" << estate->price() << endl;

				for (int i = 0; i < 6; i++)
				{
					if (estate->rent(i) > 0)
						t << "rent" << i << "=" << estate->rent(i) << endl;
				}
				break;

			case Tax:
				if (estate->tax() > 0)
					t << "tax=" << estate->tax() << endl;
				if (estate->taxPercentage() > 0)
					t << "taxpercentage=" << estate->taxPercentage() << endl;
				break;

			case Jail:
				t << "jail=1" << endl;
				if (estate->forceBg().isValid())
					t << "bgcolor=" << estate->forceBg().name() << endl;
				break;

			case ToJail:
				t << "tojail=1" << endl;
				if (estate->forceBg().isValid())
					t << "bgcolor=" << estate->forceBg().name() << endl;
				break;

			case Cards:
				t << "takecard=" << estate->takeCard() << endl;
				break;

			case FreeParking:
				t << "freeparking=1" << endl;
				if (estate->forceBg().isValid())
					t << "bgcolor=" << estate->forceBg().name() << endl;
				break;
		}

		if (estate->passMoney() > 0)
			t << "passmoney=" << estate->passMoney() << endl;
	}

	f.flush();
	doCaption(false);
}

void AtlanticDesigner::copy()
{
	copiedEstate = editor->theEstate();
}

void AtlanticDesigner::paste()
{
	ConfigEstate *estate = editor->theEstate();
	for (int i = 0; i < 6; i++)
		estate->setRent(i, copiedEstate->rent(i));
	estate->setPrice(copiedEstate->price());
	estate->setTax(copiedEstate->tax());
	estate->setTaxPercentage(copiedEstate->taxPercentage());
	estate->setGroup(copiedEstate->group());
	estate->setTakeCard(copiedEstate->takeCard());
	estate->setType(copiedEstate->type());
	estate->setName(copiedEstate->name());
	estate->setPassMoney(copiedEstate->passMoney());

	editor->setEstate(estate);

	groupsChanged();
}

void AtlanticDesigner::closeEvent(QCloseEvent *e)
{
	if (warnClose())
		return;

	saveMainWindowSettings(KGlobal::config(), "DesignerTopLevelWindow");
	recentAct->saveEntries(KGlobal::config(), "Designer recent files");

	e->accept();
	kapp->quit();
}

void AtlanticDesigner::changeEstate(int index)
{
	if (index < 0 || index > max - 1)
		return;

	changeEstate(estates.at(index));
}

void AtlanticDesigner::changeEstate(Estate *estate)
{
	if (!estate)
		return;

	setFocus();

	(void) editor->saveEstate();

	editor->setEstate(static_cast<ConfigEstate *>(estate));
	movePlayer(estate);
}

void AtlanticDesigner::movePlayer(Estate *estate)
{
	estateAct->setCurrentItem(estate->id());
	board->setFocus();
	m_player->setLocation(estate);
	m_player->update();
}

// this will add a square to all sides
void AtlanticDesigner::larger()
{
	max += 4;
	int sideLen = max/4;

	initBoard();

	ConfigEstate *estate = 0;
	for (int i = 0; i < max; ++i)
	{
		estate = estates.at(i);
		estate->setEstateId(i);
		board->addEstateView(estate);

			// make a newEstate
		if ((i % sideLen - 1) == 0 || i == 1)
		{
			newEstate(i);
		}
	}

	updateJumpMenu();
	groupsChanged();
	updateBackground();

	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));
	doCaption(true);
}

void AtlanticDesigner::smaller()
{
	if (max < 12)
		return;

	max -= 4;
	int sideLen = max/4;

	initBoard();

	bool remove = true;

	ConfigEstate *estate = 0;
	for (int i = 0; i < max; ++i)
	{
		// remove estate
		if (((i % sideLen - 1) == 0 || i == 1) && remove)
		{
			estates.remove(i);
			i--;
			remove = false;
		}
		else
		{
			estate = estates.at(i);
			estate->setEstateId(i);
			board->addEstateView(estate);

			remove = true;
		}
	}

	updateJumpMenu();
	groupsChanged();
	updateBackground();
	QTimer::singleShot(500, this, SLOT(setPlayerAtBeginning()));

	doCaption(true);
}

void AtlanticDesigner::modified()
{
	doCaption(true);
}

void AtlanticDesigner::doCaption(bool modified)
{
	setCaption(filename.isNull()? i18n("Atlantik Gameboard Editor") : filename, modified);
	isMod = modified;
}

void AtlanticDesigner::editGroups()
{
	if (groupEditor.isNull())
	{
		groupEditor = new GroupEditor(&groups, this);
		groupEditor->show();

		connect(groupEditor, SIGNAL(changed()), this, SLOT(modified()));
		connect(groupEditor, SIGNAL(update()), this, SLOT(groupsChanged()));
		connect(groupEditor, SIGNAL(update()), editor, SLOT(groupsChanged()));
	}
	else
		groupEditor->raise();
}

void AtlanticDesigner::goChanged(int id)
{
	ConfigEstate *curEstate = 0;
	for (curEstate = estates.first(); curEstate; curEstate = estates.next())
		if (curEstate->go() && curEstate->id() != id)
			curEstate->setGo(false);
}

void AtlanticDesigner::groupsChanged()
{
	ConfigEstate *curEstate = 0;
	for (curEstate = estates.first(); curEstate; curEstate = estates.next())
	{
		for (ConfigEstateGroupList::Iterator it =  groups.begin(); it != groups.end(); ++it)
		{
			if ((*it).name() == curEstate->group() && curEstate->type() == Street)
			{
				curEstate->setBgColor((*it).bgColor().isValid()? (*it).bgColor() : boardInfo.bgColor);
				curEstate->setColor((*it).fgColor());
				curEstate->update();
				break;
			}
		}
	}
}

void AtlanticDesigner::updateBackground()
{
	ConfigEstate *curEstate = 0;
	for (curEstate = estates.first(); curEstate; curEstate = estates.next())
	{
		if (curEstate->type() != Street || (curEstate->type() == Street && curEstate->group().isNull()))
		{
			curEstate->setColor(QColor());
			curEstate->setBgColor(boardInfo.bgColor);
		}

		if (curEstate->forceBg().isValid() && (curEstate->type() == FreeParking || curEstate->type() == Jail || curEstate->type() == ToJail))
		{
			curEstate->setBgColor(curEstate->forceBg());
		}

		curEstate->update();
	}
}

void AtlanticDesigner::info()
{
	if (boardInfoDlg.isNull())
	{
		// non-modal
		boardInfoDlg = new BoardInfoDlg(true, &boardInfo, this, "Board Information", false);
		boardInfoDlg->show();

		connect(boardInfoDlg, SIGNAL(okClicked()), this, SLOT(updateBackground()));
		connect(boardInfoDlg, SIGNAL(okClicked()), this, SLOT(modified()));
	}
	else
		boardInfoDlg->raise();
}

// now some fun functions ;)

void AtlanticDesigner::up()
{
	if (editor->upArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->id() + 1;
	int dest = estateId - 1;

	if (estateId <= 2*fourth && estateId > fourth) // left side
		dest++;
	else if (estateId > (3*fourth + 1)) // right side
		dest--;
	else if (estateId == 1)
		dest = max - 1;

	changeEstate(dest);
}

void AtlanticDesigner::down()
{
	if (editor->downArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->id() + 1;
	int dest = estateId - 1;

	if (estateId <= (2*fourth + 1) && estateId > (fourth + 1)) // left side
		dest--;
	else if (estateId > 3*fourth && estateId < max) // right side
		dest++;
	else if (estateId == max)
		dest = 0;

	changeEstate(dest);
}

void AtlanticDesigner::left()
{
	if (editor->leftArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->id() + 1;
	int dest = estateId - 1;

	if (estateId <= fourth) // bottom
		dest++;
	else if (estateId > (2*fourth + 1) && estateId <= (3*fourth + 1)) // top
		dest--;

	changeEstate(dest);
}

void AtlanticDesigner::right()
{
	if (editor->rightArrow())
		return;
	int fourth = max / 4;
	int estateId = editor->theEstate()->id() + 1;
	int dest = estateId - 1;

	if (estateId <= (fourth + 1) && estateId != 1) // bottom
		dest--;
	else if (estateId > 2*fourth && estateId <= 3*fourth) // top
		dest++;

	changeEstate(dest);
}

#include "designer.moc"
