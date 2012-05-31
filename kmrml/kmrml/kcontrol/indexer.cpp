/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qfile.h>
#include <qregexp.h>

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kprocio.h>

#include "indexer.h"

#include <kdeversion.h>
#if KDE_VERSION < 306
  #define QUOTE( x ) x
#else
  #define QUOTE( x ) KProcess::quote( x )
#endif

using namespace KMrmlConfig;

Indexer::Indexer( const KMrml::Config* config,
                  QObject *parent, const char *name )
    : QObject( parent, name ),
      m_config( config ),
      m_dirCount( 0 )
{
    m_process = new KProcIO();
#if KDE_VERSION >= 306
    m_process->setUseShell( true );
#endif
    m_process->setEnvironment( "LC_ALL", "C" );
    connect( m_process, SIGNAL( processExited( KProcess * )),
             SLOT( processFinished( KProcess * )));
    connect( m_process, SIGNAL( readReady( KProcIO * )),
             SLOT( slotCanRead( KProcIO * )) );
}

Indexer::~Indexer()
{
    delete m_process;
}

void Indexer::startIndexing( const QStringList& dirs )
{
    if ( m_process->isRunning() )
        return;

    m_dirs = dirs;
    m_dirCount = dirs.count();
    processNext();
}

void Indexer::processFinished( KProcess *proc )
{
    // still more directories to index?
    if ( !m_dirs.isEmpty() )
        processNext();
    else
    {
        if ( proc->normalExit() )
            emit finished( proc->exitStatus() );
	else
	    emit finished( -1000 );
    }
}


void Indexer::processNext()
{
    m_currentDir = m_dirs.first();
    m_dirs.pop_front();
    while ( m_currentDir.endsWith( "/" ) )
        m_currentDir.remove( m_currentDir.length() -1, 1 );

    m_process->resetAll();

    QString cmd = m_config->addCollectionCommandLine().simplifyWhiteSpace().stripWhiteSpace();

    // in the commandline, replace %d with the directory to process and
    // %t with the thumbnail dir
    int index = cmd.find( "%d" ); // ### QFile::encodeName()?
    if ( index != -1 )
        cmd.replace( index, 2, QUOTE( m_currentDir ) );
    index = cmd.find( "%t" );
    if ( index != -1 )
        cmd.replace( index, 2, QUOTE(m_currentDir + "_thumbnails") );

//     qDebug("****** command: %s", cmd.latin1());
#if KDE_VERSION >= 306
    *m_process << cmd;
#else
     QStringList params = QStringList::split( ' ', cmd );
     QStringList::Iterator it = params.begin();
     for ( ; it != params.end(); ++it )
         *m_process << *it;
#endif

    emit progress( 0, i18n("<qt>Next Folder: <br><b>%1</b>").arg( m_currentDir ));
    m_process->start();
}

void Indexer::slotCanRead( KProcIO *proc )
{
    static const QString& sprogress = KGlobal::staticQString("PROGRESS: ");
    static const QString& r1 = /* PROGRESS: 1 of 6 done (15%) */
        KGlobal::staticQString( "(\\d+) of (\\d+) done \\((\\d+)%\\)" );

    QString line;
    int bytes = -1;
    while ( (bytes = proc->readln( line )) != -1 )
    {
        // examine the output.
        // We're looking for lines like:
        // PROGRESS: 1 of 6 done (15%)
        // PROGRESS: 99%
        // PROGRESS: 100%

        if ( !line.startsWith( sprogress ) ) // uninteresting debug output
            continue;
        else // parse output
        {
            // cut off "PROGRESS: "
            line = line.mid( sprogress.length() );
            line = line.simplifyWhiteSpace().stripWhiteSpace();
//              qDebug("*** START LINE ***");
//              qDebug("%s", line.latin1());
//              qDebug("*** END LINE ***");

            // case 1: image processing, below 99%
            if ( line.at( line.length() -1 ) == ')' )
            {
                QRegExp regxp( r1 );
                int pos = regxp.search( line );
                if ( pos > -1 )
                {
                    QString currentFile = regxp.cap( 1 );
                    QString numFiles = regxp.cap( 2 );
                    QString percent = regxp.cap( 3 );

//                     qDebug( "current: %s, number: %s, percent: %s", currentFile.latin1(), numFiles.latin1(), percent.latin1());
                    bool ok = false;
                    int perc = percent.toInt( &ok );
                    if ( ok )
                    {
                        uint dirsLeft = m_dirs.count();
                        QString message = i18n( "<qt>Processing folder %1 of %2: <br><b>%3</b><br>File %4 of %5.</qt>").arg( m_dirCount - dirsLeft ).arg( m_dirCount).arg( m_currentDir ).arg( currentFile ).arg( numFiles );
                        emit progress( perc, message );
                    }
                }
            }


            // case 2: file writing, 99% or done, 100%
            else
            {
                QString percent = line.left( line.length() - 1 );

                bool ok = false;
                int number = percent.toInt( &ok );
                if ( ok )
                {
                    QString message = (number == 100) ?
                                  i18n("Finished.") : i18n("Writing data...");
                    emit progress( number, message );
                }
                else
                    kdDebug() << "Error while parsing gift-add-collection.pl output" << endl;
            }
        }
    }
}

#include "indexer.moc"
