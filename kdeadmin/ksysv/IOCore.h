/***************************************************************************
                          ksv::IO.h  -  description
                             -------------------
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-99 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KSV_IOCORE_H
#define KSV_IOCORE_H

#include <qdir.h>
#include <qstring.h>

#include <ktrader.h>
#include <kservice.h>

template<class T> class QValueList;
class QFileInfo;
class QDataStream;

class KSVDragList;
class KSVData;

namespace ksv
{
  namespace IO
  {
    void removeFile (const QFileInfo& info, QDir& dir,
                     QString& rich, QString& plain);

    QString relToAbs (const QString& dir, const QString& rel);

    void makeSymlink (const KSVData& data, int runlevel, bool start,
                      QString& rich, QString& plain);

    void dissectFilename (const QString& file, QString& name, int& nr);

    QString makeRelativePath (const QString& from, const QString& to);

    bool saveConfiguration (QDataStream&,
                            KSVDragList** start,
                            KSVDragList** stop);

    bool loadSavedConfiguration (QDataStream&,
                                 QValueList<KSVData>* start,
                                 QValueList<KSVData>* stop);

    KTrader::OfferList servicesForFile (const QString& filename);
    KService::Ptr preferredServiceForFile (const QString& filename);

  } // namespace IO
} // namespace ksv

#endif

