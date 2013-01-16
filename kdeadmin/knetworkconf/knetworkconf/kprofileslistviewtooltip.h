/***************************************************************************
                          kprofileslistviewtooltip.h  -  description
                             -------------------
    begin                : Wed Aug 24 2005
    copyright            : (C) 2005 by Juan Luis Baptiste
    email                : juan.baptiste@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef KPROFILESLISTVIEWTOOLTIP_H
#define KPROFILESLISTVIEWTOOLTIP_H

#include <qtooltip.h>
#include <qlistview.h>
#include <qheader.h>
#include <qptrlist.h>

#include "knetworkinfo.h"
#include "knetworkconfigparser.h"

class KProfilesListViewToolTip : public QToolTip
{
public:
  KProfilesListViewToolTip(QListView* parent);
  void setProfiles(QPtrList<KNetworkInfo> profiles_);
  ~KProfilesListViewToolTip();

protected:
    void maybeTip( const QPoint& p );
    KNetworkInfo *getProfile(QPtrList<KNetworkInfo> profilesList, QString selectedProfile);
private:
    QListView* listView;
  //KNetworkConf* conf;
  QPtrList<KNetworkInfo> profiles;
    
};

inline void KProfilesListViewToolTip::setProfiles(QPtrList<KNetworkInfo> profiles_)
{
  profiles = profiles_;
}

inline KProfilesListViewToolTip::KProfilesListViewToolTip( QListView* parent ):QToolTip( parent->viewport() ), listView( parent ) {}
 
inline void KProfilesListViewToolTip::maybeTip( const QPoint& p ) 
{
 
  if ( !listView )
    return;
 
  const QListViewItem* item = listView->itemAt( p );
 
  if ( !item )
    return;
 
  const QRect itemRect = listView->itemRect( item );
 
  if ( !itemRect.isValid() )
    return;
  
  const int col = listView->header()->sectionAt( p.x() );
 
  if ( col == -1 )
    return;
 
  const QRect headerRect = listView->header()->sectionRect( col );
 
  if ( !headerRect.isValid() )
    return;
 
  const QRect cellRect( headerRect.left(), itemRect.top(),headerRect.width() + 60, itemRect.height() );
 
  QString tipStr;
 
  if( col == 0 )
  {
    tipStr = QString(i18n("<b>Network Configuration of this Profile:</b>" ));
    KNetworkInfo *profile = getProfile(profiles,item->text(0)); 
    if (profile != NULL)
    {
      QPtrList<KNetworkInterface> devices = profile->getDeviceList();  
      KNetworkInterface *device = NULL;
      for (device = devices.first(); device; device = devices.next())
      {
        if (device->getType() != LOOPBACK_IFACE_TYPE)
        {
          tipStr.append(i18n("<p><b>Interface:</b> %1").arg(device->getDeviceName().latin1()));
          tipStr.append(i18n("<br><b>Type:</b> %1").arg(device->getType()));
          QString bootProto;
          if (device->getBootProto() == "none")
            bootProto = "Manual";
          else
            bootProto = device->getBootProto();
          tipStr.append(i18n("<br><b>Boot Protocol:</b> %1").arg(bootProto));
          if (bootProto != "dhcp")
          {
            tipStr.append(i18n("<br><b>IP Address:</b>   %1").arg(device->getIpAddress()));
            tipStr.append(i18n("<br><b>Broadcast Address:</b> %1").arg(device->getBroadcast()));
          }  
          tipStr.append(i18n("<br><b>On Boot:</b> %1").arg(device->getOnBoot()));    
        }
      }
      KRoutingInfo *route = profile->getRoutingInfo();
      tipStr.append(i18n("</p><p><b>Default Gateway:</b>   %1").arg(route->getGateway()));
      KDNSInfo *dns = profile->getDNSInfo();
      tipStr.append(i18n("<br><b>Domain Name:</b> %1").arg(dns->getDomainName()));
      tipStr.append(i18n("<br><b>Machine Name:</b> %1").arg(dns->getMachineName()));
      QStringList nameServers = dns->getNameServers();
      for ( QStringList::Iterator it = nameServers.begin(); it != nameServers.end(); ++it)
      {
        tipStr.append(i18n("<br><b>DNS Name Server:</b> %1").arg((*it)));
      }
    }  
  }
  tip( cellRect, tipStr );
}

inline KNetworkInfo *KProfilesListViewToolTip::getProfile(QPtrList<KNetworkInfo> profilesList, QString selectedProfile)
{
  QPtrListIterator<KNetworkInfo> it(profilesList);
  KNetworkInfo *net = NULL;
    
  while ((net = it.current()) != 0)
  {
    ++it;
    if (net->getProfileName() == selectedProfile)
      break;
  }
  return net;
}

#endif
