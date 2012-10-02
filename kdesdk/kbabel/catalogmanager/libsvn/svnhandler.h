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


#ifndef SVNHANDLER_H
#define SVNHANDLER_H

// Qt include files
#include <qmap.h>
#include <qobject.h>
// Project specific include files
#include "svndialog.h"
#include "svnresources.h"
// Forwarding Qt classes
class QString;
class QStringList;

class KSharedConfig;

/**
 * This class is the backend for SVN support in Catalog Manager.
 *
 * @short  Backend for SVN support in Catalog Manager
 */
class SVNHandler : public QObject
{
  Q_OBJECT

  public:
    enum FileStatus {
      NO_REPOSITORY,
      NOT_IN_SVN,
      LOCALLY_ADDED,
      LOCALLY_REMOVED,
      LOCALLY_MODIFIED,
      CONFLICT,
      UP_TO_DATE,
      ERROR_IN_WC ///< The working copy has data that cannot be handled
    };

    SVNHandler( const QString& poBaseDir = QString::null, const QString& potBaseDir = QString::null );

    void setPOBaseDir( const QString& dir );
    void setPOTBaseDir( const QString& dir );

    FileStatus fstatus( const QString& filename ) const;
    QString fileStatus( const FileStatus status ) const;
    QString svnStatus( const QString& filename ) const;

    void execSVNCommand( QWidget* parent, SVN::Command cmd, const QString& filename, bool templates, KSharedConfig* config );
    void execSVNCommand( QWidget* parent, SVN::Command cmd, const QStringList& files, bool templates, KSharedConfig* config );

    void setAutoUpdateTemplates( bool update );

    /**
     * True if the file was modified or has another status considered as a modification
     */
    bool isConsideredModified( const FileStatus status ) const;

  signals:
    void signalIsPORepository( bool );
    void signalIsPOTRepository( bool );
    void signalFilesCommitted( const QStringList& );

  private:
      void showDialog( QWidget* parent, SVN::Command cmd, const QStringList& files, const QString& commandLine, KSharedConfig* config );
    /// Check quickly if the file is part of a SVN repository
    bool isInSvn( const QString& path );
    void checkToAdd( const QStringList& files );
    void processStatusOutput( const QString& status );
    void processDiff( QString output );

  private:
    QString _poBaseDir;
    QString _potBaseDir;
    bool _isPORepository;
    bool _isPOTRepository;
    bool _autoUpdateTemplates;
    QString _addCommand;

    /** Mapping the output of 'svn status' against the filename. */
    QMap<QString,QString> map;
};

class SVNOutputCollector: public QObject
{
  Q_OBJECT

 public:
  SVNOutputCollector( KProcess* );
  void setProcess( KProcess* );

  const QString& getOutput() const { return m_gatheredOutput; }
  const QString& getStderr() const { return m_stderrOutput; }
  const QString& getStdout() const { return m_stdoutOutput; }

 private slots:
  void slotGatherStderr( KProcess*, char*, int );
  void slotGatherStdout( KProcess*, char*, int );

 private:
  QString m_gatheredOutput;
  QString m_stderrOutput;
  QString m_stdoutOutput;
  KProcess* m_process;
};

#endif // SVNHANDLER_H
