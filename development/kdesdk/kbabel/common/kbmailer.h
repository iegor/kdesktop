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


#ifndef KBMAILER_H
#define KBMAILER_H

#include <kbproject.h>
#include <kdemacros.h>
#include <ktempdir.h>

class QWidget;

class KURL;

namespace KBabel
{

/**
 * Utility class for providing the capability to send compressed mail
 * archives from within KBabel and the Catalog Manager.
 *
 * @short Utility class for compressed mail attachments.
 * @author Marco Wegner <mail@marcowegner.de>
 * 
 * @internal
 */
class KDE_EXPORT KBabelMailer
{
  public:
    /**
     * Constructor.
     *
     * @param parent The parent widget.
     * @param project The project to be used.
     * @since 1.11.2 (KDE 3.5.2): @p parent parameter
     */
    KBabelMailer( QWidget* parent, Project::Ptr project);
    virtual ~KBabelMailer();

    /**
     * Send only one file as a mail attachment. The file can either be sent
     * as a compressed or an uncompressed file.
     *
     * @param fileName the name of the file to be sent.
     */
    void sendOneFile(const QString& fileName);
    /**
     * Send only one file as a mail attachment. The file can either be sent
     * as a compressed or an uncompressed file.
     *
     * @param url the URL of the file to be sent.
     * @since 1.11.2 (KDE 3.5.2)
     */
    void sendOneFile(const KURL& url);
    /**
     * Send several files as a mail attachment. The files will be included in
     * an archive.
     *
     * @param fileList contains the names of the files to be sent.
     * @param initialName the possible name of the archive
     */
    void sendFiles(QStringList fileList, const QString& initialName = QString::null);
    /**
     * Sets the the PO files' base directory.
     *
     * @param dir the PO file base dir.
     */
    void setPOBaseDir(const QString& dir) { _poBaseDir = dir; }


    /**
     * This is where the archive is actually created.
     *
     * @param fileList contains the names of the files to be included in
     *  the archive.
     * @param initialName the name used as initial name when the archive
     *  name is queried.
     *
     * @return name of the archive if it was created successfully,
     *  otherwise QString::null.
     */
    QString createArchive(QStringList fileList, QString initialName);
    
    /**
     * Write the archive file.
     */
    QString buildArchive(QStringList fileList, QString fileName, QString mimetype, bool remove = true);
  private:
    /**
     * Read the config file to extract the values for compression
     * method and compression of single files.
     */
    void readConfig();
    /**
     * Save the config settings, only the list of recent archive names for now.
     */
    void saveConfig( );


  private:
    /**
     * Temporary directory
     * @since 1.11.2 (KDE 3.5.2)
     */
    KTempDir m_tempDir;

    /**
     * Whether to use bzip2 as compression method. If false use gzip.
     */
    bool bzipCompression;
    /**
     * Whether to compress single files as well.
     */
    bool singleFileCompression;
    /**
     * This QStringList stores the recently used archive names.
     */
    QStringList archiveList;
    /**
     * The project object.
     */
    Project::Ptr _project;
    /**
     * The path to the PO Base directory
     */
    QString _poBaseDir;

    /// The parent widget
    QWidget* m_parent;
};

}

#endif // KBMAILER_H
