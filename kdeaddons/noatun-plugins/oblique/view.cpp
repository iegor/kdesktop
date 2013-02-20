// Copyright (c) 2003-2005 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "view.h"
#include "oblique.h"
#include "menu.h"

#include <noatun/app.h>

#include <kstdaction.h>
#include <klocale.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <qlabel.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <ktabwidget.h>
#include <qtabbar.h>

class TabWidget : public KTabWidget
{
public:
	TabWidget(QWidget *parent)
		: KTabWidget(parent)
	{
	}
public:
	QTabBar *tabBar() const { return KTabWidget::tabBar(); }
};


View::View(Oblique *oblique)
	: KMainWindow(0, 0)
{
	mOblique = oblique;
	mTree = 0;

	mTabs = new TabWidget(this);
	mTabs->tabBar()->hide();
	connect(mTabs, SIGNAL(currentChanged(QWidget*)), SLOT(currentTabChanged(QWidget*)));

	setCentralWidget(mTabs);

	KAction *ac;
	ac = new KAction(i18n("Add &Files..."), "queue", 0, this, SLOT(addFiles()), actionCollection(), "add_files");
	ac->setWhatsThis(i18n("Add a reference to a media file on disk to this collection."));
	ac = new KAction(i18n("Add Fol&ders..."), "folder", 0, this, SLOT(addDirectory()), actionCollection(), "add_dir");
	
	
//	ac = new KAction(i18n("&Reload"), "reload", 0, oblique, SLOT(reload()), actionCollection(), "reload");
//	ac->setWhatsThis(i18n("Reread the collection and meta-information from its files."));

	ac = new SliceListAction(
			i18n("&Slices"), oblique,
			this, SLOT(use(Slice*)), QValueList<File>(), actionCollection(), "slices"
		);
	ac->setWhatsThis(i18n("Select a sub-collection to display"));

	mSchemaListAction = new SchemaListAction(
			i18n("&Schemas"), this, SLOT(setSchema(const QString&)), actionCollection(), "schemas"
		);
	mSchemaListAction->setWhatsThis(i18n("Select a schema to use to collate the tree."));

	ac = new KAction(
			i18n("&New Tab"), "tab_new", "CTRL+SHIFT+N;CTRL+T", this, SLOT(addTab()),
			actionCollection(), "newtab"
		);

	mRemoveTabAction = new KAction(
			i18n("&Close Current Tab"), "tab_remove", CTRL+Key_W, this, SLOT(removeTab()),
			actionCollection(), "removecurrenttab"
		);

	{
		QLabel *l = new QLabel(i18n("&Jump:"), 0, "kde toolbar widget");
		l->setBackgroundMode( Qt::PaletteButton );
		l->setAlignment(
				(QApplication::reverseLayout() ? Qt::AlignRight : Qt::AlignLeft) |
					Qt::AlignVCenter | Qt::ShowPrefix
			);
		l->adjustSize();
		new KWidgetAction(l, i18n("&Jump:"), KShortcut(ALT + Key_J), 0, 0, actionCollection(), "jump_label");

		LineEditAction *jumpAction = new LineEditAction(i18n("Jump Bar"), 0, 0, actionCollection(), "jump_text");
		jumpAction->setWhatsThis(i18n("Only display items which contain this string"));
		l->setBuddy(jumpAction->lineEdit());
		connect(jumpAction->lineEdit(), SIGNAL(textChanged(const QString&)), SLOT(jumpTextChanged(const QString&)));
	}

	KStdAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());

	applyMainWindowSettings(KGlobal::config(), "Oblique View");
	createGUI("obliqueui.rc");

	KConfigGroup g(KGlobal::config(), "oblique");
	QStringList tabids = g.readListEntry("tabids");
	
	for (QStringList::Iterator i(tabids.begin()); i != tabids.end(); ++i)
	{
		QString t = *i;
		int sliceid = t.section(':', 0, 0).toInt();
		QString fileOfQuery = t.section(':', 1, 1);
		
		Slice *slice = oblique->base()->sliceById(sliceid);
		if (!slice)
			slice = oblique->base()->defaultSlice();
		Tree *tree = new Tree(oblique, mTabs);
		mTrees.append(tree);
		tree->setSlice(slice);
		tree->setSchema(fileOfQuery);
		mTabs->addTab(tree, slice->name());
	}
	
	if (mTabs->count() == 0)
	{ // no tabs, so use the "normal" route
		addTab();
	}
	else
	{
		// ok, there's a tab, so make it present
		if (mTabs->count() >= 1)
		{
			mTree = static_cast<Tree*>(mTrees.first());
			currentTabChanged(mTrees.first());
		}
	
		if (mTabs->count() > 1)
			mTabs->tabBar()->show();
	}
}

View::~View()
{
	QStringList tabids;
	for (int i=0; i < mTabs->count(); i++)
	{
		Tree *tree = static_cast<Tree*>(mTabs->page(i));
		int slice = tree->slice()->id();
		QString query = tree->fileOfQuery();

		QString t = QString("%1:%2").arg(slice).arg(query);
		tabids.append(t);
	}
	
	KConfigGroup g(KGlobal::config(), "oblique");
	g.writeEntry("tabids", tabids);
	g.sync();
}

void View::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "Oblique View");
	KEditToolbar dlg(actionCollection(), "obliqueui.rc");
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void View::newToolBarConfig()
{
	createGUI("obliqueui.rc");
	applyMainWindowSettings(KGlobal::config(), "Oblique View");
}

void View::closeEvent(QCloseEvent*)
{
	hide();
}


void View::addFiles()
{
	KURL::List files=KFileDialog::getOpenURLs(":mediadir", napp->mimeTypes(), this, i18n("Select Files to Add"));

	for(KURL::List::Iterator it=files.begin(); it!=files.end(); ++it)
		oblique()->addFile(KURL(*it));
}

void View::addDirectory()
{
	QString folder = KFileDialog::getExistingDirectory(":mediadir:", this,
		i18n("Select Folder to Add"));
	
	if (!folder)
		return;

	KURL url;
	url.setPath(folder);
	oblique()->beginDirectoryAdd(url);
}

void View::addTab()
{
	Tree *t = new Tree(oblique(), mTabs);
	if (!mTree) mTree = t;
	mTrees.append(t);

	mTabs->addTab(t, t->slice()->name());
	mTabs->showPage(t);
	if (mTabs->count() > 1)
		mTabs->tabBar()->show();
	currentTabChanged(t);
}

void View::removeTab()
{
	Tree *current = static_cast<Tree*>(mTabs->currentPage());
	if (current == mTree) return;
	mTabs->removePage(current);
	mTrees.remove(current);
	delete current;

	if (mTabs->count() == 1)
		mTabs->tabBar()->hide();
}

void View::currentTabChanged(QWidget *w)
{
	mRemoveTabAction->setEnabled(w != mTree);
	mSchemaListAction->setTree(static_cast<Tree*>(w));
}

void View::setSchema(const QString &file)
{
	Tree *current = static_cast<Tree*>(mTabs->currentPage());
	current->setSchema(file);
}


void View::jumpTextChanged(const QString &text)
{
	Tree *current = static_cast<Tree*>(mTabs->currentPage());
	current->setLimit(text);
}

void View::use(Slice *s)
{
	Tree *current = static_cast<Tree*>(mTabs->currentPage());
	current->setSlice(s);
	mTabs->setTabLabel(current, current->slice()->name());
}



LineEditAction::LineEditAction(const QString &text, const QObject *reciever, const char *slot, KActionCollection *parent, const char *name)
	: KWidgetAction(new KLineEdit(0), text, KShortcut(0), reciever, slot, parent, name)
{
	setAutoSized(true);
}

#include "view.moc"

