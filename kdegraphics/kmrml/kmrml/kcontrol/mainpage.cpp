/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qcheckbox.h>
#include <qlabel.h>
#include <qsizepolicy.h>
#include <qtooltip.h>
#include <qwidget.h>
#include <qvgroupbox.h>

#include <kcombobox.h>
#include <kdialog.h>
#include <keditlistbox.h>
#include <kglobalsettings.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprogress.h>
#include <kurlrequester.h>

#include <kdeversion.h>
#include <kdebug.h>
#include <kio/slaveconfig.h>
#include <kio/ioslave_defaults.h> // MAX_PORT_VALUE

#include "serverconfigwidget.h"
#include "mainpage.h"
#include "indexer.h"
#include "indexcleaner.h"

#include <limits.h>
#include <string.h>

using namespace KMrmlConfig;


MainPage::MainPage( QWidget *parent, const char *name )
    : QVBox( parent, name ),
      m_indexer( 0L ),
      m_indexCleaner( 0L ),
      m_progressDialog( 0L ),
      m_performIndexing( false ),
      m_locked( false )
{
    m_config = new KMrml::Config();
    setSpacing( KDialog::spacingHint() );

    QVGroupBox *gBox = new QVGroupBox( i18n("Indexing Server Configuration"),
                                       this );
    m_serverWidget = new ServerConfigWidget( gBox, "server config widget" );
    QString tip = i18n("Hostname of the Indexing Server");
    QToolTip::add( m_serverWidget->m_hostLabel, tip );
    QToolTip::add( m_serverWidget->m_hostCombo, tip );

    m_serverWidget->m_portInput->setRange( 0, MAX_PORT_VALUE );

#if KDE_VERSION >= 306
    KURLRequester *requester = new KURLRequester( this, "dir requester" );
    requester->setMode( KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly );
    requester->setURL( KGlobalSettings::documentPath() );
    connect( requester, SIGNAL( openFileDialog( KURLRequester * )),
             SLOT( slotRequesterClicked( KURLRequester * )));

    m_listBox = new KEditListBox( i18n("Folders to Be Indexed" ),
                                  requester->customEditor(), this, "listbox",
                                  false,
                                  KEditListBox::Add |  KEditListBox::Remove );
#else
    m_listBox = new KEditListBox( i18n("Folders to Be Indexed" ),
                                  this, "listbox", false,
                                  KEditListBox::Add |  KEditListBox::Remove );
#endif

    connect( m_listBox, SIGNAL( changed() ), SLOT( slotDirectoriesChanged() ));
    connect( m_serverWidget->m_hostCombo, SIGNAL( textChanged(const QString&)),
             SLOT( slotHostChanged() ));
    connect( m_serverWidget->m_portInput, SIGNAL( valueChanged( int )),
             SLOT( slotPortChanged( int ) ));
    connect ( m_serverWidget->m_useAuth, SIGNAL( toggled(bool) ),
              SLOT( slotUseAuthChanged( bool ) ));
    connect( m_serverWidget->m_userEdit, SIGNAL( textChanged( const QString&)),
             SLOT( slotUserChanged( const QString& ) ));
    connect( m_serverWidget->m_passEdit, SIGNAL( textChanged( const QString&)),
             SLOT( slotPassChanged( const QString& ) ));

    connect( m_serverWidget->m_addButton, SIGNAL( clicked() ),
             SLOT( slotAddClicked() ));
    connect( m_serverWidget->m_removeButton, SIGNAL( clicked() ),
             SLOT( slotRemoveClicked() ));

    connect( m_serverWidget->m_hostCombo, SIGNAL( activated( const QString& )),
             SLOT( slotHostActivated( const QString& )));
    connect( m_serverWidget->m_hostCombo, SIGNAL( returnPressed() ),
             SLOT( slotAddClicked() ));

    connect( m_serverWidget->m_autoPort, SIGNAL( toggled( bool ) ),
             SLOT( slotAutoPortChanged( bool ) ));

    m_serverWidget->m_hostCombo->setTrapReturnKey( true );
    m_serverWidget->m_hostCombo->setFocus();
}

MainPage::~MainPage()
{
    delete m_config;
}

void MainPage::resetDefaults()
{
    blockSignals( true );

    initFromSettings( KMrml::ServerSettings::defaults() );

    m_serverWidget->m_hostCombo->clear();
    m_serverWidget->m_hostCombo->insertItem( m_settings.host );

    m_listBox->clear();

    // slotHostChanged(); not necessary, will be called by Qt signals
    slotUseAuthChanged( m_serverWidget->m_useAuth->isChecked() );

    blockSignals( false );
}

void MainPage::load()
{
    blockSignals( true );

    initFromSettings( m_config->defaultSettings() );

    m_serverWidget->m_hostCombo->clear();
    m_serverWidget->m_hostCombo->insertStringList( m_config->hosts() );
    m_serverWidget->m_hostCombo->setCurrentItem( m_settings.host );

    m_listBox->clear();
    m_listBox->insertStringList( m_config->indexableDirectories() );

    // slotHostChanged(); not necessary, will be called by Qt signals
    slotUseAuthChanged( m_serverWidget->m_useAuth->isChecked() );

    blockSignals( false );
}

void MainPage::save()
{
    m_config->addSettings( m_settings );
    m_config->setDefaultHost( m_settings.host );

    QStringList indexDirs = m_listBox->items();
    QStringList oldIndexDirs = m_config->indexableDirectories();
    QStringList removedDirs = difference( oldIndexDirs, indexDirs );

    m_config->setIndexableDirectories( indexDirs );
    if ( indexDirs.isEmpty() )
        KMessageBox::information( this,
                                  i18n("You did not specify any folders to "
                                       "be indexed. This means you will be "
                                       "unable to perform queries on your "
                                       "computer."),
                                  "kcmkmrml_no_directories_specified" );

    if ( m_config->sync() )
        KIO::SlaveConfig::self()->reset();

    processIndexDirs( removedDirs );
}

QStringList MainPage::difference( const QStringList& oldIndexDirs,
                                  const QStringList& newIndexDirs ) const
{
    QStringList result;

    QString slash = QString::fromLatin1("/");
    QStringList::ConstIterator oldIt = oldIndexDirs.begin();
    QString oldDir, newDir;

    for ( ; oldIt != oldIndexDirs.end(); oldIt++ )
    {
        bool removed = true;
        oldDir = *oldIt;

        while ( oldDir.endsWith( slash ) ) // remove slashes
            oldDir.remove( oldDir.length() - 1, 1 );

        QStringList::ConstIterator newIt = newIndexDirs.begin();
        for ( ; newIt != newIndexDirs.end(); newIt++ )
        {
            newDir = *newIt;
            while ( newDir.endsWith( slash ) ) // remove slashes
                newDir.remove( newDir.length() - 1, 1 );

            if ( oldDir == newDir )
            {
                removed = false;
                break;
            }
        }

        if ( removed )
            result.append( *oldIt ); // not oldDir -- maybe gift needs slashes
    }

    return result;
}

void MainPage::initFromSettings( const KMrml::ServerSettings& settings )
{
    m_settings = settings;

    m_locked = true;

    m_serverWidget->m_portInput->setValue( settings.configuredPort );
    m_serverWidget->m_autoPort->setChecked( settings.autoPort );
    m_serverWidget->m_useAuth->setChecked( settings.useAuth );
    m_serverWidget->m_userEdit->setText( settings.user );
    m_serverWidget->m_passEdit->setText( settings.pass );

    m_locked = false;
}

void MainPage::slotHostActivated( const QString& host )
{
    // implicitly save the current settings when another host was chosen
    m_config->addSettings( m_settings );

    initFromSettings( m_config->settingsForHost( host ) );
}

void MainPage::slotHostChanged()
{
    QString host = m_serverWidget->m_hostCombo->currentText();
    m_listBox->setEnabled( (host == "localhost") );

    KMrml::ServerSettings settings = m_config->settingsForHost( host );
    enableWidgetsFor( settings );
}

void MainPage::slotUseAuthChanged( bool enable )
{
    m_settings.useAuth = enable;
    m_serverWidget->m_userEdit->setEnabled( enable );
    m_serverWidget->m_passEdit->setEnabled( enable );

    if ( enable )
        m_serverWidget->m_userEdit->setFocus();

    if ( !m_locked )
        changed();
}

void MainPage::slotUserChanged( const QString& user )
{
    if ( m_locked )
        return;

    m_settings.user = user;
    changed();
}

void MainPage::slotPassChanged( const QString& pass )
{
    if ( m_locked )
        return;

    m_settings.pass = pass;
    changed();
}

void MainPage::slotPortChanged( int port )
{
    if ( m_locked )
        return;

    m_settings.configuredPort = (unsigned short int) port;
    changed();
}

void MainPage::slotAutoPortChanged( bool on )
{
    if ( m_locked )
        return;

    m_settings.autoPort = on;
    m_serverWidget->m_portInput->setEnabled( !on );
    changed();
}

void MainPage::slotRequesterClicked( KURLRequester *requester )
{
    static bool init = true;
    if ( !init )
        return;

    init = false;

    requester->setCaption(i18n("Select Folder You Want to Index"));
}

void MainPage::slotAddClicked()
{
    QString host = m_serverWidget->m_hostCombo->currentText();
    m_settings.host = host;

    m_config->addSettings( m_settings );
    m_serverWidget->m_hostCombo->insertItem( host );
    m_serverWidget->m_hostCombo->setCurrentItem( host );

    enableWidgetsFor( m_settings );
}

void MainPage::slotRemoveClicked()
{
    QString host = m_serverWidget->m_hostCombo->currentText();
    if ( host.isEmpty() ) // should never happen
        return;

    m_config->removeSettings( host );
    m_serverWidget->m_hostCombo->removeItem( m_serverWidget->m_hostCombo->currentItem() );
    m_serverWidget->m_hostCombo->setCurrentItem( 0 );

    host = m_serverWidget->m_hostCombo->currentText();
    initFromSettings( m_config->settingsForHost( host ) );
}

void MainPage::enableWidgetsFor( const KMrml::ServerSettings& settings )
{
    QString host = settings.host;
    bool enableWidgets = (m_config->hosts().findIndex( host ) > -1);
    m_serverWidget->m_addButton->setEnabled(!enableWidgets && !host.isEmpty());
    m_serverWidget->m_removeButton->setEnabled( enableWidgets &&
                                                !host.isEmpty() &&
                                                host != "localhost" );

    m_serverWidget->m_autoPort->setEnabled( host == "localhost" );
    bool portEnable = enableWidgets && (settings.autoPort ||
                                        !m_serverWidget->m_autoPort->isEnabled());
    m_serverWidget->m_portLabel->setEnabled( portEnable && !m_serverWidget->m_autoPort->isChecked());
    m_serverWidget->m_portInput->setEnabled( portEnable && !m_serverWidget->m_autoPort->isChecked());

    m_serverWidget->m_useAuth->setEnabled( enableWidgets );
    m_serverWidget->m_userLabel->setEnabled( enableWidgets );
    m_serverWidget->m_passLabel->setEnabled( enableWidgets );
    m_serverWidget->m_userEdit->setEnabled( enableWidgets );
    m_serverWidget->m_passEdit->setEnabled( enableWidgets );

    bool useAuth = m_serverWidget->m_useAuth->isChecked();
    m_serverWidget->m_userEdit->setEnabled( useAuth );
    m_serverWidget->m_passEdit->setEnabled( useAuth );
}

void MainPage::slotDirectoriesChanged()
{
    m_performIndexing = true;
    changed();
}

void MainPage::processIndexDirs( const QStringList& removeDirs )
{
    // ### how to remove indexed directories?
    if ( !m_performIndexing ||
         (removeDirs.isEmpty() && m_config->indexableDirectories().isEmpty()) )
        return;

    delete m_progressDialog;
    delete m_indexCleaner;
    m_indexCleaner = 0L;
    delete m_indexer;
    m_indexer = 0L;

    m_progressDialog = new KProgressDialog( this, "indexing dialog",
                                            i18n("Removing old Index Files"),
                                            i18n("Processing..."),
                                            true );
    m_progressDialog->setAutoClose( false );
    m_progressDialog->setMinimumWidth( 300 );
    connect( m_progressDialog, SIGNAL( cancelClicked() ),
             SLOT( slotCancelIndexing() ));

    // argh -- don't automatically show the dialog
    m_progressDialog->setMinimumDuration( INT_MAX );

    if ( !removeDirs.isEmpty() )
    {
        m_indexCleaner = new IndexCleaner( removeDirs, m_config, this );
        connect( m_indexCleaner, SIGNAL( advance( int ) ),
                 m_progressDialog->progressBar(), SLOT( advance( int ) ));
        connect( m_indexCleaner, SIGNAL( finished() ),
                 SLOT( slotMaybeIndex() ) );
        m_indexCleaner->start();
    }
    else
    {
        slotMaybeIndex();
    }
    if ( m_progressDialog )
        m_progressDialog->exec();
}

void MainPage::slotMaybeIndex()
{
    delete m_indexCleaner; // Stop in the name of the law!
    m_indexCleaner = 0L;

    m_progressDialog->setLabel( i18n("Finished.") );

    if ( m_config->indexableDirectories().isEmpty() )
        return;

    if ( KMessageBox::questionYesNo( this,
                                     i18n("The settings have been saved. Now, "
                                          "the configured directories need to "
                                          "be indexed. This may take a while. "
                                          "Do you want to do this now?"),
                                     i18n("Start Indexing Now?"),
                                     i18n("Index"), i18n("Do Not Index"),
                                     "ask_startIndexing"
             ) != KMessageBox::Yes )
        return;
    m_progressDialog->setCaption( i18n("Indexing Folders") );
    m_progressDialog->setLabel( i18n("Processing...") );
    m_progressDialog->progressBar()->setProgress( 0 );

    // do the indexing
    m_indexer = new Indexer( m_config, this, "Indexer" );
    connect( m_indexer, SIGNAL( progress( int, const QString& )),
             SLOT( slotIndexingProgress( int, const QString& ) ));
    connect( m_indexer, SIGNAL( finished( int )),
             SLOT( slotIndexingFinished( int ) ));
    m_indexer->startIndexing( m_config->indexableDirectories() );
}


void MainPage::slotIndexingProgress( int percent, const QString& message )
{
    m_progressDialog->progressBar()->setValue( percent );
    m_progressDialog->setLabel( message );
}

void MainPage::slotIndexingFinished( int returnCode )
{
    if ( returnCode != 0 )
    {
        QString syserr;
        if ( returnCode == 127 )
            syserr = i18n("Is the \"GNU Image Finding Tool\" properly installed?");
        else
	{
            char *err = strerror( returnCode );
            if ( err )
                syserr = QString::fromLocal8Bit( err );
            else
                syserr = i18n("Unknown error: %1").arg( returnCode );
        }

        KMessageBox::detailedError( this, i18n("An error occurred during indexing. The index might be invalid."),
                                    syserr, i18n("Indexing Aborted") );
    }
    else
        m_performIndexing = false;

    delete m_indexer;
    m_indexer = 0L;
    if ( m_progressDialog )
    {
        m_progressDialog->deleteLater();
        m_progressDialog = 0L;
    }
}

void MainPage::slotCancelIndexing()
{
    delete m_indexCleaner;
    m_indexCleaner = 0L;

    delete m_indexer;
    m_indexer = 0L;
    if ( m_progressDialog )
    {
        m_progressDialog->deleteLater();
        m_progressDialog = 0L;
    }
}


#include "mainpage.moc"
