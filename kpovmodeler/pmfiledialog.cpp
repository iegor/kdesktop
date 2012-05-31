/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Andreas Zehender
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


#include "pmfiledialog.h"
#include "pmpart.h"

#include <kfilefiltercombo.h>


PMFileDialog::PMFileDialog( const QString& startDir, const QString& filter, QWidget* parent, const char* name, bool modal )
      : KFileDialog( startDir, filter, parent, name, modal )
{

}

QString PMFileDialog::getImportFileName( QWidget* parent, PMPart* part,
                                         PMIOFormat*& format )
{
   PMIOManager* manager = part->ioManager( );
   QString filter;
   QPtrListIterator<PMIOFormat> it( manager->formats( ) );
   QPtrList<PMIOFormat> formats;

   for( ; it.current( ); ++it )
   {
      if( it.current( )->services( ) & PMIOFormat::Import )
      {
         QStringList patterns = it.current( )->importPatterns( );
         QStringList::Iterator pit;
         for( pit = patterns.begin( ); pit != patterns.end( ); ++pit )
         {
            if( !filter.isEmpty( ) )
               filter += "\n";
            filter += *pit;
            formats.append( it.current( ) );
         }
      }
   }

   PMFileDialog dlg( QString::null, filter, parent, "import file dialog", true );
   dlg.setOperationMode( Opening );
   dlg.setMode( KFile::File | KFile::LocalOnly );
   dlg.setCaption( i18n( "Import" ) );
   dlg.filterWidget->setEditable( false );
   dlg.exec( );

   format = formats.at( dlg.filterWidget->currentItem( ) );

   return dlg.selectedFile( );
}

QString PMFileDialog::getExportFileName( QWidget* parent, PMPart* part,
                                         PMIOFormat*& format, QString& selectedFilter )
{
   PMIOManager* manager = part->ioManager( );
   QString filter;
   QPtrListIterator<PMIOFormat> it( manager->formats( ) );
   QPtrList<PMIOFormat> formats;

   for( ; it.current( ); ++it )
   {
      if( it.current( )->services( ) & PMIOFormat::Export )
      {
         QStringList patterns = it.current( )->exportPatterns( );
         QStringList::Iterator pit;
         for( pit = patterns.begin( ); pit != patterns.end( ); ++pit )
         {
            if( !filter.isEmpty( ) )
               filter += "\n";
            filter += *pit;
            formats.append( it.current( ) );
         }
      }
   }

   PMFileDialog dlg( QString::null, filter, parent, "export file dialog", true );
   dlg.setOperationMode( Saving );
   dlg.setMode( KFile::File | KFile::LocalOnly );
   dlg.setCaption( i18n( "Export" ) );
   dlg.filterWidget->setEditable( false );
   dlg.exec( );

   format = formats.at( dlg.filterWidget->currentItem( ) );
   selectedFilter = dlg.currentFilter( );

   return dlg.selectedFile( );
}

#include "pmfiledialog.moc"
