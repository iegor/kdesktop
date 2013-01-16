/***************************************************************************
                          knetworkinterface.cpp  -  description
                             -------------------
    begin                : Mon Jan 13 2003
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

#include "knetworkinterface.h"

KNetworkInterface::KNetworkInterface(){
}
KNetworkInterface::~KNetworkInterface(){
}

QString KNetworkInterface::getBootProto(){
  return bootProto;
}
QString KNetworkInterface::getBroadcast(){
  return broadcast;
}
QString KNetworkInterface::getDeviceName(){
  return deviceName;
}
QString KNetworkInterface::getIpAddress(){
  return ipAddress;
}
QString KNetworkInterface::getNetmask(){
  return netmask;
}
QString KNetworkInterface::getNetwork(){
  return network;
}
QString KNetworkInterface::getOnBoot(){
  return onBoot;
}
QString KNetworkInterface::getGateway(){
  return gateway;
}

void KNetworkInterface::setBootProto(const QString &bootProto){
  KNetworkInterface::bootProto = bootProto;
}
void KNetworkInterface::setBroadcast(const QString &broadcast){
  KNetworkInterface::broadcast = broadcast;
}
void KNetworkInterface::setDeviceName(const QString &deviceName){
  KNetworkInterface::deviceName = deviceName;
}
void KNetworkInterface::setIpAddress(const QString &ipAddress){
  KNetworkInterface::ipAddress = ipAddress;
}
void KNetworkInterface::setNetmask(const QString &netmask){
  KNetworkInterface::netmask = netmask;
}
void KNetworkInterface::setNetwork(const QString &network){
  KNetworkInterface::network = network;
}
void KNetworkInterface::setOnBoot(const QString &onBoot){
  KNetworkInterface::onBoot = onBoot;
}
void KNetworkInterface::setGateway(const QString &gateway){
  KNetworkInterface::gateway = gateway;
}
bool KNetworkInterface::isActive(){
  return active;
}
void KNetworkInterface::setActive(bool active){
  KNetworkInterface::active = active;  
}
/** No descriptions */
QString KNetworkInterface::getMacAddress(){
  return hwAddress;
}
/** No descriptions */
void KNetworkInterface::setMacAddress(const QString &addr){
   KNetworkInterface::hwAddress = addr;
}
/** No descriptions */
QString KNetworkInterface::getDescription(){
  return description;
}
/** No descriptions */
void KNetworkInterface::setDescription(const QString &desc){
  KNetworkInterface::description = desc;
}

/** No descriptions */
QString KNetworkInterface::getType(){
  return type;
}
/** No descriptions */
void KNetworkInterface::setType(const QString &type){
  KNetworkInterface::type = type;
}
