#ifndef VIEW_H
#define VIEW_H

#include <qevent.h>
#include <qptrlist.h>
#include <klistview.h>
#include <kmainwindow.h>
#include <qrect.h>
#include <qdict.h>
#include <kio/global.h>
#include <noatun/downloader.h>

class Finder;
class View;
namespace KIO { class ListJob; }


class SafeListViewItem
	: public QCheckListItem
	, public PlaylistItemData
	, public DownloadItem
{
public:
	SafeListViewItem(QListView *parent, QListViewItem *after, const KURL &text);
	SafeListViewItem(QListView *parent, QListViewItem *after, const QMap<QString,QString> &properties);
	virtual ~SafeListViewItem();

	virtual QString property(const QString &, const QString & = 0) const;
	virtual void setProperty(const QString &, const QString &);
	virtual void clearProperty(const QString &);
	virtual QStringList properties() const;
	virtual bool isProperty(const QString &) const;

	virtual QString file() const;

	int compare(QListViewItem * i, int col, bool ascending) const;
	virtual void remove();

protected:
	virtual void downloaded(int percent);
	virtual void downloadTimeout();
	virtual void downloadFinished();
	virtual void modified();
	virtual void stateChange(bool s);

	void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);

private:
	struct Property
	{
		QString key;
		QString value;
	};
	QValueList<Property> mProperties;
	bool removed;
};

class List : public KListView
{
Q_OBJECT
friend class View;
public:
	List(View *parent);
	virtual ~List();
	QListViewItem *openGlobal(const KURL&, QListViewItem * =0);
	QListViewItem *importGlobal(const KURL&, QListViewItem * =0);
	QListViewItem *addFile(const KURL&, bool play=false, QListViewItem * =0);
	void addDirectoryRecursive(const KURL &dir, QListViewItem *after= 0);

public slots:
	virtual void clear();

signals:
	void modified(void);
	void deleteCurrentItem();

protected:
	virtual bool acceptDrag(QDropEvent *event) const;
	virtual void keyPressEvent(QKeyEvent *e);

protected slots:
	virtual void dropEvent(QDropEvent *event, QListViewItem *after);
	void move();

protected:
	QListViewItem *recursiveAddAfter;

protected slots:
	// used when adding directories via KIO::listRecursive
	void slotResult(KIO::Job *job);
	void slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries);
	void slotRedirection(KIO::Job *, const KURL & url);

protected:
	void addNextPendingDirectory();
	KURL::List pendingAddDirectories;
	KIO::ListJob *listJob;
	KURL currentJobURL;
};

class KFileDialog;
class KToggleAction;
class KToolBar;

class View : public KMainWindow
{
Q_OBJECT
public:
	View(SplitPlaylist *mother);
	// load the SM playlist
	void init();
	virtual ~View();
	List *listView() const { return list; }
	QListViewItem *addFile(const KURL &u, bool play=false)
		{ return list->addFile(u, play, list->lastItem()); }


public slots:
	void deleteSelected();
	void addFiles();
	void addDirectory();
	void save();
	void saveAs();
	void open();
	void openNew();
	void setSorting(bool on, int column = 0);
	void setNoSorting() { setSorting(false); }
	void headerClicked(int column);void find();
	void findIt(Finder *);


private slots:
	void setModified();
	void saveState();

	void configureToolBars();
	void newToolBarConfig();

protected:
	void setupActions();

	bool saveToURL(const KURL &);
	void exportTo(const KURL &);

	void setModified(bool);
	virtual void closeEvent(QCloseEvent*e);
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

signals:
	void hidden();
	void shown();

private:
	List *list;
	KAction *mOpen, *mDelete, *mSave, *mSaveAs, *mOpenpl, *mOpenNew;
	KAction *mClose;
	KAction *mFind;
	Finder *mFinder;

	KURL mPlaylistFile;
	bool modified;
};

#endif
