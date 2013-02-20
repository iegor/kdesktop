/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#include <kapplication.h>
#include <kdebug.h>
#include "kio.h"

Kio::Kio()
{
}

bool Kio::download(const KURL & from, const QString & to)
{
    KIO::Job *iojob = KIO::file_copy(from, to);
    connect( iojob, SIGNAL( result(KIO::Job*) ),
	     SLOT( slotIOJobFinished( KIO::Job* )));
    // missing modal widget hack here.
    // I'd recommend using KIO::NetAccess instead (David).
    kapp->enter_loop();
    return worked;
}

void Kio::slotIOJobFinished( KIO::Job * job)
{
    worked = (job->error() == 0);
    kapp->exit_loop();
}

Kiod::Kiod()
{
    file=0L;
    fileT = 0L;
}

Kiod::~Kiod()
{
    delete file;
    delete fileT;
}

bool Kiod::listDir(const QString &url, const QString &fname, bool subdirs)
{
    delete file;
  file = new QFile(fname);
  if (file->open(IO_WriteOnly)) {
      delete fileT;
    fileT = new QTextStream(file);
    KIO::ListJob *job;
    if (!subdirs)
        job = KIO::listDir( url );
    else
        job = KIO::listRecursive( url, false);

    kdDebug() << "started " << job <<  " " << subdirs << endl;

    QObject::connect( job, SIGNAL( entries( KIO::Job*, const KIO::UDSEntryList& ) ),
		      SLOT( slotListEntries( KIO::Job*, const KIO::UDSEntryList& ) ) );
    QObject::connect( job, SIGNAL( result( KIO::Job * ) ),
		      SLOT( slotFinished( KIO::Job* ) ) );

    kapp->enter_loop();

    file->close();
    if (worked)
      return TRUE;
    else
      return FALSE;
  } else
    return FALSE;
}

void Kiod::slotListEntries( KIO::Job *, const KIO::UDSEntryList& entries )
{
  long size = 0;
  QString text;

  KIO::UDSEntryList::ConstIterator entryIt = entries.begin();

  for (; entryIt != entries.end(); ++entryIt) {
      //kdDebug() << "listDir " << dynamic_cast<KIO::ListJob*>(job)->url() << endl;
      for (KIO::UDSEntry::ConstIterator it = (*entryIt).begin();
	   it != (*entryIt).end(); it++ )
      {
	  if ( (*it).m_uds == KIO::UDS_SIZE )
	      size = (*it).m_long;
	  else if ( (*it).m_uds == KIO::UDS_NAME )
	      text = (*it).m_str;
      }
      *fileT << text << "\n" << size << "\n";
      kdDebug() << text << " " << size << "\n";
  }
}

void Kiod::slotFinished( KIO::Job *job )
{
    //kdDebug() << "finished" << " " << job << " " << dynamic_cast<KIO::ListJob*>(job)->url() << endl;
    worked = (job->error() == 0);
    kapp->exit_loop();
}

#include "kio.moc"
