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

#include <stdlib.h>
#include <unistd.h>

#include <qdir.h>
#include <qregexp.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#include "BackupProfileManager.h"

#include "BackupProfileManager.moc"

BackupProfileManager::BackupProfileManager()
{
    _backupProfiles.setAutoDelete( TRUE );

    // Get a list of all available backup profiles.
    QStringList relList;
    (void) KGlobal::dirs()->findAllResources( "appdata", "*.bp", false, true, relList);
    
    for(QStringList::Iterator it = relList.begin();
        it != relList.end();
        it++)
    {
        QString fn = *it;
        // Strip extension
        _backupProfileNames.append( fn.left( fn.length() - 3 ) );
    }
}

BackupProfileManager::~BackupProfileManager()
{
}

BackupProfileManager* BackupProfileManager::_instance = 0;

BackupProfileManager* BackupProfileManager::instance()
{
    if ( _instance == 0 ) {
        _instance = new BackupProfileManager();
    }

    return _instance;
}

const QStringList& BackupProfileManager::getBackupProfileNames()
{
    return _backupProfileNames;
}

BackupProfile* BackupProfileManager::findBackupProfile( const QString & name )
{
    BackupProfile* backupProfile = _backupProfiles[ name ];

    if ( !backupProfile ) {
        backupProfile = new BackupProfile( name );
        _backupProfiles.insert( backupProfile->getName(), backupProfile );
    }

    return backupProfile;
}

void BackupProfileManager::addBackupProfile( BackupProfile* backupProfile )
{
    BackupProfile* old = _backupProfiles[ backupProfile->getName() ];
    if ( old ) {
        removeBackupProfile( old );
    }

    _backupProfileNames.append( backupProfile->getName() );
    _backupProfiles.insert( backupProfile->getName(), backupProfile );

    emit sigBackupProfileAdded( backupProfile );
}

void BackupProfileManager::removeBackupProfile( BackupProfile* backupProfile )
{
    emit sigBackupProfileRemoved( backupProfile );

    // Remove the index file.
    QString filename = locateLocal( "appdata", 
                                    QString(backupProfile->getName()) + ".bp");
    
    unlink( QFile::encodeName(filename) );

    _backupProfileNames.remove( backupProfile->getName() );
    _backupProfiles.remove( backupProfile->getName() );
}

void BackupProfileManager::backupProfileModified( BackupProfile* backupProfile )
{
    emit sigBackupProfileModified( backupProfile );
}
