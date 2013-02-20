//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
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


#ifndef PMOBJECTDRAG_H
#define PMOBJECTDRAG_H

#include <qdragobject.h>
#include <qstringlist.h>

#include "pmobject.h"
class PMParser;
class PMPart;

/**
 * Supports drag/drop and copy/paste of kpovmodeler objects
 */

class PMObjectDrag : public QDragObject
{
public:
   /**
    * Creates a drag object for one object
    *
    * The object drag will contain all formats that are supported
    * by the part.
    * @see PMIOManager
    */
   PMObjectDrag( PMPart* part, PMObject* object, QWidget* dragSource = 0,
                 const char* name = 0 );
   /**
    * Creates a drag object for all objects in objList
    */
   PMObjectDrag( PMPart* part, const PMObjectList& objList, QWidget* dragSource = 0,
                 const char* name = 0 );
   /**
    * Deletes the drag object
    */
   ~PMObjectDrag( );
   /**
    * Returns the encoded payload of this object, in the
    * specified MIME format.
    */
   virtual QByteArray encodedData( const char* ) const;
   /**
    * Returns the ith format, or NULL.
    */
   virtual const char* format( int i = 0 ) const;
   /**
    * Returns true if the information in e can be decoded
    */
   static bool canDecode( const QMimeSource* e, PMPart* part );
   /**
    * Returns a pointer to a parser for this drag object or 0, if the data
    * can't be decoded.
    *
    * The caller is responsible to delete the parser.
    */
   static PMParser* newParser( const QMimeSource* e, PMPart* part );

private:
   QValueList<QByteArray> m_data;
   QStringList m_mimeTypes;
};

#endif
