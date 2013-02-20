/***************************************************************************
                          knetworkinfo.h  -  description
                             -------------------
    begin                : Fri Jan 17 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : jbaptiste@merlinux.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KNETWORKINFO_H
#define KNETWORKINFO_H

#include <qstring.h>
#include <qptrlist.h>
#include "knetworkinterface.h"
#include "kwirelessinterface.h"
#include "kaddressvalidator.h"
#include "kroutinginfo.h"
#include "kdnsinfo.h"

/**Represents the network information
  *@author Juan Luis Baptiste
  */

class KNetworkInfo {
  public: 
	  KNetworkInfo();
	  ~KNetworkInfo();
  private: // Private attributes
    QPtrList<KNetworkInterface> deviceList;
    KRoutingInfo *routingInfo;
  /** Path to the network script that permits starting/stopping/reloading the
  network. */
    QString networkScript;
    KDNSInfo *dnsInfo;
    /** Platform name. */
    QString platformName;
    QPtrList<KNetworkInfo> profilesList;
    QString profileName;
  
  public: // Public attributes
    void setDeviceList(QPtrList<KNetworkInterface> deviceList);
    void setRoutingInfo(KRoutingInfo *routingInfo);
    void setDNSInfo(KDNSInfo *dnsInfo);
    QPtrList<KNetworkInterface> getDeviceList();
    KRoutingInfo *getRoutingInfo();
    KDNSInfo *getDNSInfo();
    QString getNetworkScript();
    void setNetworkScript(QString script);
    /** Returns the platform name KnetworkConf is running on. */
    QString getPlatformName();
    /** Sets the platform name KNetworkConf is running on. */
    void setPlatformName(QString name);
    void setProfilesList(QPtrList<KNetworkInfo> profilesList);
    QPtrList<KNetworkInfo> getProfilesList();
    void setProfileName(QString profileName);
    QString getProfileName();
};

#endif
