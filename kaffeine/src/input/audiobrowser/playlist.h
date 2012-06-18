/*
 * playlist.h
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2005-2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <kdialogbase.h>
#include <kdiroperator.h>
#include <kfileiconview.h>
#include <kaction.h>
#include <kstdaction.h>

#include <qwidget.h>
#include <qptrlist.h>
#include <qvbox.h>
#include <qsplitter.h>
#include <qpainter.h>
#include <qtoolbutton.h>

#include "urllistview.h"
#include "kaffeineinput.h"

class UrlListView;
class MRL;
class KURL;
class QListViewItem;
class QString;
class QLabel;
class QColor;
class QDropEvent;
class QPixmap;
class KLineEdit;
class KComboBox;
class KURLComboBox;
class KConfig;
class KPushButton;
class GoogleFetcher;



class GalleryDialog : public KDialogBase
{
	Q_OBJECT
public:
	GalleryDialog( QWidget *parent, GoogleFetcher *goog );
protected slots:
	void slotOk();
	void slotNext();
private:
	QFrame *imageFrame;
	GoogleFetcher *google;
	QTimer next;
	QHBox *hbox;
};



class RollTitle : public QLabel
{
	Q_OBJECT

public:
	RollTitle( QWidget *parent );
	void setTitle( MRL mrl );

private:
	void setTitle( QString t );
	QTimer titleTimer;
	QPixmap titlePix;
	QColor back, fore;
	int titleOffset;
	QString title;

protected:
	void paintEvent ( QPaintEvent * );

private slots:
	void rollTitle();
};



class CoverFrame : public QFrame
{
	Q_OBJECT

public:
	CoverFrame( QWidget *parent );
	~CoverFrame();
	void setCoverPixmap( const QString &path );

protected:
	void mousePressEvent( QMouseEvent *e );

private:
	void popup( const QPixmap &pix ) const;
	QString imagePath;
	QPixmap noCoverPix;

signals:
	void changeCurrentCover();
	void gallery();
};



class PlayList : public KaffeineInput
{
	Q_OBJECT

public:
	PlayList(QWidget *parent, QObject *objParent, const char *name=0);
	~PlayList();

	// Reimplemented from KaffeineInput
public:
	QWidget *wantPlayerWindow();
	QWidget *inputMainWidget();
	void mergeMeta(const MRL&);
	bool nextTrack( MRL& );
	bool previousTrack( MRL& );
	bool currentTrack( MRL& );
	bool trackNumber( int, MRL& );
	bool playbackFinished( MRL& );
	void getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames );
	void togglePanel();
	bool execTarget( const QString& );
	void saveConfig();
	bool showPlayer() {return !m_showPlayer->isChecked();}
	//***************************************

public:
	MRL getCurrent();  /* get current playlist entry */
	MRL getNext();
	MRL getEntryWithNumber(int number);
	void setCurrentEntry(QListViewItem*, bool playIcon = true);

	QListViewItem* getLast();
	QListViewItem* getFirst();
	QListViewItem* findByURL(const QString&);

	/* insert a KURL(list) after a specific item */
	void add(const QString& url, QListViewItem* after);
	void add(const QStringList& urls, QListViewItem* after);
	void add(const QValueList<MRL>&, QListViewItem* after);

	bool isQueueMode() { return m_queue.count(); }

	void setPlaylist(const QString& name, bool clear = false);
	void nextPlaylist();
	void removeCurrentPlaylist();
	void saveCurrentPlaylist();
	QString queryCurrentPlaylist();
	void loadConfig(KConfig*);
	void saveConfig(KConfig*);
	void clearList();

	void setEndless(bool);
	void setRandom(bool);

	void useAlternateEncoding(bool);
	void setAlternateEncoding(const QString&);

	void setFileFilter(const QString& filter);
	void savePlaylist(const QString&);
	void loadPlaylist(const QString&);

	void exportM3UPlaylist(const QString&);
	void exportPLSPlaylist(const QString&);

	const bool getReadMetaOnLoading() const { return m_metaOnLoading; }

	QVBox *mainWidget;
	QVBox *playerBox;

public slots:
	void slotToggleShuffle();

protected:
	void closeEvent(QCloseEvent*);

private slots:
	void slotNewList();
	void slotSetReadMetaOnLoading(bool read) { m_metaOnLoading = read; }
	void slotSetAlternateColor(const QColor&);
	void slotPlayDirect(QListViewItem* item);  /* doubleclick */
	void slotDropEvent(QDropEvent*, QListViewItem*);
	void slotPreDropEvent();
	void slotCut();
	void slotPaste();
	void slotCopy();
	void slotSelectAll();
	void slotPlaylistFromSelected();
	void slotAddToQueue(MRL);
	void slotRemoveSelected();
	void slotFindText(const QString&);
	void slotSort(int);
	void slotPlaylistActivated(int);
	void slotNewPlaylistName(const QString&);
	void slotRepeat();
	void slotShuffle();
	void slotAutoCover();
	void slotShowPlayer();
	void slotPlaylistLoad();
	void slotPlaylistSaveAs();
	void slotPlaylistRemove();
	void setCover( QString s1, QString s2, bool forceFetch=false );
	void chooseCurrentCover();
	void showGallery();
	void startPlaylist();
	void fileSelected( const KFileItem* );
	void setBrowserURL( const QString& );
	void setBrowserURL( const KURL& );
	void browserURLChanged( const KURL& );
	void slotClearList();
	void resetSearch();

private:
	QListViewItem* insertItem(QListViewItem* after, const MRL&);
	void updateStatus();
	void createRandomList();
	void getMetaInfo(MRL& mrl, const QString& mimeName);
	void setupActions();
	/* helpers */
	static QString msToTimeString(int);
	static int timeStringToMs(const QString&);
	static bool endsWith(QString, QString, bool);
	static bool startsWith(QString, QString, bool);

private:
	QColor m_altCol;
	KComboBox* m_playlistSelector;
	QToolButton *searchBtn;
	KLineEdit* m_playlistFilter;
	int m_nextPlaylistNumber;
	QString m_playlistDirectory;
	int m_currentPlaylist;
	KURLComboBox *browserComb;

	QSplitter *hSplit, *vSplit;
	QVBox *panel;
	QWidget *playlist;
	KDirOperator *fileBrowser;
	KFileIconView *view;
	CoverFrame *coverFrame;
	GoogleFetcher *google;
	QString coverImageFormat;
	RollTitle *roller;

	KToggleAction *m_repeat, *m_shuffle, *m_autoCover, *m_showPlayer;

	QValueList<MRL> m_queue;

	uint m_playTime;
	uint m_playTimeVisible;
	uint m_countVisible;

	bool m_searchSelection;
	bool m_metaOnLoading;
	bool m_sortAscending;

	UrlListView* m_list;
	QListViewItem* m_currentEntry;
	MRL m_currentEntryMRL;

	QString m_fileFilter;
	QString m_metaInfoString;
	QString m_lastPlaylist;

	QPtrList<QListViewItem> m_randomList;
	int m_currentRandomListEntry;

	QPixmap m_isCurrentEntry;
	QPixmap m_cdPixmap;

	bool m_endless;
	bool m_random;
	bool m_cover;

	bool m_useAlternateEncoding;
	QString m_alternateEncoding;

signals:
	void signalRequestForAudioCD(const QString&);
	void signalRequestForDVD(const QString&);
	void signalRequestForVCD(const QString&);
};

class QListBox;
class QStringList;

class SubtitleChooser : public KDialogBase
{
	Q_OBJECT
public:
	SubtitleChooser(QStringList, QString, QWidget *parent=0, const char *name = 0);
	virtual ~SubtitleChooser();

	QString getSelection();

private:
	QListBox *table;
};

class MovieChooser : public KDialogBase
{
	Q_OBJECT
public:
	MovieChooser(QStringList, QString, QWidget *parent=0, const char *name = 0);
	virtual ~MovieChooser();

	QString getSelection();

private:
	QListBox *table;
};

#endif /* PLAYLIST_H */
