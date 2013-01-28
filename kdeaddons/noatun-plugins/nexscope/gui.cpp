#include <klocale.h>
#include <qheader.h>
#include <qlayout.h>
#include <qdragobject.h>
#include <kfiledialog.h>
#include <kstdaction.h>
#include <kaction.h>

#include "gui.h"
#include "renderers.h"
#include "nex.h"

class CreatorItem : public QListViewItem
{
public:
	CreatorItem(QListView *parent, const QString &title)
		: QListViewItem(parent, title)
		{}
};

class TreeItem : public QListViewItem
{
public:
	TreeItem(TreeItem *parent, TreeItem *after, CreatorItem *creator)
		: QListViewItem(parent, after, creator->text(0))
	{
		RendererList *list;
		if (parent)
			list=static_cast<RendererList*>(parent->mRenderer);
		else
			list=nex->rendererList();
		
		Renderer *afterRenderer=0;
		if (after)
			afterRenderer=after->mRenderer;
		
		list->lock();
		int pos=list->renderers().findRef(afterRenderer);
		if (pos==-1) pos=list->renderers().count();
		
		list->renderers().insert((uint)pos, mRenderer=nex->renderer(creator->text(0)));
		list->unlock();
	}
	
	TreeItem(QListView *parent, TreeItem *after, const QString &title)
		: QListViewItem(parent, after, title)
	{
		mRenderer=nex->rendererList();
		setExpandable(true);
	}
	
	~TreeItem()
	{
		RendererList *list;
		if (parent())
			list=static_cast<RendererList*>(static_cast<TreeItem*>(parent())->mRenderer);
		else
			return;
		
		list->lock();
		list->renderers().removeRef(mRenderer);
		list->unlock();
		delete mRenderer;
	}

	Renderer *renderer() { return mRenderer; }
private:
	Renderer *mRenderer;
};

Control::Control() : mConfigurator(0)
{
	{
		KToolBar *tools=toolBar();
		KStdAction::save(this, SLOT(save()), actionCollection())->plug(tools);
		KStdAction::saveAs(this, SLOT(saveAs()), actionCollection())->plug(tools);
		KStdAction::open(this, SLOT(open()), actionCollection())->plug(tools);
	
	}
	
	setCaption(i18n("Nex Configuration"));
	QSplitter *mainSplitter=new QSplitter(this);
	setCentralWidget(mainSplitter);
	QSplitter *left=new QSplitter(Qt::Vertical, mainSplitter);
	{
		mTree=new RendererListView(left);
		mTree->setItemsMovable(true);
		mTree->setSorting(-1);
		mTree->setRootIsDecorated(true);
		
		connect(mTree, SIGNAL(dropped(QDropEvent*, QListViewItem*, QListViewItem*)),
		        SLOT(dropEvent(QDropEvent*, QListViewItem*, QListViewItem*)));
		
		connect(mTree, SIGNAL(currentChanged(QListViewItem*)),
		        SLOT(currentChanged(QListViewItem*)));
		
		mCreatorsList=new RendererListView(left);
		mCreatorsList->addColumn(i18n("Name"));
		mCreatorsList->header()->hide();
		mCreatorsList->setDropVisualizer(false);
	}
		
	mRight=new QWidget(mainSplitter);
	(new QHBoxLayout(mRight))->setAutoAdd(true);
	
	QStringList list=nex->renderers();
	for (QStringList::Iterator i=list.begin(); i!=list.end(); ++i)
		addCreator(*i);
		
	new TreeItem(mTree, 0, i18n("Main"));
}

void Control::save(const KURL &file)
{
// TODO
}

void Control::save()
{
	if (mCurrentURL.isEmpty())
		saveAs();
	else
		save(mCurrentURL);
}

void Control::saveAs()
{
	KURL url;
	url=KFileDialog::getSaveURL(0, "application/x-nexscope", this);
	if (!url.isEmpty())
		save(mCurrentURL=url);
}

void Control::open()
{
	KURL url=KFileDialog::getOpenURL(0, "application/x-nexscope");
	if (!url.isEmpty())
		open(mCurrentURL=url);
}

void Control::open(const KURL &file)
{
// TODO
}


void Control::addCreator(const QString &title)
{
	new CreatorItem(mCreatorsList, title);
}

void Control::dropEvent(QDropEvent *e, QListViewItem *parent, QListViewItem *pafter)
{
//	if ((e->source() == mCreatorsList) && parent)
	{
		CreatorItem *i=static_cast<CreatorItem*>(mCreatorsList->currentItem());
		if (!i)
		{
			std::cerr << "no creatoritem" << std::endl;

		}
		new TreeItem(static_cast<TreeItem*>(parent), static_cast<TreeItem*>(pafter), i);
	}
}

void Control::currentChanged(QListViewItem *item)
{
	TreeItem *treeItem=static_cast<TreeItem*>(item);
	delete mConfigurator;
	mConfigurator=treeItem->renderer()->configure(mRight);
	if (mConfigurator)
		mConfigurator->show();
}


RendererListView::RendererListView(QWidget *p) : KListView(p)
{
	addColumn(i18n("Name"));
	header()->hide();
	setDragEnabled(true);
	setAcceptDrops(true);
	setSelectionMode(QListView::Single);
}
	
bool RendererListView::acceptDrag(QDropEvent *event) const
{
	return true; //QCString(event->format()) == "application/x-nex-rendererdrag";
}

QDragObject *RendererListView::dragObject() const
{
	if (!currentItem()) return 0;
	return new QStoredDrag("application/x-nex-rendererdrag", (QWidget*)this);
}


#include "gui.moc"
