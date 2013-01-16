/***************************************************************************
                          knetworkinfo.cpp  -  description
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

#include "knetworkinfo.h"

KNetworkInfo::KNetworkInfo(){
}
KNetworkInfo::~KNetworkInfo(){
}
void KNetworkInfo::setDeviceList(QPtrList<KNetworkInterface> deviceList){
  KNetworkInfo::deviceList = deviceList;
}  
void KNetworkInfo::setRoutingInfo(KRoutingInfo *routingInfo){
  KNetworkInfo::routingInfo = routingInfo;
}  
void KNetworkInfo::setDNSInfo(KDNSInfo *dnsInfo){
  KNetworkInfo::dnsInfo = dnsInfo;

}  
QPtrList<KNetworkInterface> KNetworkInfo::getDeviceList(){
  return deviceList;
}  
KRoutingInfo * KNetworkInfo::getRoutingInfo(){
  return routingInfo;
}  
KDNSInfo * KNetworkInfo::getDNSInfo(){
  return dnsInfo;
}  

void KNetworkInfo::setNetworkScript(QString script){
  KNetworkInfo::networkScript = script;
}
QString KNetworkInfo::getNetworkScript(){
  return networkScript;
}
/** Returns the platform name KnetworkConf is running on. */
QString KNetworkInfo::getPlatformName(){
 return platformName;
}
/** Sets the platform name KNetworkConf is running on. */
void KNetworkInfo::setPlatformName(QString name){
  KNetworkInfo::platformName = name;
}

void KNetworkInfo::setProfilesList(QPtrList<KNetworkInfo> profilesList){
  KNetworkInfo::profilesList = profilesList;
}  

QPtrList<KNetworkInfo> KNetworkInfo::getProfilesList(){
  return profilesList;
}

/** Returns the platform name KnetworkConf is running on. */
QString KNetworkInfo::getProfileName(){
 return profileName;
}
/** Sets the platform name KNetworkConf is running on. */
void KNetworkInfo::setProfileName(QString profileName){
  KNetworkInfo::profileName = profileName;
}


