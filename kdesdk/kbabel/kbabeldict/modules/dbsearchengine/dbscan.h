/***************************************************************************
                          dbscan.h  - 
                             -------------------
    begin                : Fri Sep 8 2000
    copyright            : (C) 2000 by Andrea Rizzi
    email                : rizzi@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *                                                                         *
 *   In addition, as a special exception, the copyright holders give       *
 *   permission to link the code of this program with any edition of       *
 *   the Qt library by Trolltech AS, Norway (or with modified versions     *
 *   of Qt that use the same license as Qt), and distribute linked         *
 *   combinations including the two.  You must obey the GNU General        *
 *   Public License in all respects for all of the code used other than    *
 *   Qt. If you modify this file, you may extend this exception to         *
 *   your version of the file, but you are not obligated to do so.  If     *
 *   you do not wish to do so, delete this exception statement from        *
 *   your version.                                                         *
 ***************************************************************************/
/*
 Translation search engine  
 
 
  Copyright  2000
  Andrea Rizzi rizzi@kde.org
 
*/


#ifndef _DBSCAN_H_
#define _DBSCAN_H_

#include <catalog.h>
#include "database.h"

class PoScanner : public QObject
{
  Q_OBJECT

public:
 
 PoScanner(DataBaseManager *dbm,QObject *parent=0,const char *name=0);

 /*
   Scan a single PO file.
 */ 
 bool scanFile(QString fileName); 
 
 /*
   Scan a list of space separated files with possible MetaCharacters
  */
 bool scanPattern(QString pathName,QString pattern="*.po",bool rec=false);

 


signals: 
 void fileStarted();
 void fileProgress(int);
 void fileFinished();
 void fileLoading(int);
 void patternStarted();
 void patternProgress(int);
 void patternFinished();
 void added(int);
 void filename(QString);
private:
 
 // If true when a translation is found in a CATALOG the old translation for this CATALOG
  // will be removed  
 bool removeOldCatalogTranslation;
 int count;
 DataBaseManager *dm;
// InfoItem cinfo;
};


#endif
