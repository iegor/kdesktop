#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qlistview.h>
#include <qhbox.h>

#include <knuminput.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "mailsender.h"
#include "kbbprefs.h"
#include "kbbmainwindow.h"
#include "serverconfigdialog.h"
#include "bugsystem.h"
#include "bugserver.h"
#include "bugserverconfig.h"

#include "preferencesdialog.h"

class ServerItem : public QListViewItem
{
  public:
    ServerItem( QListView *listView, const BugServerConfig &cfg )
      : QListViewItem( listView )
    {
      setServerConfig( cfg );
    }

    void setServerConfig( const BugServerConfig &cfg )
    {
      mServerConfig = cfg;
      setText( 0, cfg.name() );
      setText( 1, cfg.baseUrl().prettyURL() );
      setText( 2, cfg.user() );
      setText( 3, cfg.bugzillaVersion() );
    }

    const BugServerConfig &serverConfig() const { return mServerConfig; }

  private:
    BugServerConfig mServerConfig;
};

class ServerListView : public QListView
{
  public:
    ServerListView( QWidget *parent ) : QListView( parent )
    {
      addColumn( i18n("Name") );
      addColumn( i18n("Base URL") );
      addColumn( i18n("User") );
      addColumn( i18n("Version") );
    }
};

PreferencesDialog::PreferencesDialog( QWidget* parent,  const char* name )
    : KDialogBase ( IconList, i18n("Preferences"), Ok|Apply|Cancel, Ok,
                    parent, name, false, true )
{
    setupServerPage();
    setupAdvancedPage();

    readConfig();
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::setupServerPage()
{
  QFrame *topFrame = addPage( i18n("Servers"), 0,
                                DesktopIcon( "gohome", KIcon::SizeMedium ) );

  QBoxLayout *layout = new QVBoxLayout( topFrame );
  layout->setSpacing( spacingHint() );

  mServerList = new ServerListView( topFrame );
  layout->addWidget( mServerList );

  QHBox *buttonBox = new QHBox( topFrame );
  buttonBox->setSpacing( spacingHint() );
  layout->addWidget( buttonBox );

  QPushButton *addButton = new QPushButton( i18n("Add Server..."), buttonBox );
  connect( addButton, SIGNAL( clicked() ), SLOT( addServer() ) );

  QPushButton *editButton = new QPushButton( i18n("Edit Server..."), buttonBox );
  connect( editButton, SIGNAL( clicked() ), SLOT( editServer() ) );

  QPushButton *removeButton = new QPushButton( i18n("Delete Server"), buttonBox );
  connect( removeButton, SIGNAL( clicked() ), SLOT( removeServer() ) );

  QPushButton *button = new QPushButton( i18n("Select Server From List..."),
                                         topFrame );
  layout->addWidget( button );
  connect( button, SIGNAL( clicked() ), SLOT( selectServer() ) );
  connect( mServerList, SIGNAL( doubleClicked ( QListViewItem *)), this, SLOT( editServer()));
}

void PreferencesDialog::setupAdvancedPage()
{
  QFrame *topFrame = addPage( i18n("Advanced"), 0,
                                DesktopIcon( "misc", KIcon::SizeMedium ) );

  QBoxLayout *layout = new QVBoxLayout( topFrame );
  layout->setSpacing( spacingHint() );

  QButtonGroup *mailGroup = new QButtonGroup( 1, Horizontal,
                                              i18n( "Mail Client" ), topFrame );
  layout->addWidget( mailGroup );

  mKMailButton = new QRadioButton( i18n( "&KMail" ), mailGroup );
  mDirectButton = new QRadioButton( i18n( "D&irect" ), mailGroup );
  mSendmailButton = new QRadioButton( i18n( "&Sendmail" ), mailGroup );

  mShowClosedCheckBox = new QCheckBox( i18n( "Show closed bugs" ), topFrame );
  layout->addWidget( mShowClosedCheckBox );

  mShowWishesCheckBox = new QCheckBox( i18n( "Show wishes" ), topFrame );
  layout->addWidget( mShowWishesCheckBox );

  mShowVotedCheckBox = new QCheckBox(  i18n( "Show bugs with number of votes greater than:" ), topFrame );
  layout->addWidget( mShowVotedCheckBox );

  mMinVotesInput = new KIntNumInput( topFrame );
  mMinVotesInput->setMinValue( 0 );
  connect( mShowVotedCheckBox, SIGNAL(toggled(bool)),
           mMinVotesInput, SLOT(setEnabled(bool)) );
  layout->addWidget( mMinVotesInput );

  mSendBccCheckBox = new QCheckBox( i18n( "Send BCC to myself" ), topFrame );
  layout->addWidget( mSendBccCheckBox );
}

void PreferencesDialog::setDefaults()
{
    KBBPrefs::instance()->setDefaults();
    readConfig();
}

void PreferencesDialog::slotApply()
{
  writeConfig();
}

void PreferencesDialog::slotOk()
{
    writeConfig();
    accept();
}

void PreferencesDialog::slotCancel()
{
    hide();
}

void PreferencesDialog::addServer()
{
  ServerConfigDialog *dlg = new ServerConfigDialog( this );
  int result = dlg->exec();
  if ( result == QDialog::Accepted ) {
    new ServerItem( mServerList, dlg->serverConfig() );
  }
}

void PreferencesDialog::editServer()
{
  ServerItem *item = static_cast<ServerItem *>( mServerList->currentItem() );
  if ( !item ) return;

  ServerConfigDialog *dlg = new ServerConfigDialog( this );
  dlg->setServerConfig( item->serverConfig() );

  int result = dlg->exec();
  if ( result == QDialog::Accepted ) {
    item->setServerConfig( dlg->serverConfig() );
  }
}

void PreferencesDialog::removeServer()
{
  QListViewItem *item = mServerList->currentItem();
  if ( !item ) return;

  delete item;
}

void PreferencesDialog::selectServer()
{
  SelectServerDlg *dlg =new SelectServerDlg( this, "Select Server" );

  int result = dlg->exec();
  if ( result == QDialog::Accepted ) {
      ServerItem *item = dlg->serverSelected();
    if ( item ) {
      new ServerItem( mServerList, item->serverConfig() );
    }
  }
  delete dlg;
}

void PreferencesDialog::createServerItem( ServerListView *listView,
                                          const QString &name,
                                          const QString &url,
                                          const QString &version )
{
  BugServerConfig cfg( name, KURL( url ) );
  cfg.setBugzillaVersion( version );
  new ServerItem( listView, cfg );
}

void PreferencesDialog::readConfig()
{
    int client = KBBPrefs::instance()->mMailClient;
    switch(client) {
      default:
      case MailSender::KMail:
        mKMailButton->setChecked(true);
        break;
      case MailSender::Sendmail:
        mSendmailButton->setChecked(true);
        break;
      case MailSender::Direct:
        mDirectButton->setChecked(true);
        break;
    }
    mShowClosedCheckBox->setChecked( KBBPrefs::instance()->mShowClosedBugs );
    mShowWishesCheckBox->setChecked( KBBPrefs::instance()->mShowWishes );
    mShowVotedCheckBox->setChecked( KBBPrefs::instance()->mShowVoted );
    mMinVotesInput->setValue( KBBPrefs::instance()->mMinVotes );
    mSendBccCheckBox->setChecked( KBBPrefs::instance()->mSendBCC );

    mServerList->clear();
    QValueList<BugServer *> servers = BugSystem::self()->serverList();
    QValueList<BugServer *>::ConstIterator it;
    for( it = servers.begin(); it != servers.end(); ++it ) {
        new ServerItem( mServerList, (*it)->serverConfig() );
    }
}

void PreferencesDialog::writeConfig()
{
    MailSender::MailClient client = MailSender::KMail;

    if (mKMailButton->isChecked()) client = MailSender::KMail;
    if (mSendmailButton->isChecked()) client = MailSender::Sendmail;
    if (mDirectButton->isChecked()) client = MailSender::Direct;

    KBBPrefs::instance()->mMailClient = client;
    KBBPrefs::instance()->mShowClosedBugs = mShowClosedCheckBox->isChecked();
    KBBPrefs::instance()->mShowWishes = mShowWishesCheckBox->isChecked();
    KBBPrefs::instance()->mShowVoted = mShowVotedCheckBox->isChecked();
    KBBPrefs::instance()->mMinVotes = mMinVotesInput->value();
    KBBPrefs::instance()->mSendBCC = mSendBccCheckBox->isChecked();
    KBBPrefs::instance()->writeConfig();

    QValueList<BugServerConfig> servers;
    QListViewItem *item;
    for ( item = mServerList->firstChild(); item;
          item = item->nextSibling() ) {
        servers.append( static_cast<ServerItem *>( item )->serverConfig() );
    }

    BugSystem::self()->setServerList( servers );

    emit configChanged();
}

SelectServerDlg::SelectServerDlg(PreferencesDialog *parent, const char */*name*/ )
    :KDialogBase(parent, 0, true, i18n("Select Server"),
                   KDialogBase::Ok | KDialogBase::Cancel)
{
  list = new ServerListView(this );
  setMainWidget( list );

  parent->createServerItem( list, "KDE",     "http://bugs.kde.org",                       "KDE" );
  parent->createServerItem( list, "GNOME",   "http://bugzilla.gnome.org",                 "2.10" );
  parent->createServerItem( list, "Mozilla", "http://bugzilla.mozilla.org",               "2.17.1" );
  parent->createServerItem( list, "Apache",  "http://nagoya.apache.org/bugzilla/",        "2.14.2" );
  parent->createServerItem( list, "XFree86", "http://bugs.xfree86.org/cgi-bin/bugzilla/", "2.14.2" );
  parent->createServerItem( list, "Ximian",  "http://bugzilla.ximian.com",                "2.10" );
  parent->createServerItem( list, "RedHat",  "http://bugzilla.redhat.com/bugzilla/",      "2.17.1" );
  parent->createServerItem( list, "Mandriva",  "http://qa.mandriva.com/",      "2.17.4" );
  connect( list, SIGNAL( doubleClicked ( QListViewItem *)), this, SLOT( slotDoubleClicked( QListViewItem *)));
}


ServerItem *SelectServerDlg::serverSelected()
{
    return  static_cast<ServerItem *>( list->currentItem() );
}

void SelectServerDlg::slotDoubleClicked( QListViewItem *)
{
    accept();
}

#include "preferencesdialog.moc"
