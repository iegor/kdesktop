/*
 * playlist.cpp
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

#include <krandomsequence.h>
#include <kapplication.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <kio/job.h>
#include <kprogress.h>
#include <kfilemetainfo.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <kurl.h>
#include <kaccel.h>
#include <dcopref.h>
#include <ktoolbar.h>
#include <kcolordialog.h>
#include <kurlcombobox.h>
#include <kurlcompletion.h>

#include <qlabel.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qxml.h>
#include <qregexp.h>
#include <qheader.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qvbox.h>
#include <qlistbox.h>
#include <qtextcodec.h>
#include <qtooltip.h>
#include <qinputdialog.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qpopupmenu.h>

#include "googlefetcher.h"
#include "mrl.h"
#include "playlistimport.h"
#include "playlistitem.h"
#include "playlist.h"
#include "playlist.moc"



RollTitle::RollTitle( QWidget *parent ) : QLabel( "I", parent )
{
	QColorGroup cg = parentWidget()->colorGroup();
	QColor base = cg.base();
	QColor selection = cg.highlight();
	int r = (base.red() + selection.red()) / 2;
	int b = (base.blue() + selection.blue()) / 2;
	int g = (base.green() + selection.green()) / 2;
	back = QColor(r, g, b);
	fore = parentWidget()->colorGroup().text();
	setPaletteBackgroundColor( back );
	setPaletteForegroundColor( fore );
	setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	title = " ##### ";
	titleOffset = 0;
	setTitle( title );
	connect( &titleTimer, SIGNAL( timeout() ), this, SLOT( rollTitle() ) );
	titleTimer.start( 50 );
}

void RollTitle::setTitle( MRL mrl )
{
	title = " ##### ";
	QString s1 = mrl.artist().stripWhiteSpace();
	QString s2 = mrl.album().stripWhiteSpace();
	QString s3 = mrl.title().stripWhiteSpace();
	if ( !s1.isEmpty() )
		title+= s1+" - ";
	if ( !s2.isEmpty() )
		title+= s2+" - ";
	if ( s3.isEmpty() )
		title+= mrl.url();
	else
		title+= s3;
	titleOffset = 0;
	setTitle( title );
}

void RollTitle::setTitle( QString t )
{
	QLabel *lab = new QLabel( t, this );
	lab->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	lab->resize( lab->sizeHint() );
	int x = 2*lab->frameWidth();
	titlePix = QPixmap( lab->width()-x, height()-x, -1, QPixmap::BestOptim );
	delete lab;
	QPainter p( &titlePix );
	p.fillRect( 0, 0, titlePix.width(), titlePix.height(), back );
	p.setPen( fore );
	p.setFont( font() );
	p.drawText( QRect(0,(titlePix.height()-QFontInfo(font()).pointSize())/2,titlePix.width(),QFontInfo(font()).pointSize()*2), Qt::AlignAuto|Qt::SingleLine, t );
	p.end();
}

void RollTitle::rollTitle()
{
	int w = 2*frameWidth();
	QPixmap pix( width()-w, height()-w, -1, QPixmap::BestOptim );
	QPainter p1( &pix );
	p1.fillRect( 0, 0, pix.width(), pix.height(), back );
	p1.end();
	int x = titlePix.width()-titleOffset;
	bitBlt( &pix, 0, 0, &titlePix, titleOffset, 0, x, titlePix.height(), CopyROP );
	while ( x<pix.width() ) {
		bitBlt( &pix, x, 0, &titlePix, 0, 0, titlePix.width(), titlePix.height(), CopyROP );
		x+= titlePix.width();
	}
	++titleOffset;
	if ( titleOffset>titlePix.width() )
		titleOffset = 0;
	bitBlt( this, w/2, w/2, &pix, 0, 0, width()-w, height()-w, CopyROP );
}

void RollTitle::paintEvent( QPaintEvent *pe )
{
	QLabel::paintEvent( pe );
	QColorGroup cg = parentWidget()->colorGroup();
	QColor base = cg.base();
	QColor selection = cg.highlight();
	int r = (base.red() + selection.red()) / 2;
	int b = (base.blue() + selection.blue()) / 2;
	int g = (base.green() + selection.green()) / 2;
	back=QColor(r,g,b);
	setPaletteBackgroundColor( back );
	fore = parentWidget()->colorGroup().text();
	setPaletteForegroundColor( fore );
	setTitle( title );
}



struct CoverPopup : public QWidget
{
    CoverPopup(const QPixmap &image, const QPoint &p) :
        QWidget(0, 0, WDestructiveClose | WX11BypassWM)
    {
        QHBoxLayout *layout = new QHBoxLayout(this);
        QLabel *label = new QLabel(this);

        layout->addWidget(label);
        label->setFrameStyle(QFrame::Box | QFrame::Raised);
        label->setLineWidth(1);
        label->setPixmap(image);

        setGeometry(p.x(), p.y(), label->width(), label->height());
        show();
    }
    virtual void leaveEvent(QEvent *) { close(); }
    virtual void mouseReleaseEvent(QMouseEvent *) { close(); }
};

CoverFrame::CoverFrame( QWidget *parent ) : QFrame( parent )
{
	imagePath = "";
	QString path = locate("appdata", "nocover.png");
	if ( !path )
		path = locate("data", "kaffeine/nocover.png");
	noCoverPix.convertFromImage( QImage(path) );
	setPaletteBackgroundPixmap( noCoverPix );
}

CoverFrame::~CoverFrame()
{
}

void CoverFrame::setCoverPixmap( const QString &path )
{
	QPixmap pix;
	QImage img;

	img = QImage( path );
	if ( !img.isNull() ) {
		img = img.smoothScale( 80, 80 );
		pix.convertFromImage( img );
		setPaletteBackgroundPixmap( pix );
		imagePath = path;
	}
	else {
		setPaletteBackgroundPixmap( noCoverPix );
		imagePath = "";
	}
}

void CoverFrame::popup( const QPixmap &pix ) const
{
    QPoint mouse  = QCursor::pos();
    QRect desktop = KApplication::desktop()->screenGeometry(mouse);
    int x = mouse.x();
    int y = mouse.y();
    int height = pix.height() + 4;
    int width  = pix.width() + 4;

    // Detect the right direction to pop up (always towards the center of the
    // screen), try to pop up with the mouse pointer 10 pixels into the image in
    // both directions.  If we're too close to the screen border for this margin,
    // show it at the screen edge, accounting for the four pixels (two on each
    // side) for the window border.

    if(x - desktop.x() < desktop.width() / 2)
        x = (x - desktop.x() < 10) ? desktop.x() : (x - 10);
    else
        x = (x - desktop.x() > desktop.width() - 10) ? desktop.width() - width +desktop.x() : (x - width + 10);

    if(y - desktop.y() < desktop.height() / 2)
        y = (y - desktop.y() < 10) ? desktop.y() : (y - 10);
    else
        y = (y - desktop.y() > desktop.height() - 10) ? desktop.height() - height + desktop.y() : (y - height + 10);

    new CoverPopup( pix, QPoint(x, y) );
}

void CoverFrame::mousePressEvent( QMouseEvent *e )
{
	int i;
	QPixmap pix;

	switch ( e->button() ) {
		case LeftButton: {
			if ( imagePath.isEmpty() )
				break;
			pix.convertFromImage( QImage( imagePath ) );
			if ( !pix.isNull() )
				popup( pix );
			break;
		}
		case RightButton: {
			QPopupMenu pop( this );
			pop.insertItem( i18n("Choose a Cover..."), 1 );
			pop.insertItem( i18n("Gallery..."), 2 );
			i = pop.exec( QCursor::pos() );
			if ( i==1 )
				emit changeCurrentCover();
			else if ( i==2 )
				emit gallery();
			break;
		}
		default:
			break;
	}
}


PlayList::PlayList( QWidget* parent, QObject *objParent, const char *name ) : KaffeineInput(objParent , name),
		m_playTime(0), m_playTimeVisible(0), m_countVisible(0), m_searchSelection(false),
		m_metaOnLoading(true), m_sortAscending(true), m_currentEntry(NULL), m_currentRandomListEntry(-1),
		m_endless(false), m_random(false), m_useAlternateEncoding(false), m_alternateEncoding("ISO 8859-1")
{
	google = NULL;

	mainWidget = new QVBox( parent );
	//mainWidget->setSizePolicy( QSizePolicy (QSizePolicy::Preferred, QSizePolicy::Preferred) );
	hSplit = new QSplitter( mainWidget );
	hSplit->setOpaqueResize( true );
	panel = new QVBox( hSplit );
	playlist = new QWidget( hSplit );
	hSplit->moveToFirst( panel );
	hSplit->setResizeMode( panel, QSplitter::KeepSize );

	vSplit = new QSplitter( QSplitter::Vertical, panel );
	vSplit->setOpaqueResize( true );
	QWidget *vb = new QWidget( vSplit );
	KToolBar *tb = new KToolBar( vb );
	QVBoxLayout *v = new QVBoxLayout( vb, 0, 0 );
	v->addWidget( tb );
	tb->setIconSize( 16 );

	browserComb = new KURLComboBox( KURLComboBox::Directories, true, vb );
	browserComb->setCompletionObject( new KURLCompletion( KURLCompletion::DirCompletion ) );
	browserComb->setAutoDeleteCompletionObject( true );
        browserComb->setMaxItems( 10 );
        browserComb->setFocusPolicy(QWidget::ClickFocus);


       /* if (!m_medium)
            m_combo->lineEdit()->setText( location->path() );
        else
            m_combo->lineEdit()->setText( "/" );*/

	QVBoxLayout *v1 = new QVBoxLayout( 0, 3, 3 );
	v1->addWidget( browserComb );
	fileBrowser = new KDirOperator( KURL("/"), vb );
	fileBrowser->setupMenu( KDirOperator::SortActions|KDirOperator::NavActions|KDirOperator::ViewActions );
	view = new KFileIconView( fileBrowser, "view" );
	view->setSelectionMode( KFile::Multi );
	fileBrowser->setMode( KFile::Files );
	fileBrowser->setView( view );
	fileBrowser->setViewConfig( KGlobal::config(), "PlaylistFileBrowser" );
	fileBrowser->setOnlyDoubleClickSelectsFiles( true );
	v1->addWidget( fileBrowser );
	v->addLayout( v1 );

	KActionCollection *col = fileBrowser->actionCollection();
	col->action( "up" )->plug( tb );
	col->action( "back" )->plug( tb );
	col->action( "forward" )->plug( tb );
	col->action( "reload" )->plug( tb );
	col->action( "home" )->plug( tb );

	vSplit->moveToFirst( vb );
	playerBox = new QVBox( vSplit );
	playerBox->setMinimumHeight( 50 );
	playerBox->setMinimumWidth( 200 );
	vSplit->moveToLast( playerBox );

	m_playlistDirectory = locateLocal("appdata", "playlists");
	m_playlistDirectory.append("/");
	KIO::NetAccess::mkdir(m_playlistDirectory, mainWidget);
	kdDebug() << "PLAYLIST" << endl;

	QGridLayout* layout = new QGridLayout( playlist, 4, 2, 3 );

	m_list = new UrlListView(playlist);
	mainWidget->setAcceptDrops(true);
	m_list->setHScrollBarMode(KListView::AlwaysOff);
	m_list->setItemMargin(2);
	m_list->setMargin(0);
	m_list->setSelectionMode(QListView::Extended);
	m_list->addColumn("");
	m_list->addColumn(i18n("Title"));
	m_list->addColumn(i18n("Artist"));
	m_list->addColumn(i18n("Album"));
	m_list->addColumn(i18n("Track"));
	m_list->addColumn(i18n("Length"));
	m_list->setShowToolTips(true);
	m_list->setColumnWidthMode(MIME_COLUMN, QListView::Manual);
	m_list->setColumnWidthMode(TITLE_COLUMN, QListView::Manual);
	m_list->setColumnWidthMode(ARTIST_COLUMN, QListView::Manual);
	m_list->setColumnWidthMode(ALBUM_COLUMN, QListView::Manual);
	m_list->setColumnWidthMode(TRACK_COLUMN, QListView::Manual);
	m_list->setColumnWidthMode(LENGTH_COLUMN, QListView::Manual);
	m_list->setResizeMode(QListView::NoColumn);
	KConfig* config = kapp->config();
	m_list->restoreLayout(config, "Playlist Layout");
	m_list->setFullWidth(true);
	m_list->setSorting(-1);
	m_list->setDragEnabled(true);
	m_list->setAcceptDrops(true);
	m_list->setDropVisualizer(true);
	m_list->setItemsMovable(true);
	m_list->setItemsRenameable(true);
	m_list->setRenameable(TITLE_COLUMN);
	m_list->setRenameable(ARTIST_COLUMN);
	m_list->setRenameable(ALBUM_COLUMN);
	m_list->setRenameable(TRACK_COLUMN);
	m_list->setAllColumnsShowFocus(true);
	m_list->setShowSortIndicator(true);
	layout->addMultiCellWidget(m_list, 3, 3, 0, 1);

	coverFrame = new CoverFrame( playlist );
	coverFrame->setFixedWidth( 80 );
	coverFrame->setFixedHeight( 80 );
	layout->addMultiCellWidget(coverFrame, 0, 2, 0, 0);

	QHBoxLayout *h1 = new QHBoxLayout();
	searchBtn = new QToolButton( playlist );
	searchBtn->setAutoRaise( true );
	QToolTip::add( searchBtn, i18n("Search"));
	searchBtn->setIconSet( KGlobal::iconLoader()->loadIconSet("locationbar_erase", KIcon::Small) );
	h1->addWidget( searchBtn );
	QLabel* filterLabel = new QLabel(i18n("Filter") + ":", playlist);
	h1->addWidget(filterLabel);
	m_playlistFilter = new KLineEdit(playlist);
	m_playlistFilter->setFocusPolicy(QWidget::ClickFocus);
	h1->addWidget(m_playlistFilter);
	layout->addLayout( h1, 2, 1 );

	QHBoxLayout *h2 = new QHBoxLayout();
	QLabel* playlistLabel = new QLabel(i18n("Playlist:"), playlist);
	h2->addWidget(playlistLabel);
	m_playlistSelector = new KComboBox( playlist );
	m_playlistSelector->setMinimumWidth(10);
	m_playlistSelector->setSizePolicy( QSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::Preferred) );
	m_playlistSelector->setEditable(true);
	m_playlistSelector->setDuplicatesEnabled(false);
	m_playlistSelector->setFocusPolicy(QWidget::ClickFocus);
	QToolTip::add(m_playlistSelector, i18n("Select the active playlist. To change playlist name edit it and confirm with 'Return'."));
	h2->addWidget(m_playlistSelector);
	layout->addLayout( h2, 1, 1 );

	roller = new RollTitle( playlist );
	layout->addWidget( roller, 0, 1 );

	KAccel* accel = new KAccel(mainWidget);
	accel->insert("Delete selected", Qt::Key_Delete, this, SLOT(slotRemoveSelected()));

	m_isCurrentEntry = UserIcon("playing");
	m_cdPixmap = KGlobal::iconLoader()->loadIcon("cdtrack", KIcon::Small);

	setXMLFile("kaffeineplaylist.rc");
	setupActions();

	QStrList formats = QImageIO::outputFormats();
	coverImageFormat = "PNG";
	for (int i=0; i<(int)formats.count(); i++ ) {
		if ( QString(formats.at(i))=="JPEG" ) {
			coverImageFormat = "JPEG";
			break;
		}
	}

	loadConfig( KGlobal::config() );

	connect(coverFrame, SIGNAL(changeCurrentCover()), this, SLOT(chooseCurrentCover()));
	connect(coverFrame, SIGNAL(gallery()), this, SLOT(showGallery()));
	connect(browserComb, SIGNAL(urlActivated( const KURL& )), this, SLOT(setBrowserURL( const KURL& )));
	connect(browserComb, SIGNAL(returnPressed( const QString& )), this, SLOT(setBrowserURL( const QString& )));
	connect(fileBrowser, SIGNAL(urlEntered( const KURL& )), this, SLOT(browserURLChanged( const KURL& )));
	connect(fileBrowser, SIGNAL(fileSelected(const KFileItem*)), this, SLOT(fileSelected(const KFileItem*)));
	connect(m_playlistFilter, SIGNAL(textChanged(const QString&)), this, SLOT(slotFindText(const QString&)));
	connect( searchBtn, SIGNAL(clicked()), this, SLOT(resetSearch()) );
	connect(m_playlistSelector, SIGNAL(returnPressed(const QString&)), this, SLOT(slotNewPlaylistName(const QString&)));
	connect(m_playlistSelector, SIGNAL(activated(int)),this, SLOT(slotPlaylistActivated(int)));
	connect(m_list, SIGNAL(dropped(QDropEvent*, QListViewItem*)), this, SLOT(slotDropEvent(QDropEvent*, QListViewItem*)));
	connect(m_list, SIGNAL(aboutToMove()), this, SLOT(slotPreDropEvent()));
	connect(m_list, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(slotPlayDirect(QListViewItem*)));
	connect(m_list, SIGNAL(returnPressed(QListViewItem*)), this, SLOT(slotPlayDirect(QListViewItem*)));
	connect(m_list, SIGNAL(signalCut()), this, SLOT(slotCut()));
	connect(m_list, SIGNAL(signalCopy()), this, SLOT(slotCopy()));
	connect(m_list, SIGNAL(signalPaste()), this, SLOT(slotPaste()));
	connect(m_list, SIGNAL(signalSelectAll()), this, SLOT(slotSelectAll()));
	connect(m_list, SIGNAL(signalPlayItem(QListViewItem*)), this, SLOT(slotPlayDirect(QListViewItem*)));
	connect(m_list, SIGNAL(signalPlaylistFromSelected()), this, SLOT(slotPlaylistFromSelected()));
	connect(m_list, SIGNAL(signalAddToQueue(MRL)), this, SLOT(slotAddToQueue(MRL)));
	connect(m_list->header(), SIGNAL(clicked(int)), this, SLOT(slotSort(int)));
}

void PlayList::togglePanel()
{
	if ( panel->isHidden() )
		panel->show();
	else
		panel->hide();
}

void PlayList::getTargets( QStringList &uiNames, QStringList &iconNames, QStringList &targetNames )
{
	uiNames.append( i18n("Play Playlist") );
	iconNames.append( "view_text" );
	targetNames.append( "PLAYLIST" );
}

bool PlayList::execTarget( const QString &target )
{
	if ( target=="PLAYLIST" ) {
		emit showMe( this );
		QTimer::singleShot( 100, this, SLOT(startPlaylist()) );
		return true;
	}
	return false;
}

void PlayList::setFileFilter( const QString& filter )
{
	m_fileFilter = filter;
	fileBrowser->setNameFilter( m_fileFilter );
}

PlayList::~PlayList()
{
	delete m_list;
}

void PlayList::setupActions()
{
	m_repeat = new KToggleAction(i18n("&Repeat"), 0 , CTRL|Key_E, this, SLOT(slotRepeat()), actionCollection(),"playlist_repeat");
	m_repeat->setStatusText(i18n("Loop playlist"));
	m_shuffle = new KToggleAction(i18n("Sh&uffle"), 0 , CTRL|Key_R, this, SLOT(slotShuffle()), actionCollection(),"playlist_shuffle");
	m_shuffle->setStatusText(i18n("Play items in random order"));
	m_autoCover = new KToggleAction(i18n("Autodownload covers"), 0 , 0, this, SLOT(slotAutoCover()), actionCollection(),"playlist_autocover");
	m_autoCover->setStatusText(i18n("Automatic dowloading of covers"));
	m_showPlayer = new KToggleAction(i18n("Don't switch to player window"), 0 , 0, this, SLOT(slotShowPlayer()), actionCollection(),"playlist_showplayer");
	m_showPlayer->setStatusText(i18n("Don't switch automatically to player window"));
	new KAction(i18n("&Clear Current Playlist"), "eraser", 0, this, SLOT(slotClearList()), actionCollection(), "playlist_clear");
	new KAction(i18n("Ne&w Playlist"), "filenew", 0, this, SLOT(slotNewList()), actionCollection(), "playlist_new");
	new KAction(i18n("&Import Playlist..."), "project_open", 0, this, SLOT(slotPlaylistLoad()), actionCollection(), "playlist_load");
	new KAction(i18n("&Save Current Playlist As..."), "filesaveas", 0, this, SLOT(slotPlaylistSaveAs()), actionCollection(), "playlist_save_as");
	new KAction(i18n("Re&move Current Playlist"), "fileclose", 0, this, SLOT(slotPlaylistRemove()), actionCollection(), "playlist_remove");
}

void PlayList::slotClearList()
{
	clearList();
}

void PlayList::slotRepeat()
{
	setEndless(m_repeat->isChecked());
}

void PlayList::slotShuffle()
{
	setRandom(m_shuffle->isChecked());
}

void PlayList::slotAutoCover()
{
	m_cover = m_autoCover->isChecked();
}

void PlayList::slotShowPlayer()
{
}

void PlayList::slotToggleShuffle()
{
	m_shuffle->setChecked(!m_shuffle->isChecked());
	slotShuffle();
}

void PlayList::slotPlaylistLoad()
{
	QString path = KFileDialog::getOpenFileName(":kaffeine_openPlaylist", QString("*.kaffeine|") + i18n("Kaffeine Playlists") + "\n*.*|" + i18n("All Files"), 0, i18n("Open Playlist"));
	if (path.isEmpty() || (!path.contains(".kaffeine", false)))
		return;

	loadPlaylist(path);
}

void PlayList::slotPlaylistSaveAs()
{
	QString startPath = ":kaffeine_savePlaylist";
	//if (!lastPlaylist.isEmpty()) startPath = lastPlaylist;
	QString path = KFileDialog::getSaveFileName(startPath,
		QString("*.kaffeine|") + i18n("Kaffeine Playlists") + "\n" +
		QString("*.m3u|") + i18n("M3U Playlists") + "\n" +
		QString("*.pls|") + i18n("PLS Playlists") + "\n" +
		"*.*|" + i18n("All Files"), 0, i18n("Save Playlist"));

	if ( path.isEmpty() )
		return;
	if ( path.right(4).lower() == ".m3u" ) {
		exportM3UPlaylist(path);
	} else if ( path.right(4).lower() == ".pls" ) {
		exportPLSPlaylist(path);
	} else if ( path.right(9).lower() == ".kaffeine" ) {
		savePlaylist(path);
	} else {
		path.append(".kaffeine");
		savePlaylist(path);
	}
}

void PlayList::slotPlaylistRemove()
{
	removeCurrentPlaylist();
}

QWidget* PlayList::wantPlayerWindow()
{
	return playerBox;
}

QWidget* PlayList::inputMainWidget()
{
	return mainWidget;
}

void PlayList::loadConfig(KConfig* config)
{
	QValueList<int> sl;
	bool b;

	config->setGroup("Playlist");
	b = config->readBoolEntry("Endless Mode", false);
	m_repeat->setChecked(b);
	setEndless(b);
	b = config->readBoolEntry("Random Mode", false);
	m_shuffle->setChecked(b);
	setRandom(b);
	b = config->readBoolEntry("AutoCover", false);
	m_autoCover->setChecked(b);
	m_cover = b;
	b = config->readBoolEntry("DontShowPlayer", false);
	m_showPlayer->setChecked(b);

	sl = config->readIntListEntry("HSplitSizes");
	if ( sl.count() )
		hSplit->setSizes( sl );
	else {
		sl.append( 200 );
		sl.append( 200 );
		hSplit->setSizes( sl );
	}
	vSplit->setSizes( config->readIntListEntry("VSplitSizes") );
	QStringList list;
	list = config->readListEntry("Playlists");
	if (!list.count())
	{
		list.append(i18n("Playlist") + "1");
	}
	int lastRegularPlaylist = list.count();
	list.append(i18n("NEW"));
	m_playlistSelector->insertStringList(list);
	m_currentPlaylist = config->readNumEntry("Current", 0);
	if (m_currentPlaylist > lastRegularPlaylist)
		m_currentPlaylist = 0;
	m_playlistSelector->setCurrentItem(m_currentPlaylist);
	add(m_playlistDirectory + m_playlistSelector->text(m_currentPlaylist) + ".kaffeine", NULL);
	m_nextPlaylistNumber = config->readNumEntry("Next Playlist", 2);
	QString currentEntry = config->readEntry("Current Entry", QString());
	if ((!currentEntry.isEmpty()) && (m_list->childCount()))
	{
		QListViewItem* tmp = findByURL(currentEntry);
		if (tmp)
			setCurrentEntry(tmp, false);
	}
	fileBrowser->readConfig( config, "PlaylistFileBrowser" );
	fileBrowser->setURL( KURL(config->readEntry( "FileBrowserURL", "/" )), true );
	browserComb->setURL( KURL(config->readEntry( "FileBrowserURL", "/" )) );
}

void PlayList::saveConfig()
{
	saveConfig( KGlobal::config() );
}

void PlayList::saveConfig(KConfig* config)
{
	saveCurrentPlaylist();
	config->setGroup("Playlist");
	config->writeEntry("Endless Mode", m_repeat->isChecked());
	config->writeEntry("Random Mode", m_shuffle->isChecked());
	config->writeEntry("AutoCover", m_autoCover->isChecked());
	config->writeEntry("DontShowPlayer", m_showPlayer->isChecked());
	config->writeEntry( "HSplitSizes", hSplit->sizes() );
	config->writeEntry( "VSplitSizes", vSplit->sizes() );
	QStringList list;
	QString text;
	for (uint i=0; i < m_playlistSelector->listBox()->count(); i++)
	{
		text = m_playlistSelector->text(i);
		if ( text != i18n("NEW") )
			list.append(text);
	}
	config->writeEntry("Playlists", list);
	config->writeEntry("Current", m_currentPlaylist);
	config->writeEntry("Next Playlist", m_nextPlaylistNumber);
	config->writeEntry("Current Entry", m_currentEntryMRL.url());
	m_list->saveLayout(config, "Playlist Layout");
	fileBrowser->writeConfig( config, "PlaylistFileBrowser" );
	config->writeEntry( "FileBrowserURL", fileBrowser->url().url() );
}

void PlayList::closeEvent(QCloseEvent* ce)
{
	kdDebug() << "Playlist: close Event" << endl;
	ce->ignore();
}

/******************************************
 *      get the urls from playlist
 ******************************************/
void PlayList::startPlaylist()
{
	MRL mrl;
	if ( trackNumber( 1, mrl ) )
		emit play( mrl, this );
}

bool PlayList::currentTrack( MRL &mrl )
{
	mrl = getCurrent();
	if ( !mrl.isEmpty() )
		return true;
	else
		return false;
}

MRL PlayList::getCurrent()
{
	if (isQueueMode())
	{
		m_queue.clear();
		updateStatus();
	}

	if (m_random)
	{
		if (m_currentRandomListEntry == -1) return MRL();
		setCurrentEntry(m_randomList.at(m_currentRandomListEntry));
		return m_currentEntryMRL;
	}

	if (!m_currentEntry)
		if (m_list->childCount() > 0)
		{
			if (m_list->firstChild()->isVisible())
				setCurrentEntry(m_list->firstChild());
			else
			{
				if (m_list->firstChild()->itemBelow())
					setCurrentEntry(m_list->firstChild()->itemBelow());
				else
					return MRL();
			}
		}
		else
			return MRL();

	setCurrentEntry(m_currentEntry);
	return m_currentEntryMRL;
}

bool PlayList::playbackFinished( MRL &mrl )
{
	return nextTrack( mrl );
}

bool PlayList::nextTrack( MRL &mrl )
{
	if (isQueueMode())
	{
		mrl = m_queue.first();
		m_queue.remove(m_queue.begin());
		updateStatus();
		return true;
	}

	if (!m_currentEntry) {
		mrl = getCurrent();
		if ( !mrl.isEmpty() )
			return true;
		else
			return false;
	}

	if (m_random)
	{
		if ((m_currentRandomListEntry+1) < (int)m_randomList.count())
			m_currentRandomListEntry += 1;
		else
		{
			if (m_endless)
				m_currentRandomListEntry = 0;
			else
				return false;
		}
		setCurrentEntry(m_randomList.at(m_currentRandomListEntry));
		mrl = m_currentEntryMRL;
		return true;
	}

	QListViewItem* tmpItem;
	tmpItem = m_currentEntry->itemBelow();

	if (tmpItem)
	{
		setCurrentEntry(tmpItem);
		mrl = m_currentEntryMRL;
		return true;
	}
	else
	{
		if (m_endless)
		{
			setCurrentEntry(m_list->firstChild());
			mrl =  m_currentEntryMRL;
			return true;
		}
		else
			return false;
	}
}

bool PlayList::previousTrack( MRL &mrl )
{
	if (isQueueMode())
	{
		m_queue.clear();
		updateStatus();
	}

	if (!m_currentEntry) {
		mrl = getCurrent();
		if ( !mrl.isEmpty() )
			return true;
		else
			return false;
	}

	if (m_random) {
		if (m_currentRandomListEntry > 0)
			m_currentRandomListEntry -= 1;
		else {
			if (m_endless)
				m_currentRandomListEntry = m_randomList.count()-1;
			else
				return false;
		}
		setCurrentEntry(m_randomList.at(m_currentRandomListEntry));
		mrl = m_currentEntryMRL;
		return true;
	}

	QListViewItem* tmpItem;
	tmpItem = m_currentEntry->itemAbove();

	if (tmpItem) {
		setCurrentEntry(tmpItem);
		mrl = m_currentEntryMRL;
		return true;
	}
	else {
		if (m_endless) {
			setCurrentEntry(getLast());
			mrl = m_currentEntryMRL;
			return true;
		}
		else
			return false;
	}
}

bool PlayList::trackNumber( int number, MRL &mrl )
{
	if (number > m_list->childCount())
		return false;

	QListViewItem * item = m_list->firstChild();
	for (int i = 1; i < number; i++)
		item = item->nextSibling();

	setCurrentEntry(item);
	mrl = m_currentEntryMRL;
	return true;
}

QListViewItem* PlayList::getLast()
{
	return m_list->lastItem();
}

QListViewItem* PlayList::getFirst()
{
	return m_list->firstChild();
}

QListViewItem* PlayList::findByURL(const QString& url)
{
	QListViewItemIterator it(m_list);
	while (it.current())
	{
		if (dynamic_cast<PlaylistItem*>(*it)->url() == url)
		{
			return (*it);
		}
		++it;
	}

	/* fallback */
	return getFirst();
}

/********* set current entry (with play icon) ****************/

void PlayList::setCurrentEntry(QListViewItem* item, bool playIcon)
{
	PlaylistItem* newItem = dynamic_cast<PlaylistItem*>(item);
	PlaylistItem* oldItem = dynamic_cast<PlaylistItem*>(m_currentEntry);
	if (m_currentEntry)
	{
		oldItem->setPlaying(false);
		m_currentEntry->setPixmap(TITLE_COLUMN, QPixmap());
	}
	if (playIcon)
	{
		newItem->setPlaying(true);
		item->setPixmap(TITLE_COLUMN, m_isCurrentEntry);
	}
	m_currentEntry = item;
	if (m_random)
		m_currentRandomListEntry = m_randomList.find(item);
	m_currentEntryMRL = newItem->toMRL();
	roller->setTitle( m_currentEntryMRL );
	m_list->setCurrentItem(m_currentEntry);

	m_list->ensureVisible(10, m_list->itemPos(m_currentEntry), 10, 30);
}

QListViewItem* PlayList::insertItem(QListViewItem* after, const MRL& m)
{
	PlaylistItem* tmp = NULL;
	MRL mrl(m);

	m_list->setSorting(-1);
	QListViewItemIterator it(m_list);
	while (it.current())
	{
		if (dynamic_cast<PlaylistItem*>(*it)->url() == mrl.url())
		{
			kdDebug() << "PlayList: Source '" << mrl.url() << "' still exists. Skipped." << endl;
			return after;
		}
		++it;
	}

	if (mrl.mime().isNull())
	{
		KMimeType::Ptr mime = KMimeType::findByURL(mrl.kurl().path());
		mrl.setMime(mime->name());
	}

	tmp = new PlaylistItem(m_list, dynamic_cast<KListViewItem *>(after), mrl);
	if (!tmp) return after;

	if ((mrl.mime() == "video/dvd") || (mrl.mime() == "video/vcd") || (mrl.mime() == "audio/cd"))
		tmp->setPixmap(MIME_COLUMN, m_cdPixmap);
	else
		tmp->setPixmap(MIME_COLUMN, KMimeType::mimeType(mrl.mime())->pixmap(KIcon::Small));

	if (tmp->length().contains(':'))
		m_playTime += timeStringToMs(tmp->length());

	if (m_searchSelection)
	{
		QString text = m_playlistFilter->text();
		if ((!tmp->title().contains(text, false)) && (!tmp->url().contains(text, false))
		        && (!tmp->artist().contains(text, false)) && (!tmp->album().contains(text, false)))
		{
			tmp->setVisible(false);
		}
	}

	if (tmp->isVisible())
	{
		if (tmp->length().contains(':'));
		m_playTimeVisible += timeStringToMs(tmp->length());
		m_countVisible++;
	}

	return tmp;
}

void PlayList::fileSelected( const KFileItem *item )
{
	if ( !item )
		return;
	add( QStringList( item->url().path() ), NULL );
	QListViewItem* tmp = findByURL(item->url().path());
	if (tmp) {
		setCurrentEntry(tmp);
		emit play(m_currentEntryMRL, this);
	}
}

void PlayList::setBrowserURL( const QString &path )
{
	fileBrowser->setURL( KURL(path), true );
}

void PlayList::setBrowserURL( const KURL &path )
{
	fileBrowser->setURL( path, true );
}

void PlayList::browserURLChanged( const KURL &u )
{
	QString url;

	 if ( u.isLocalFile() )
	 	url = u.path();
	 else
	 	url = u.prettyURL();

	QStringList urls = browserComb->urls();
	urls.remove( url ); // do not duplicate
	urls.prepend( url );
	browserComb->setURLs( urls, KURLComboBox::RemoveBottom );
}

void PlayList::add(const QString& url, QListViewItem* after)
{
	add(QStringList(url), after);
}


void PlayList::add(const QStringList& urlList, QListViewItem* after)
{
	QListViewItem* tmp = NULL;
	QStringList urls(urlList);

	QString ext;
	QString subtitleURL;
	QStringList subs;
	KURL tmpKURL;
	bool playlist;
	MRL mrl;
	QValueList<MRL> mrlList;
	QString mediadevice="";

	KProgressDialog* progress = NULL;
	progress = new KProgressDialog(mainWidget, "importprogress", QString::null, i18n("Importing media resources..."));
	progress->progressBar()->setTotalSteps(urls.count());
	progress->show();

	kdDebug() << "PlayList: add " << urls.count() << " items to playlist" << endl;

	for (uint i = 0; i < urls.count(); i++)
	{
		mrl = MRL(urls[i]);
		QString url(mrl.kurl().prettyURL());

		if (mrl.kurl().protocol() == "media" || url.startsWith("system:/media/"))
		{
			QString mediaName;
			if (url.startsWith("system:/media/"))
				mediaName = url.mid(14);
			else
				mediaName = url.mid(7);
			int slash = mediaName.find("/");
			QString filePath(mediaName.mid(slash));
			if (filePath == "/")
				filePath = "";
			mediaName = mediaName.left(slash);

			DCOPRef mediamanager("kded","mediamanager");
			DCOPReply reply = mediamanager.call("properties(QString)",mediaName);
			if (reply.isValid())
			{
				QStringList properties = reply;
				if (properties.isEmpty()) break;
				url = properties[6]  + filePath;
				if (filePath.isEmpty())
				{
					if (properties[10] == "media/audiocd")
					{
						mediadevice = properties[5];
						delete progress;
						emit signalRequestForAudioCD(mediadevice);
						return;
					}
					if (properties[10] == "media/dvdvideo")
					{
						mediadevice = properties[5];
						delete progress;
						emit signalRequestForDVD(mediadevice);
						return;
					}
					if ((properties[10] == "media/vcd") || (properties[10] == "media/svcd"))
					{
						mediadevice = properties[5];
						delete progress;
						emit signalRequestForVCD(mediadevice);
						return;
					}
				}
				mrl = MRL(url);
			}
		}

		if (mrl.kurl().isLocalFile())
		{
			if (!QFile::exists(mrl.kurl().path()))
				continue;
			mrl.setTitle(mrl.kurl().fileName());
			mrl.setURL(mrl.kurl().path());
		}
		else
		{
			mrl.setTitle(mrl.url());
		}

		//kdDebug() << "PlayList: Check url " << mrl.url() << endl;
		/*********** determine extension and mime type ************/

		ext = mrl.kurl().fileName();
		ext = ext.remove(0 , ext.findRev('.') +1).lower();
		// kdDebug() << "Extension: " << ext << endl;

		//kdDebug() << "PlayList: Try to determine mime of: " << mrl.url() << endl;
		KMimeType::Ptr mime = KMimeType::findByURL(mrl.kurl().path()); /* works only with path() (without protocol)
		                                                                      e.g. http://www.somafm.com/indipop.pls
		                                                                      path: /indipop.pls */
		//kdDebug() << "Mime: " << mime->name() << endl;
		/*** check for kaffeine/noatun/pls/asx/m3u playlist ***/
		mrl.setMime(mime->name());

		/******** some special processing for local files! *******/
		if (mrl.kurl().isLocalFile())
		{
			/* playlist ? */
			if ((mime->name() == "text/plain") || (mime->name() == "text/xml") || (mime->name() == "application/x-kaffeine")
			        || (mime->name() == "audio/x-scpls") || (mime->name() == "audio/x-mpegurl" || (mime->name() == "audio/mpegurl")
			                || (ext == "asx") || (ext == "asf") || (ext == "wvx") || (ext == "wax"))) /* windows meta files */
			{
				kdDebug() << "PlayList: Check for kaffeine/noatun/m3u/pls/asx playlist\n";

				mrlList.clear();
				playlist = false;
				QFile file(mrl.url());
				file.open(IO_ReadOnly);

				QTextStream stream(&file);
				QString firstLine = stream.readLine();
				QString secondLine = stream.readLine();
				file.close();

				if (secondLine.contains("kaffeine", false))
				{
					kdDebug() << "PlayList: Try loading kaffeine playlist\n";
					playlist = PlaylistImport::kaffeine(mrl.url(), mrlList);
					if (!playlist)
						continue;
				}
				if (secondLine.contains("noatun", false))
				{
					kdDebug() << "PlayList: Try loading noatun playlist\n";
					playlist = PlaylistImport::noatun(mrl.url(), mrlList);
				}
				if (firstLine.contains("asx", false))
				{
					kdDebug() << "PlayList: Try loading asx playlist\n";
					playlist = PlaylistImport::asx(mrl.url(), mrlList);
				}
				if ( (firstLine.contains("[playlist]", false)) || (ext == "pls") )
				{
					kdDebug() << "PlayList: Try loading pls playlist\n";
					playlist = PlaylistImport::pls(mrl.url(), mrlList);
				}
				if (ext == "m3u")  //identify by extension
				{
					kdDebug() << "PlayList: Try loading m3u playlist\n";
					playlist = PlaylistImport::m3u(mrl.url(), mrlList);
				}
				if (playlist)
				{
					QValueList<MRL>::ConstIterator end(mrlList.end());
					for (QValueList<MRL>::ConstIterator it = mrlList.begin(); it != end; ++it)
						after = insertItem(after, *it);
					continue;
				}

			}

			/* check for ram playlist */
			if ( (ext == "ra") || (ext == "rm") || (ext == "ram") || (ext == "lsc") || (ext == "pl") )
			{
				mrlList.clear();
				kdDebug() << "PlayList: Try loading ram playlist\n";
				if (PlaylistImport::ram(mrl, mrlList, mainWidget->parentWidget()))
				{
					QValueList<MRL>::ConstIterator end(mrlList.end());
					for (QValueList<MRL>::ConstIterator it = mrlList.begin(); it != end; ++it)
						after = insertItem(after, *it);
					continue;
				}
			}

			/**** a directory ? ****/
			if (mime->name() == "inode/directory")
			{
				kdDebug() << "PlayList: Add Directory: " << mrl.url() << endl;

				QDir d(mrl.url());
				uint x;

				/* subdirs */
				QStringList entryList = d.entryList(QDir::Dirs, QDir::Name);
				for (x = 0; x < entryList.count(); x++)
				{
					if (entryList[x][0] != '.')
					{
						urls.append(mrl.url() + "/" + entryList[x]);
					}
				}

				/* Media files */
				/* Exclude subtitles because they are examined twice : */
				/* Once for movie and once per subtitle - very annoying*/
				QString fileFilter = m_fileFilter;
				fileFilter.remove("*.srt",false);
				fileFilter.remove("*.ssa",false);
				fileFilter.remove("*.txt",false);
				fileFilter.remove("*.asc",false);
				fileFilter.remove("*.smi",false);
				fileFilter.remove("*.sub",false);
				entryList = d.entryList(fileFilter, QDir::Files, QDir::Name );
				for (x = 0; x < entryList.count(); x++)
				{
					urls.append(mrl.url() + "/" + entryList[x]);
				}

				progress->progressBar()->setTotalSteps(urls.count());
				continue; /* dont add directory to playlist */
			}

			/*** append subtitle files to the movie ***/
			if ((ext == "srt") || (ext == "ssa") || (ext == "txt") ||
			        (ext == "asc") || (ext == "smi") || (ext == "sub"))
			{
				QStringList movies;
				QString movieURL;
				QListViewItemIterator it(m_list);
				while (it.current())
				{
					if (dynamic_cast<PlaylistItem*>(*it)->mime().contains("video"))
						movies << dynamic_cast<PlaylistItem*>(*it)->url();
					++it;
				}
				if (movies.count() == 1)
					movieURL = movies[0];
				else if (movies.count() >= 2)
				{
					MovieChooser *mc = new MovieChooser(movies, mrl.url(), mainWidget);
					if(mc->exec() == QDialog::Accepted)
						movieURL = mc->getSelection();
					delete mc;
				}
				it = m_list;
				while (it.current())
				{
					if (dynamic_cast<PlaylistItem*>(*it)->url() == movieURL)
					{
						kdDebug() << "PlayList: adding subtitle to movie " << dynamic_cast<PlaylistItem*>(*it)->url() << endl;
						(dynamic_cast<PlaylistItem*>(*it))->addSubtitle(mrl.url());
						break;
					}
					++it;
				}
				continue;
			}

			/*** get meta tags ****/
			if (m_metaOnLoading)
				getMetaInfo(mrl, mime->name());

			/* Get all suported subs in movie dir, clear out
			 * those starting with a different name than the movie,
			 * prompt the user to select a sub
			 */
			if (mime->name().contains("video"))
			{
				kdDebug() << "PlayList: Check for subtitle files" << endl;
				subtitleURL = QString::null;
				QDir *d = new QDir(mrl.url().section('/',0,-2));
				QString filename = mrl.url().section('/',-1,-1);
				subs = QStringList();
				//Do a case insensitive search for subs
				QStringList dirListing = d->entryList(QDir::Files|QDir::NoSymLinks); //cache directory listing
				bool matches = false;
				QStringList::Iterator end(dirListing.end());
				for(QStringList::Iterator it = dirListing.begin(); it != end; ++it )
				{
					if(startsWith(*it, filename.section('.',0,-2), false)) //If filenames (without extensions) match
					{
						if( endsWith(*it, ".srt", false) || endsWith(*it, ".ssa", false) ||
						        endsWith(*it, ".txt", false) || endsWith(*it, ".smi", false) ||
						        endsWith(*it, ".asc", false) || endsWith(*it, ".sub", false) )
							matches = true;

						if(matches) {
							subs.append(*it); //This is a subtitle for our movie
						}
						matches = false;
					}
				}

				if((subs.count() > 1)) {
					subs.prepend(i18n("(no subtitles)"));
					subs.append(i18n("Other subtitle..."));
					subtitleURL = QString::null;
					SubtitleChooser *sc = new SubtitleChooser(subs, filename, mainWidget);

				THERE:     if(sc->exec() == QDialog::Accepted)
					{
						if((sc->getSelection() != i18n("(no subtitles)")) && (sc->getSelection() != i18n("Other subtitle...")))
							subtitleURL = d->path()+"/"+sc->getSelection();
						if((sc->getSelection() == i18n("Other subtitle...")))
						{
							subtitleURL = KFileDialog::getOpenURL(d->path(), i18n("*.smi *.srt *.sub *.txt *.ssa *.asc|Subtitle Files\n*.*|All Files"), 0, i18n("Select Subtitle File")).path();
							if(subtitleURL.isEmpty())
							{
								subtitleURL = QString::null;
								goto THERE;
							}
							else //The user has selected another sub, so add this to the list
							{
								subs.clear();
								subs.append(subtitleURL);
							}
						}
					}
					delete sc;
					subs.remove(i18n("(no subtitles)"));
					subs.remove(i18n("Other subtitle..."));
				}

				//Add the full path to the subtitle name
				for (unsigned int i = 0; i < subs.size(); i++ )
					if(!subs[i].startsWith(d->path()))
						subs[i] = d->path()+"/"+subs[i];

				mrl.setSubtitleFiles(subs);
				if ( subs.count()==1 )
					subtitleURL = subs[0]; // Use this one.
				if (!subtitleURL.isNull())
				{
					kdDebug() << "PlayList: Use subtitle file: " << subtitleURL << " for: " << mrl.url() << endl;
					mrl.setCurrentSubtitle(subs.findIndex(subtitleURL));
				}
			}
		} /* if localFile() */

		tmp = insertItem(after, mrl);
		if (tmp)
			after = tmp;

		if ( progress->wasCancelled() )
			break;
		progress->progressBar()->setProgress(i+1);
		progress->setLabel(QString::number(i+1) + " / " + QString::number(progress->progressBar()->totalSteps()) + " " + i18n("Files"));
		KApplication::kApplication()->processEvents();
	}

	delete progress;
	if (m_random) createRandomList();
	updateStatus();
}

void PlayList::add(const QValueList<MRL>& mrlList, QListViewItem* after)
{
	QValueList<MRL>::ConstIterator end(mrlList.end());
	for (QValueList<MRL>::ConstIterator it = mrlList.begin(); it != end; ++it)
		after = insertItem(after, *it);
	updateStatus();
	if (m_random) createRandomList();
}

void PlayList::slotPlayDirect(QListViewItem* item)
{
	if (!item) return;

	setCurrentEntry(item);
	emit play(m_currentEntryMRL, this);
}

void PlayList::setEndless(bool e)
{
	m_endless = e;
}

void PlayList::setRandom(bool r)
{
	m_random = r;
	if (m_random) createRandomList();
}

void PlayList::createRandomList()
{
	kdDebug() << "PlayList: Create a random playlist" << endl;
	m_randomList.clear();
	m_currentRandomListEntry = 0;

	QListViewItem* tmpItem = NULL;
	tmpItem = m_list->firstChild();
	if ( (tmpItem) && (!tmpItem->isVisible()) )
		tmpItem = tmpItem->itemBelow();

	while (tmpItem)
	{
		m_randomList.append(tmpItem);
		tmpItem = tmpItem->itemBelow();
	}

	if (!(m_randomList.count() > 0))
	{
		m_currentRandomListEntry = -1;
		return;
	}

	KRandomSequence r(KApplication::random());
	r.randomize(&m_randomList);
}

void PlayList::clearList()
{
	m_list->clear();
	m_randomList.clear();
	m_playTime = 0;
	m_playTimeVisible = 0;
	m_countVisible = 0;
	if (m_searchSelection)
	{
		m_playlistFilter->clear();
		m_searchSelection = false;
	}
	updateStatus();
	m_currentEntry = NULL;
	m_currentRandomListEntry = -1;
}

void PlayList::slotDropEvent(QDropEvent* dev, QListViewItem* after)
{
	QStringList urls ,newurls;

	m_list->setSorting(-1);
	if (QUriDrag::decodeLocalFiles(dev, urls))
	{
		if (urls.count())
		{
			for (uint i=0; i < urls.count() ;i++)
			{
				//KURL url(QUriDrag::unicodeUriToUri(urls[i]));
				//newurls << url.path(-1);
				//kdDebug() << "PlayList: Dropped " << url.path() << endl;
				newurls << urls[i];
				kdDebug() << "PlayList: Dropped " << urls[i] << endl;
			}
			add(newurls, after);
		}
		else
		{
			QUriDrag::decodeToUnicodeUris(dev, urls);
			if (urls.count())
				add(urls, after);
		}
	}
	else
		if (strcmp(dev->format(), "text/x-moz-url") == 0)    /* for mozilla drops */
		{
			QByteArray data = dev->encodedData("text/plain");
			QString md(data);
			add(md, after);
		}
}

void PlayList::slotPreDropEvent()
{
	m_list->setSorting(-1);
}

void PlayList::slotSetAlternateColor( const QColor& col )
{
	m_list->setAlternateBackground( col );
	m_altCol = col;
	m_list->triggerUpdate();
}

void PlayList::useAlternateEncoding(bool useEncoding)
{
	m_useAlternateEncoding = useEncoding;
}

void PlayList::setAlternateEncoding(const QString& encoding)
{
	m_alternateEncoding = encoding;
}


void PlayList::getMetaInfo(MRL& mrl, const QString& mimeName)
{
	KFileMetaInfo* metaInfo = NULL;
	KFileMetaInfoGroup metaGroup;
	uint m, n;

	metaInfo = new KFileMetaInfo(mrl.url(), mimeName);
	QStringList groups = metaInfo->groups();
	QStringList keys;
	QString title;
	QString artist;
	QString album;

	QTextCodec *altCodec;
	QTextCodec *CodecUtf8;

	altCodec = QTextCodec::codecForName(m_alternateEncoding.ascii());
	CodecUtf8 = QTextCodec::codecForName("UTF-8");
	//kdDebug() << "playlist: locale " << Codec->name << " index: " << m_alternateEncoding << endl;

	for (m = 0; m < groups.count(); m++)
	{
		//kdDebug() << "Metainfo-Group: " << groups[m] << endl;
		metaGroup = metaInfo->group(groups[m]);
		keys = metaGroup.keys();
		for (n = 0; n < keys.count(); n++)
		{
			//kdDebug() << "Metainfo-Key: " << keys[n] << endl;
			if (keys[n] == "Length")
			{
				mrl.setLength(QTime().addSecs(metaGroup.item(keys[n]).value().toUInt()));
			}

			if (keys[n] == "Title")
			{
				title = metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace();
				if ((!title.isEmpty()) && (title.contains(QRegExp("\\w")) > 2) && (title.left(5).lower() != "track"))
				{
					if ((m_useAlternateEncoding) && (CodecUtf8->heuristicContentMatch(title.ascii(), title.length()) < 0))
					{
						mrl.setTitle(altCodec->toUnicode(title.ascii()));
						//kdDebug() << "playlist: Convert, locale name: " << altCodec->name() << title << endl;
					}
					else
					{
						mrl.setTitle(title);
						//kdDebug() << "playlist: non Convert, locale name: " << CodecUtf8->name() << title << endl;
					}
				}
			}

			if (keys[n] == "Artist")
			{
				artist = (metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace());
				if (!artist.isEmpty())
				{
					if ((m_useAlternateEncoding) && (CodecUtf8->heuristicContentMatch(artist.ascii(),artist.length()) < 0 ))
					{
						mrl.setArtist(altCodec->toUnicode(artist.ascii()));
					}
					else
					{
						mrl.setArtist(artist);
					}
				}
			}

			if (keys[n] == "Album")
			{
				album = (metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace());
				if (!album.isEmpty())
				{
					if ((m_useAlternateEncoding) && (CodecUtf8->heuristicContentMatch(album.ascii(),album.length()) < 0 ))
					{
						mrl.setAlbum(altCodec->toUnicode(album.ascii()));
					}
					else
					{
						mrl.setAlbum(album);
					}
				}
			}

			if (keys[n] == "Tracknumber")
				mrl.setTrack(metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace());
			if (keys[n] == "Date")
				mrl.setYear(metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace());
			if (keys[n] == "Genre")
				mrl.setGenre(metaGroup.item(keys[n]).value().toString().simplifyWhiteSpace());
		}
	}

	delete metaInfo;
}

void PlayList::mergeMeta(const MRL& mrl)
{
	if (!m_currentEntry) return;
	PlaylistItem* tmp = dynamic_cast<PlaylistItem*>(m_currentEntry);
	bool addTime = (tmp->length() == QString("00:00:00"));
	if ((tmp) && (tmp->url() == mrl.url()))
	{
		tmp->setTitle(mrl.title());
		tmp->setArtist(mrl.artist());
		tmp->setAlbum(mrl.album());
		tmp->setTrack(mrl.track());
		tmp->setYear(mrl.year());
		tmp->setGenre(mrl.genre());
		tmp->setLength(mrl.length().toString("h:mm:ss"));
		tmp->setCurrentSubtitle(mrl.currentSubtitle());
		tmp->setSubtitles(mrl.subtitleFiles());
	}
	if (addTime)
	{
		if (tmp->length().contains(':'));
		{
			m_playTime += timeStringToMs(tmp->length());
			if (tmp->isVisible())
				m_playTimeVisible += timeStringToMs(tmp->length());

			updateStatus();
		}
	}

	roller->setTitle( mrl );
	setCover( mrl.artist().stripWhiteSpace(), mrl.album().stripWhiteSpace() );
}

GalleryDialog::GalleryDialog( QWidget *parent, GoogleFetcher *goog )
	: KDialogBase( parent, QString(i18n("Gallery")).latin1(), true, QString::null, Ok, NoDefault, true )
{
	google = goog;
	hbox = new QHBox(this);
	imageFrame = new QFrame( hbox );
	imageFrame->setFixedSize(500,500);
	setMainWidget(hbox);
	connect( &next, SIGNAL(timeout()), this, SLOT(slotNext()) );
	next.start(1000,true);
}

void GalleryDialog::slotNext()
{
	QPixmap pix;
	bool end;

	actionButton(Ok)->setEnabled(false);
loop:
	QImage img = google->galleryNext( end );
	if ( end ) {
		slotOk();
		return;
	}
	if ( !img.isNull() ) {
		if ( img.width()>img.height() )
			imageFrame->setFixedSize(500, 500*img.height()/img.width());
		else
			imageFrame->setFixedSize(500*img.width()/img.height(), 500);
		img = img.smoothScale(imageFrame->width(),imageFrame->height());
		pix.convertFromImage( img );
		imageFrame->setPaletteBackgroundPixmap( pix );
	}
	else
		goto loop;

	actionButton(Ok)->setEnabled(true);
	next.start(5000,true);
}

void GalleryDialog::slotOk()
{
	if ( next.isActive() )
		next.stop();
	hide();
}

void PlayList::showGallery()
{
	if ( google )
		return;
	QString s = getCurrent().artist().stripWhiteSpace();
	if ( s.isEmpty() )
		return;
	google = new GoogleFetcher( s, "", true );
	GalleryDialog dlg( 0, google );
	dlg.exec();
	delete google;
	google = NULL;
}

void PlayList::setCover( QString s1, QString s2, bool forceFetch )
{
	if ( s1.isEmpty() || s2.isEmpty() ) {
		coverFrame->setCoverPixmap( "" );
		return;
	}
	QString s = locateLocal("appdata", "");
	QImage img;
	QPixmap pix;
	QString fname = s1+"_"+s2;
	fname = fname.replace( QChar( '/' ), "_" );
	fname = fname.upper();
	fname = s+"covers/"+fname;
	if ( !forceFetch && QFile( fname ).exists() )
		coverFrame->setCoverPixmap( fname );
	else {
		if ( !forceFetch )
			coverFrame->setCoverPixmap( "" );
		if ( google )
			return;
		if ( m_cover || forceFetch ) {
			google = new GoogleFetcher( s1, s2 );
			pix = google->pixmap( forceFetch );
			delete google;
			google = NULL;
		}
		if ( !QDir( s+"covers" ).exists() )
			QDir().mkdir( s+"covers" );
		if ( !pix.isNull() ) {
			img = pix.convertToImage();
			img.save( fname, coverImageFormat.latin1() );
			MRL mrl = getCurrent();
			if ( s1==mrl.artist().stripWhiteSpace() && s2==mrl.album().stripWhiteSpace() )
				coverFrame->setCoverPixmap( fname );
		}
		else if ( !forceFetch && m_cover ) {
			QString path = locate("appdata", "nocover.png");
			if ( !path )
				path = locate("data", "kaffeine/nocover.png");
			KIO::file_copy( KURL::fromPathOrURL( path ), KURL::fromPathOrURL( fname ), -1, true );
		}
	}
}

void PlayList::chooseCurrentCover()
{
	MRL mrl = getCurrent();
	setCover( mrl.artist().stripWhiteSpace(), mrl.album().stripWhiteSpace(), true );
}

/************ sort playlist ******************/

void PlayList::slotSort(int section)
{
	m_list->setSorting(section, m_sortAscending);
	m_list->sort();
	m_sortAscending = !m_sortAscending;
}


/***  remove items ***/

void PlayList::slotRemoveSelected()
{
	QPtrList<QListViewItem> selected;

	if (m_currentEntry)
		if (m_currentEntry->isSelected())
		{
			m_currentEntry = NULL;
			m_currentRandomListEntry = -1;
		}

	selected = m_list->selectedItems();
	PlaylistItem* item = NULL;

	for(uint i = 0; i<selected.count(); i++)
	{
		// kdDebug() << "Remove " << selected.at(i)->text(TITLE_COLUMN) << "\n";
		item = dynamic_cast<PlaylistItem *>(selected.at(i));
		if (item->length().contains(':'))
		{
			m_playTime -= timeStringToMs(item->length());
			m_playTimeVisible -= timeStringToMs(item->length());
		}

		m_countVisible--;
		delete selected.at(i);
	}

	if (m_random) createRandomList();
	updateStatus();
}

void PlayList::updateStatus()
{
	QString status;
	if (isQueueMode())
	{
		QTime total;
		QValueList<MRL>::ConstIterator end(m_queue.end());
		for (QValueList<MRL>::ConstIterator it = m_queue.begin(); it != end; ++it)
			total = total.addSecs(QTime().secsTo((*it).length()));
		status = i18n("Queue: %1 Entries, Playtime: %2").arg(m_queue.count()).arg(total.toString("h:mm:ss"));
	}
	else
	{
		//status = i18n("Entries: %1, Playtime: %2  (Total: %3, %4)").arg(QString::number(m_countVisible)).arg(msToTimeString(m_playTimeVisible)).arg(QString::number(m_list->childCount())).arg(msToTimeString(m_playTime));


		status = i18n("Entries: %1, Playtime: %2").arg(QString::number(m_countVisible)).arg(msToTimeString(m_playTimeVisible));
	}
	emit statusBarMessage(status);
}

void PlayList::slotNewList()
{
	m_list->setSorting(-1);
	saveCurrentPlaylist();
	m_playlistSelector->insertItem(i18n("Playlist") + QString::number(m_nextPlaylistNumber), 0);
	m_nextPlaylistNumber++;
	m_playlistSelector->setCurrentItem(0);
	m_currentPlaylist = 0;
	clearList();
}

void PlayList::setPlaylist(const QString& name, bool clear)
{
	saveCurrentPlaylist();
	if (clear)
		clearList();
	int index = 0;
	if (m_playlistSelector->listBox()->findItem(name))
		index = m_playlistSelector->listBox()->index(m_playlistSelector->listBox()->findItem(name));
	else
		m_playlistSelector->insertItem(name, 0);
	m_playlistSelector->setCurrentItem(index);
	m_currentPlaylist = index;
}

void PlayList::nextPlaylist()
{
	int nextPlaylist = m_playlistSelector->currentItem();
	QString pl = NULL;
	do
	{
		nextPlaylist++;
		if (nextPlaylist == m_playlistSelector->count())
			nextPlaylist = 0;
		pl = m_playlistSelector->text(nextPlaylist);
	}
	while (nextPlaylist != m_playlistSelector->currentItem());

	saveCurrentPlaylist();
	clearList();
	m_playlistSelector->setCurrentItem(nextPlaylist);
	m_currentPlaylist = nextPlaylist;
	add(m_playlistDirectory + pl + ".kaffeine", NULL);
	if (mainWidget->parentWidget())
		mainWidget->parentWidget()->setFocus();
}

void PlayList::slotPlaylistActivated(int index)
{
	kdDebug() << "PlayList: Switch to playlist: " << m_playlistSelector->text(index) << endl;
	QString pl = m_playlistSelector->text(index);

	saveCurrentPlaylist();
	clearList();
	m_currentPlaylist = index;
	add(m_playlistDirectory + pl + ".kaffeine", NULL);
	if (mainWidget->parentWidget())
		mainWidget->parentWidget()->setFocus();
	/*  if (!isQueueMode())
	  {
	    getCurrent();
	    emit signalPlay(m_currentEntryMRL);
	  }  */
}

void PlayList::slotNewPlaylistName(const QString& text)
{
	if ((text.isEmpty()) || (text == m_playlistSelector->text(m_currentPlaylist)))
		return;
	if (m_playlistSelector->listBox()->findItem(text))
	{
		kdDebug() << "PlayList: Name still exists!" << endl;
		return;
	}
	QString oldPl = m_playlistDirectory + m_playlistSelector->text(m_currentPlaylist) + ".kaffeine";
	kdDebug() << "Playlist: removing file: " << oldPl << endl;
	if (!KIO::NetAccess::del(oldPl, mainWidget))
		kdError() << "Playlist: " << KIO::NetAccess::lastErrorString() << endl;

	kdDebug() << "PlayList: Set playlist name to: " << text << endl;
	m_playlistSelector->changeItem(text, m_currentPlaylist);
	saveCurrentPlaylist();
	if (mainWidget->parentWidget())
		mainWidget->parentWidget()->setFocus();
}

QString PlayList::queryCurrentPlaylist()
{
	QString pl = m_playlistSelector->text(m_currentPlaylist);
	return (m_playlistDirectory + pl + ".kaffeine");
}

void PlayList::saveCurrentPlaylist()
{
	QString pl = m_playlistSelector->text(m_currentPlaylist);
	savePlaylist(m_playlistDirectory + pl + ".kaffeine");
}

void PlayList::removeCurrentPlaylist()
{
	int code = KMessageBox::warningContinueCancel(0, i18n("Remove '%1' from list and from disk?").arg(m_playlistSelector->text(m_currentPlaylist)),QString::null,KStdGuiItem::del());
	if (code == KMessageBox::Continue)
	{
		QString pl = m_playlistDirectory + m_playlistSelector->text(m_currentPlaylist) + ".kaffeine";
		if (!KIO::NetAccess::del(pl, mainWidget))
			kdError() << "Playlist: " << KIO::NetAccess::lastErrorString() << endl;

		m_playlistSelector->removeItem(m_currentPlaylist);
		m_currentPlaylist = m_playlistSelector->currentItem();
		clearList();
		add(m_playlistDirectory + m_playlistSelector->text(m_currentPlaylist) + ".kaffeine", NULL);
	}
}

void PlayList::loadPlaylist(const QString& pl)
{
	saveCurrentPlaylist();
	QString plName = KURL(pl).fileName();
	plName = plName.remove(".kaffeine", false);
	if (m_playlistSelector->listBox()->findItem(plName))
	{
		QString plNewName = NULL;
		while (true)
		{
			bool ok;
			plNewName = QInputDialog::getText(i18n("Playlist Name Already Exists"),
			                                  i18n("Enter different playlist name:"), QLineEdit::Normal, plName, &ok);
			if ((ok) && (!plNewName.isEmpty()))
			{
				if (m_playlistSelector->listBox()->findItem(plNewName))
					continue;
				else
					break;
			}
			else
			{
				kdDebug() << "Playlist: Import aborted, playlist not added" << endl;
				return;
			}
		}
		plName = plNewName;
	}
	m_playlistSelector->insertItem(plName, 0);
	m_playlistSelector->setCurrentItem(0);
	m_currentPlaylist = 0;
	clearList();
	add(pl, NULL);
}

/******************************************
 *         save xml playlist
 ******************************************/

void PlayList::savePlaylist(const QString& pl)
{
	QDomDocument doc("playlist");
	doc.setContent(QString("<!DOCTYPE XMLPlaylist>"));
	QDomElement root = doc.createElement("playlist");
	root.setAttribute("client", "kaffeine");
	doc.appendChild(root);

	QDomElement entry;
	PlaylistItem * tmp = NULL;

	QListViewItemIterator it(m_list);
	while (it.current())
	{
		tmp = dynamic_cast<PlaylistItem *>(it.current());

		entry = doc.createElement("entry");

		entry.setAttribute("title", tmp->title());
		entry.setAttribute("artist", tmp->artist());
		entry.setAttribute("album", tmp->album());
		entry.setAttribute("track", tmp->track());
		entry.setAttribute("year", tmp->year());
		entry.setAttribute("genre", tmp->genre());
		entry.setAttribute("url", tmp->url());
		entry.setAttribute("mime", tmp->mime());
		entry.setAttribute("length", tmp->length());

		if(!(tmp->subtitles().isEmpty()))
		{
			QString subList;
			for(unsigned int i=0; i<tmp->subtitles().count(); i++)
				subList += tmp->subtitles()[i] + "&";

			entry.setAttribute("subs", subList);
		}
		entry.setAttribute("currentSub", QString::number(tmp->currentSubtitle()));
		root.appendChild(entry);

		++it;
	}

	QFile file(pl);
	if (!file.open(IO_WriteOnly)) return;
	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);

	stream << doc.toString();

	file.close();
	m_list->setCleared(false);
}

/**********************/

void PlayList::exportM3UPlaylist(const QString& pl)
{
	PlaylistItem * tmp = NULL;

	QListViewItemIterator it(m_list);

	int length = -1;
	QString m3uList = "#EXTM3U\n";

	while (it.current())
	{
		tmp = dynamic_cast<PlaylistItem *>(it.current());

		length = timeStringToMs(tmp->length()) / 1000;
		if ( length == 0 )
			length = -1;

		m3uList.append("#EXTINF:" + QString::number(length) + "," + tmp->title());
		if (tmp->artist().isEmpty() )
			m3uList.append("\n");
		else
			m3uList.append(" - " + tmp->artist() + "\n");
		m3uList.append(tmp->url() + "\n");
		++it;
	}

	QFile file(pl);
	if (!file.open(IO_WriteOnly)) return;
	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);

	stream << m3uList;

	file.close();
}

void PlayList::exportPLSPlaylist(const QString& pl)
{
	PlaylistItem * tmp = NULL;

	QListViewItemIterator it(m_list);

	int counter = 0;
	int length = -1;

	QString plsList = "[playlist]\n";

	while (it.current())
	{
		tmp = dynamic_cast<PlaylistItem *>(it.current());
		++counter;

		length = timeStringToMs(tmp->length()) / 1000;

		if ( length == 0 ) {
			length = -1;
		}

		plsList.append("File" + QString::number(counter) + "=" + tmp->url() + "\n");
		plsList.append("Title" + QString::number(counter) + "=" + tmp->title());
		if (tmp->artist().isEmpty() )
			plsList.append("\n");
		else
			plsList.append(" - " + tmp->artist() + "\n");
		plsList.append("Length" + QString::number(counter) + "=" + QString::number(length) + "\n");
		++it;

	}

	plsList.append("NumberOfEntries=" + QString::number(counter) + "\n");
	plsList.append("Version=2");

	QFile file(pl);
	if (!file.open(IO_WriteOnly)) return;
	QTextStream stream(&file);
	stream.setEncoding(QTextStream::UnicodeUTF8);

	stream << plsList;

	file.close();
}

void PlayList::resetSearch()
{
	m_playlistFilter->clear();
	slotFindText( "" );
}

void PlayList::slotFindText(const QString& text)
{
	if (text == i18n("Filter")) return;

	QListViewItemIterator it(m_list);
	m_playTimeVisible = 0;
	m_countVisible = 0;
	PlaylistItem* tmp = NULL;
	while ( it.current() )
	{
		tmp = dynamic_cast<PlaylistItem *>(it.current());
		if (text.isEmpty() || tmp->title().contains(text, false) || tmp->url().contains(text, false)
		        || tmp->artist().contains(text, false) || tmp->album().contains(text, false) )
		{
			tmp->setVisible(true);
			if (tmp->length().contains(':'))
				m_playTimeVisible += timeStringToMs(tmp->length());

			m_countVisible++;
		}
		else
		{
			tmp->setVisible(false);
			if (tmp == m_currentEntry)
			{
				tmp->setPixmap(TITLE_COLUMN, QPixmap());
				m_currentEntry = NULL;
				m_currentRandomListEntry = -1;
			}
		}
		++it;
	}

	if (text.isEmpty())
		m_searchSelection = false;
	else
		m_searchSelection = true;

	if (m_random) createRandomList();
	updateStatus();
}


/***************** cut/copy/paste *************************/

void PlayList::slotCut()
{
	slotCopy();
	slotRemoveSelected();
}

void PlayList::slotPaste()
{
	QPtrList<QListViewItem> selected;
	selected = m_list->selectedItems();
	QListViewItem* lastSelected = NULL;
	if (selected.count())
		lastSelected = selected.at(selected.count() - 1);
	else
		lastSelected = m_list->lastItem();

	QStrList list;

	if (QUriDrag::decode(QApplication::clipboard()->data(), list))
	{
		QStringList urls;
		for (QStrListIterator it(list); *it; ++it)
			urls.append(QUriDrag::uriToUnicodeUri(*it));
		add(urls, lastSelected);
		return;
	}
	/** try to decode as text **/
	QString text;
	if (QTextDrag::decode(QApplication::clipboard()->data(), text))
	{
		add(text, lastSelected);
	}
}

void PlayList::slotCopy()
{
	QPtrList<QListViewItem> selected;
	selected = m_list->selectedItems();

	QStrList urlList;

	for (uint i=0; i<selected.count(); i++)
	{
		urlList.append(QUriDrag::unicodeUriToUri(dynamic_cast<PlaylistItem *>(selected.at(i))->url()));
	}

	QApplication::clipboard()->setData(new QUriDrag(urlList));
}

void PlayList::slotSelectAll()
{
	QListViewItemIterator it(m_list);
	while (it.current())
	{
		if ((*it)->isVisible())
			m_list->setSelected(*it, true);
		++it;
	}
}

void PlayList::slotPlaylistFromSelected()
{
	QPtrList<QListViewItem> selected;
	selected = m_list->selectedItems();

	QValueList<MRL> mrlList;

	for (uint i=0; i<selected.count(); i++)
	{
		mrlList.append(dynamic_cast<PlaylistItem *>(selected.at(i))->toMRL());
	}

	if (mrlList.count())
	{
		slotNewList();
		add(mrlList, NULL);
	}
}

void PlayList::slotAddToQueue(MRL mrl)
{
	m_queue.append(mrl);
	updateStatus();
}

/**** helper ****/

QString PlayList::msToTimeString(int msec)
{
	/*
	QTime t;
	t = t.addMSecs(msec);
	return t.toString("h:mm:ss");
	*/
	/* can be >24h */
	int hours;
	int min;
	int sec;
	int my_msec=msec;
	QString tmp;
	QString t;

	msec = msec/1000;  //sec
	hours = msec/3600;
	my_msec -= hours*3600*1000;
	t = t.setNum(hours);
	t.append(":");

	msec = msec - (hours*3600);
	min = msec / 60;
	my_msec -= min*60*1000;
	tmp = tmp.setNum(min);
	tmp = tmp.rightJustify(2, '0');
	t.append(tmp);
	t.append(":");

	sec = msec - (min*60);
	my_msec -= sec*1000;
	if(my_msec > 500)
		sec++;
	tmp = tmp.setNum(sec);
	tmp = tmp.rightJustify(2, '0');
	t.append(tmp);

	return t;
}

int PlayList::timeStringToMs(const QString& timeString)
{
	int sec = 0;
	QStringList tokens = QStringList::split(':',timeString);

	bool ok = false;
	sec += tokens[0].toInt(&ok)*3600; //hours
	if (!ok)
		return 0;
	sec += tokens[1].toInt(&ok)*60; //minutes
	if (!ok)
		return 0;
	sec += tokens[2].toInt(&ok); //secs

	if (ok)
		return sec*1000; //return millisecs
	else
		return 0;
}

bool PlayList::endsWith(QString s1, QString s2, bool caseSensitive )
{
	if ( s1.isNull() || s2.isNull() )
		return false;
	int startPos = s1.length() - s2.length();

	for (unsigned int i = 0; i < s2.length(); i++ )
	{
		if(caseSensitive)
		{
			if ( s1[startPos + i] != s2[i] )
				return false;
		}
		else
		{
			if ( s1[startPos + i].lower() != s2[i].lower() )
				return false;
		}
	}

	return true;
}

bool PlayList::startsWith(QString s1, QString s2, bool caseSensitive )
{
	if ( s1.isNull() || s2.isNull() )
		return false;

	if(s2.length() > s1.length())
		return false;

	for (unsigned int i = 0; i < s2.length(); i++ )
	{
		if(caseSensitive)
		{
			if ( s1[i] != s2[i] )
				return false;
		}
		else
		{
			if ( s1[i].lower() != s2[i].lower() )
				return false;
		}
	}

	return true;
}

SubtitleChooser::SubtitleChooser(QStringList values, QString filename,QWidget *parent, const char * name) :
		KDialogBase(parent,name,true,i18n("Select Subtitle"), Ok|Cancel, Ok, true)
{
	QVBox *page = makeVBoxMainWidget();
	QLabel *label =  new QLabel(page);
	label->setText("<qt>" + i18n("Media file:") + " <b>" + filename + "</b></qt>");
	table = new QListBox(page);
	this->setMinimumSize(300,200);

	table->setFocus();
	table->insertStringList(values);
	table->setSelected(0, true);
}

SubtitleChooser::~SubtitleChooser(){}

QString SubtitleChooser::getSelection()
{
	return table->selectedItem()->text();
}

MovieChooser::MovieChooser(QStringList values, QString filename,QWidget *parent, const char * name) :
		KDialogBase(parent,name,true,i18n("Select Movie"), Ok|Cancel, Ok, true)
{
	QVBox *page = makeVBoxMainWidget();
	QLabel *label =  new QLabel(page);
	label->setText("<qt> " + i18n("Subtitle file:") + " <b>" + filename + "</b></qt>");
	table = new QListBox(page);
	this->setMinimumSize(450,200);

	table->setFocus();
	table->insertStringList(values);
	table->setSelected(0, true);
}

MovieChooser::~MovieChooser(){}

QString MovieChooser::getSelection()
{
	return table->selectedItem()->text();
}
