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

#ifndef _BackupProfile_h_
#define _BackupProfile_h_

#include <qstring.h>
#include <qstrlist.h>

/**
 * @short This class stores all the information necessary to perform a backup.
 */
class BackupProfile {
    QString  _name;
    QString  _lastSavedName;
    QString  _archiveName;
    QString  _workingDirectory;
    QStringList _relativeFiles;
    QStringList _absoluteFiles;
    bool     _oneFilesystem;
    bool     _incremental;
    QString  _snapshotFile;
    bool     _removeSnapshot;

    void calcRelativeFiles();
public:
    /**
     * Create a new backup profile.
     */
    BackupProfile();

    /**
     * Load the named backup profile from disk.
     *
     * @param name The name of the backup profile.
     */
    BackupProfile( const QString & name );

    /**
     * Destroy the backup profile.
     */
    ~BackupProfile();

    /**
     * Load the backup profile from disk.
     */
    void load();

    /**
     * Save the backup profile to disk.  If the backup profile was renamed,
     * the old backup profile will be removed.
     */
    void save();

    /**
     * Query the name of this backup profile.
     *
     * @return The name of this profile.
     */
    QString getName();

    /**
     * Query the name of the archive.
     *
     * @return The name of the new archive.
     */
    QString getArchiveName();

    /**
     * Query the working directory for the tar command.
     *
     * @return The working directory.
     */
    QString getWorkingDirectory();

    /**
     * Query the list of files to backup, relative to the working directory.
     *
     * @return The file list.
     */
    const QStringList& getRelativeFiles();

    /**
     * Query the list of files to backup, with their full paths.
     *
     * @return The file list.
     */
    const QStringList& getAbsoluteFiles();

    /**
     * Query whether or not to cross filesystem boundaries when performing the
     * backup.
     *
     * @return TRUE if the backup is restricted to a single filesystem, FALSE
     *         if the backup can cross filesystem boundaries.
     */
    bool isOneFilesystem();

    /**
     * Query whether this is to be a GNU incremental backup.
     *
     * @return TRUE if incremental, otherwise FALSE.
     */
    bool isIncremental();

    /**
     * Query the name of the snapshot file to use for an incremental backup.
     *
     * @return The name of the snapshot file.
     */
    QString getSnapshotFile();

    /**
     * Query whether to remove the snapshot file before beginning an
     * incremental backup.  This has the effect of performing a full backup.
     *
     * @return TRUE if the snapshot file should be removed, otherwise FALSE.
     */
    bool getRemoveSnapshot();

    /**
     * Set the name of this backup profile.
     *
     * @param name The name of this profile.
     */
    void setName( const QString & name );
    
    /**
     * Set the name of the archive.
     *
     * @param archiveName The name of the new archive.
     */
    void setArchiveName( const QString & archiveName );

    /**
     * Set the working directory for the tar command.
     *
     * @param workingDirectory The working directory.
     */
    void setWorkingDirectory( const QString & workingDirectory );

    /**
     * Set the list of files to backup, with their full paths.
     *
     * @param files The file list.
     */
    void setAbsoluteFiles( const QStringList& files );

    /**
     * Set whether or not to cross filesystem boundaries when performing the
     * backup.
     *
     * @param oneFilesystem TRUE if the backup is restricted to a single
     *                      filesystem, FALSE if the backup can cross
     *                      filesystem boundaries.
     */
    void setOneFilesystem( bool oneFilesystem );

    /**
     * Set whether this is to be a GNU incremental backup.
     *
     * @param incremental TRUE if incremental, otherwise FALSE.
     */
    void setIncremental( bool incremental );

    /**
     * Set the name of the snapshot file to use for an incremental backup.
     *
     * @param snapshotFile The name of the snapshot file.
     */
    void setSnapshotFile( const QString & snapshotFile );

    /**
     * Set whether to remove the snapshot file before beginning an
     * incremental backup.  This has the effect of performing a full backup.
     *
     * @param removeSnapshot TRUE if the snapshot file should be removed,
     *                       otherwise FALSE.
     */
    void setRemoveSnapshot( bool removeSnapshot );
};

#endif
