#include "menumaker.h"
#include <stdio.h>
#include <assert.h>
#include <arts/debug.h>

using namespace std;

MenuEntry::MenuEntry(MenuMaker *menumaker, KActionMenu *parent, const char *text)
	: menumaker(menumaker), text(text)
{
	action = new KAction(QString::fromLocal8Bit(text));
	parent->insert(action);
	connect(action, SIGNAL(activated()), this, SLOT(activated()));
}

void MenuEntry::activated()
{
	menumaker->menuActivated(text);
}

MenuCategory::MenuCategory(const QString& name, const char *prefix, KActionMenu *menu)
{
	_menu = menu;
	_name = name;
	_catchall = (strcmp(prefix,"*") == 0);
	addPrefix(prefix);
}

QString MenuCategory::name()
{
	return _name;
}

bool MenuCategory::catchall()
{
	return _catchall;
}

void MenuCategory::addPrefix(const char *prefix)
{
	prefixList.push_back(prefix);
}

bool MenuCategory::matches(const char *item)
{
	string pattern = string(item)+"$";
	size_t patternlen = strlen(pattern.c_str());

	list<string>::iterator i;

	for(i=prefixList.begin();i != prefixList.end(); ++i)
	{
		const char *prefix = (*i).c_str();

		if(patternlen >= strlen(prefix))
		{
			if(strncmp(prefix,pattern.c_str(),strlen(prefix)) == 0) return true;
		}
	}
	return false;
}

KActionMenu *MenuCategory::menu()
{
	return _menu;
}

MenuMaker::MenuMaker(KActionMenu *root)
{
	categories.push_back(new MenuCategory("","",root));
}

// Add more specific categories later, more general categories first.
//
// for instance use
//  addCategory("&Synthesis", "Synth_");
//  addCategory("&Synthesis/&Waveforms", "Synth_WAVE");

void MenuMaker::addCategory(const QString& name, const char *prefix)
{
	MenuCategory *mc = 0,*pc = 0;
	
	mc = lookupCategoryByName(name);
	if(mc)
	{
		mc->addPrefix(prefix);
		return; // already exists
	}

	pc = lookupCategoryByName(basename(name));
	if(pc)
	{
		KActionMenu *newMenu = new KActionMenu(catname(name));
		pc->menu()->insert(newMenu);
		/* 000 */
		/*connect(newMenu,SIGNAL(activated(int)),this,SLOT(menuactivated(int)));
		pc->menu()->insertItem(catname(name).c_str(), newMenu, CAT_MAGIC_ID);*/
		arts_debug("inserting a menu called '%s' in the parent menu '%s'",
			catname(name).local8Bit().data(),pc->name().local8Bit().data());
		categories.push_back(new MenuCategory(name,prefix,newMenu));
	}
	else
	{
		arts_debug("Parent category '%s' for '%s' is missing.",
				basename(name).local8Bit().data(),name.local8Bit().data());
	}
}

MenuCategory *MenuMaker::lookupCategoryByName(const QString& name)
{
	MenuCategory *mc = 0;
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();++i)
	{
		if((*i)->name() == name) mc = (*i);
	}
	return mc;
}

void MenuMaker::addItem(const char *name, int index)
{
	MenuCategory *mc = 0;

	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();++i)
		if((*i)->matches(name)) mc = (*i);
	assert(mc);		// root category should always match

	if(mc->name().isEmpty())
	{
		// if we hit the root category, it may be better to move the thing
		// into the catchall category (looks cleaner)
		for(i=categories.begin();i != categories.end();++i)
			if((*i)->catchall()) mc = (*i);
	}
	//mc->menu()->insertItem(name,index); 000
	//mc->menu()->insert(new KAction(name)); // index??
	new MenuEntry(this, mc->menu(), name);
	/*
	KAction *action = new KAction(QString(name));
	mc->menu()->insert(action);
	*/
	//action->plug(mc->menu());
	//mc->menu()->insert(new KAction(name)); // index??
}

QString MenuMaker::basename(const QString& name)
{
	QString result = "";

	int i = name.findRev('/');
	if(i != -1)
		result = name.left(i);

	arts_debug("basename(%s) => %s",name.local8Bit().data(),
									result.local8Bit().data());
	return result;
}

QString MenuMaker::catname(const QString& name)
{
	int i = name.findRev('/');
	if(i >= 0)
		return name.mid(i+1);

	return name;
}

void MenuMaker::clear()
{
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();++i)
	{
		/* 000
		KActionMenu *m = (*i)->menu();
		unsigned int k;

		k = 0;
		while(k<m->count())
		{
			if(m->idAt(k) != CAT_MAGIC_ID)
			{
				m->removeItemAt(k);
				k = 0;
			}
			else
			{
				k++;
			}
		}
		*/
	}
}

const char *MenuMaker::findID(int id)
{
	/* 000 ?
	list<MenuCategory *>::iterator i;
	for(i=categories.begin();i != categories.end();i++)
	{
		const char *name = (*i)->menu()->text(id);
		if(name) return(name);
	} */
	return 0;
}

void MenuMaker::menuActivated(const char *text)
{
	emit activated(text);
}

#include "menumaker.moc"
