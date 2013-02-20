/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2003 by Marco Wegner <mail@marcowegner.de>
  Copyright (C) 2005, 2006 by Nicolas GOUTTE <goutte@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */


// Qt include files
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextedit.h>
// KDE include files
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprocess.h>
#include <ktempfile.h>
#include <kmessagebox.h>
#include <kstringhandler.h>
// Project specific include files
#include "svndialog.h"


SVNDialog::SVNDialog( SVN::Command cmd, QWidget * parent, KSharedConfig* config )
  : KDialog( parent, "SVN DIALOG", true ), m_tempFile( 0 ), m_config( config )
{
  _cmd = cmd;
  p=0L;
  setCaption( i18n( "SVN Dialog" ) );

  QString temp;

  QVBoxLayout * layout = new QVBoxLayout( this, 6, 6, "MAIN LAYOUT" );

  // Set the label's text depending on the SVN command.
  switch ( cmd ) {
    case SVN::Update:
      temp = i18n( "Update the following files:" );
      break;
    case SVN::Commit:
      temp = i18n( "Commit the following files:" );
      break;
    case SVN::StatusRemote:
      temp = i18n( "Get remote status for the following files:" );
      break;
    case SVN::StatusLocal:
      temp = i18n( "Get local status for the following files:" );
      break;
    case SVN::Diff:
      temp = i18n( "Get diff for the following files:" );
      break;
    case SVN::Info:
      temp = i18n( "Get information for the following files:" );
      break;
  }
  layout->addWidget( new QLabel( temp, this ) );

  // Widget for showing the list of files.
  filebox = new QListBox( this );
  layout->addWidget( filebox );

  // Add special widgets for 'svn commit'.
  if ( cmd == SVN::Commit ) {
    QLabel * label;

    // Combobox for displaying old log messages.
    label = new QLabel( i18n( "&Old messages:" ), this );
    oldMessages = new QComboBox( this );
    oldMessages->setDuplicatesEnabled( false );
    label->setBuddy( oldMessages );
    layout->addWidget( label );
    layout->addWidget( oldMessages );

    // Textfield for entering a log message.
    label = new QLabel( i18n( "&Log message:" ), this );
    logedit = new QTextEdit( this );
    label->setBuddy( logedit );
    layout->addWidget( label );
    layout->addWidget( logedit );

    connect( oldMessages, SIGNAL( activated( int ) ),
      this, SLOT( slotComboActivated( int ) ) );
  }

  QHBoxLayout * buttons = new QHBoxLayout( 0, 0, 6, "BUTTON LAYOUT" );
  // Add special buttons for 'svn commit'.
  if ( cmd == SVN::Commit ) {
    autoAddBox = new QCheckBox( i18n( "Auto&matically add files if necessary" ), this );
    buttons->addWidget( autoAddBox );
  }
  buttons->addItem( new QSpacerItem( 1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  // Set the main button's text depending on the SVN comand.
  switch ( cmd ) {
    case SVN::Update:
      temp = i18n( "&Update" );
      break;
    case SVN::Commit:
      temp = i18n( "&Commit" );
      break;
    case SVN::StatusRemote:
    case SVN::StatusLocal:
      temp = i18n( "&Get Status" );
      break;
    case SVN::Diff:
      temp = i18n( "&Get Diff" );
      break;
    case SVN::Info:
      temp = i18n( "&Get Information" );
      break;
  }
  mainBtn = new QPushButton( temp, this );
  mainBtn->setDefault( true );
  buttons->addWidget( mainBtn );

  cancelBtn = new QPushButton( i18n( "C&ancel" ), this );
  buttons->addWidget( cancelBtn );
  layout->addLayout( buttons );

  QFrame * line = new QFrame( this );
  line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  layout->addWidget( line );

  layout->addWidget( new QLabel( i18n( "Command output:" ), this ) );

  output = new QTextEdit( this );
  output->setReadOnly( true );
  layout->addWidget( output );

  resize( QSize( 600, 450 ).expandedTo( minimumSizeHint( ) ) );

  if ( cmd == SVN::Commit )
    logedit->setFocus( );

  readSettings( );

  connect( mainBtn, SIGNAL( clicked( ) ), this, SLOT( slotExecuteCommand( ) ) );
  connect( cancelBtn, SIGNAL( clicked( ) ), this, SLOT( reject( ) ) );
}

void SVNDialog::slotComboActivated( int index )
{
  if ( index < 0 || index >= m_logMessages.count() )
    return;
  logedit->setText( m_logMessages[index] );
}

SVNDialog::~SVNDialog()
{
    delete m_tempFile;
    delete p;
}

void SVNDialog::accept( )
{
  saveSettings( );
  KDialog::accept( );
}

void SVNDialog::setFiles( const QStringList& files )
{
  filebox->insertStringList( files );
}

void SVNDialog::setCommandLine( const QString& command )
{
  _commandLine = command;
}

void SVNDialog::setAddCommand( const QString& command )
{
  _addCommand = command;
}

void SVNDialog::slotExecuteCommand( )
{
  // Nothing to do here.
  if ( _commandLine.isEmpty( ) ) return;

  kdDebug() << "Preparing KProcess" << endl;

  // Create a new shell process
  p = new KProcess;
  p->setUseShell( true, "/bin/sh" );

  if ( _cmd == SVN::Commit ) {
    // Include command for 'svn add'.
    if ( autoAddBox->isChecked( ) && !_addCommand.isEmpty( ) )
      _commandLine.prepend( _addCommand );

    const QString msg( logedit->text() );

    if ( msg.isEmpty() )
    {
        // A good commit should never have an empty comment, so ask the user if he really wants it.
        const int res = KMessageBox::warningContinueCancel( this,
            i18n( "The commit log message is empty. Do you want to continue?" ) );
        if ( res != KMessageBox::Continue )
            return;
    }

    // Write the commit log message from the input field to a temporary file
    m_tempFile = new KTempFile;
    m_tempFile->setAutoDelete( true );
    QTextStream* stream = m_tempFile->textStream();
    if ( !stream )
    {
        kdError() << "Could not create QTextStream for file " << m_tempFile->name();
        delete m_tempFile;
        m_tempFile = 0;
        KMessageBox::error( this, i18n( "Cannot open temporary file for writing. Aborting.") );
        return;
    }
    stream->setEncoding( QTextStream::UnicodeUTF8 );
    *stream << msg;
    m_tempFile->close();
    
    if ( m_tempFile->status() )
    {
        kdError() << "Could not write to file " << m_tempFile->name();
        delete m_tempFile;
        m_tempFile = 0;
        KMessageBox::error( this, i18n( "Cannot write to temporary file. Aborting.") );
        return;
    }
    
    // Change the command line to have the real name of the temporary file
    _commandLine.replace( "@LOG@FILE@", KProcess::quote( m_tempFile->name() ) );

    // Update the list of log messages
    if ( !msg.isEmpty() ) {
      const QString shortLog = KStringHandler::csqueeze( msg, 80 );
      

      // Remove the message from the list if it already exists
      m_logMessages.remove( msg );
      // Prepend the current message to the list
      m_logMessages.prepend( msg );

      // At this time of the process, we do not need the combobox anymore, so we do not squeeze the changed strings.
    }
  }

  // Set the KProcess' command line.
  *p << _commandLine;

  connect( p, SIGNAL( receivedStdout( KProcess*, char*, int ) ),
    this, SLOT ( slotProcessStdout( KProcess*, char*, int ) ) );
  connect( p, SIGNAL( receivedStderr( KProcess*, char*, int ) ),
    this, SLOT ( slotProcessStderr( KProcess*, char*, int ) ) );
  connect( p, SIGNAL( processExited( KProcess* ) ),
    this, SLOT( slotProcessExited( KProcess* ) ) );

  output->append( i18n( "[ Starting command ]" ) );

  if ( p->start( KProcess::NotifyOnExit, KProcess::Communication( KProcess::AllOutput ) ) ) {
    // Disable the main button (and the log edit if in commit mode) to
    // indicate activity.
    mainBtn->setEnabled( false );
    if ( _cmd == SVN::Commit )
      logedit->setEnabled( false );
  } else
  {
    kdError() << "Process could not be started." << endl;
    KMessageBox::error( this, i18n( "The process could not be started." ) );
  }
}

void SVNDialog::slotProcessStdout( KProcess*, char * buffer, int len )
{
  output->append( QString::fromLocal8Bit( buffer, len ) );
  // Set the cursor's position at the end of the output.
  output->setCursorPosition( output->lines( ), 0 );

  // If the command is 'svn status' or 'svn diff' collect the output of stdout.
  if ( ( _cmd == SVN::StatusLocal ) || ( _cmd == SVN::StatusRemote ) || ( _cmd == SVN::Diff ) )
    _statusOutput += QString::fromLocal8Bit( buffer, len );
}

void SVNDialog::slotProcessStderr( KProcess*, char * buffer, int len )
{
  // If an error occurs while executing the command display stderr in
  // another color.
  QColor oldColor( output->color( ) );
  output->setColor( Qt::red );
  output->append( QString::fromLocal8Bit( buffer, len ) );
  output->setColor( oldColor );
  output->setCursorPosition( output->lines( ), 0 );
}

void SVNDialog::slotProcessExited( KProcess * p )
{
  if ( p->exitStatus( ) )
    output->append( i18n( "[ Exited with status %1 ]" ).arg( p->exitStatus( ) ) );
  else
    output->append( i18n( "[ Finished ]" ) );

  // The command is finished. Now we can reconnect the main button.
  disconnect( mainBtn, 0, 0, 0 );
  if ( _cmd == SVN::Diff )
    mainBtn->setText( i18n( "&Show Diff" ) );
  else
    mainBtn->setText( i18n( "&Close" ) );
  connect( mainBtn, SIGNAL( clicked( ) ), this, SLOT( accept( ) ) );

  // Reenable the button and the log edit now that the process is finished.
  mainBtn->setEnabled( true );
  if ( _cmd == SVN::Commit )
    logedit->setEnabled( true );
}

QString SVNDialog::statusOutput( )
{
  return _statusOutput;
}

void SVNDialog::readSettings( )
{
  KSharedConfig * config = m_config;
  config->setGroup( "SVNSupport" );

  if ( _cmd == SVN::Commit ) {
    autoAddBox->setChecked( config->readBoolEntry( "AutoAddFiles", true ) );

    // Fill the combobox with old messages.
    m_logMessages.clear();
    m_squeezedLogMessages.clear();
    for ( int cnt = 0; cnt < 10; cnt++ )
      if ( config->hasKey( QString( "CommitLogMessage%1" ).arg( cnt ) ) )
      {
        const QString logMessage = config->readEntry( QString( "CommitLogMessage%1" ).arg( cnt ) );
        if ( !logMessage.isEmpty() )
        {
          // If the message is too long, cut it to 80 characters (or the combo box becomes too wide)
          // ### FIXME: if the string matches the squeezed 80 chars, it might overwrite another entry
          const QString shortLog = KStringHandler::csqueeze( logMessage );
          m_logMessages.append( logMessage );
          m_squeezedLogMessages.append( shortLog );
          oldMessages->insertItem( shortLog );
        }
      }

  }
}

void SVNDialog::saveSettings( )
{
  KSharedConfig * config = m_config;
  config->setGroup( "SVNSupport" );
  if ( _cmd == SVN::Commit ) {
    config->writeEntry( "AutoAddFiles", autoAddBox->isChecked( ) );

    // Write the log messages to the config file.
    int cnt = 0;
    QStringList::const_iterator it;
    for ( it = m_logMessages.constBegin( ); it != m_logMessages.constEnd( ) && cnt < 10 ; ++it, ++cnt )
      config->writeEntry( QString( "CommitLogMessage%1" ).arg( cnt ), *it );
  }
  m_config->sync();
}

#include "svndialog.moc"

// kate: space-indent on; indent-width 2; replace-tabs on;
