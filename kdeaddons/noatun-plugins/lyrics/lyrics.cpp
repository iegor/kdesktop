#include <klocale.h>
#include <klineeditdlg.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <khtmlview.h>
#include <kstdaction.h>
#include <kstatusbar.h>
#include <qregexp.h>
#include "lyrics.h"
#include <noatun/player.h>
#include <kdebug.h>
#include "historymanager.h"


extern "C"
{
  Plugin *create_plugin()
  {
    KGlobal::locale()->insertCatalogue("lyrics");
    return new Lyrics();
  }
}

Lyrics *lyrics;

Lyrics::Lyrics() : KMainWindow(), Plugin(), active(false)
{
  lyrics = this;
  /* Create default actions */
  (void)KStdAction::close(this, SLOT(close()), actionCollection());
  ( void )KStdAction::goTo( this, SLOT( goTo() ), actionCollection(), "go_web_goTo" );
  //(void)KStdAction::print(this, SLOT(print()), actionCollection());
  //(void)KStdAction::printPreview(this, SLOT(printPreview()), actionCollection());
  //(void)KStdAction::mail(this, SLOT(mail()), actionCollection());
  //(void)KStdAction::find(this, SLOT(find()), actionCollection());
  follow_act = new KToggleAction(i18n("&Follow Noatun Playlist"), "goto", 0, actionCollection(), "follow");
  KStdAction::redisplay(this, SLOT(viewLyrics()), actionCollection());
  attach_act = new KToggleAction(i18n("&Link URL to File"), "attach", KShortcut("CTRL+ALT+A"), actionCollection(), "attach_url");
  connect(attach_act, SIGNAL(toggled(bool)), this, SLOT(attach(bool)));
  back_act = KStdAction::back(this, SLOT(back()), actionCollection());
  back_act->setEnabled(false);
  forward_act = KStdAction::forward(this, SLOT(forward()), actionCollection());
  forward_act->setEnabled(false);
  new KWidgetAction( new QLabel(i18n("Search provider:"), this, "kde toolbar widget"), i18n("Search Provider"), 0, 0, 0, actionCollection(), "search_label");
  site_act = new KSelectAction(i18n("&Search Provider"), 0, this, SLOT(viewLyrics()), actionCollection(), "search_provider");

  /* Add entry to menu */
  menuID = napp->pluginMenuAdd(i18n("&View Lyrics"), this, SLOT(viewLyrics()));

  /* Create history manager and htmlpart */
  history = new HistoryManager(this);
	htmlpart = new KHTMLPart(this);
	//htmlpart->view()->setMinimumSize(350, 420);

  /* Connect signals/slots */
	connect( htmlpart->browserExtension(), SIGNAL(openURLRequestDelayed( const KURL &, const KParts::URLArgs & )), this, SLOT(openURLRequest( const KURL &, const KParts::URLArgs & )));
  connect( htmlpart, SIGNAL(started(KIO::Job *)), this, SLOT(loadingURL(KIO::Job *)) );
  connect( htmlpart, SIGNAL(completed()), this, SLOT(loadedURL()) );
  connect( history, SIGNAL(uiChanged(int, bool)), this, SLOT(changeUI(int, bool)) );
  connect( napp->player(), SIGNAL(newSong()), this, SLOT(newSong()) );

  /* Status bar */
  statusBar()->insertItem(i18n("Ready"), 0, 1);
  statusBar()->setItemAlignment(0, Qt::AlignLeft);

  /* Finalize the GUI */
	setCentralWidget(htmlpart->view());
  createGUI("lyricsui.rc");
  setAutoSaveSettings("Lyrics");

  /* Load configuration */
  KConfig *config = KGlobal::config();
  config->setGroup("Lyrics");
  follow_act->setChecked(config->readBoolEntry("follow", true));
  /* Create config object */
  new LyricsCModule(this);
  
}

void Lyrics::goTo() {
  bool sel = false;
  QString url = KLineEditDlg::getText( i18n( "Please enter the URL you want to go to:" ), htmlpart->url().prettyURL(), &sel, this );
  if ( !url.isEmpty() && sel )
    go( KURL( url ) );
}

void Lyrics::setProviders( QValueVector<SearchProvider> &sites ) {
  mSites = sites;
  QStringList sitesName;
  for (unsigned int i = 0; i < mSites.size(); ++i) {
    sitesName += mSites[i].name;
  }
  site_act->setItems(sitesName);
  site_act->setCurrentItem(0);
}

void Lyrics::attach(bool a) {
  if ( !napp->player()->current() )
    return;
  if (a) {
    KMessageBox::information(this, i18n("Choosing this option, the current URL will be attached to the current file. This way, if you try to view the lyrics of this file later, you won't have to search for it again. This information can be stored between sessions, as long as your playlist stores metadata about the multimedia items (almost all the playlists do). If you want to be able to search for other lyrics for this music, you must select this option again to clear the stored URL."), QString::null, "lyrics::attach_info");
    kdDebug(90020) << "Setting URL for (attach)" << napp->player()->current().title() << endl;
    napp->player()->current().setProperty("Lyrics::URL", htmlpart->url().url());
    site_act->setEnabled(false);
    actionCollection()->action("search_label")->setEnabled(false);
  }
  else {
    kdDebug(90020) << "Clearing URL for " << napp->player()->current().title() << endl;
    napp->player()->current().clearProperty("Lyrics::URL");
    site_act->setEnabled(true);
    actionCollection()->action("search_label")->setEnabled(true);
  }
}

void Lyrics::loadingURL(KIO::Job *)
{
  statusBar()->changeItem(i18n("Loading..."), 0);
}

void Lyrics::loadedURL()
{
  if ( !napp->player()->current() )
    return;
  statusBar()->changeItem(i18n("Loaded"), 0);
  setCaption(i18n("Lyrics: %1").arg(napp->player()->current().property("title")));
  if (!htmlpart->url().url().isEmpty() && napp->player()->current() && !napp->player()->current().property("Lyrics::URL").isEmpty()) {
    kdDebug(90020) << "Setting URL for (loaded)" << napp->player()->current().title() << endl;
    napp->player()->current().setProperty("Lyrics::URL", htmlpart->url().url());
  }
}

void Lyrics::back() 
{
  KURL url = history->back();
  if (url.isEmpty())
    return;
  kdDebug(90020) << "Going to " << url.url() << endl;
  htmlpart->openURL(url);
}

void Lyrics::forward()
{
  KURL url = history->forward();
  if (url.isEmpty())
    return;
  kdDebug(90020) << "Going to " << url.url() << endl;
  htmlpart->openURL(url);
}

void Lyrics::changeUI(int button, bool enable)
{
  if (button == HistoryManager::Back)
    back_act->setEnabled( enable );
  else if (button == HistoryManager::Forward)
    forward_act->setEnabled( enable );
}

void Lyrics::go(const KURL &url)
{
  history->addURL(url);
  kdDebug(90020) << "Going to " << url.url() << endl;
  htmlpart->openURL(url);
}

void Lyrics::newSong() {
  kdDebug(90020) << "New song!" << endl;
  // If I'm not following, just forget about it
  if (!follow_act->isChecked())
    return;
  if (active)
    viewLyrics();
  // TODO: If not visible, maybe it would be a good idea
  // to load the lyrics on advance
}


void Lyrics::viewLyrics(int index)
{
  if (!napp->player()->current()) {
    KMessageBox::sorry(this, i18n("You can only view the lyrics of the current song, and currently there is none."));
    return;
  }
//X <small>(if the above information is incorrect, the search may not find the correct music. If that's the case, use a tag editor to fix it. Hint: Use the luckytag plugin from the kdeaddons module to try to guess title, author and album from the song filename)</small><br>
  QString url, name;
  if (index < 0)
    index = site_act->currentItem();
  if ( index < 0 )
    return;
  url = mSites[index].url;
  name = mSites[index].name;
  QRegExp props_regexp("\\$\\((\\w+)\\)");
  int pos = props_regexp.search(url);
  while (pos >= 0) {
    QString property = props_regexp.cap(1);
    url.replace(pos, props_regexp.matchedLength(), napp->player()->current().property(property));
    pos = props_regexp.search(url);
  }
	QString title(napp->player()->current().property("title"));
  setCaption(i18n("Loading Lyrics for %1").arg(title));
	// Check if we have a music playing
	if (napp->player()->current()) {
  	htmlpart->begin();
  	htmlpart->write(i18n("<HTML>"
  "<BODY>"
    "<p><strong>Please wait! Searching for...</strong></p>"
    "<TABLE BORDER=1 WIDTH=\"100%\">"
      "<TR><TD BGCOLOR=\"#707671\"><strong>Title</strong></TD><TD>%1</TD></TR>"
      "<TR><TD BGCOLOR=\"#707671\"><strong>Author</strong></TD><TD>%2</TD></TR>"
      "<TR><TD BGCOLOR=\"#707671\"><strong>Album</strong></TD><TD>%3</TD></TR>"
    "</TABLE>").arg( napp->player()->current().property( "title" ) ).arg( napp->player()->current().property( "author" ) ).arg( napp->player()->current().property( "album" ) ) );
    KURL _url;
    /* Check if we should use the store url or the query one */
    if (napp->player()->current().property("Lyrics::URL").isEmpty()) {
      /* Use the query one */
      _url = url;
      _url.setQuery(_url.query().replace(QRegExp("%20"), "+"));
      kdDebug(90020) << "I'm using the query url" << endl;
      attach_act->setChecked(false);
      site_act->setEnabled(true);
      actionCollection()->action("search_label")->setEnabled(true);
      htmlpart->write( i18n( "<hr><p><strong>Searching at %1</strong><br><small>(<a href=\"%3\">%2</a></small>)</p>" ).arg( name ).arg( _url.prettyURL() ).arg( _url.url() ) );
    } else {
      _url = napp->player()->current().property("Lyrics::URL");
      _url.setQuery(_url.query().replace(QRegExp("%20"), "+"));
      kdDebug(90020) << "I'm using the stored url" << endl;
      attach_act->setChecked(true);
      site_act->setEnabled(false);
      actionCollection()->action("search_label")->setEnabled(false);
      htmlpart->write( i18n( "<hr><p><strong>Using the stored URL</strong><br><small>(<a href=\"%2\">%1</a></small>)</p>" ).arg( _url.prettyURL() ).arg( _url.url() ) );
    }
    htmlpart->write( "</BODY></HTML>" );
    htmlpart->end();
  	go(_url);
  	this->show();
    KMessageBox::information( this, i18n( "In order to find the lyrics for the current song, this plugin uses the properties stored with each song, such as its title, author and album. These properties are usually retrieved by a tag reader, but in some cases they may not be present or be incorrect. In that case, the Lyrics plugin will not be able to find the lyrics until these properties are fixed (you can fix them using the tag editor).\nHint: The lucky tag plugin, present in the kdeaddons module, can try to guess properties such as title and author from the filename of a song. Enabling it may increase the probability of finding lyrics." ), QString::null, "Lyrics::usage_info" );
    active = true;
  }
}

void Lyrics::openURLRequest( const KURL &url, const KParts::URLArgs & )
{
  go(url);
}

Lyrics::~Lyrics()
{
  /* Save configurations */
  KConfig *config = KGlobal::config();
  config->setGroup("Lyrics");
  config->writeEntry("follow", follow_act->isChecked());
  // Force saving, as closeEvent is probably never called
  saveMainWindowSettings(config, "Lyrics");
	napp->pluginMenuRemove(menuID);
}

bool Lyrics::queryClose()
{
  if( kapp->sessionSaving())
      return true;
	hide();
  htmlpart->closeURL();
  htmlpart->begin();
  htmlpart->end();
  active = false;
  return false;
}

#include "lyrics.moc"
