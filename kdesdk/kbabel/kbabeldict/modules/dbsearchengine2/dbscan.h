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
 ***************************************************************************/
/*
 Translation search engine  
  Copyright  2000
  Andrea Rizzi rizzi@kde.org
  
*/


#ifndef _DBSCAN_H_
#define _DBSCAN_H_

#include <catalog.h>
#include <kurl.h>
#include "database.h"

#include "sourcedialog.h"

class KConfigBase;

class MessagesSource
{
    //FIXME Use KURL and add network support
    
    public: 
    MessagesSource();
    void writeConfig(KConfigBase *config);
    void readConfig(KConfigBase *config);
    void setDialogValues(SourceDialog *sd);
    void getDialogValues(SourceDialog *sd);
    
    /**
      * It returns a list of urls to scan, the list contains single file if type is "SingleFile" 
      * or a list of files if type is "SingleDir" or "RecDir"
      */
    QValueList<KURL> urls();
    
    private:
    QValueList<KURL> filesInDir(KURL url,bool recursive);
    
    KURL location;
    // The source type "SingleFile" =0, "SingleDirectory"=1, "RecursiveDirectory"=2
    uint type;
    
    /**
	* A filter to apply on source files
*/	
    
    SearchFilter filter;
    
    QString projectName;
    QString projectKeywords;
    QString status;
};


class PoScanner : public QObject
{
    Q_OBJECT;
    
    public:
    
    PoScanner(DataBaseInterface *dbi,QObject *parent=0,const char *name=0);
    
    /*
   Scan a single PO file.
 */ 
    bool scanFile(QString fileName); 
    
    /*
   Scan a single URL file.
 */ 
    bool scanURL(KURL u); 
    
    
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
    DataBaseInterface *di;
    // InfoItem cinfo;
};


#endif
