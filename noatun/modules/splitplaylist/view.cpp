/**
 * Copyright (c) 2000-2004 Charles Samuels <charles@kde.org>
 *               2000-2001 Neil Stevens <neil@qualityassistant.com>
 *
 * Copyright (c) from the patches of:
 *               2001 Klas Kalass <klas.kalass@gmx.de>
 *               2001 Anno v. Heimburg <doktor.dos@gmx.de>
 **/


// Abandon All Hope, Ye Who Enter Here

#include <qdragobject.h>
#include <qheader.h>
#include <qlayout.h>
#include <qmap.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qpainter.h>

#include <kaction.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmenubar.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <kedittoolbar.h>
#include <kurldrag.h>
#include <kmessagebox.h>

#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/playlistsaver.h>

#include "playlist.h"
#include "view.h"
#include "find.h"

#define SPL SplitPlaylist::SPL()

SafeListViewItem::SafeListViewItem(QListView *parent, QListViewItem *after, const KURL &text)
	: QCheckListItem(parent,0, QCheckListItem::CheckBox), PlaylistItemData(), removed(false)
{
	addRef();
	setUrl(text);

	static_cast<KListView*>(parent)->moveItem(this, 0, after);
	setOn(true);

	// is this really needed, it makes the listview too wide for me :(
//	setText(0,text.filename());

	//	if (!isDownloaded()) setText(1, "0%");

//	mProperties.setAutoDelete(true);

	if (!streamable() && enqueue(url()))
		setUrl(KURL(localFilename()));

	PlaylistItemData::added();
}

SafeListViewItem::SafeListViewItem(QListView *parent, QListViewItem *after, const QMap<QString,QString> &props)
	: QCheckListItem(parent, 0, QCheckListItem::CheckBox), removed(false)
{
	addRef();

	setOn(true);

	// A version of setProperty that assumes a key is unique,
	// and doesn't call modified for every new key.
	// Ugly, but this function is a very hot path on playlist loading
	for (QMap<QString,QString>::ConstIterator i=props.begin(); i!=props.end(); ++i )
	{
		QString n = i.key();
		QString val = i.data();

		if (n=="enabled")
		{
			setOn(val!="false" && val!="0");
		}
		else
		{
			Property p={n,val};
			mProperties += p;
		}
	}

	static_cast<KListView*>(parent)->moveItem(this, 0, after);
	modified();

	if (!streamable() && enqueue(url()))
	{
		KURL u;
		u.setPath(localFilename());

		setUrl(u);
	}
	PlaylistItemData::added();
}

SafeListViewItem::~SafeListViewItem()
{
	remove();
}

QString SafeListViewItem::file() const
{
	return localFilename();
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

int SafeListViewItem::compare(QListViewItem * i, int col, bool) const
{
	QString text1 = text(col);
	QString text2 = i->text(col);

	pad(text1);
	pad(text2);
	return text1.compare(text2);
}

QString SafeListViewItem::property(const QString &n, const QString &def) const
{
	for (QValueList<Property>::ConstIterator i=mProperties.begin(); i != mProperties.end(); ++i)
	{
		if ((*i).key==n)
			return (*i).value;
	}
	if (n=="enabled")
	{
		if (isOn()) return "true";
		else return "false";
	}
	return def;
}

void SafeListViewItem::setProperty(const QString &n, const QString &val)
{
	if (n=="enabled")
	{
		setOn(val!="false" && val!="0");
	}
	else
	{
		if ( property(n,"") == val )
		{
//			kdDebug(66666) << "SafeListViewItem::setProperty(), property unchanged!" << endl;
			return;
		}

		clearProperty(n);
		Property p={n,val};
		mProperties += p;
	}
	modified();
}

void SafeListViewItem::clearProperty(const QString &n)
{
	if (n=="enabled")
	{
		setOn(true);
		modified();
		return;
	}

	for (QValueList<Property>::Iterator i=mProperties.begin(); i != mProperties.end(); ++i)
	{
		if ((*i).key==n)
		{
			mProperties.remove(i);
			modified();
			break;
		}
	}
}

QStringList SafeListViewItem::properties() const
{
	QStringList list;
	for (QValueList<Property>::ConstIterator i=mProperties.begin(); i != mProperties.end(); ++i)
		list += (*i).key;
	list += "enabled";
	return list;
}

bool SafeListViewItem::isProperty(const QString &n) const
{
	for (QValueList<Property>::ConstIterator i=mProperties.begin(); i != mProperties.end(); ++i)
	{
		if ((*i).key==n)
			return true;
	}
	return n=="enabled";
}

void SafeListViewItem::downloaded(int percent)
{
	if (!removed)
		setText(1, QString::number(percent)+'%');
}

void SafeListViewItem::downloadTimeout()
{
	if (!removed)
		setText(1, "-");
}

void SafeListViewItem::downloadFinished()
{
	if (!removed)
		setText(1, "");
}

void SafeListViewItem::modified()
{
	bool widthChangeNeeded = false;

	if (text(0)!=title())
	{
		setText(0, title());
		widthChangeNeeded = true;
	}

	if (isDownloaded() && length()!=-1 && text(1)!=lengthString())
	{
		setText(1, lengthString());
		widthChangeNeeded = true;
	}

	if (widthChangeNeeded)
		widthChanged(-1);

	PlaylistItemData::modified();
}

void SafeListViewItem::stateChange(bool s)
{
	// if you uncheck this, uncheck thet others that
	// are selected too

	QPtrList<QListViewItem> list=SPL->view->listView()->selectedItems();

	// but not if I'm not selected
	if (list.containsRef(this))
		for (QListViewItem *i=list.first(); i != 0; i=list.next())
			static_cast<QCheckListItem*>(i)->setOn(s);
	else
		QCheckListItem::stateChange(s);
}

void SafeListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
	QCheckListItem::paintCell(p, cg, column, width, align);

	if (SPL->current() == this)
	{
		p->save();
		p->setRasterOp(XorROP);
		p->fillRect(0, 0, width, height(), QColor(255,255,255));

		p->restore();
	}
}

void SafeListViewItem::remove()
{
	removed=true;
	if (napp->player()->current()==this && !itemAbove() && !itemBelow())
	{
		napp->player()->stop();
		SPL->setCurrent(0);
	}
	else if (napp->player()->current()==this)
	{
//		SPL->setCurrent(0);
//		napp->player()->playCurrent();

		if (napp->player()->isPlaying() && !SPL->exiting())
			napp->player()->forward();
		else
			SPL->setCurrent(0);
	}

	if (listView())
	{
		if (SPL->currentItem==this) // just optimizing for least unreadably
			SPL->setCurrent(static_cast<SafeListViewItem*>(itemBelow()));

		listView()->takeItem(this);
	}
	else if (SPL->currentItem==this)
	{
		SPL->setCurrent(0);
	}

	dequeue();
	PlaylistItemData::removed();
}

List::List(View *parent)
	: KListView(parent), recursiveAddAfter(0), listJob(0)
{
	addColumn(i18n("File"));
	addColumn(i18n("Time"));
	setAcceptDrops(true);
	setSorting(-1);
	setDropVisualizer(true);
	setDragEnabled(true);
	setItemsMovable(true);
	setSelectionMode(QListView::Extended);
	connect(this, SIGNAL(dropped(QDropEvent*, QListViewItem*)), SLOT(dropEvent(QDropEvent*, QListViewItem*)));
	connect(this, SIGNAL(moved()), SLOT(move()));
	connect(this, SIGNAL(aboutToMove()), parent, SLOT(setNoSorting()));
	connect(this, SIGNAL(deleteCurrentItem()), parent, SLOT(deleteSelected()));
}

List::~List()
{
}

void List::move()
{
	emit modified();
}

bool List::acceptDrag(QDropEvent *event) const
{
	return KURLDrag::canDecode(event) || KListView::acceptDrag(event);
}

void List::dropEvent(QDropEvent *event, QListViewItem *after)
{
	static_cast<View*>(parent())->setNoSorting();
	KURL::List textlist;
	if (!KURLDrag::decode(event, textlist)) return;
	event->acceptAction();

	for (KURL::List::Iterator i=textlist.begin(); i != textlist.end(); ++i)
	{
		after= addFile(*i, false, after);
	}

	emit modified();
}

void List::keyPressEvent(QKeyEvent *e)
{
	if (e->key()==Key_Enter || e->key()==Key_Return)
	{
		if (currentItem())
		{
			emit KListView::executed(currentItem());
		}

		return;
	}

	if (e->key()==Key_Delete)
	{
		if (currentItem())
		{
			emit deleteCurrentItem();
		}

		return;
	}

	KListView::keyPressEvent(e);


}

/**
 * use this only once!!!
 **/
class NoatunSaver : public PlaylistSaver
{
	List *mList;
	SafeListViewItem *after, *mFirst;
public:
	NoatunSaver(List *l, QListViewItem *after=0)
		: mList(l)
	{
		this->after = static_cast<SafeListViewItem*>(after);
		mFirst = 0;
	}

	QListViewItem *getAfter() { return after; }
	QListViewItem *getFirst() { return mFirst; }

protected:
	virtual void readItem(const QMap<QString,QString> &properties)
	{
		after = new SafeListViewItem(mList, after, properties);
		if (mFirst==0)
			mFirst = after;
	}

	virtual PlaylistItem writeItem()
	{
		if (!after)
		{
			after=static_cast<SafeListViewItem*>(mList->firstChild());
		}
		else
		{
			after=static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(after)->nextSibling());
		}
		return PlaylistItem(after);
	}
};


bool View::saveToURL(const KURL &url)
{
	NoatunSaver saver(list);
	if(saver.save(url))
	{
		return true;
	}
	else
	{
		KMessageBox::error( this, i18n("Could not write to %1.").arg(url.prettyURL()) );
		return false;
	}
}

void View::exportTo(const KURL &url)
{
	QString local(napp->tempSaveName(url.path()));
	QFile saver(local);
	saver.open(IO_ReadWrite | IO_Truncate);
	QTextStream t(&saver);
	// navigate the list
	for (SafeListViewItem *i=static_cast<SafeListViewItem*>(listView()->firstChild());
	     i != 0; i=static_cast<SafeListViewItem*>(i->itemBelow()))
	{
		KURL u=i->url();
		if (u.isLocalFile())
			t<< u.path() << '\n';
		else
			t << u.url() << '\n';
	}
	saver.close();

	KIO::NetAccess::upload(local, url, this);

	saver.remove();
}

QListViewItem *List::openGlobal(const KURL &u, QListViewItem *after)
{
	clear();
	NoatunSaver saver(this, after);
	saver.metalist(u);

	return saver.getAfter();
}

// for m3u files
QListViewItem *List::importGlobal(const KURL &u, QListViewItem *after)
{
	NoatunSaver saver(this, after);
	if (!saver.metalist(u))
	{
		after=new SafeListViewItem(this, after, u);
//		SPL->listItemSelected(after);
		return after;
	}

	// return the first item added from this playlist
	// that way noatun can start playing the first item
	if (saver.getFirst())
		return saver.getFirst();

	// failsafe in case nothing was added, getFirst() may return 0
	return saver.getAfter();
}

QListViewItem *List::addFile(const KURL& url, bool play, QListViewItem *after)
{
	// when a new item is added, we don't want to sort anymore
	SPL->view->setNoSorting();

	if (
			url.path().right(4).lower()==".m3u"
			|| url.path().right(4).lower()==".pls"
			|| url.protocol().lower()=="http"
		)
	{
		// a playlist is requested
		QListViewItem *i = importGlobal(url, after);
		if (play)
			SPL->listItemSelected(i);
		return i;
	}
	else
	{
		if (!after) after=lastItem();
		KFileItem fileItem(KFileItem::Unknown,KFileItem::Unknown,url);
		if (fileItem.isDir())
		{
			addDirectoryRecursive(url, after);
			return after; // don't (and can't) know better!?
		}
		else
		{
			QListViewItem *i = new SafeListViewItem(this, after, url);
			if (play)
				SPL->listItemSelected(i);
			return i;
		}
	}
}

// starts a new listJob if there is no active but work to do
void List::addNextPendingDirectory()
{
	KURL::List::Iterator pendingIt= pendingAddDirectories.begin();
	if (!listJob &&	(pendingIt!= pendingAddDirectories.end()))
	{
		currentJobURL= *pendingIt;
		listJob= KIO::listRecursive(currentJobURL, false,false);
		connect(
				listJob, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
				SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&))
			);
		connect(
				listJob, SIGNAL(result(KIO::Job *)),
				SLOT(slotResult(KIO::Job *))
			);
		connect(
				listJob, SIGNAL(redirection(KIO::Job *, const KURL &)),
				SLOT(slotRedirection(KIO::Job *, const KURL &))
			);
		pendingAddDirectories.remove(pendingIt);
	}
}

void List::addDirectoryRecursive(const KURL &dir, QListViewItem *after)
{
	if (!after) after=lastItem();
	recursiveAddAfter= after;
	pendingAddDirectories.append(dir);
	addNextPendingDirectory();
}

void List::slotResult(KIO::Job *job)
{
	listJob= 0;
	if (job && job->error())
		job->showErrorDialog();
	addNextPendingDirectory();
}

void List::slotEntries(KIO::Job *, const KIO::UDSEntryList &entries)
{
	QMap<QString,KURL> __list; // temp list to sort entries

	KIO::UDSEntryListConstIterator it = entries.begin();
	KIO::UDSEntryListConstIterator end = entries.end();

	for (; it != end; ++it)
	{
		KFileItem file(*it, currentJobURL, false /* no mimetype detection */, true);
		// "prudhomm:
		// insert the path + url in the map to sort automatically by path
		// note also that you use audiocd to rip your CDs then it will be sorted the right way
		// now it is an easy fix to have a nice sort BUT it is not the best
		// we should sort based on the tracknumber"
		// - copied over from old kdirlister hack <hans_meine@gmx.de>
		if (!file.isDir())
			__list.insert(file.url().path(), file.url());
	}
	QMap<QString,KURL>::Iterator __it;
	for( __it = __list.begin(); __it != __list.end(); ++__it )
	{
		recursiveAddAfter= addFile(__it.data(), false, recursiveAddAfter);
	}
}

void List::slotRedirection(KIO::Job *, const KURL & url)
{
	currentJobURL= url;
}

/////////////////////////////////

View::View(SplitPlaylist *)
	: KMainWindow(0, "NoatunSplitplaylistView")
{
	list=new List(this);
	setCentralWidget(list);
	connect(list, SIGNAL(modified(void)), this, SLOT(setModified(void)) );
	// connect the click on the header with sorting
	connect(list->header(),SIGNAL(clicked(int)),this,SLOT(headerClicked(int)) );

	mOpen=new KAction(i18n("Add &Files..."), "queue", 0, this, SLOT(addFiles()), actionCollection(), "add_files");
	(void) new KAction(i18n("Add Fol&ders..."), "folder", 0, this, SLOT(addDirectory()),  actionCollection(), "add_dir");
	mDelete=new KAction(i18n("Delete"), "editdelete", Key_Delete, this, SLOT(deleteSelected()),  actionCollection(), "delete");

	mClose=KStdAction::close(this, SLOT(close()), actionCollection());
	mFind=KStdAction::find(this, SLOT(find()), actionCollection());

	(void) KStdAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());
	mOpenNew=KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	mOpenpl=KStdAction::open(this, SLOT(open()), actionCollection());
	mSave=KStdAction::save(this, SLOT(save()), actionCollection());
	mSaveAs=KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());

	(void) new KAction(i18n("Shuffle"), "misc", 0, SPL, SLOT( randomize() ), actionCollection(), "shuffle");
	(void) new KAction(i18n("Clear"), "editclear", 0, list, SLOT( clear() ), actionCollection(), "clear");

	createGUI("splui.rc");

	mFinder = new Finder(this);

	applyMainWindowSettings(KGlobal::config(), "SPL Window");
	list->setFocus();
}

void View::find()
{
	mFinder->show();
	connect(mFinder, SIGNAL(search(Finder*)), SLOT(findIt(Finder*)));
}

static bool testWord(QListViewItem *i, const QString &finder)
{
	PlaylistItemData *item=static_cast<SafeListViewItem*>(i);
	if (item->title().find(finder, 0, false) >=0)
		return true;
	if (item->file().find(finder, 0, false) >=0)
		return true;
	if (item->url().path().find(finder.local8Bit(), 0, false) >=0)
		return true;
	if (item->lengthString().find(finder, 0, false) >=0)
		return true;
	if (item->mimetype().find(finder.local8Bit(), 0, false) >=0)
		return true;
	return false;
}

static bool testWord(QListViewItem *i, const QRegExp &finder)
{
	PlaylistItemData *item=static_cast<SafeListViewItem*>(i);
	if (item->title().find(finder) >=0)
		return true;
	if (item->file().find(finder) >=0)
		return true;
	if (item->url().path().find(finder) >=0)
		return true;
	if (item->lengthString().find(finder) >=0)
		return true;
	if (item->mimetype().find(finder) >=0)
		return true;
	return false;
}

void View::findIt(Finder *f)
{
	QListViewItem *search=list->currentItem();

	if (list->currentItem())
	{
		if (f->isForward())
			search=list->currentItem()->itemBelow();
		else
			search=list->currentItem()->itemAbove();
	}
	else
	{
		if (f->isForward())
			search=list->firstChild();
		else
			search=list->lastChild();
	}


	while (search)
	{
		if (f->regexp())
		{
			if (testWord(search, QRegExp(f->string(), false)))
				break;
		}
		else
		{
			if (testWord(search, f->string()))
				break;
		}

		if (f->isForward())
			search=search->itemBelow();
		else
			search=search->itemAbove();

		if (!search)
		{
			if (f->isForward())
			{
				if (KMessageBox::questionYesNo(this, i18n("End of playlist reached. Continue searching from beginning?"),QString::null,KStdGuiItem::cont(),KStdGuiItem::cancel()) == KMessageBox::Yes)
					search=list->firstChild();
			}
			else
			{
				if (KMessageBox::questionYesNo(this, i18n("Beginning of playlist reached. Continue searching from end?"),QString::null,KStdGuiItem::cont(),KStdGuiItem::cancel()) == KMessageBox::Yes)
					search=list->lastChild();
			}
		}
	}

	if (search)
	{
		{ // select none
			QPtrList<QListViewItem> sel=list->selectedItems();
			for (QListViewItem *i=sel.first(); i!=0; i=sel.next())
				list->setSelected(i, false);
		}
		list->setSelected(search, true);
		list->setCurrentItem(search);
		list->ensureItemVisible(search);
	}
}

View::~View()
{
	napp->player()->stop();
	hide();
	saveState();
	delete list;
}

void View::init()
{
	// see if we are importing an old-style list
	bool importing= ! QFile(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylist.xml").exists();

	if (importing)
	{
		KURL internalURL;
		internalURL.setPath(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylistdata");
		NoatunSaver saver(list, 0);
		saver.load(internalURL, PlaylistSaver::M3U);
	}
	else
	{
		KURL internalURL;
		internalURL.setPath(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylist.xml");
		list->openGlobal(internalURL);
	}

	KConfig &config = *KGlobal::config();
	config.setGroup("splitplaylist");

	// this has to come after openGlobal, since openGlobal emits modified()
	setModified(config.readBoolEntry("modified", false));
	QString path = config.readPathEntry("file");
	// don't call setPath with an empty path, that would make the url "valid"
	if ( !path.isEmpty() )
		mPlaylistFile.setPath(path);

	SPL->reset();
	int saved = config.readNumEntry("current", 0);

	PlaylistItem item=SPL->getFirst();
	for(int i = 0 ; i < saved ; i++)
	{
		item=SPL->getAfter(item);
	}
	if (item)
		SPL->setCurrent(item);
}

void View::save()
{
	if(mPlaylistFile.isEmpty() || !mPlaylistFile.isValid())
	{
		saveAs();
		return;
	}

	if(saveToURL(mPlaylistFile))
		setModified(false);
}

void View::saveAs()
{
	KURL u=KFileDialog::getSaveURL(0, "*.xml splitplaylistdata *.pls *.m3u\n*", this, i18n("Save Playlist"));
	if(!u.isValid())
		return;
	mPlaylistFile = u;
	save();
}

void View::open()
{
	KURL u=KFileDialog::getOpenURL(0, "*.xml splitplaylistdata *.pls *.m3u\n*", this, i18n("Open Playlist"));
	if(!u.isValid())
		return;
	mPlaylistFile = u;
	list->openGlobal(u);
	setModified(false);
}

void View::openNew()
{
	mPlaylistFile = "";
	listView()->clear();
}

void List::clear()
{
	SPL->setCurrent(0);

	QListView::clear();
}

void View::deleteSelected()
{
	QPtrList<QListViewItem> items(list->selectedItems());

	bool stopped=false;
	// noatun shouldn't play files for now
	QListViewItem *afterLast=0;

	for (QPtrListIterator<QListViewItem> it(items); it.current(); ++it)
	{
		SafeListViewItem *i = static_cast<SafeListViewItem*>(*it);
		if (!stopped && SPL->current() == i)
		{
			napp->player()->stop();
			SPL->setCurrent(0);
			stopped = true;
		}
		i->remove();

		afterLast = i->itemBelow();
	}

	if (stopped)
		SPL->setCurrent(static_cast<SafeListViewItem*>(afterLast));

	setModified(true);
}

void View::addFiles()
{
	KURL::List files=KFileDialog::getOpenURLs(":mediadir", napp->mimeTypes(), this, i18n("Select File to Play"));

	QListViewItem *last = list->lastItem();
	for(KURL::List::Iterator it=files.begin(); it!=files.end(); ++it)
		last = addFile(KURL(*it), false);

	setModified(true);
}

void View::addDirectory()
{
	QString file=KFileDialog::getExistingDirectory(0, this, i18n("Select Folder"));

	if (!file) return;
	KURL url;
	url.setPath(file);
	list->addDirectoryRecursive(url);

	setModified(true);
}

void View::closeEvent(QCloseEvent*)
{
	hide();
}

void View::showEvent(QShowEvent *)
{
	emit shown();
}

void View::hideEvent(QHideEvent *)
{
	emit hidden();
}

void View::setModified(bool b)
{
	modified = b;
	setCaption(i18n("Playlist"), modified);
}

void View::setModified(void)
{
	setModified(true);
}

void View::saveState(void)
{
	KConfig &config = *KGlobal::config();
	config.setGroup("splitplaylist");

	config.writeEntry("modified", modified);
	config.writePathEntry("file", mPlaylistFile.path());
	saveToURL(napp->dirs()->saveLocation("data", "noatun/") + "splitplaylist.xml");

	unsigned int i;
	PlaylistItem item=SPL->getFirst();
	for(i = 0; item && item != SPL->current(); )
		item=SPL->getAfter(item), i++;

	config.writeEntry("current", i);
	saveMainWindowSettings(KGlobal::config(), "SPL Window");

	config.sync();
}

void View::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "SPL Window");
	KEditToolbar dlg(actionCollection(), "splui.rc");
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void View::newToolBarConfig()
{
	createGUI("splui.rc");
	applyMainWindowSettings(KGlobal::config(), "SPL Window");
}

// turns the sorting on or off
void View::setSorting(bool on, int column)
{
	if (on)
	{
		list->setSorting(column,true);
		list->setShowSortIndicator(true);
	}
	else
	{
		list->setShowSortIndicator(false);
		list->setSorting(-1);
	}
}

void View::headerClicked(int column)
{
	// this is to avoid that if we already have it sorted,
	// we sort it again ascendingly this way, clicking on
	// the header a second time will correctly toggle
	// ascending/descending sort
	if (list->showSortIndicator())
	{
		return;
	}
	else
	{
		setSorting(true,column);
	}
}

#include "view.moc"

