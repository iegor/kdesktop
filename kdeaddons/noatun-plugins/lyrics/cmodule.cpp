#include "cmodule.h"
#include "lyrics.h"
#include <qlayout.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <kmessagebox.h>
#include <qvgroupbox.h>
#include <qgrid.h>
#include <klistbox.h>
#include <klineedit.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>

extern Lyrics *lyrics;

const char *const DEFAULT_NAME =
  "Google,"
  "Pure Lyrics,"
  "Sing365,"
  "Lyrics Planet,"
  "Lyrics World,"
  "Get Lyrics,"
  "AZLyrics,"
  "Astraweb,"
  "SongMeanings,"
  "Google (Feeling Lucky),"
  "Everything2,"
  "Everything2 (author info)";

const char *const DEFAULT_QUERY =
  "http://www.google.com/search?q=lyrics+$(title)+$(author)+$(album),"
  "http://www.purelyrics.com/index.php?search_artist=$(author)&search_album=$(album)&search_title=$(title)&search_lyrics=&search_advsubmit2=Search,"
  "http://search.sing365.com/search.php?searchstr=$(title)&submit=search&category=song,"
  "http://www.lyricsplanet.com/index.php3?style=searchtitle&fix=1&searchstring=$(title),"
  "http://www.lyricsworld.com/cgi-bin/search.cgi?q=$(title)+$(author),"
  "http://www.getlyrics.com/search.php?Song=$(title),"
  "http://www.azlyrics.com/cgi-bin/s.cgi?q=$(title)+$(author),"
  "http://search.lyrics.astraweb.com?word=$(title)+$(author)+$(album),"
  "http://www.songmeanings.net/search.php?type=titles&query=$(title),"
  "http://www.google.com/search?q=lyrics+%22$(title)%22+%22$(author)%22+%22$(album)%22&btnI=I%27m+Feeling+Lucky,"
  "http://everything2.com/index.pl?node=$(title),"
  "http://everything2.com/index.pl?node=$(author),"
  "http://www.letssingit.com/cgi-exe/am.cgi?a=search&p=1&s=$(title)&l=song";

LyricsCModule::LyricsCModule(QObject *_parent) : CModule(i18n("Lyrics"), i18n("Configure Lyrics Plugin"), "document", _parent) {
  /* Thanks to the kde-usability guys for the help designing this dialog!
   * help to simon edwards of KGuardGod, for a big help designing it */
  QVBoxLayout *vlayout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
  QHBoxLayout *hlayout = new QHBoxLayout(vlayout, KDialog::spacingHint());
  vlayout->setStretchFactor( hlayout, 1 );

  // Search box
  QVBoxLayout *boxlayout = new QVBoxLayout( hlayout, KDialog::spacingHint() );
  boxlayout->addWidget( new QLabel( i18n("Search providers:" ), this ) );
  providersBox = new KListBox( this, "providersBox" );
  boxlayout->addWidget(providersBox);

  boxButtons = new KButtonBox( this, Vertical );
  boxButtons->addButton( i18n( "New Search Provider" ), this, SLOT( newSearch() ) );
  boxButtons->addButton( i18n( "Delete Search Provider" ), this, SLOT( delSearch() ) );
  boxButtons->addButton( i18n( "Move Up" ), this, SLOT( moveUpSearch() ) );
  boxButtons->addButton( i18n( "Move Down" ), this, SLOT( moveDownSearch() ) );
  boxButtons->layout();
  boxlayout->addWidget( boxButtons );

  // Edit box
  QGroupBox *propBox = new QVGroupBox( i18n("Search Provider Properties" ), this );
  QGrid *editGrid = new QGrid(2, propBox );
  editGrid->setSpacing(  propBox->insideSpacing() );
  new QLabel( i18n( "Name:" ), editGrid );
  nameEdit = new KLineEdit( editGrid );
  new QLabel( i18n( "Query:" ), editGrid );
  queryEdit = new KLineEdit( editGrid );
  /* ATTENTION to translators:
   * The property names can't be translated. This means that $(author) must be kept as $(author), $(title) as $(title), etc, or it won't work.*/
  QLabel *textLabel = new QLabel(i18n("For your query, you can use any property of your multimedia item, just enclosing it with a $(property).\n\nSome common properties used are $(title), $(author) and $(album). For example, to search in Google for the author, title and track, just use:\nhttp://www.google.com/search?q=$(author)+$(title)+$(track)"), propBox, "textLabel");
  textLabel->setAlignment(Qt::WordBreak);
  hlayout->addWidget( propBox, 1 );

  /* Signal/slots */
  nameEdit->setEnabled( false );
  queryEdit->setEnabled( false );
  connect( providersBox, SIGNAL( highlighted( QListBoxItem * ) ), this, SLOT( selected( QListBoxItem * ) ) );
  connect( nameEdit, SIGNAL( textChanged( const QString &) ), this, SLOT( nameChanged( const QString & ) ) );
  connect( queryEdit, SIGNAL( textChanged( const QString & ) ), this, SLOT( queryChanged( const QString & ) ) );



  vlayout->addStretch();
  reopen();
  save();
}

void LyricsCModule::newSearch(QString name, QString query) {
  kdDebug(90020) << "New search" << endl;
  SearchProvider s = { name, query };
  mProviders.push_back( s );
  providersBox->insertItem( name );
  providersBox->setCurrentItem( providersBox->count()-1 );
  nameEdit->setEnabled( true );
  queryEdit->setEnabled( true );
//X   if ( providersBox->count() == 1 )
//X     providersBox->setCurrentItem( 0 );
}

void LyricsCModule::selected( QListBoxItem *i ) {
  kdDebug(90020) << "selected" << endl;
  int index = providersBox->index( i );
  if ( index < 0 )
    return;
  if ( nameEdit->text() != mProviders[ index ].name )
    nameEdit->setText( mProviders[ index ].name );
  if ( queryEdit->text() != mProviders[ index ].url )
    queryEdit->setText( mProviders[ index ].url );
}


void LyricsCModule::delSearch() {
  if ( mProviders.size() == 1 ) {
    KMessageBox::sorry( this, i18n( "You must have at least one search provider. The current one will not be removed." ) );
    return;
  }
  int index = providersBox->currentItem();
  QValueVector<SearchProvider>::iterator it;
  for ( it = mProviders.begin(); ( *it ).name != mProviders[ index ].name || ( *it ).url != mProviders[ index ].url; ++it );
  mProviders.erase( it );
  providersBox->removeItem( index );
  providersBox->setSelected( providersBox->currentItem(), true );
}

void LyricsCModule::moveUpSearch() {
  if ( providersBox->currentItem() <= 0 )
    return;
  int index = providersBox->currentItem();
  QString name = mProviders[ index ].name;
  QString url = mProviders[ index ].url;
  mProviders[ index ].name = mProviders[ index-1 ].name;
  mProviders[ index ].url = mProviders[ index-1 ].url;
  mProviders[ index-1 ].name = name;
  mProviders[ index-1 ].url = url;
  providersBox->changeItem( mProviders[ index-1 ].name, index-1 );
  providersBox->changeItem( mProviders[ index ].name, index );
  providersBox->setSelected( index-1, true );
}

void LyricsCModule::moveDownSearch() {
  if ( static_cast<unsigned int>( providersBox->currentItem() ) >= providersBox->count()-1 )
    return;
  int index = providersBox->currentItem();
  QString name = mProviders[ index ].name;
  QString url = mProviders[ index ].url;
  mProviders[ index ].name = mProviders[ index+1 ].name;
  mProviders[ index ].url = mProviders[ index+1 ].url;
  mProviders[ index+1 ].name = name;
  mProviders[ index+1 ].url = url;
  providersBox->changeItem( mProviders[ index+1 ].name, index+1 );
  providersBox->changeItem( mProviders[ index ].name, index );
  providersBox->setSelected( index+1, true );
}

void LyricsCModule::nameChanged( const QString &name ) {
  kdDebug(90020) << "name changed" << endl;
  if ( providersBox->currentItem() < 0 )
    return;
  mProviders[ providersBox->currentItem() ].name = name;
  if ( name != providersBox->text( providersBox->currentItem() ) )
    providersBox->changeItem( name, providersBox->currentItem() );
}

void LyricsCModule::queryChanged( const QString &query ) {
  kdDebug(90020) << "query changed" << endl;
  if ( providersBox->currentItem() < 0 )
    return;
  mProviders[ providersBox->currentItem() ].url = query;
}


void LyricsCModule::save() {
  KConfig *conf = KGlobal::config();
  conf->setGroup( "Lyrics" );
  QStringList queryList, nameList;
  QValueVector<SearchProvider>::iterator it;
  for ( it = mProviders.begin(); it != mProviders.end(); ++it ) {
    kdDebug(90020) << "query:" << ( *it ).url << endl;
    queryList += ( *it ).url;
    nameList += ( *it ).name;
  }
  conf->writeEntry( "queryList", queryList );
  conf->writeEntry( "nameList", nameList );
  /* TODO */
  // APPLY settings
  if ( lyrics )
    lyrics->setProviders( mProviders );
}

void LyricsCModule::reopen() {
  QStringList queryList, nameList;
  KConfig *conf = KGlobal::config();
  mProviders.clear();
  providersBox->clear();
  kdDebug(90020) << "config read" << endl;
  conf->setGroup( "Lyrics" );
  queryList = conf->readListEntry( "queryList" );
  nameList = conf->readListEntry( "nameList" );
  if ( queryList.count() == 0 && nameList.count() == 0 ) {
    queryList = QStringList::split( ",", DEFAULT_QUERY );
    nameList = QStringList::split( ",", DEFAULT_NAME );
  }
  QStringList::Iterator queryIt, nameIt;
  for ( queryIt = queryList.begin(), nameIt = nameList.begin(); queryIt != queryList.end() && nameIt != nameList.end(); ++queryIt, ++nameIt ) {
    kdDebug(90020) << "Read:" << *queryIt << " and " << *nameIt << endl;
    newSearch( *nameIt, *queryIt );
  }
}

#include "cmodule.moc"
