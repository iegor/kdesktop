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


// System include files
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
// Qt include files
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
// KDE include files
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktempfile.h>
// project specific include files
#include "cvshandler.h"


CVSHandler::CVSHandler( const QString& poBaseDir, const QString& potBaseDir )
{
  setPOBaseDir( poBaseDir );
  setPOTBaseDir( potBaseDir );
  _autoUpdateTemplates = false;
}

void CVSHandler::setPOBaseDir( const QString& dir )
{
  // check if 'CVS/Entries' exists in the PO base directory
  if ( QFileInfo( dir + "/CVS/Entries" ).exists( ) ) {
    _isPORepository = true;
    _poBaseDir = dir;
  } else
    _isPORepository = false;
  emit signalIsPORepository( _isPORepository );
}

void CVSHandler::setPOTBaseDir( const QString& dir )
{
  // check if 'CVS/Entries' exists in the POT base directory
  if ( QFileInfo( dir + "/CVS/Entries" ).exists( ) ) {
    _isPOTRepository = true;
    _potBaseDir = dir;
  } else
    _isPOTRepository = false;
  emit signalIsPOTRepository( _isPOTRepository );
}

QString CVSHandler::fileStatus( const FileStatus status ) const
{
  switch ( status ) {
    case NO_REPOSITORY:
      return i18n( "No CVS repository" );
      break;
    case NOT_IN_CVS:
      return i18n( "Not in CVS" );
      break;
    case LOCALLY_ADDED:
      return i18n( "Locally added" );
      break;
    case LOCALLY_REMOVED:
      return i18n( "Locally removed" );
      break;
    case LOCALLY_MODIFIED:
      return i18n( "Locally modified" );
      break;
    case UP_TO_DATE:
      return i18n( "Up-to-date" );
      break;
    case CONFLICT:
      return i18n( "Conflict" );
      break;
    default:
      return i18n( "Unknown" );
      break;
  }
}

CVSHandler::FileStatus CVSHandler::fstatus( const QString& filename ) const
{
  // no valid repository
  if ( !_isPORepository )
    return NO_REPOSITORY;

  QString fn( filename );
  fn = fn.remove( QRegExp( "/$" ) );

  QFileInfo info( fn );

  // check if 'CVS/Entries' exists and can be read
  QFile entries( info.dir( true ).path( ) + "/CVS/Entries" );
  if ( !entries.open( IO_ReadOnly ) )
    return NOT_IN_CVS;  // we already know that it's a repository

  // ### FIXME: it does not take care of CVS/Entries.Log
  // a line in CVS/Entries has the following format:
  // [D]/NAME/REVISION/[CONFLICT+]TIMESTAMP/OPTIONS/TAGDATE
  QRegExp rx( QString( "^D?/%1/" ).arg( info.fileName( ) ) );

  QString temp;
  QTextStream stream( &entries );

  bool isInRepository = false;
  while ( !stream.atEnd() ) {
    temp = stream.readLine( );
    if ( temp.find( rx ) == 0 ) {
      isInRepository = true;
      break;
    }
  }
  entries.close( );

  // no entry found
  if ( !isInRepository )
    return NOT_IN_CVS;

  const QStringList fields = QStringList::split( '/', temp, true );
  // bool isDir = ( fields[0] == "D" );
  const QString cvsname( fields[1] );
  const QString revision( fields[2] );
  const QString timestamp( fields[3] );
  // ignore the other fields for now

  if ( revision == "0" && timestamp == "dummy timestamp" )
    return LOCALLY_ADDED;
  if ( revision.startsWith( "-" ) && timestamp == "dummy timestamp" )
    return LOCALLY_REMOVED;

  // check for conflicts
  if ( timestamp.find( '+' ) >= 0 )
    return CONFLICT;

  // calculate the UTC time from the file's last modified date
  struct stat st;
  lstat( QFile::encodeName(fn), &st );
  struct tm * tm_p = gmtime( &st.st_mtime );
  QString ftime = QString( asctime( tm_p ) );
  ftime.truncate( ftime.length( ) - 1 );
  if ( ftime != timestamp )
    return LOCALLY_MODIFIED;

  return UP_TO_DATE;
}

QString CVSHandler::cvsStatus( const QString& filename ) const
{
  return map[filename];
}

void CVSHandler::execCVSCommand( QWidget* parent, CVS::Command cmd, const QString& filename, bool templates, KSharedConfig* config )
{
  if ( !_isPORepository ) {
    // This message box should never be visible but who knows... ;-)
    KMessageBox::sorry( parent, i18n( "This is not a valid CVS repository. "
      "The CVS commands cannot be executed." ) );
    return;
  }

  QFileInfo info( filename );
  if ( !info.isDir( ) ) {
    execCVSCommand( parent, cmd, QStringList( filename ), templates, config );
    return;
  }

  // ### FIXME: instead of making a QString, use KProcess directly, so that it cares about quoting.
  // ### FIXME: use KProcess::setWorkingDirectory instead of using "cd" (therefore allowing to use KProcess without a shell.)
  // it's a dir
  QString command( "cd " + filename + " && cvs " );
  switch ( cmd ) {
    case CVS::Update:
      command += "update -dP";
      break;
    case CVS::Commit:
      // The cvs client does not care about the encoding, so we cannot set anything here
      command += "commit -F @LOG@FILE@";
      checkToAdd( QStringList( filename ) );
      break;
    case CVS::Status:
      command += "status";
      break;
    case CVS::Diff:
      command += "diff";
      break;
  }

  showDialog( parent, cmd, QStringList( filename ), command, config );
}

void CVSHandler::execCVSCommand( QWidget* parent, CVS::Command cmd, const QStringList& files, bool templates, KSharedConfig* config )
{
  if ( !_isPORepository ) {
    // This message box should never be visible but who knows... ;-)
    KMessageBox::sorry( parent, i18n( "This is not a valid CVS repository. "
      "The CVS commands cannot be executed." ) );
    return;
  }

  // ### FIXME: instead of making a QString, use KProcess directly, so that it cares about quoting.
  // ### FIXME: use KProcess::setWorkingDirectory instead of using "cd" (therefore allowing to use KProcess without a shell.)
  QString command("cd " + (templates ? _potBaseDir : _poBaseDir) + " && cvs ");
  switch ( cmd ) {
    case CVS::Update:
      command += "update -dP";
      break;
    case CVS::Commit:
      command += "commit -F @LOG@FILE@";
      checkToAdd( files );
      break;
    case CVS::Status:
      command += "status";
      break;
    case CVS::Diff:
      command += "diff";
      break;
  }

  QRegExp rx;
  if (templates)
    rx.setPattern(_potBaseDir + "/?");
  else
    rx.setPattern(_poBaseDir + "/?");

  QStringList::ConstIterator it;
  for ( it = files.begin( ); it != files.end( ); ++it ) {
    QString temp = *it;
    temp.remove(rx);
    command += " \'" + temp + "\'";
  }

  showDialog( parent, cmd, files, command, config );
}

void CVSHandler::setAutoUpdateTemplates( bool update )
{
  _autoUpdateTemplates = update;
}

void CVSHandler::showDialog( QWidget* parent, CVS::Command cmd, const QStringList& files, const QString& commandLine, KSharedConfig* config )
{
  CVSDialog * dia = new CVSDialog( cmd, parent, config );
  dia->setFiles( files );
  dia->setCommandLine( commandLine );
  if ( cmd == CVS::Commit ) {
    dia->setAddCommand( _addCommand );
  }

  if ( dia->exec( ) == KDialog::Accepted ) {
    if ( cmd == CVS::Status )
      processStatusOutput( dia->statusOutput( ) );
    if ( cmd == CVS::Diff )
      processDiff( dia->statusOutput( ) );
  }

  delete dia;

  // file status display update necessary in Catalog Manager
  if ( cmd == CVS::Commit )
    emit signalFilesCommitted( files );
}

void CVSHandler::checkToAdd( const QStringList& files )
{
  if ( files.isEmpty( ) )
    return;

  QStringList toBeAdded;

  QStringList::ConstIterator it;
  for ( it = files.begin( ); it != files.end( ); ++it ) {
    // check for every entry if it needs to be added
    if ( fstatus( *it ) == NOT_IN_CVS ) {
      QFileInfo info( *it );
      QString temp;    // will hold the dir path
      if ( info.isDir( ) ) {
        toBeAdded << *it;
        temp = *it;
      } else {
        toBeAdded << *it;
        temp = QFileInfo( *it ).dirPath( true );
      }
      // check recursivlely if parent dirs have to be added as well
      while ( fstatus( temp ) == NOT_IN_CVS && toBeAdded.findIndex( temp ) == -1 ) {
        toBeAdded << temp;
        temp = QFileInfo( temp ).dirPath( true );
      }
    }
  }

  // remove an old command
  _addCommand.setLength( 0 );

  // make sure the diectories are added before the files
  toBeAdded.sort( );

  // create a command line for adding the files and dirs
  for ( it = toBeAdded.begin( ); it != toBeAdded.end( ); ++it ) {
    QFileInfo info( *it );
    _addCommand += "cd " + info.dirPath( true ) + " && cvs add " + info.fileName( ) + "; ";
  }
}

void CVSHandler::processStatusOutput( const QString& status )
{
  if ( !_isPORepository )
    return;

  // at first we need to extract the name of the base directory on the server
  QFile f( _poBaseDir + "/CVS/Root" );
  if ( !f.open( IO_ReadOnly ) )
    return;

  QTextStream stream( &f );
  // extract the string after the last colon in the first line
  QString basedir = stream.readLine( ).section( ':', -1 );

  f.close( );

  // divide the complete status output in little chunks for every file
  QStringList entries = QStringList::split( QRegExp( "={67,67}" ), status );
  QStringList::Iterator it;
  for ( it = entries.begin( ); it != entries.end( ); ++it ) {
    QString entr = *it;
    // translate the filename from repository to local
    QRegExp rx( basedir + ".*,v" );
    int pos = entr.find( rx );
    QString file = _poBaseDir + entr.mid( pos + basedir.length( ),
      rx.matchedLength( ) - basedir.length( ) - 2 );

    entr = "<qt>" + entr + "</qt>";

    // TODO: do some markup

    map.replace( file, entr );
  }
}

void CVSHandler::processDiff( QString output )
{
  output.remove( QRegExp( "\\[ .* \\]$" ));
  output.remove( QRegExp( "^" + i18n("[ Starting command ]" ).replace("[","\\[").replace("]","\\]")));

  KTempFile tmpFile;
  *(tmpFile.textStream()) << output;
  tmpFile.close();

  QString error;
  if ( KApplication::startServiceByName( "Kompare", tmpFile.name(), &error ) )
    KMessageBox::error( 0, error );
}

bool CVSHandler::isConsideredModified( const FileStatus status ) const
{
  /*
   * A file is modified if it is either:
   * - locally modified for CVS
   * - directory under CVS control but not the file
   */
   return status == LOCALLY_MODIFIED || status == NOT_IN_CVS;
}



#include "cvshandler.moc"
