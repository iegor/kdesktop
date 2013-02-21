/***************************************************************************
                          kroutinginfo.cpp  -  description
                             -------------------
    begin                : Mon Mar 3 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : juancho@linuxmail.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kroutinginfo.h"

KRoutingInfo::KRoutingInfo(){
}
KRoutingInfo::~KRoutingInfo(){
}
QString KRoutingInfo::isNetworkingEnabled(){
  return networking;
}
QString KRoutingInfo::isForwardIPv4Enabled(){
  return forwardIPv4;
}
QString KRoutingInfo::getHostName(){
  return hostName;
}
QString KRoutingInfo::getDomainName(){
  return domainName;
}
QString KRoutingInfo::getGateway(){
  return gateway;
}
QString KRoutingInfo::getGatewayDevice(){
  return gatewayDevice;
}
void KRoutingInfo::setNetworkingEnabled(const QString &networking){
  KRoutingInfo::networking = networking;
}
void KRoutingInfo::setForwardIPv4Enabled(const QString &forwardIPv4){
  KRoutingInfo::forwardIPv4 = forwardIPv4;
}
void KRoutingInfo::setHostName(const QString &hostName){
  KRoutingInfo::hostName = hostName;
}
void KRoutingInfo::setDomainName(const QString &domainName){
  KRoutingInfo::domainName = domainName;
}
void KRoutingInfo::setGateway(const QString &gateway){
  KRoutingInfo::gateway = gateway;
}
void KRoutingInfo::setGatewayDevice(const QString &gatewayDevice){
  KRoutingInfo::gatewayDevice = gatewayDevice;
}

