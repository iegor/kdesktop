//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#ifndef PMFACTORY_H
#define PMFACTORY_H

#include <kparts/factory.h>

class KAboutData;
class KInstance;

class PMFactory : public KParts::Factory
{
   Q_OBJECT
public:
   PMFactory( );
   virtual ~PMFactory( );

   virtual KParts::Part* createPartObject( QWidget* parentWidget,
                                     const char* widgetName,
                                     QObject* parent, const char* name,
                                     const char* classname,
                                     const QStringList& args );
   
   static KInstance* instance( );
   static const KAboutData* aboutData( );
private:
   static KInstance* s_instance;
   static KAboutData* s_aboutData;
};

#endif
