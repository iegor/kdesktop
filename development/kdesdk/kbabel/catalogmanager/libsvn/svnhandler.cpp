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
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
// Qt include files
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>
// KDE include files
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktempfile.h>
#include <kdebug.h>
#include <kprocess.h>
// project specific include files
#include "svnhandler.h"

SVNHandler::SVNHandler( const QString& poBaseDir, const QString& potBaseDir )
{
  setPOBaseDir( poBaseDir );
  setPOTBaseDir( potBaseDir );
  _autoUpdateTemplates = false;
}

void SVNHandler::setPOBaseDir( const QString& dir )
{
  // check if '.svn/entries' exists in the PO base directory
  if ( QFileInfo( dir + "/.svn/entries" ).exists( ) ) {
    _isPORepository = true;
    _poBaseDir = dir;
  } else
    _isPORepository = false;
  emit signalIsPORepository( _isPORepository );
}

void SVNHandler::setPOTBaseDir( const QString& dir )
{
  // check if '.svn/entries' exists in the POT base directory
  if ( QFileInfo( dir + "/.svn/entries" ).exists( ) ) {
    _isPOTRepository = true;
    _potBaseDir = dir;
  } else
    _isPOTRepository = false;
  emit signalIsPOTRepository( _isPOTRepository );
}

QString SVNHandler::fileStatus( const FileStatus status ) const
{
  switch ( status ) {
    case NO_REPOSITORY:
      return i18n( "No SVN repository" );
      break;
    case NOT_IN_SVN:
      return i18n( "Not in SVN" );
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
    case ERROR_IN_WC:
      return i18n( "Error in Working Copy" );
    default:
      return i18n( "Unknown" );
      break;
  }
}

SVNHandler::FileStatus SVNHandler::fstatus( const QString& filename ) const
{
  // no valid repository
  if ( !_isPORepository )
    return NO_REPOSITORY;

  QString fn( filename );
  fn = fn.remove( QRegExp( "/$" ) );

  QFileInfo info( fn );

  // check if '.svn/entries' exists.
  QFile entries( info.dir( true ).path( ) + "/.svn/entries" );

  if ( !entries.exists() )
    return NOT_IN_SVN;

  KProcess proc;
  SVNOutputCollector out( &proc );

  proc << "svn" << "status" << "-v" << "--xml" << info.absFilePath();

  if( !proc.start( KProcess::Block, KProcess::Stdout ) )
    return ERROR_IN_WC;

  QDomDocument doc;
  QString errorMsg;
  int errorLine, errorCol;
  QDomNodeList nodelist;
  QDomNode node;
  QDomElement entry, wcStatus;

  // Parse the output.
  if ( !doc.setContent( out.getOutput(), &errorMsg, &errorLine, &errorCol ) ) {
    kdDebug(8109) << "Cannot parse \"svn status -v --xml\" output for"
        << filename << endl << "Line: " << errorLine << " Column: "
        << errorCol << " Error: " << errorMsg << endl;
    goto no_status_xml;
  }

  // There should be only one "entry" element. If it doesn't exist, path
  // isn't repo path at all.
  nodelist = doc.elementsByTagName("entry");
  if (nodelist.count() < 1)
    return NOT_IN_SVN;

  entry = nodelist.item(0).toElement();

  // Shouldn't fail, but just in case there is some weird error.
  if ( entry.attributeNode("path").value() != info.absFilePath() )
    return ERROR_IN_WC;

  for ( node = entry.firstChild(); !node.isNull(); node = node.nextSibling() ) {
    if ( !node.isElement() )
      continue;
    if (node.toElement().tagName() == "wc-status")
      break;
  }

  if ( node.isNull() )
    return ERROR_IN_WC;

  wcStatus = node.toElement();

  if ( wcStatus.attributeNode("item").value() == "normal" )
    return UP_TO_DATE;
  if ( wcStatus.attributeNode("item").value() == "modified" )
    return LOCALLY_MODIFIED;
  if ( wcStatus.attributeNode("item").value() == "conflicted" )
    return CONFLICT;
  if ( wcStatus.attributeNode("item").value() == "unversioned" )
    return NOT_IN_SVN;
  // TODO Ignored entry should have separate return value probably.
  if ( wcStatus.attributeNode("item").value() == "ignored" )
    return NOT_IN_SVN;
  if ( wcStatus.attributeNode("item").value() == "added" )
    return LOCALLY_ADDED;
  if ( wcStatus.attributeNode("item").value() == "deleted" )
    return LOCALLY_REMOVED;
  // TODO What to do with "missing", "incomplete", "replaced", "merged",
  // "obstructed", "external"? Can these appear at all in our case?

  return ERROR_IN_WC;

no_status_xml:
  if ( !entries.open( IO_ReadOnly ) )
    return ERROR_IN_WC;  // we already know that it is a repository

  // Parse the entries file
  if ( !doc.setContent( &entries, &errorMsg, &errorLine, &errorCol ) ) {
    kdDebug() << "Cannot parse .svn/entries file for " << filename << endl
        << "Line: " << errorLine << " Column: " << errorCol << " Error: " << errorMsg << endl;
    return ERROR_IN_WC;
  }
  entries.close();

  QDomElement element;
  // File name that we are searching
  const QString findName = info.fileName();
  // The entries are <entry> elements, so we have to check them
  QDomNode child = doc.documentElement().firstChild();
  for ( ; !child.isNull() ; child = child.nextSibling() )
  {
    if ( !child.isElement() )
      continue;
    element = child.toElement();
    if ( element.tagName() != "entry" ) {
      // We have another kind of element, so skip it
      // Should not happend with svn 1.1.x
      continue;
    }
    const QString name = element.attribute("name");
    if ( name == findName )
      break;
  }

  if ( child.isNull() ) {
    // We have not found an entry for the file
    return NOT_IN_SVN;
  }

  // ### TODO: should we check the attribute kind to be file and not dir?

  // ### TODO: what do copy and move add here?
  const QString onSchedule = element.attribute( "schedule" );
  if ( onSchedule == "delete" )
    return LOCALLY_REMOVED;
  else if ( onSchedule == "added" )
    return LOCALLY_ADDED;

  if ( element.hasAttribute( "conflict-new" ) || element.hasAttribute( "conflict-old" ) || element.hasAttribute( "conflict-wrk" ) ) {
    return CONFLICT;
  }

  // Note: we do not check the property time stamp
  const QString textTimeStamp( element.attribute( "text-time" ) );

  // calculate the UTC time from the file's last modified date
  struct stat st;
  lstat( QFile::encodeName(fn), &st );
  struct tm * tm_p = gmtime( &st.st_mtime );
  const int year = tm_p->tm_year + 1900;
  const int month = tm_p->tm_mon + 1;
  QString fileTime;
  fileTime.sprintf( "%04i-%02i-%02iT%02i:%02i:%02i.000000Z",
                    year, month, tm_p->tm_mday, tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec );
  //kdDebug() << "File: " << filename << " SVN time: " << textTimeStamp << " File time: " << fileTime << endl;
  if ( fileTime > textTimeStamp ) // ISO 8601 dates/times can be compared as strings if they have the exact same format.
    return LOCALLY_MODIFIED;

  return UP_TO_DATE;

}

QString SVNHandler::svnStatus( const QString& filename ) const
{
  return map[filename];
}

void SVNHandler::execSVNCommand( QWidget* parent, SVN::Command cmd, const QString& filename, bool templates, KSharedConfig* config)
{
    // Unlike cvs, svn works also from outside the repository(as long as the path is in a repository of course!)
    // ### FIXME: wrong, svn commit cannot work if the current directory is not a SVN one
    execSVNCommand( parent, cmd, QStringList( filename ), templates, config );
}

void SVNHandler::execSVNCommand( QWidget* parent, SVN::Command cmd, const QStringList& files, bool templates, KSharedConfig* config )
{
  if ( !_isPORepository ) {
    // This message box should never be visible but who knows... ;-)
    KMessageBox::sorry( parent, i18n( "This is not a valid SVN repository. "
      "The SVN commands cannot be executed." ) );
    return;
  }

  // ### TODO: instead of making a QString, use KProcess directly, so that it cares about quoting.
  // ### TODO: use KProcess::setWorkingDirectory instead of using "cd" (therefore allowing to use KProcess without a shell.)
  QString command("cd " + (templates ? _potBaseDir : _poBaseDir) + " && svn ");
  switch ( cmd ) {
    case SVN::Update:
      command += "update --non-interactive";
      break;
    case SVN::Commit:
      // The svn client allows to choose the encoding, so we select UTF-8
      command += "commit -F @LOG@FILE@ --encoding UTF-8 --non-interactive";
      checkToAdd( files );
      break;
    case SVN::StatusRemote:
      command += "status -u --non-interactive";
      break;
    case SVN::StatusLocal:
      command += "status --non-interactive";
      break;
    case SVN::Diff:
      command += "diff --non-interactive";
      break;
    case SVN::Info:
      command += "info"; // Does not allow  --non-interactive (at least svn 1.1.4).
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

void SVNHandler::setAutoUpdateTemplates( bool update )
{
  _autoUpdateTemplates = update;
}

void SVNHandler::showDialog( QWidget* parent, SVN::Command cmd, const QStringList& files, const QString& commandLine, KSharedConfig* config )
{
  SVNDialog * dia = new SVNDialog( cmd, parent, config );
  dia->setFiles( files );
  dia->setCommandLine( commandLine );
  if ( cmd == SVN::Commit ) {
    dia->setAddCommand( _addCommand );
  }

  if ( dia->exec( ) == KDialog::Accepted ) {
    if ( cmd == SVN::StatusLocal || cmd == SVN::StatusRemote )
      processStatusOutput( dia->statusOutput( ) );
    if ( cmd == SVN::Diff )
      processDiff( dia->statusOutput( ) );
  }

  delete dia;

  // file status display update necessary in Catalog Manager
  if ( cmd == SVN::Commit )
    emit signalFilesCommitted( files );
}

bool SVNHandler::isInSvn( const QString& path )
{
    if ( path.isEmpty() )
        return false;

    /*
     * We need to check if a file is in a SVN repository.
     *
     * But as we want to do it quickly (because this function will be called for a few files in a row)
     * we should avoid to parse the .svn/entries files
     *
     * Therefore we only check for the SVN auxilary files that are typical for a file controlled by SVN:
     * - for a directory: checks if the directory has a .svn/entries file
     * - for a file: check if there is a corresponding file in .svn/text-base/
     */

    const QFileInfo info( path );
    if ( info.isDir() ) {
        // It is a directory, so find a .svn/entries file
        QDir dir( path );
        return dir.exists( ".svn/entries", true );
    }
    else {
        // It is a file, so find the corresponding file in .svn/text-base
        QDir dir( info.dirPath() );
        if ( ! dir.cd( ".svn/text-base" ) ) {
            // There is not even a .svn/text-base directory, so the file is not under control
            return false;
        }
        const QString textBaseFilename( info.fileName() + ".svn-base" );
        return dir.exists( textBaseFilename, true );
    }
}

void SVNHandler::checkToAdd( const QStringList& files )
{
  if ( files.isEmpty( ) )
    return;

  QStringList toBeAdded;

  QStringList::ConstIterator it;
  for ( it = files.begin( ); it != files.end( ); ++it ) {
    // check for every entry if it needs to be added
    if ( ! isInSvn( *it ) ) {
      QFileInfo info( *it );
      QString temp;    // will hold the dir path
      if ( info.isDir( ) ) {
        toBeAdded << *it;
        temp = *it;
      } else {
        toBeAdded << *it;
        temp = QFileInfo( *it ).dirPath( true );
      }

      // ### TODO: does SVN really needs this or does it do it automatically?
      // check recursivlely if parent dirs have to be added as well
      while ( ! isInSvn( temp ) && toBeAdded.findIndex( temp ) == -1 ) {
        toBeAdded << temp;
        temp = QFileInfo( temp ).dirPath( true );
      }

    }
  }

  // remove an old command
  _addCommand = QString();

  // ### TODO: does SVN really need this?
  // make sure the directories are added before the files
  toBeAdded.sort( );

  // ### TODO: try to make this better
  // create a command line for adding the files and dirs
  for ( it = toBeAdded.begin( ); it != toBeAdded.end( ); ++it ) {
    QFileInfo info( *it );
    _addCommand += "cd " + info.dirPath( true ) + " && svn add " + info.fileName( ) + "; ";
  }
}

// ### TODO: convert to SVN
void SVNHandler::processStatusOutput( const QString& status )
{
  if ( !_isPORepository )
    return;

#if 0
  // at first we need to extract the name of the base directory on the server
  QFile f( _poBaseDir + "/SVN/Root" ); // ### FIXME
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
#endif
}

void SVNHandler::processDiff( QString output )
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

bool SVNHandler::isConsideredModified( const FileStatus status ) const
{
  /*
   * A file is modified if it is either:
   * - locally modified for SVN
   * - directory under SVN control but not the file
   */

  // ### TODO: what about moved and copied?
  return status == LOCALLY_MODIFIED || status == NOT_IN_SVN;
}

SVNOutputCollector::SVNOutputCollector( KProcess* p )
  : m_process(0)
{
  setProcess( p );
}

void SVNOutputCollector::setProcess( KProcess* p )
{
  if( m_process )
    m_process->disconnect( this );

  m_process = p;
  if( p ) {
    connect( p, SIGNAL(receivedStdout(KProcess*, char*, int)),
             this, SLOT(slotGatherStdout(KProcess*, char*, int)) );
    connect( p, SIGNAL(receivedStderr(KProcess*, char*, int)),
             this, SLOT(slotGatherStderr(KProcess*, char*, int)) );
  }

  m_gatheredOutput.truncate( 0 );
  m_stderrOutput.truncate( 0 );
  m_stdoutOutput.truncate( 0 );
}

void SVNOutputCollector::slotGatherStderr( KProcess*, char* data, int len )
{
  m_gatheredOutput.append( QString::fromLocal8Bit( data, len ) );
  m_stderrOutput.append( QString::fromLocal8Bit( data, len ) );
}

void SVNOutputCollector::slotGatherStdout( KProcess*, char* data, int len )
{
  m_gatheredOutput.append( QString::fromLocal8Bit( data, len ) );
  m_stdoutOutput.append( QString::fromLocal8Bit( data, len ) );
}

#include "svnhandler.moc"

// kate: space-indent on; indent-width 2; replace-tabs on;
