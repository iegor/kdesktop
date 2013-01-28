/*
    This file is part of KAddressbook.
    Copyright (c) 2003 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <float.h>

#include "geo_xxport.h"

class FlagInfo
{
  public:
    double latitude;
    double longitude;
    QColor color;
};

K_EXPORT_KADDRESSBOOK_XXFILTER_CATALOG( libkaddrbk_geo_xxport, GeoXXPort, "libkaddrbk_geo_xxport" )

GeoXXPort::GeoXXPort( KABC::AddressBook *ab, QWidget *parent, const char *name )
  : KAB::XXPort( ab, parent, name )
{
  createExportAction( i18n( "Export Geo Data..." ) );
}

bool GeoXXPort::exportContacts( const KABC::AddresseeList &list, const QString& )
{
  KConfig config( "kworldclockrc" );

  // At first we read all exiting flags and compare it with ours to
  // avoid duplicated flags
  int flags = config.readNumEntry( "Flags", 0 );
  QValueList<FlagInfo> availableFlags;

  if ( flags != 0 ) {
    for ( int i = 0; i < flags; ++i ) {
      FlagInfo info;
      info.latitude = config.readDoubleNumEntry( QString( "Flag_%1_Latitude" ).arg( i ) );
      info.longitude = config.readDoubleNumEntry( QString( "Flag_%1_Longitude" ).arg( i ) );
      info.color = config.readColorEntry( QString( "Flag_%1_Color" ).arg( i ) );

      availableFlags.append( info );
    }
  }


  QValueList<FlagInfo> flagList;
  KABC::AddresseeList::ConstIterator addrIt;
  for ( addrIt = list.begin(); addrIt != list.end(); ++addrIt ) {
    KABC::Geo geo( (*addrIt).geo() );
    if ( !geo.isValid() )
      continue;

    bool available = false;
    QValueList<FlagInfo>::Iterator it;
    for ( it = availableFlags.begin(); it != availableFlags.end(); ++it ) {
      if ( !( KABS( (*it).latitude - geo.latitude() ) > DBL_EPSILON ) &&
           !( KABS( (*it).longitude - geo.longitude() ) > DBL_EPSILON ) ) {
        available = true;
        break;
      }
    }

    if ( !available ) {
      FlagInfo info;
      info.latitude = geo.latitude();
      info.longitude = geo.longitude();
      info.color = QColor( 0, 255, 0 );

      flagList.append( info );
    }
  }

  if ( flagList.count() == 0 ) // nothing to export
    return true;

  flagList += availableFlags;

  int startVal = 0;
  QValueList<FlagInfo>::Iterator it;
  for ( it = flagList.begin(); it != flagList.end(); ++it, ++startVal ) {
    config.writeEntry( QString( "Flag_%1_Color" ).arg( startVal ), (*it).color );
    config.writeEntry( QString( "Flag_%1_Latitude" ).arg( startVal ), (*it).latitude );
    config.writeEntry( QString( "Flag_%1_Longitude" ).arg( startVal ), (*it).longitude );
  }
  config.writeEntry( "Flags", startVal );

  return true;
}

#include "geo_xxport.moc"
