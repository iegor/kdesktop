#ifndef __MENUMAKER_H__
#define __MENUMAKER_H__

#include <kaction.h>
#include <qobject.h>
#include <string>
#include <list>

class MenuMaker;

class MenuEntry : public QObject
{
	Q_OBJECT

protected:
	MenuMaker *menumaker;
	KAction *action;
	QCString text;

public:
	MenuEntry(MenuMaker *menumaker, KActionMenu *parent, const char *text);

public slots:
	void activated();
};

class MenuCategory
{
protected:
	KActionMenu *_menu;
	QString _name;
	std::list<std::string> prefixList;
	bool _catchall;

public:
	MenuCategory(const QString& name, const char *prefix, KActionMenu *menu);

	void addPrefix(const char *prefix);
	QString name();
	KActionMenu *menu();

	bool catchall();
	bool matches(const char *item);
};

class MenuMaker :public QObject
{
	Q_OBJECT

	std::list<MenuCategory *> categories;
public:
	enum { CAT_MAGIC_ID = 10000 };

	MenuMaker(KActionMenu *root);

	void addCategory(const QString& name, const char *prefix);
	MenuCategory *lookupCategoryByName(const QString& name);
	void addItem(const char *name, int i);
	QString basename(const QString& name);
	QString catname(const QString& name);

	void clear();

	const char *findID(int id);

	void menuActivated(const char *text);
signals:
	void activated(const char *text);
};
#endif
