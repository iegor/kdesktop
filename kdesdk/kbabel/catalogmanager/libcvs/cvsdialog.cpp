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
#include <qtextcodec.h>
// KDE include files
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kprocess.h>
#include <ktempfile.h>
#include <kmessagebox.h>
#include <kstringhandler.h>
#include <kcombobox.h>
#include <kcharsets.h>
// Project specific include files
#include "cvsdialog.h"


CVSDialog::CVSDialog( CVS::Command cmd, QWidget * parent, KSharedConfig* config )
  : KDialog( parent, "CVSDIALOG", true ), m_config( config )
{
  _cmd = cmd;
  p=0L;
  setCaption( i18n( "CVS Dialog" ) );

  QString temp;

  QVBoxLayout * layout = new QVBoxLayout( this, 6, 6, "MAIN LAYOUT" );

  // Set the label's text depending on the CVS command.
  switch ( cmd ) {
    case CVS::Update:
      temp = i18n( "Update the following files:" );
      break;
    case CVS::Commit:
      temp = i18n( "Commit the following files:" );
      break;
    case CVS::Status:
      temp = i18n( "Get status for the following files:" );
      break;
    case CVS::Diff:
      temp = i18n( "Get diff for the following files:" );
      break;
  }
  layout->addWidget( new QLabel( temp, this ) );

  // Widget for showing the list of files.
  filebox = new QListBox( this );
  layout->addWidget( filebox );

  // Add special widgets for 'cvs commit'.
  if ( cmd == CVS::Commit ) {
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

    label = new QLabel( i18n( "E&ncoding:" ), this );
    m_encodingComboBox = new KComboBox( this );
    label->setBuddy( m_encodingComboBox );
    layout->addWidget( label );
    layout->addWidget( m_encodingComboBox );
    QStringList encodingList;
    // The last encoding will be added at the top of the list, when the seetings will be read.
    encodingList << i18n( "Descriptive encoding name", "Recommended ( %1 )" ).arg( "UTF-8" );
    encodingList << i18n( "Descriptive encoding name", "Locale ( %1 )" ).arg( QTextCodec::codecForLocale()->mimeName() );
    encodingList += KGlobal::charsets()->descriptiveEncodingNames();
    m_encodingComboBox->insertStringList( encodingList );
    
    connect( oldMessages, SIGNAL( activated( int ) ),
      this, SLOT( slotComboActivated( int ) ) );
  }

  QHBoxLayout * buttons = new QHBoxLayout( 0, 0, 6, "BUTTON LAYOUT" );
  // Add special buttons for 'cvs commit'.
  if ( cmd == CVS::Commit ) {
    autoAddBox = new QCheckBox( i18n( "Auto&matically add files if necessary" ), this );
    buttons->addWidget( autoAddBox );
  }
  buttons->addItem( new QSpacerItem( 1, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  // Set the main button's text depending on the CVS comand.
  switch ( cmd ) {
    case CVS::Update:
      temp = i18n( "&Update" );
      break;
    case CVS::Commit:
      temp = i18n( "&Commit" );
      break;
    case CVS::Status:
      temp = i18n( "&Get Status" );
      break;
    case CVS::Diff:
      temp = i18n( "&Get Diff" );
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

  if ( cmd == CVS::Commit )
    logedit->setFocus( );

  readSettings( );

  connect( mainBtn, SIGNAL( clicked( ) ), this, SLOT( slotExecuteCommand( ) ) );
  connect( cancelBtn, SIGNAL( clicked( ) ), this, SLOT( reject( ) ) );
}

void CVSDialog::slotComboActivated( int index )
{
  if ( index < 0 || index >= m_logMessages.count() )
    return;
  logedit->setText( m_logMessages[index] );
}

CVSDialog::~CVSDialog()
{
    delete p;
}

void CVSDialog::accept( )
{
  saveSettings( );
  KDialog::accept( );
}

void CVSDialog::setFiles( const QStringList& files )
{
  filebox->insertStringList( files );
}

void CVSDialog::setCommandLine( const QString& command )
{
  _commandLine = command;
}

void CVSDialog::setAddCommand( const QString& command )
{
  _addCommand = command;
}

void CVSDialog::slotExecuteCommand( )
{
  // Nothing to do here.
  if ( _commandLine.isEmpty( ) ) return;

  kdDebug() << "Preparing KProcess" << endl;
  
  // Create a new shell process
  p = new KProcess;
  p->setUseShell( true, "/bin/sh" );

  if ( _cmd == CVS::Commit ) {
    // Include command for 'cvs add'.
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

    m_encoding = KGlobal::charsets()->encodingForName( m_encodingComboBox->currentText() );
    QTextCodec* codec =  QTextCodec::codecForName( m_encoding.utf8() );

    if ( !codec )
    {
        KMessageBox::error( this, i18n( "Cannot find encoding: %1" ).arg( m_encoding ) );
        return;
    }
    else if ( !codec->canEncode( msg ) )
    {
        const int res = KMessageBox::warningContinueCancel( this,
            i18n( "The commit log message cannot be encoded in the selected encoding: %1.\n"
            "Do you want to continue?" ).arg( m_encoding ) );
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
    stream->setCodec( codec );
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
    if ( _cmd == CVS::Commit )
      logedit->setEnabled( false );
  } else
  {
      kdError() << "Process could not be started." << endl;
      KMessageBox::error( this, i18n( "The process could not be started." ) );
  }
}

void CVSDialog::slotProcessStdout( KProcess*, char * buffer, int len )
{
  output->append( QString::fromLocal8Bit( buffer, len ) );
  // Set the cursor's position at the end of the output.
  output->setCursorPosition( output->lines( ), 0 );

  // If the command is 'cvs status' or 'cvs diff' collect the output of stdout.
  if ( (_cmd == CVS::Status) || (_cmd == CVS::Diff) )
    _statusOutput += QString::fromLocal8Bit( buffer, len );
}

void CVSDialog::slotProcessStderr( KProcess*, char * buffer, int len )
{
  // If an error occurs while executing the command display stderr in
  // another color.
  QColor oldColor( output->color( ) );
  output->setColor( Qt::red );
  output->append( QString::fromLocal8Bit( buffer, len ) );
  output->setColor( oldColor );
  output->setCursorPosition( output->lines( ), 0 );
}

void CVSDialog::slotProcessExited( KProcess * p )
{
  if ( p->exitStatus( ) )
    output->append( i18n( "[ Exited with status %1 ]" ).arg( p->exitStatus( ) ) );
  else
    output->append( i18n( "[ Finished ]" ) );

  // The command is finished. Now we can reconnect the main button.
  disconnect( mainBtn, 0, 0, 0 );
  if ( _cmd == CVS::Diff )
    mainBtn->setText( i18n( "&Show Diff" ) );
  else
    mainBtn->setText( i18n( "&Close" ) );
  connect( mainBtn, SIGNAL( clicked( ) ), this, SLOT( accept( ) ) );

  // Reenable the button and the log edit now that the process is finished.
  mainBtn->setEnabled( true );
  if ( _cmd == CVS::Commit )
    logedit->setEnabled( true );
}

QString CVSDialog::statusOutput( )
{
  return _statusOutput;
}

void CVSDialog::readSettings( )
{
  KSharedConfig * config = m_config;
  config->setGroup( "CVSSupport" );

  if ( _cmd == CVS::Commit ) {
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

    m_encoding = config->readEntry( "CVSEncoding", "UTF-8" );
    m_encodingComboBox->insertItem( i18n( "Descriptive encoding name", "Last choice ( %1 )" ).arg( m_encoding ), 0);
  }
}

void CVSDialog::saveSettings( )
{
  KSharedConfig * config = m_config;
  config->setGroup( "CVSSupport" );
  if ( _cmd == CVS::Commit ) {
    config->writeEntry( "AutoAddFiles", autoAddBox->isChecked( ) );

    // Write the log messages to the config file.
    int cnt = 0;
    QStringList::const_iterator it;
    for ( it = m_logMessages.constBegin( ); it != m_logMessages.constEnd( ) && cnt < 10 ; ++it, ++cnt )
      config->writeEntry( QString( "CommitLogMessage%1" ).arg( cnt ), *it );

    config->writeEntry( "CVSEncoding", m_encoding );
  }
  m_config->sync();
}

#include "cvsdialog.moc"
