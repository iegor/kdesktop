#include "serverconfigdialog.h"

#include "bugserverconfig.h"

#include <kpassdlg.h>
#include <kdebug.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qcombobox.h>

ServerConfigDialog::ServerConfigDialog( QWidget *parent, const char *name ) :
  KDialogBase( parent, name, true, i18n("Edit Bugzilla Server"), Ok|Cancel )
{
  QWidget *topFrame = makeMainWidget();

  QGridLayout *topLayout = new QGridLayout( topFrame );
  topLayout->setSpacing( spacingHint() );

  QLabel *label;

  mServerName = new QLineEdit( topFrame );
  label = new QLabel( mServerName, i18n("Name:"), topFrame );
  topLayout->addWidget( label, 0, 0 );
  topLayout->addWidget( mServerName, 0, 1 );
  mServerName->setFocus();

  mServerUrl = new QLineEdit( topFrame );
  label = new QLabel( mServerUrl, i18n("URL:"), topFrame );
  topLayout->addWidget( label, 1, 0 );
  topLayout->addWidget( mServerUrl, 1, 1 );

  mUser = new QLineEdit( topFrame );
  label = new QLabel( mUser, i18n("User:"), topFrame );
  topLayout->addWidget( label, 2, 0 );
  topLayout->addWidget( mUser, 2, 1 );

  mPassword = new KPasswordEdit( topFrame );
  label = new QLabel( mPassword, i18n("Password:"), topFrame );
  topLayout->addWidget( label, 3, 0 );
  topLayout->addWidget( mPassword, 3, 1 );

  mVersion = new QComboBox( topFrame );
  label = new QLabel( mVersion, i18n("Bugzilla version:"), topFrame );
  topLayout->addWidget( label, 4, 0 );
  topLayout->addWidget( mVersion, 4, 1 );
  mVersion->insertStringList( BugServerConfig::bugzillaVersions() );
}

void ServerConfigDialog::setServerConfig( const BugServerConfig &cfg )
{
  mServerName->setText( cfg.name() );
  mServerUrl->setText( cfg.baseUrl().url() );
  mUser->setText( cfg.user() );
  mPassword->setText( cfg.password() );

  int i;
  for( i = 0; i < mVersion->count(); ++i ) {
    if ( mVersion->text( i ) == cfg.bugzillaVersion() ) {
      mVersion->setCurrentItem( i );
      break;
    }
  }
}

BugServerConfig ServerConfigDialog::serverConfig()
{
  BugServerConfig cfg;

  cfg.setName( mServerName->text() );
  cfg.setBaseUrl( KURL( mServerUrl->text() ) );
  cfg.setUser( mUser->text() );
  cfg.setPassword( mPassword->text() );
  cfg.setBugzillaVersion( mVersion->currentText() );

  return cfg;
}

#include "serverconfigdialog.moc"
