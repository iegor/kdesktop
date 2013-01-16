/***************************************************************************
                          kdnsinfo.h  -  description
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

#ifndef KDNSINFO_H
#define KDNSINFO_H

#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include "kknownhostinfo.h"

/**
  *@author Juan Luis Baptiste
  */

class KDNSInfo {
public: 
	KDNSInfo();
	~KDNSInfo();
  QString getDomainName();
  QString getMachineName();
  QStringList getNameServers();
  QStringList getSearchDomains();
  void setNameServers(const QStringList &nameServer);
  void setSearchDomains(const QStringList &searchDomains);
  void setDomainName(const QString &domain);
  void setMachineName(const QString &machine);  
  QPtrList<KKnownHostInfo> getKnownHostsList();
  void addKnownHost(KKnownHostInfo *host);
  bool removeKnownHost(int index);
  void setKnownHostsList(QPtrList<KKnownHostInfo> hostsList);
private: // Private attributes
  QStringList nameServers;
  QStringList searchDomains;
  QString domainName;
  QString machineName;
  /** List of known hosts (/etc/hosts). */
  QPtrList<KKnownHostInfo> knownHosts;
};

#endif
