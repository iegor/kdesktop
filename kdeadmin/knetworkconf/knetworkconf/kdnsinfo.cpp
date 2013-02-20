/***************************************************************************
                          kdnsinfo.cpp  -  description
                             -------------------
    begin                : Sat Jan 18 2003
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

#include "kdnsinfo.h"

KDNSInfo::KDNSInfo(){
}
KDNSInfo::~KDNSInfo(){
}
QStringList KDNSInfo::getNameServers(){
  return nameServers;
}
QString KDNSInfo::getDomainName(){
  return domainName;
}
QString KDNSInfo::getMachineName(){
  return machineName;
}

QStringList KDNSInfo::getSearchDomains(){
  return searchDomains;
}
void KDNSInfo::setNameServers(const QStringList &nameServers){
  KDNSInfo::nameServers = nameServers;
}
void KDNSInfo::setDomainName(const QString &domain){
  KDNSInfo::domainName = domain;
}
void KDNSInfo::setMachineName(const QString &machine){
  KDNSInfo::machineName = machine;
}

void KDNSInfo::setSearchDomains(const QStringList &searchDomains){
  KDNSInfo::searchDomains = searchDomains;
}
QPtrList<KKnownHostInfo> KDNSInfo::getKnownHostsList(){
  return knownHosts;
}
void KDNSInfo::addKnownHost(KKnownHostInfo *host){
  knownHosts.append(host);
}
bool KDNSInfo::removeKnownHost(int index){
  return knownHosts.remove(index);
}
void KDNSInfo::setKnownHostsList(QPtrList<KKnownHostInfo> hostsList){
  KDNSInfo::knownHosts = hostsList;
}
