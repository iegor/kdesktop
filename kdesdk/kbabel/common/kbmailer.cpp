/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2003 by Marco Wegner <mail@marcowegner.de>
		2004	  by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006      by Nicolas GOUTTE <goutte@kde.org>

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


#include <qfileinfo.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktar.h>
#include <kurl.h>
#include <kdebug.h>
#include <ktempdir.h>
#include <kinputdialog.h>

#include "kbmailer.h"

using namespace KBabel;

KBabelMailer::KBabelMailer( QWidget* parent, Project::Ptr project)
  : _project( project ), m_parent( parent )
{
  m_tempDir.setAutoDelete( true );
  readConfig();
}

KBabelMailer::~KBabelMailer()
{
  saveConfig( );
}

void KBabelMailer::sendOneFile(const QString& fileName)
{
  if (!singleFileCompression) {
    kapp->invokeMailer("", "", "", "", "", "", fileName);
  } else {
    const QString archive ( createArchive( QStringList( fileName ), QFileInfo( fileName ).baseName() ) );
    if ( !archive.isEmpty() ) {
      kapp->invokeMailer("", "", "", "", "", "", archive);
    }
  }
}

void KBabelMailer::sendOneFile( const KURL& url)
{
#if KDE_IS_VERSION( 3, 5, 0)
  const KURL localUrl( KIO::NetAccess::mostLocalURL( url, m_parent ) );
#else
  const KURL localUrl( url );
#endif
  if ( localUrl.isLocalFile() )
  {
    sendOneFile( localUrl.path() );
    return;
  }

  if (!singleFileCompression) {
    QString fileName( url.filename() );
    if ( fileName.isEmpty() )
    {
      fileName = "attachment";
    }
    // ### TODO: the current implementation has the default to possibly overwrite an already existing temporary file
    QString tempName( m_tempDir.name() );
    tempName += fileName;
    if ( KIO::NetAccess::download( url, tempName, m_parent ) )
      kapp->invokeMailer("", "", "", "", "", "", fileName);
    else
    {
      KMessageBox::error( m_parent, i18n("Error while trying to download file %1.").arg( url.prettyURL() ) );
    }
  }
  else
  {
    const QString archive ( createArchive( QStringList( url.url() ), url.filename() ) );
    if ( !archive.isEmpty() ) {
      kapp->invokeMailer("", "", "", "", "", "", archive);
    }
  }
}

void KBabelMailer::sendFiles(QStringList fileList, const QString& initialName)
{
  const QString archive ( createArchive( fileList, initialName ) );
  if ( !archive.isEmpty() ) {
    kapp->invokeMailer("", "", "", "", "", "", archive);
  }
}

QString KBabelMailer::createArchive(QStringList fileList, QString initialName)
{
  if ( m_tempDir.name().isEmpty() )
  {
    kdWarning() << "KBabelMailer has not a valid temporary directory!" << endl;
    return QString(); // No temporary directory!
  }

  // do nothing if there are no files in the list
  if (fileList.empty())
    return QString::null;

  // determine the name of the archive, do nothing if none is given
  // or Cancel was pressed
  initialName = ( initialName.isEmpty() ? QString("translations") : initialName );
  bool ok = false;
  QStringList list( archiveList );
  list.prepend( initialName );
  QString archiveName ( KInputDialog::getItem( i18n("Save"),
    i18n( "Enter the name of the archive without file extension" ),
    archiveList, 0, true, &ok, m_parent ) );
  if ( !ok || archiveName.isEmpty() )
    return QString();

  // file extensions are determined from the type of compression
  archiveName.remove( QRegExp( "\\.tar\\.(gz|bz2)$" ) );

  // Update the list of archive names, keep only the ten most recent ones.
  archiveList.remove( archiveName );
  archiveList.prepend( archiveName );
  if ( archiveList.count( ) > 10 )
    archiveList.pop_back();

  // set the correct extension and mimetype
  QString mimetype;
  if (bzipCompression) {
    archiveName += ".tar.bz2";
    mimetype = "application/x-bzip2";
  } else {
    archiveName += ".tar.gz";
    mimetype = "application/x-gzip";
  }
  
  return buildArchive (fileList, m_tempDir.name()+archiveName, mimetype, true);
}

QString KBabelMailer::buildArchive(QStringList fileList, QString archiveName, QString mimetype, bool remove)
{
  Q_UNUSED( remove );
  // create a new archive
  KTar archive(archiveName, mimetype);
  if (!archive.open(IO_WriteOnly)) {
    KMessageBox::error( m_parent, i18n("Error while trying to create archive file.") );
    return QString::null;
  }

  // add files to this archive
  QStringList::const_iterator it;
  for (it = fileList.constBegin(); it != fileList.constEnd(); ++it) {
#if KDE_IS_VERSION( 3, 5, 0 )
    // Try to get a local URL instead of a remote one
    const KURL url( KIO::NetAccess::mostLocalURL( KURL::fromPathOrURL( *it ), m_parent ) );
#else
    const KURL url( KURL::fromPathOrURL( *it ) );
#endif
    QString poTempName;
    if ( !KIO::NetAccess::download( url, poTempName, m_parent ) ) {
      KMessageBox::error( m_parent, i18n("Error while trying to read file %1.").arg( url.prettyURL() ) );
      continue;
    }

    // The files in the archive are stored relatively to the PO base dir
    // but only if "PoBaseDir" in the config file is set and the files
    // actually reside in one of its subdirectories. Else they are stored
    // without relative path.
    QString poArchFileName = url.path();
    if (_poBaseDir.isEmpty( ) || poArchFileName.find(_poBaseDir) != 0)
      poArchFileName = QFileInfo( poArchFileName ).fileName();
    else
      poArchFileName.remove( QRegExp( "^" + QRegExp::escape( _poBaseDir ) + "/?" ) );
    if ( !archive.addLocalFile( poTempName, poArchFileName ) ) 
    {
      KMessageBox::error( m_parent, i18n("Error while trying to copy file %1 into archive.").arg( url.prettyURL() ) );
    }
   
    KIO::NetAccess::removeTempFile(poTempName);
  }
  archive.close();

  return archive.fileName();
}

void KBabelMailer::readConfig()
{
  // The relevant variables are never stored in catalogmanagerrc but in
  // project config file. Therefore they are read from the project.

  MiscSettings _settings = _project->miscSettings();
  
  bzipCompression = _settings.useBzip;
  singleFileCompression = _settings.compressSingleFile;

  KConfig* conf = _project->config();
  
  conf->setGroup ("Misc");
  archiveList = conf->readListEntry("MailArchiveNames");

  _poBaseDir = _project->catManSettings().poBaseDir;
}

void KBabelMailer::saveConfig()
{
  // For an explanation see readConfig( )
  KConfig* _config = _project->config();

  _config->setGroup("Misc");
  _config->writeEntry("MailArchiveNames", archiveList);
}

