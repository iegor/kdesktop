/***************************************************************************
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

#ifndef KSV_CORE_H
#define KSV_CORE_H

class QPushButton;
class QStyleSheet;
class KAboutData;
class KCompletion;
class KSVItem;

namespace ksv
{
  bool getServiceDescription (const QString& path, QString& res);

  QString breakWords (const QString& s, int amount);

  /**
   * \return the Unicode string for the (c) symbol.
   */
  const QString& copyrightSymbol ();

  QStyleSheet* styleSheet ();

  const QString& logFileFilter ();
  const QString& nativeFileFilter ();

  const QString& logFileExtension ();
  const QString& nativeFileExtension ();

  KCompletion* serviceCompletion ();
  KCompletion* numberCompletion ();

  extern const int runlevelNumber;
  extern KAboutData* about;

  enum Messages {
    RunlevelsReadOnly = 0,
    CouldNotGenerateSortingNumber
  };
  
  extern const char* notifications[];
}

#endif // KSV_CORE_H

