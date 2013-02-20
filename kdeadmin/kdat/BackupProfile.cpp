// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qfile.h>

#include <kstandarddirs.h>

#include "BackupProfile.h"
#include "BackupProfileManager.h"
#include "Util.h"

BackupProfile::BackupProfile()
{
}

BackupProfile::BackupProfile( const QString & name )
        : _name( name ),
          _lastSavedName( name )
{
    load();
}

BackupProfile::~BackupProfile()
{
}

void BackupProfile::load()
{
    QString filename = locateLocal( "appdata", _lastSavedName + ".bp");
    
    FILE* fptr = fopen( QFile::encodeName(filename), "r" );
    if ( !fptr ) {
        return;
    }

    char buf[4096];

    fgets( buf, 4096, fptr );
    int version = 0;
    sscanf( buf, "%d", &version );

    fgets( buf, 4096, fptr );
    _name = buf;
    _name.truncate( _name.length() - 1 );

    fgets( buf, 4096, fptr );
    _archiveName = buf;
    _archiveName.truncate( _archiveName.length() - 1 );

    fgets( buf, 4096, fptr );
    _workingDirectory = buf;
    _workingDirectory.truncate( _workingDirectory.length() - 1 );

    _absoluteFiles.clear();
    fgets( buf, 4096, fptr );
    int filecount = 0;
    sscanf( buf, "%d", &filecount );
    for ( int i = 0; i < filecount; i++ ) {
        fgets( buf, 4096, fptr );
        QString filename = buf;
        filename.truncate( filename.length() - 1 );
        _absoluteFiles.append( filename );
    }

    calcRelativeFiles();

    fgets( buf, 4096, fptr );
    int tmpBool = 0;
    sscanf( buf, "%d", &tmpBool );
    _oneFilesystem = tmpBool;

    fgets( buf, 4096, fptr );
    sscanf( buf, "%d", &tmpBool );
    _incremental = tmpBool;

    fgets( buf, 4096, fptr );
    _snapshotFile = buf;
    _snapshotFile.truncate( _snapshotFile.length() - 1 );

    fgets( buf, 4096, fptr );
    sscanf( buf, "%d", &tmpBool );
    _removeSnapshot = tmpBool;

    fclose( fptr );
}

void BackupProfile::save()
{
    QString filename = locateLocal( "appdata", _lastSavedName + ".bp");
    bool    null_name_p = _lastSavedName.isEmpty();

    _lastSavedName = _name.copy();
    
    if( TRUE == null_name_p ){
      return;
    }

    unlink( QFile::encodeName(filename) );

    FILE* fptr = fopen( QFile::encodeName(filename), "w" );
    if ( !fptr ) {
        return;
    }

    fprintf( fptr, "%d\n", 1 ); // Version
    fprintf( fptr, "%s\n", _name.utf8().data() );
    fprintf( fptr, "%s\n", _archiveName.utf8().data() );
    fprintf( fptr, "%s\n", _workingDirectory.utf8().data() );
    fprintf( fptr, "%d\n", _absoluteFiles.count() );
    for ( QStringList::Iterator it = _absoluteFiles.begin();
          it != _absoluteFiles.end();
          ++it )
        fprintf( fptr, "%s\n", (*it).utf8().data() );
    fprintf( fptr, "%d\n", _oneFilesystem );
    fprintf( fptr, "%d\n", _incremental );
    fprintf( fptr, "%s\n", _snapshotFile.utf8().data() );
    fprintf( fptr, "%d\n", _removeSnapshot );

    fclose( fptr );
}

QString BackupProfile::getName()
{
    return _name;
}

QString BackupProfile::getArchiveName()
{
    return _archiveName;
}

QString BackupProfile::getWorkingDirectory()
{
      return _workingDirectory;

  // LEW: is this fix necessary?
#ifdef LEW
    if ( FALSE == _workingDirectory.isEmpty() ) {
      return _workingDirectory;
    } else {
      return 0;
    }
#endif
}

const QStringList& BackupProfile::getRelativeFiles()
{
    return _relativeFiles;
}

const QStringList& BackupProfile::getAbsoluteFiles()
{
    return _absoluteFiles;
}

bool BackupProfile::isOneFilesystem()
{
    return _oneFilesystem;
}

bool BackupProfile::isIncremental()
{
    return _incremental;
}

QString BackupProfile::getSnapshotFile()
{
    return _snapshotFile;
}

bool BackupProfile::getRemoveSnapshot()
{
    return _removeSnapshot;
}

void BackupProfile::setName( const QString & name )
{
    _name = name;
    
    BackupProfileManager::instance()->backupProfileModified( this );
}
    
void BackupProfile::setArchiveName( const QString & archiveName )
{
    _archiveName = archiveName;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setWorkingDirectory( const QString & workingDirectory )
{
    _workingDirectory = workingDirectory;

    // Regenerate the list of relative files.
    calcRelativeFiles();
    
    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setAbsoluteFiles( const QStringList& files )
{
    _absoluteFiles = files;

    // Make sure working directory is still valid.
    QStringList::Iterator it = _absoluteFiles.begin();
    for ( ;
          it != _absoluteFiles.end();
          ++it )
    {
        if ( *it != _workingDirectory ) {
            // Working directory is not a prefix for this file.
            break;
        }
    }

    if ( it != _absoluteFiles.end() ) {
        // Pick another working directory.
        _workingDirectory = Util::longestCommonPath( _absoluteFiles );
    }

    // Regenerate the list of relative files.
    calcRelativeFiles();
    
    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setOneFilesystem( bool oneFilesystem )
{
    _oneFilesystem = oneFilesystem;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setIncremental( bool incremental )
{
    _incremental = incremental;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setSnapshotFile( const QString & snapshotFile )
{
    _snapshotFile = snapshotFile;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::setRemoveSnapshot( bool removeSnapshot )
{
    _removeSnapshot = removeSnapshot;

    BackupProfileManager::instance()->backupProfileModified( this );
}

void BackupProfile::calcRelativeFiles()
{
    _relativeFiles.clear();
    int remove = _workingDirectory.length();
    if ( remove > 1 ) {
        remove++;
    }

    for ( QStringList::Iterator it = _absoluteFiles.begin();
          it != _absoluteFiles.end();
          ++it )
    {
        QString fn = *it;
        fn.remove( 0, remove );
        if ( fn.isEmpty() ) {
            fn = ".";
        }
        _relativeFiles.append( fn );
    }
}
