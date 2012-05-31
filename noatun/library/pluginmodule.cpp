// Copyright (c) 2000-2001 Charles Samuels <charles@kde.org>
// Copyright (c) 2000-2001 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <noatun/app.h>

#include <kdebug.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qtabwidget.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>

#include "noatunlistview.h"
#include "pluginmodule.h"

#include <qwhatsthis.h>

#include "common.h"

PluginListItem::PluginListItem(const bool _exclusive, bool _checked, const NoatunLibraryInfo &_info, QListView *_parent)
	: QCheckListItem(_parent, _info.name, CheckBox)
	, mInfo(_info)
	, silentStateChange(false)
	, exclusive(_exclusive)
{
	setChecked(_checked);
	if(_checked) static_cast<PluginListView *>(listView())->count++;
}


void PluginListItem::setChecked(bool b)
{
	silentStateChange = true;
	setOn(b);
	silentStateChange = false;
}

void PluginListItem::stateChange(bool b)
{
	if(!silentStateChange)
		static_cast<PluginListView *>(listView())->stateChanged(this, b);
}

void PluginListItem::paintCell(QPainter *p, const QColorGroup &cg, int a, int b, int c)
{
	if(exclusive) myType = RadioButton;
	QCheckListItem::paintCell(p, cg, a, b, c);
	if(exclusive) myType = CheckBox;
}

PluginListView::PluginListView(unsigned _min, unsigned _max, QWidget *_parent, const char *_name)
	: KListView(_parent, _name)
	, hasMaximum(true)
	, max(_max)
	, min(_min <= _max ? _min : _max)
	, count(0)
{
}

PluginListView::PluginListView(unsigned _min, QWidget *_parent, const char *_name)
	: KListView(_parent, _name)
	, hasMaximum(false)
	, min(_min)
	, count(0)
{
}

PluginListView::PluginListView(QWidget *_parent, const char *_name)
	: KListView(_parent, _name)
	, hasMaximum(false)
	, min(0)
	, count(0)
{
}

void PluginListView::clear()
{
	count = 0;
	KListView::clear();
}

void PluginListView::stateChanged(PluginListItem *item, bool b)
{
	if(b)
	{
		count++;
		emit stateChange(item, b);

		if(hasMaximum && count > max)
		{
			// Find a different one and turn it off

			QListViewItem *cur = firstChild();
			PluginListItem *curItem = dynamic_cast<PluginListItem *>(cur);

			while(cur == item || !curItem || !curItem->isOn())
			{
				cur = cur->nextSibling();
				curItem = dynamic_cast<PluginListItem *>(cur);
			}

			curItem->setOn(false);
		}
	}
	else
	{
		if(count == min)
		{
			item->setChecked(true);
		}
		else
		{
			count--;
			emit stateChange(item, b);
		}
	}
}

Plugins::Plugins(QObject *_parent)
	: CModule(i18n("Plugins"), i18n("Select Your Plugins"), "gear", _parent)
	, shown(false)
{
	(new QVBoxLayout(this))->setAutoAdd(true);
	QTabWidget *tabControl = new QTabWidget(this,"tabControl");

	QFrame *interfaceTab = new QFrame(tabControl);
	(new QVBoxLayout(interfaceTab, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
	(void)new QLabel(i18n("<b>Select one or more interfaces to use:</b>"), interfaceTab);
	// At least one interface is required
	interfaceList = new PluginListView(1, interfaceTab);
	interfaceList->addColumn(i18n("Name"));
	interfaceList->addColumn(i18n("Description"));
	interfaceList->addColumn(i18n("Author"));
	interfaceList->addColumn(i18n("License"));
	connect(interfaceList, SIGNAL(stateChange(PluginListItem *, bool)), this, SLOT(stateChange(PluginListItem *, bool)));
	tabControl->addTab(interfaceTab, i18n("&Interfaces"));

	QFrame *playlistTab = new QFrame(tabControl);
	(new QVBoxLayout(playlistTab, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
	(void)new QLabel(i18n("<b>Select one playlist to use:</b>"), playlistTab);
	// Exactly one playlist is required
	playlistList = new PluginListView(1, 1, playlistTab);
	playlistList->addColumn(i18n("Name"));
	playlistList->addColumn(i18n("Description"));
	playlistList->addColumn(i18n("Author"));
	playlistList->addColumn(i18n("License"));
	connect(playlistList, SIGNAL(stateChange(PluginListItem *, bool)), this, SLOT(stateChange(PluginListItem *, bool)));
	tabControl->addTab(playlistTab, i18n("&Playlist"));

	QFrame *visTab = new QFrame(tabControl);
	(new QVBoxLayout(visTab, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
	(void)new QLabel(i18n("<b>Select any visualizations to use:</b>"), visTab);
	visList = new PluginListView(0, visTab);
	visList->addColumn(i18n("Name"));
	visList->addColumn(i18n("Description"));
	visList->addColumn(i18n("Author"));
	visList->addColumn(i18n("License"));
	connect(visList, SIGNAL(stateChange(PluginListItem *, bool)), this, SLOT(stateChange(PluginListItem *, bool)));
	tabControl->addTab(visTab, i18n("&Visualizations"));

	// Other plugins are not restricted
	QFrame *otherTab = new QFrame(tabControl);
	(new QVBoxLayout(otherTab, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
	(void)new QLabel(i18n("<b>Select any other plugins to use:</b>"), otherTab);
	otherList = new PluginListView(0, otherTab);
	otherList->addColumn(i18n("Name"));
	otherList->addColumn(i18n("Description"));
	otherList->addColumn(i18n("Author"));
	otherList->addColumn(i18n("License"));
	connect(otherList, SIGNAL(stateChange(PluginListItem *, bool)), this, SLOT(stateChange(PluginListItem *, bool)));
	tabControl->addTab(otherTab, i18n("O&ther Plugins"));
}

void Plugins::reopen()
{
	playlistList->clear();
	interfaceList->clear();
	otherList->clear();
	visList->clear();

	QValueList<NoatunLibraryInfo> available = napp->libraryLoader()->available();
	QValueList<NoatunLibraryInfo> loaded = napp->libraryLoader()->loaded();

	for(QValueList<NoatunLibraryInfo>::Iterator i = available.begin(); i != available.end(); ++i)
	{
		PluginListView *parent;
		bool exclusive = false;

		if((*i).type == "userinterface")
		{
			parent = interfaceList;
		}
		else if((*i).type == "playlist")
		{
			parent = playlistList;
			exclusive = true;
		}
		else if((*i).type == "sm" || (*i).type=="hidden")
		{
			parent = 0;
		}
		else if ((*i).type == "visualization")
		{
			parent = visList;
		}
		else
		{
			parent = otherList;
		}

		if(parent)
		{
			PluginListItem *item = new PluginListItem(exclusive, loaded.contains(*i), *i, parent);
			item->setText(0, (*i).name);
			item->setText(1, (*i).comment);
			item->setText(2, (*i).author);
			item->setText(3, (*i).license);
		}
	}
}

void Plugins::stateChange(PluginListItem *item, bool b)
{
	if(b)
		addPlugin(item->info());
	else
		removePlugin(item->info());
}

void Plugins::addPlugin(const NoatunLibraryInfo &info)
{
	// Load any that this one depends upon
	for(QStringList::ConstIterator i = info.require.begin(); i != info.require.end(); ++i)
	{
		NoatunLibraryInfo requiredInfo = napp->libraryLoader()->getInfo(*i);
		PluginListItem *item = findItem(requiredInfo);
		if(item) item->setOn(true);
	}

	if(mDeleted.contains(info.specfile))
		mDeleted.remove(info.specfile);
	else if(!mAdded.contains(info.specfile))
		mAdded.append(info.specfile);
}

void Plugins::removePlugin(const NoatunLibraryInfo &info)
{
	LibraryLoader &loader = *(napp->libraryLoader());

	// Here are the ones loaded
	QValueList<NoatunLibraryInfo> loaded = napp->libraryLoader()->loaded();

	// Add the ones marked for loading
	for(QStringList::ConstIterator i = mAdded.begin(); i != mAdded.end(); ++i)
		loaded.append(loader.getInfo(*i));

	// Subtract the ones marked for removal
	for(QStringList::ConstIterator i = mDeleted.begin(); i != mDeleted.end(); ++i)
		loaded.remove(loader.getInfo(*i));

	// If any depend on this plugin, mark them for removal (or remove them from mAdded)
	for(QValueList<NoatunLibraryInfo>::Iterator i = loaded.begin(); i != loaded.end(); ++i)
	{
		for(QStringList::ConstIterator j = (*i).require.begin(); j != (*i).require.end(); ++j)
		{
			if(*j == info.specfile)
			{
				PluginListItem *item = findItem(*i);
				if(item) item->setOn(false);
			}
		}
	}

	if (mAdded.contains(info.specfile))
		mAdded.remove(info.specfile);
	else if(!mDeleted.contains(info.specfile))
		mDeleted.append(info.specfile);
}

PluginListItem *Plugins::findItem(const NoatunLibraryInfo &info) const
{
	for(QListViewItem *cur = otherList->firstChild(); cur != 0; cur = cur->itemBelow())
	{
		PluginListItem *item = dynamic_cast<PluginListItem *>(cur);
		if(item && item->info() == info)
			return item;
	}

	// visualizations
	for(QListViewItem *cur = visList->firstChild(); cur != 0; cur = cur->itemBelow())
	{
		PluginListItem *item = dynamic_cast<PluginListItem *>(cur);
		if(item && item->info() == info)
			return item;
	}

	// If our only interface has a dependency removed, that's a double dose of trouble
	// We may as well have this here for completeness, though
	for(QListViewItem *cur = interfaceList->firstChild(); cur != 0; cur = cur->itemBelow())
	{
		PluginListItem *item = dynamic_cast<PluginListItem *>(cur);
		if(item && item->info() == info)
			return item;
	}

	// If a playlist is added or removed due to a dependency, we're doom-diddly-oomed
	// We may as well have this here for completeness, though
	for(QListViewItem *cur = playlistList->firstChild(); cur != 0; cur = cur->itemBelow())
	{
		PluginListItem *item = dynamic_cast<PluginListItem *>(cur);
		if(item && item->info() == info)
			return item;
	}

	return 0;
}

void Plugins::save()
{
	LibraryLoader &loader = *(napp->libraryLoader());

	// Load the plugins the user added
	//loader.loadAll(mAdded);

	QString oldPlaylist, newPlaylist;

	// first load all non playlist things
	for (QStringList::Iterator i = mAdded.begin(); i != mAdded.end(); ++i)
	{
		NoatunLibraryInfo info = loader.getInfo(*i);
		if(info.type != "playlist")
			loader.loadAll(QStringList(*i));
		else
			newPlaylist = (*i);
	}

	// Remove the plugins the user removed
	for (QStringList::Iterator i = mDeleted.begin(); i != mDeleted.end(); ++i)
	{
		NoatunLibraryInfo info = loader.getInfo(*i);
		if(info.type != "playlist")
			loader.remove(*i);
		else
			oldPlaylist = *i;
	}

	// Loading normal plugins works the other way round!
	// If you unload a playlist it sets the global playlist pointer to NULL,
	// that also means you cannot first load the new and then unload the old one.
	if(!newPlaylist.isEmpty() && !oldPlaylist.isEmpty())
	{
		kdDebug(66666) << k_funcinfo << "Unloading " << oldPlaylist << endl;
		loader.remove(oldPlaylist);
		kdDebug(66666) << k_funcinfo << "Loading " << oldPlaylist << endl;
		loader.loadAll(QStringList(newPlaylist));
	}


	// Round up the ones that weren't loaded right now, for saving in the configuration
	QStringList specList(mAdded);

	QValueList<NoatunLibraryInfo> loaded = loader.loaded();
	for(QValueList<NoatunLibraryInfo>::Iterator i = loaded.begin(); i != loaded.end(); ++i)
	{
		if(!specList.contains((*i).specfile) && loader.isLoaded((*i).specfile))
			specList += (*i).specfile;
	}

	// Now we actually save
	loader.setModules(specList);

	mDeleted.clear();
	mAdded.clear();
}

void Plugins::showEvent(QShowEvent *e)
{
	if(!shown)
	{
		shown = true;
		KMessageBox::information(this, i18n("<qt>Changing your playlist plugin will stop playback. Different playlists may use different methods of storing information, so after changing playlists you may have to recreate your playlist.</qt>"), QString::null, "Plugin warning");
	}
	CModule::showEvent(e);
}

#include "pluginmodule.moc"
