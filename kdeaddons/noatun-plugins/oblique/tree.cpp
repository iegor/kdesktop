// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "tree.h"
#include "file.h"
#include "query.h"
#include "menu.h"
#include "oblique.h"

#include <qpainter.h>
#include <iostream>

#include <klocale.h>
#include <string.h>

#include <noatun/player.h>

// this is used for comparing pointers
// (I should _not_ need this)
template <typename T>
inline static long subtract(const T *end, const T *begin)
{
	return long(end-begin);
}

static void treeItemMerge(
		TreeItem **set,
		TreeItem **intofirst, TreeItem **intolast,
		TreeItem **fromfirst, TreeItem **fromlast
	)
{
	const int items = subtract(intolast, intofirst) + subtract(fromlast, fromfirst)+2;
	TreeItem **temp = new TreeItem*[items];
	TreeItem **tempat = temp;

	while (1)
	{
		if (intofirst[0]->compare(fromfirst[0], 0, true) >= 0)
		{
			// from goes before into
			*tempat = *fromfirst;
			tempat++;
			fromfirst++;
			if (fromfirst > fromlast) break;
		}
		else
		{
			*tempat = *intofirst;
			tempat++;
			intofirst++;
			if (intofirst > intolast) break;
		}
	}
	while (intofirst <= intolast)
		*tempat++ = *intofirst++;
	while (fromfirst <= fromlast)
		*tempat++ = *fromfirst++;
		
	::memcpy(set, temp, items*sizeof(TreeItem**));
	delete [] temp;
}

static void treeItemSort(TreeItem **begin, TreeItem **end)
{
	if (begin == end) return;
	TreeItem **middle = subtract(end, begin)/2 + begin;
	
	if (begin != middle)
		treeItemSort(begin, middle);
	
	if (middle+1 != end)
		treeItemSort(middle+1, end);
	
	treeItemMerge(begin, begin, middle, middle+1, end);
}

static void treeItemSort(TreeItem *first)
{
	const int count = first->parent() ? first->parent()->childCount() : first->listView()->childCount();
	if (count < 2) return;
	
	Query *q = first->tree()->query();
	TreeItem **set = new TreeItem*[count];

	int manually = 0; // I store these starting at the end (of set)
	int at=0; // I store these starting at the beginning
	
	for (TreeItem *i = first; i; i = i->nextSibling())
	{
		File after;
		if (i->file() && i->file().getPosition(q, &after))
		{
			set[count-manually-1] = i;
			manually++;
		}
		else
		{
			set[at] = i;
			at++;
		}
	}

	assert(count == at + manually);

	if (at > 1)
		treeItemSort(set, set+count-manually-1);
	
	// grr, QListView sucks
	set[0]->moveItem(set[1]);
	TreeItem *previous = set[0];
	
	int manualPosition = count - manually;
	
	for (int i=1; i <count-manually; i++)
	{
		File maybeafter = previous->file();
		
		// perhaps one of the manually sorted ones fit here..
		for (int mi = manualPosition; mi < count; mi++)
		{
			TreeItem *now = set[mi];
			File after;
			if (now->file() && now->file().getPosition(q, &after))
			{
				if (after == maybeafter)
				{
					now->moveItem(previous);
					previous = now;
					// just try again now, as another manually sorted item
					// may be after previous
					maybeafter = previous->file();
					manualPosition++;
				}
			}
		
		}
		
		set[i]->moveItem(previous);
		previous = set[i];
	}
	
	delete [] set;
}

template <class T>
inline static void sortify(T *item)
{
	treeItemSort(item->firstChild());
}


TreeItem::TreeItem(Tree *parent, QueryGroup *group, const File &file, const QString &p)
	: KListViewItem(parent, p), mGroup(group), mUserOpened(false), mHidden(false)
{
	if (group->option(QueryGroup::Playable))
	{
		if (mFile = file)
			parent->mPlayableItemCount++;
	}
	
	sortify(parent);
}

TreeItem::TreeItem(TreeItem *parent, QueryGroup *group, const File &file, const QString &p)
	: KListViewItem(parent, p), mGroup(group), mUserOpened(false), mHidden(false)
{
	if (group->option(QueryGroup::Playable))
	{
		if (mFile = file)
			parent->tree()->mPlayableItemCount++;
	}
	
	sortify(parent);
}

TreeItem::~TreeItem()
{
	if (playable())
	{
		tree()->mPlayableItemCount--;
	}

	// I have to remove my children, because they need their parent
	// in tact for the below code
	while (TreeItem *c = firstChild())
		delete c;
	tree()->deleted(this);
}

void Tree::deleted(TreeItem *item)
{
	mAutoExpanded.removeRef(item);
	if (current() == item)
	{
		oblique()->next();
	}
}

static void pad(QString &str)
{
	int len=str.length();
	int at = 0;
	int blocklen=0;

	static const int paddingsize=12;

	// not static for reason
	const QChar chars[paddingsize] =
	{
		QChar('0'), QChar('0'), QChar('0'), QChar('0'),
		QChar('0'), QChar('0'), QChar('0'), QChar('0'),
		QChar('0'), QChar('0'), QChar('0'), QChar('0')
	};

	for (int i=0; i < len; i++)
	{
		if (str[i].isNumber())
		{
			if (!blocklen)
				at = i;
			blocklen++;
		}
		else if (blocklen)
		{
			int pads=paddingsize;
			pads -= blocklen;
			str.insert(at, chars, pads);
			i += pads;
			blocklen = 0;
		}
	}
	if (blocklen)
	{
		int pads=paddingsize;
		pads -= blocklen;
		str.insert(at, chars, pads);
	}
}

int TreeItem::compare(QListViewItem *i, int col, bool) const
{
	QString text1 = text(col);
	QString text2 = i->text(col);

	pad(text1);
	pad(text2);
	return text1.compare(text2);
}


Tree *TreeItem::tree()
{
	return static_cast<Tree*>(KListViewItem::listView());
}

QString TreeItem::presentation() const
{
	return text(0);
}

TreeItem *TreeItem::find(File item)
{
	TreeItem *i = firstChild();
	while (i)
	{
		if (i->file() == item) return i;

		TreeItem *found = i->find(item);
		if (found and found->playable()) return found;
		i = i->nextSibling();
	}
	return 0;
}

bool TreeItem::playable() const
{
	return mFile && mGroup->option(QueryGroup::Playable);
}

TreeItem *TreeItem::nextPlayable()
{
	TreeItem *next=this;
	do
	{
		next = next->next();
	} while (next && !next->playable());
	return next;
}

void TreeItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
	QFont font = p->font();
	if (tree()->current() == this)
	{
		font.setUnderline(true);
		p->setFont(font);
	}

	QColorGroup newcg(cg);
	if (parent() && parent()->isOpen() && !parent()->mUserOpened)
	{
		// slow, but not often used
		QColor text = newcg.text();
		QColor bg = newcg.background();

		int r = text.red() + bg.red();
		int g = text.green() + bg.green();
		int b = text.blue() + bg.blue();
		text.setRgb(r/2,g/2,b/2);
		newcg.setColor(QColorGroup::Text, text);
	}
	KListViewItem::paintCell(p, newcg, column, width, align);

	font.setUnderline(false);
	p->setFont(font);
}

void TreeItem::setOpen(bool o)
{
	if (!tree()->autoExpanding())
	{
		mUserOpened = o;
		tree()->removeAutoExpanded(this);
	}
	KListViewItem::setOpen(o);
}

void TreeItem::autoExpand()
{
	tree()->setAutoExpanding(true);
	if (tree()->current() == this)
	{
		tree()->resetAutoExpanded();
		forceAutoExpand();
	}
	tree()->setAutoExpanding(false);
}

void TreeItem::forceAutoExpand()
{
	if (parent())
		parent()->forceAutoExpand();

	if (!mUserOpened)
		tree()->addAutoExpanded(this);
	setOpen(true);
}

bool TreeItem::hideIfNoMatch(const QString &match)
{
	if (!firstChild())
	{
		if (match.length())
		{
			if (!text(0).contains(match, false))
			{
				setHidden(true);
				return false;
			}
		}
		setHidden(false);
		return true;
	}
	else
	{
		bool visible=true;

		if (match.length())
		{
			visible = text(0).contains(match, false);
		}

		if (visible)
		{
			QString empty;
			for (TreeItem *ch = firstChild(); ch; ch = ch->nextSibling())
			{
				ch->hideIfNoMatch(empty);
			}
		}
		else
		{
			for (TreeItem *ch = firstChild(); ch; ch = ch->nextSibling())
			{
				bool here = ch->hideIfNoMatch(match);
				visible = visible || here;
			}
		}

		setHidden(!visible);

		return visible;
	}
}

void TreeItem::setup()
{
	QListViewItem::setup();
	if (mHidden)
		setHeight(0);
}

void TreeItem::setHidden(bool h)
{
	mHidden = h;
	setup();
}

TreeItem *TreeItem::next()
{
	if (firstChild())
	{
		return firstChild();
	}
	else
	{ // go up the tree
		TreeItem *upYours = this;
		do
		{
			if (upYours->nextSibling())
				return upYours->nextSibling();
			upYours = upYours->parent();
		} while (upYours);
	}
	return 0;
}







Tree::Tree(Oblique *oblique, QWidget *parent)
	: KListView(parent), mOblique(oblique), mAutoExpanding(0)
{
	mCurrent = 0;
	lastMenu =0;
	mPlayableItemCount = 0;
	mLoader = 0;

	addColumn("");
	setCaption(i18n("Oblique"));
	setRootIsDecorated(true);
	
	setAcceptDrops(true);
	setDragEnabled(true);
	setItemsMovable(true);
	setDropVisualizer(true);
	setSorting(-1);
	
	((QWidget*)header())->hide();

	
	connect(
			this, SIGNAL(moved(QPtrList<QListViewItem>&, QPtrList<QListViewItem>&, QPtrList<QListViewItem>&)),
			SLOT(dropped(QPtrList<QListViewItem>&, QPtrList<QListViewItem>&, QPtrList<QListViewItem>&))
		);
	
	connect(
			this, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)),
			SLOT(contextMenu(KListView*, QListViewItem*, const QPoint&))
		);
	connect(
			this, SIGNAL(executed(QListViewItem*)),
			SLOT(play(QListViewItem*))
		);

	Base *base = oblique->base();
	connect(base, SIGNAL(added(File)), SLOT(insert(File)));
	connect(base, SIGNAL(removed(File)), SLOT(remove(File)));
	connect(base, SIGNAL(modified(File)), SLOT(update(File)));

	connect(base, SIGNAL(addedTo(Slice*, File)), SLOT(checkInsert(Slice*, File)));
	connect(base, SIGNAL(removedFrom(Slice*, File)), SLOT(checkRemove(Slice*, File)));
		
	connect(this, SIGNAL(selected(TreeItem*)), oblique, SLOT(selected(TreeItem*)));

	mSlice = oblique->base()->defaultSlice();

	KConfigGroup g(KGlobal::config(), "oblique");
	mFileOfQuery = g.readEntry("schema", "standard");
	if (!setSchema(mFileOfQuery))
	{
		setSchema("standard");
	}

}

Tree::~Tree()
{
	// have to clear here to prevent sigsegv on exit
	clear();
}

void Tree::clear()
{
	if (mCurrent)
	{
		napp->player()->stop();
		setCurrent(0);
	}
	KListView::clear();
}

void Tree::movableDropEvent (QListViewItem* parent, QListViewItem* afterme)
{
	QPtrList<QListViewItem> items = selectedItems(true);
	for (QPtrList<QListViewItem>::Iterator i(items.begin()); *i; ++i)
	{
		if ((*i)->parent() != parent)
			return;
	}
	
	KListView::movableDropEvent(parent, afterme);
}


void Tree::dropped(QPtrList<QListViewItem> &items, QPtrList<QListViewItem> &, QPtrList<QListViewItem> &afterNow)
{
	QPtrList<QListViewItem>::Iterator itemi = items.begin();
	QPtrList<QListViewItem>::Iterator afteri = afterNow.begin();
	while (*itemi)
	{
		TreeItem *item = static_cast<TreeItem*>(*itemi);
		TreeItem *after = static_cast<TreeItem*>(*afteri);
		item->file().setPosition(query(), after ? after->file() : File());
		
		++itemi;
		++afteri;
	}
}


TreeItem *Tree::firstChild()
	{ return static_cast<TreeItem*>(KListView::firstChild()); }

TreeItem *Tree::find(File item)
{
	TreeItem *i = firstChild();

	while (i)
	{
		if (i->file() == item) return i;

		TreeItem *found = i->find(item);
		if (found) return found;

		i = i->nextSibling();
	}
	return i;
}

void Tree::insert(TreeItem *replace, File file)
{
	TreeItem *created = collate(replace, file);
	if (mCurrent == replace)
	{
		mCurrent = created;
		repaintItem(created);
		if (isSelected(replace))
			setSelected(created, true);
	}
	if (created != replace)
	{
		delete replace;
	}
}

void Tree::insert(File file)
{
	collate(file);
}

void Tree::remove(File file)
{
	remove(firstChild(), file);
}

void Tree::checkInsert(Slice *slice, File f)
{
	if (slice == mSlice)
		insert(f);
}

void Tree::checkRemove(Slice *slice, File f)
{
	if (slice == mSlice)
		remove(f);
}


void Tree::update(File file)
{
	if (TreeItem *item = find(file))
	{
		insert(item, file);
	}
}

void Tree::remove(TreeItem *ti, const File &file)
{
	while (ti)
	{
		if (ti->file() == file)
		{
			TreeItem *t = ti->nextSibling();
			delete ti;
			ti = t;
		}
		else
		{
			remove(ti->firstChild(), file);
			ti = ti->nextSibling();
		}
	}
}

void Tree::setCurrent(TreeItem *cur)
{
	if (cur == mCurrent) return;
	// undo the old one
	TreeItem *old = mCurrent;
	mCurrent = cur;
	QPtrList<TreeItem> oldAutoExpanded = mAutoExpanded;
	mAutoExpanded.clear();
	repaintItem(old);
	repaintItem(cur);
	if (cur) cur->autoExpand();

	// do an anti-intersection on oldAutoUpdated and the new mAutoExpanded
	for (QPtrListIterator<TreeItem> i(mAutoExpanded); *i; ++i)
	{
		oldAutoExpanded.removeRef(*i);
	}


	bool user=false;
	for (QPtrListIterator<TreeItem> i(oldAutoExpanded); *i; ++i)
	{
		if ((*i)->userOpened())
		{
			user = true;
			break;
		}
	}
	if (!user)
	{
		for (QPtrListIterator<TreeItem> i(oldAutoExpanded); *i; ++i)
		{
			(*i)->setOpen(false);
		}
	}

	ensureItemVisible(cur);
}

void Tree::reload()
{
	delete mLoader;
	clear();
	mLoader = new Loader(this);
	connect(mLoader, SIGNAL(finished()), SLOT(destroyLoader()));
}

void Tree::setSlice(Slice *slice)
{
	if (mSlice == slice) return;
	mSlice = slice;
	reload();
}

bool Tree::setSchema(const QString &name)
{
	mFileOfQuery = name;
	if (!oblique()->loadSchema(mQuery, name))
		return false;
	reload();
	return true;
}

QDragObject *Tree::dragObject()
{
	if (currentItem() && static_cast<TreeItem*>(currentItem())->file())
		return KListView::dragObject();
	return 0;
}

void Tree::destroyLoader()
{
	delete mLoader;
	mLoader = 0;
}

void Tree::setLimit(const QString &text)
{
	for (TreeItem *ch = firstChild(); ch; ch = ch->nextSibling())
	{
		ch->hideIfNoMatch(text);
	}
}


void Tree::contextMenu(KListView*, QListViewItem* i, const QPoint& p)
{
	if (!i) return;
	delete lastMenu;
	lastMenu = new FileMenu(this, oblique(), static_cast<TreeItem*>(i) );
	lastMenu->popup(p);
}

void Tree::play(QListViewItem *_item)
{
	if (!_item) return;
	TreeItem *item = static_cast<TreeItem*>(_item);
	if (item->playable())
		emit selected(item);
	else
		play(item->nextPlayable());
}


TreeItem *Tree::collate(TreeItem *fix, QueryGroup *group, const File &file, TreeItem *childOf)
{
	do
	{
		if (group->matches(file))
		{
			TreeItem *nodefix=0;
			if (fix && fix->group() == group)
				nodefix = fix;

			TreeItem *item = node(nodefix, group, file, childOf);
			TreeItem *ti=0;
			if (group->firstChild())
			{
				ti = collate(fix, group->firstChild(), file, item);
			}
			if (ti && ti->playable())
				return ti;
			else if(item && item->playable())
				return item;
			else
				return 0;
		}

	} while (( group = group->nextSibling()));
	return 0;
}

TreeItem *Tree::node(TreeItem *fix, QueryGroup *group, const File &file, TreeItem *childOf)
{
	// search childOf's immediate children
	TreeItem *children;
	if (childOf)
		children = childOf->firstChild();
	else
		children = firstChild();

	QString presentation = group->presentation(file);
	while (children)
	{
		// merging would be done here
		bool matches=false;
		if (group->fuzzyness(QueryGroup::Case))
		{
			matches = (children->text(0).lower() == presentation.lower());
		}
		else
		{
			matches = (children->text(0) == presentation);
		}

		matches = matches && !children->group()->option(QueryGroup::Playable);

		if (matches)
		{
			children->setFile(File());
			return children;
		}

		children = children->nextSibling();
	}

	TreeItem *item;
	if (group->option(QueryGroup::ChildrenVisible))
	{
		item = childOf;
	}
	else if (fix)
	{
		item = fix;
		if (fix->parent() != childOf)
			moveItem(fix, childOf, 0);
		item->setText(0, presentation);
	}
	else if (childOf)
	{
		item = new TreeItem(childOf, group, file, presentation);
	}
	else
	{
		item = new TreeItem(this, group, file, presentation);
	}

	item->setOpen(group->option(QueryGroup::AutoOpen));

	return item;
}

#include "tree.moc"

