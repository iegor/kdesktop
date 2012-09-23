/*
    KDE Icon Editor - a small icon drawing program for the KDE.
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#ifndef __KICON_H__
#define __KICON_H__

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(HAVE_FCNTL_H) && !defined(HAVE_FLOCK)
#include <fcntl.h>
#endif

#ifdef HAVE_STDC_HEADERS
#include <stdlib.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

class QImage;

class KIconEditIcon : public QObject
{
  Q_OBJECT
public:
  KIconEditIcon(QObject*, const QImage*, KURL url = KURL() );
  ~KIconEditIcon();

  bool isLocal() { return local; }
  QString url() { return _url; }
  void setUrl( const QString & u ) { _url = u; };

public slots:
  bool open(const QImage*, KURL url = KURL());
  bool promptForFile(const QImage*);
  bool save(const QImage*, const QString &filename=QString::null);
  bool saveAs(const QImage*);

signals:
  void newmessage( const QString &);
  void newname(const QString &);
  void addrecent(const QString &);
  void opennewwin(const QString &);
  void loaded(QImage *);
  void saved();

protected:
    bool local;
    QString _url;
    QString _lastdir;
    FILE *f;
};

#endif //__KICON_H__
