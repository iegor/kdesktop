/***************************************************************************
                          kroutinginfo.h  -  description
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

#ifndef KROUTINGINFO_H
#define KROUTINGINFO_H

#include <qstring.h>


/**Has all the routing info of the machine, like default gateway, default gateway device,
  routes to other hosts and networks, etc.

  *@author Juan Luis Baptiste
  */

class KRoutingInfo {
public: 
	KRoutingInfo();
	~KRoutingInfo();
private: // Private attributes
  QString networking;
  QString forwardIPv4;
  QString hostName;
  QString domainName;
  QString gateway;
  QString gatewayDevice;

public: // Public attributes
  QString isNetworkingEnabled();
  QString isForwardIPv4Enabled();
  QString getHostName();
  QString getGatewayDevice();
  QString getGateway();
  QString getDomainName();
  void setNetworkingEnabled(const QString &networking);
  void setForwardIPv4Enabled(const QString &forwardIPv4);
  void setHostName(const QString &hostName);
  void setDomainName(const QString &domainName);
  void setGateway(const QString &gateway);
  void setGatewayDevice(const QString &gatewayDevice);
  
};

#endif
