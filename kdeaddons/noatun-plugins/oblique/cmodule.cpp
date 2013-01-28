// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "cmodule.h"
#include "oblique.h"
#include "file.h"

#include <kregexpeditorinterface.h>
#include <kparts/componentfactory.h>
#include <klocale.h>
#include <klistview.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kconfig.h>

#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qfileinfo.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>
#include <qheader.h>

SchemaConfig::SchemaConfig(QWidget *parent, Oblique *oblique)
	: QWidget(parent)
{
	mOblique = oblique;
	mIgnore = true;
	mRegexpEditor=0;

	{
		QVBoxLayout *layout = new QVBoxLayout(this, 11, 7);
		layout->setAutoAdd(true);
		layout->setSpacing(7);
	}


	{
		QHBox *box = new QHBox(this);
		box->setSpacing(7);
		mSchemaList = new QComboBox(box);
		connect(
				mSchemaList, SIGNAL(activated(const QString&)),
				SLOT(selectSchema(const QString&))
			);

		mAdd = new QPushButton(BarIconSet("filenew"), 0, box);
		mAdd->setFixedWidth(mAdd->height());
		QToolTip::add(mAdd, i18n("Create new schema"));
		connect(mAdd, SIGNAL(clicked()), SLOT(newSchema()));

		mRemove = new QPushButton(BarIconSet("editdelete"), 0, box);
		mRemove->setFixedWidth(mRemove->height());
		QToolTip::add(mRemove, i18n("Remove this schema"));
		connect(mRemove, SIGNAL(clicked()), SLOT(removeSchema()));

		mCopy = new QPushButton(BarIconSet("editcopy"), 0, box);
		mCopy->setFixedWidth(mCopy->height());
		QToolTip::add(mCopy, i18n("Copy this schema"));
		connect(mCopy, SIGNAL(clicked()), SLOT(copySchema()));
	}


	{
		QHBox *middle = new QHBox(this);
		middle->setSpacing(7);

		mSchemaTree = new KListView(middle);
		connect(
				mSchemaTree, SIGNAL(currentChanged(QListViewItem*)),
				SLOT(setCurrent(QListViewItem*))
			);
		connect(
				mSchemaTree, SIGNAL(moved(QListViewItem *, QListViewItem *, QListViewItem *)),
				SLOT(move(QListViewItem *, QListViewItem *, QListViewItem *))
			);

		mSchemaTree->setAcceptDrops(true);
		mSchemaTree->setSorting(-1);
		mSchemaTree->setDropVisualizer(true);
		mSchemaTree->setDragEnabled(true);
		mSchemaTree->setItemsMovable(true);

		mSchemaTree->addColumn(i18n("Property"));
		mSchemaTree->addColumn(i18n("Value"));
		mSchemaTree->addColumn(i18n("Presentation"));

		QVBox *buttons = new QVBox(middle);

		mAddSibling = new QPushButton(BarIconSet("1rightarrow", KIcon::SizeSmall), "",buttons);
		mAddSibling->setFixedWidth(mAddSibling->height());
		connect(mAddSibling, SIGNAL(clicked()), SLOT(addSibling()));
		QToolTip::add(mAddSibling, i18n("Create a new item after the selected one"));

		mAddChild = new QPushButton(BarIconSet("2rightarrow", KIcon::SizeSmall), "", buttons);
		mAddChild->setFixedWidth(mAddChild->height());
		connect(mAddChild, SIGNAL(clicked()), SLOT(addChild()));
		QToolTip::add(mAddChild, i18n("Create a new child item under the selected one"));

		mRemoveSelf = new QPushButton(BarIconSet("filenew", KIcon::SizeSmall), "", buttons);
		mRemoveSelf->setFixedWidth(mRemoveSelf->height());
		connect(mRemoveSelf, SIGNAL(clicked()), SLOT(removeSelf()));
		QToolTip::add(mRemoveSelf, i18n("Remove the selected item"));

		new QWidget(buttons);
	}


	{
		QVBox *side = new QVBox(this);
		side->setSpacing(7);
		// the controllers
		{
			QWidget *topSide = new QWidget(side);
			QGridLayout *grid = new QGridLayout(topSide);

			QLabel *label;

			label = new QLabel(i18n("&Property:"), topSide);
			mPropertyEdit = new KLineEdit(topSide);
			label->setBuddy(mPropertyEdit);
			grid->addWidget(label, 0, 0);
			grid->addMultiCellWidget(mPropertyEdit, 0, 0, 1, 2);
			connect(mPropertyEdit, SIGNAL(textChanged(const QString&)), SLOT(updateCurrent()));

			label = new QLabel(i18n("&Value:"), topSide);
			mValueEdit = new KLineEdit(topSide);
			label->setBuddy(mPropertyEdit);
			grid->addWidget(label, 1, 0);
			grid->addMultiCellWidget(mValueEdit, 1, 1, 1, 1);
			connect(mValueEdit, SIGNAL(textChanged(const QString&)), SLOT(updateCurrent()));

			QPushButton *editRe = new QPushButton(i18n("&Edit..."), topSide);
			grid->addWidget(editRe, 1, 2);
			connect(editRe, SIGNAL(clicked()), SLOT(editValueRegexp()));

			label = new QLabel(i18n("Pre&sentation:"), topSide);
			mPresentationEdit = new KLineEdit(topSide);
			label->setBuddy(mPropertyEdit);
			grid->addWidget(label, 2, 0);
			grid->addMultiCellWidget(mPresentationEdit, 2, 2, 1, 2);
			connect(mPresentationEdit, SIGNAL(textChanged(const QString&)), SLOT(updateCurrent()));
		}

		{
			QGroupBox *groupbox = new QGroupBox(
					3, Qt::Horizontal, i18n("Options"), side
				);

			mOptionPlayable = new QCheckBox(i18n("Play&able"), groupbox);
			QWhatsThis::add(mOptionPlayable, i18n("This branch represents an individual file.  If two items' presentation match, two items are created."));
			connect(mOptionPlayable, SIGNAL(toggled(bool)), SLOT(updateCurrent()));

			mOptionChildrenVisible = new QCheckBox(i18n("&Children visible"), groupbox);
			QWhatsThis::add(mOptionChildrenVisible, i18n("Don't create this node, this nodes children become direct children of this node's parent"));
			connect(mOptionChildrenVisible, SIGNAL(toggled(bool)), SLOT(updateCurrent()));

			mOptionAutoOpen = new QCheckBox(i18n("Auto &open"), groupbox);
			QWhatsThis::add(mOptionAutoOpen, i18n("This branch is marked as open immediately."));
			connect(mOptionAutoOpen, SIGNAL(toggled(bool)), SLOT(updateCurrent()));
		}

	}

}

class QueryGroupItem : public KListViewItem
{
	QueryGroup *mItem;

public:
	QueryGroupItem(QueryGroupItem *parent, QueryGroup *group, QueryGroupItem *after=0)
		: KListViewItem(parent, after)
	{
		init(group);
	}

	QueryGroupItem(KListView *parent, QueryGroup *group, QueryGroupItem *after=0)
		: KListViewItem(parent, after)
	{
		init(group);
	}

	QueryGroup *item() { return mItem; }
	const QueryGroup *item() const { return mItem; }

	void redisplay()
	{
		setText(0, item()->propertyName());
		setText(1, item()->value().pattern());
		setText(2, item()->presentation());
	}

	QueryGroupItem *parent()
		{ return static_cast<QueryGroupItem*>(KListViewItem::parent()); }

	KListView *listView()
		{ return static_cast<KListView*>(KListViewItem::listView()); }

private:
	void init(QueryGroup *group)
	{
		mItem = group;
		redisplay();

		if (group->firstChild())
			new QueryGroupItem(this, group->firstChild(), this);

		// do siblings now iff I don't already have them
		if (!nextSibling())
		{
			if (QueryGroup *after = group->nextSibling())
			{
				if (parent())
					new QueryGroupItem(parent(), after, this);
				else
					new QueryGroupItem(listView(), after, this);
			}
		}

		setOpen(true);
	}
};


void SchemaConfig::reopen()
{
	mSchemaList->clear();
	mQueries.clear();
	mSchemaTree->clear();

	QStringList names = oblique()->schemaNames();
	QString firstTitle;

	for (QStringList::Iterator i(names.begin()); i != names.end(); ++i)
	{
		QueryItem qi;
		qi.title = oblique()->loadSchema(qi.query, *i);
		qi.changed = false;
		mQueries.insert(*i, qi);

		if (!mSchemaList->count())
			firstTitle = qi.title;
		mSchemaList->insertItem(qi.title);
	}
	selectSchema(firstTitle);

}

void SchemaConfig::save()
{
	for (QMap<QString,QueryItem>::Iterator i(mQueries.begin()); i != mQueries.end(); ++i)
	{
		QString name = i.key();
		name = QFileInfo(name).fileName();
		if (i.data().changed)
		{
			oblique()->saveSchema(i.data().query, name, i.data().title);
			// TODO update the trees.
		}
	}
}


QString SchemaConfig::nameToFilename(const QString &_name)
{
	QString name = _name;
	name = name.replace(QRegExp("[^a-zA-Z0-9]"), "_");
	return name;
}

void SchemaConfig::newSchema()
{
	bool ok;
	QString str=KInputDialog::getText(
			i18n("Name of Schema"),
			i18n("Please enter the name of the new schema:"),
			"", &ok, this
		);
	if (!ok) return;

	QString filename = nameToFilename(str);

	if (mQueries.contains(nameToFilename(filename))) return;

	QueryItem queryitem;
	queryitem.query = Query();
	queryitem.title = str;
	queryitem.changed=true;
	mSchemaList->insertItem(str);
	mQueries.insert(filename, queryitem);

	selectSchema(str);
}

void SchemaConfig::copySchema()
{
	bool ok;
	QString str=KInputDialog::getText(
			i18n("Name of Schema"),
			i18n("Please enter the name of the new schema:"),
			"", &ok, this
		);
	if (!ok) return;

	QString filename = nameToFilename(str);

	if (mQueries.contains(nameToFilename(filename))) return;

	QueryItem queryitem;
	queryitem.query = currentQuery()->query;
	queryitem.title = str;
	queryitem.changed=true;
	mSchemaList->insertItem(str);
	mQueries.insert(filename, queryitem);

	selectSchema(str);
}

void SchemaConfig::removeSchema()
{
	QueryItem *item = currentQuery();
	mSchemaList->removeItem(mSchemaList->currentItem());
	oblique()->removeSchema(nameToFilename(item->title));
	selectSchema(mSchemaList->currentText());
}

void SchemaConfig::selectSchema(const QString &title)
{
	mSchemaTree->clear();
	mSchemaList->setCurrentText(title);

	mIgnore = true;
	if (QueryItem *grp = currentQuery())
	{
		if (QueryGroup *g = grp->query.firstChild())
			new QueryGroupItem(mSchemaTree, g);
	}
	mSchemaTree->setCurrentItem(mSchemaTree->firstChild());
	setCurrent(mSchemaTree->firstChild());
	mSchemaTree->setSelected(mSchemaTree->firstChild(), true);
	mIgnore=false;
}

void SchemaConfig::editValueRegexp()
{
	unless (mRegexpEditor)
	{
		mRegexpEditor =
			KParts::ComponentFactory::createInstanceFromQuery<QDialog>(
					"KRegExpEditor/KRegExpEditor", QString::null, this
				);
	}

	if ( mRegexpEditor )
	{
		KRegExpEditorInterface *iface =
			static_cast<KRegExpEditorInterface*>(
					mRegexpEditor->qt_cast( "KRegExpEditorInterface")
				);

		iface->setRegExp(mValueEdit->text());
		if (mRegexpEditor->exec() == QDialog::Accepted)
			mValueEdit->setText(iface->regExp());
	}
}

void SchemaConfig::setCurrent(QListViewItem *_item)
{
	if (!_item) return;
	QueryGroupItem *item = static_cast<QueryGroupItem*>(_item);
	mIgnore = true;
	mPropertyEdit->setText(item->item()->propertyName());
	mValueEdit->setText(item->item()->value().pattern());
	mPresentationEdit->setText(item->item()->presentation());

	mOptionPlayable->setChecked(item->item()->option(QueryGroup::Playable));
	mOptionAutoOpen->setChecked(item->item()->option(QueryGroup::AutoOpen));
	mOptionChildrenVisible->setChecked(item->item()->option(QueryGroup::ChildrenVisible));
	mIgnore = false;
}


void SchemaConfig::updateCurrent()
{
	QueryGroupItem *item = static_cast<QueryGroupItem*>(mSchemaTree->currentItem());
	if (mIgnore || !item) return;
	setCurrentModified();

	QueryGroup *mod = item->item();

	mod->setPropertyName(mPropertyEdit->text());
	mod->setPresentation(mPresentationEdit->text());
	mod->setValue(QRegExp(mValueEdit->text()));

	mod->setOption(QueryGroup::AutoOpen, mOptionAutoOpen->isChecked());
	mod->setOption(QueryGroup::Playable, mOptionPlayable->isChecked());
	mod->setOption(QueryGroup::ChildrenVisible, mOptionChildrenVisible->isChecked());
	item->redisplay();
}

void SchemaConfig::move(QListViewItem *_item, QListViewItem *, QListViewItem *_afterNow)
{
	setCurrentModified();
	QueryGroupItem *item = static_cast<QueryGroupItem*>(_item);
	QueryGroupItem *afterNow = static_cast<QueryGroupItem*>(_afterNow);

	QueryGroup *after, *under;
	under = item->parent() ? item->parent()->item() : 0;
	after = afterNow ? afterNow->item() : 0;

	item->item()->move(&currentQuery()->query, under, after);
}


void SchemaConfig::addSibling()
{
	QueryGroupItem *item = static_cast<QueryGroupItem*>(mSchemaTree->currentItem());
	unless (item)
	{
		addChild();
		return;
	}
	setCurrentModified();

	// add it
	QueryGroup * g = new QueryGroup;
	item->item()->insertAfter(g);

	// now match the action in the tree
	QueryGroupItem *created;
	if (item->parent())
		created = new QueryGroupItem(item->parent(), g, item);
	else
		created = new QueryGroupItem(item->listView(), g, item);

	// select it so the user can edit it now
	item->listView()->setCurrentItem(created);
	item->listView()->setSelected(created, true);
}


void SchemaConfig::addChild()
{
	QueryGroupItem *item = static_cast<QueryGroupItem*>(mSchemaTree->currentItem());
	setCurrentModified();

	// add it
	QueryGroup * g = new QueryGroup;
	if (item)
		item->item()->insertUnder(g);
	else
		currentQuery()->query.setFirstChild(g);

	// now match the action in the tree
	QueryGroupItem *created;
	if (item)
		created = new QueryGroupItem(item, g);
	else
		created = new QueryGroupItem(mSchemaTree, g);

	// select it so the user can edit it now
	mSchemaTree->setCurrentItem(created);
	mSchemaTree->setSelected(created, true);
}

void SchemaConfig::removeSelf()
{
	setCurrentModified();
	QueryGroupItem *item = static_cast<QueryGroupItem*>(mSchemaTree->currentItem());
	unless (item) return;
	QueryGroup *grp = item->item();
	delete item;
	currentQuery()->query.take(grp);
	delete grp;
}

void SchemaConfig::setCurrentModified()
{
	if (QueryItem *grp = currentQuery())
	  grp->changed = true;
}

SchemaConfig::QueryItem *SchemaConfig::currentQuery()
{
	QString title = mSchemaList->currentText();
	for (QMap<QString,QueryItem>::Iterator i(mQueries.begin()); i != mQueries.end(); ++i)
	{
		if (i.data().title != title) continue;
		return &i.data();
	}
	return 0;
}


SliceConfig::SliceConfig(QWidget *parent, Oblique *oblique)
	: QWidget(parent)
{
	mOblique = oblique;
	(new QVBoxLayout(this, 11, 7))->setAutoAdd(true);

	{
		QHBox *middle = new QHBox(this);
		middle->setSpacing(7);

		mSliceList = new KListView(middle);
		QWhatsThis::add(mSliceList, i18n("The list of slices.  A Slice is part of the full collection. This allows you to categorize your playlist. You can add an item to the list by right clicking on it and selecting the Slice you want it in."));
		mSliceList->addColumn("");
		mSliceList->header()->hide();

		mSliceList->setSorting(0);
		mSliceList->setItemsRenameable(true);

		QVBox *buttons = new QVBox(middle);

		mAdd = new QPushButton(BarIconSet("1rightarrow", KIcon::SizeSmall), "",buttons);
		mAdd->setFixedWidth(mAdd->height());
		connect(mAdd, SIGNAL(clicked()), SLOT(addSibling()));
		QToolTip::add(mAdd, i18n("Create a new item"));

		mRemove = new QPushButton(BarIconSet("filenew", KIcon::SizeSmall), "", buttons);
		mRemove->setFixedWidth(mRemove->height());
		connect(mRemove, SIGNAL(clicked()), SLOT(removeSelf()));
		QToolTip::add(mRemove, i18n("Remove the selected item"));

		new QWidget(buttons);
	}
}


class SliceListItem : public KListViewItem
{
	Slice *mSlice;
public:
	SliceListItem(KListView *parent, Slice *slice)
		 : KListViewItem(parent, slice->name()), mSlice(slice)
	{
	}

	/**
	 * new item
	 **/
	SliceListItem(KListView *parent)
		 : KListViewItem(parent, i18n("New Slice")), mSlice(0)
	{
	}

	Slice *slice() { return mSlice; }
};

SliceListItem *SliceConfig::currentItem()
{
	QListViewItem *c = mSliceList->currentItem();
	return static_cast<SliceListItem*>(c);
}


void SliceConfig::reopen()
{
	mSliceList->clear();
	mRemovedItems.clear();
	mAddedItems.clear();

	QPtrList<Slice> slices = oblique()->base()->slices();

	for (QPtrListIterator<Slice> i(slices); *i; ++i)
	{
		Slice *slice = *i;
		new SliceListItem(mSliceList, slice);
	}
}

void SliceConfig::save()
{
	for (
			QValueList<Slice*>::Iterator i(mRemovedItems.begin());
			i != mRemovedItems.end();
			++i
		)
	{
		(*i)->remove();
		delete *i;
	}

	for (
			QValueList<SliceListItem*>::Iterator i(mAddedItems.begin());
			i != mAddedItems.end();
			++i
		)
	{
		oblique()->base()->addSlice((*i)->text(0));
	}

	for (QListViewItem *i = mSliceList->firstChild(); i; i = i->nextSibling())
	{
		SliceListItem *si = static_cast<SliceListItem*>(i);

		if (si->slice())
		{
			si->slice()->setName(si->text(0));
		}
	}

	reopen();
}


void SliceConfig::addSibling()
{
	SliceListItem *si = new SliceListItem(mSliceList);
	mAddedItems.append(si);
}

void SliceConfig::removeSelf()
{
	SliceListItem *r = currentItem();
	if (mAddedItems.contains(r))
	{
		mAddedItems.remove(r);
	}
	else
	{
		Q_ASSERT(r->slice());
		mRemovedItems.append(r->slice());
	}

	delete r;
}



Configure::Configure(Oblique *oblique)
	: CModule(i18n("Oblique"), i18n("Configure Oblique Playlist"), "", oblique)
{
	(new QVBoxLayout(this))->setAutoAdd(true);
	tabs = new QTabWidget(this);

	tabs->addTab(slice = new SliceConfig(tabs, oblique), i18n("Slices"));
	tabs->addTab(schema = new SchemaConfig(tabs, oblique), i18n("Schemas"));
}

void Configure::reopen()
{
	schema->reopen();
	slice->reopen();
}

void Configure::save()
{
	schema->save();
	slice->save();
}


#include "cmodule.moc"

