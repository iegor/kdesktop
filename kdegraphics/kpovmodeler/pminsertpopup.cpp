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


#include "pminsertpopup.h"
#include <klocale.h>
#include <kiconloader.h>
#include <qcursor.h>

PMInsertPopup::PMInsertPopup( QWidget* parent, bool multipleObjects,
                              int items, bool canInsertAllAsFirstChildren,
                              bool canInsertAllAsLastChildren,
                              bool canInsertAllAsSiblings, const char* name )
      : KPopupMenu( parent, name )
{
   QString itemText;
   if( multipleObjects )
   {
      insertTitle( i18n( "Insert Objects As" ) );
      if( ( items & PMIFirstChild ) == PMIFirstChild )
      {
         itemText = i18n( "First Children" );
         if( !canInsertAllAsFirstChildren )
            itemText += " (" + i18n( "some" ) + ")";
         insertItem( SmallIcon( "pminsertfirstchild" ),
                     itemText, PMIFirstChild );
      }
      if( ( items & PMILastChild ) == PMILastChild )
      {
         itemText = i18n( "Last Children" );
         if( !canInsertAllAsLastChildren )
            itemText += " (" + i18n( "some" ) + ")";
         insertItem( SmallIcon( "pminsertlastchild" ),
                     itemText, PMILastChild );
      }
      if( ( items & PMISibling ) == PMISibling )
      {
         itemText = i18n( "Siblings" );
         if( !canInsertAllAsSiblings )
            itemText += " (" + i18n( "some" ) + ")";
         insertItem( SmallIcon( "pminsertsibling" ),
                     itemText, PMISibling );
      }
   }
   else
   {
      insertTitle( i18n( "Insert Object As" ) );
      if( ( items & PMIFirstChild ) == PMIFirstChild )
         insertItem( SmallIcon( "pminsertfirstchild" ),
                     i18n( "First Child" ), PMIFirstChild );
      if( ( items & PMILastChild ) == PMILastChild )
         insertItem( SmallIcon( "pminsertlastchild" ),
                     i18n( "Last Child" ), PMILastChild );
      if( ( items & PMISibling ) == PMISibling )
         insertItem( SmallIcon( "pminsertsibling" ),
                     i18n( "Sibling" ), PMISibling );
   }
}

int PMInsertPopup::choosePlace( QWidget* parent, bool multipleObjects,
                                int items, bool canInsertAllAsFirstChildren,
                                bool canInsertAllAsLastChildren,
                                bool canInsertAllAsSiblings )
{
   int result;
   PMInsertPopup* popup = new PMInsertPopup( parent, multipleObjects, items,
                                             canInsertAllAsFirstChildren,
                                             canInsertAllAsLastChildren,
                                             canInsertAllAsSiblings );
   result = popup->exec( QCursor::pos( ) );
   if( result < 0 )
      result = 0;
   delete popup;

   return result;
}
