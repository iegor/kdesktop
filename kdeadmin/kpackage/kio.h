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


#ifndef KP_KIO_H
#define KP_KIO_H


#include <vector>

#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include "../config.h"
#include <kio/job.h>

class Kio: public QObject
{
  Q_OBJECT

public:
  Kio();

  bool download(const KURL & from, const QString & to);

private:
  bool worked;

private slots:
  void slotIOJobFinished( KIO::Job *job );
};

class Kiod: public QObject
{
  Q_OBJECT

public:
  Kiod();
    ~Kiod();

  bool listDir(const QString &url, const QString &fname, bool subdirs);

private:
  QFile *file;
  QTextStream *fileT;
  bool worked;

private slots:
  void slotListEntries( KIO::Job *, const KIO::UDSEntryList&  );
  void slotFinished( KIO::Job *);
};
#endif
