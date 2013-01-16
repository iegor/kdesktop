/***************************************************************************
                          KKnownHostInfo.cpp  -  description
                             -------------------
    begin                : Sun May 11 2003
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

#include "kknownhostinfo.h"

KKnownHostInfo::KKnownHostInfo(){
}
KKnownHostInfo::~KKnownHostInfo(){
}
/** No descriptions */
QString KKnownHostInfo::getIpAddress(){
  return ipAddress;
}

QStringList KKnownHostInfo::getAliases(){
  return aliases;
}
void KKnownHostInfo::setIpAddress(QString ipAddress){
  KKnownHostInfo::ipAddress = ipAddress;
}

void KKnownHostInfo::addAlias(QString alias){
  KKnownHostInfo::aliases.append(alias);
}

void KKnownHostInfo::setAliases(QStringList aliases){
  KKnownHostInfo::aliases = aliases;
}
