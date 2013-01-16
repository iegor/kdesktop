/***************************************************************************
                          knetworkinterface.h  -  description
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

#ifndef KNETWORKINTERFACE_H
#define KNETWORKINTERFACE_H

#include <qstring.h>

/**This class has all the values associated with a network interface.
  *@author Juan Luis Baptiste
  */

class KNetworkInterface {
  public: 
    KNetworkInterface();
    ~KNetworkInterface();

    QString getBootProto();
    QString getBroadcast();
    QString getDeviceName();
    QString getIpAddress();
    QString getNetmask();
    QString getNetwork();
    QString getOnBoot();
    QString getGateway();
    void setBootProto(const QString &bootProto);
    void setBroadcast(const QString &broadcast);
    void setDeviceName(const QString &deviceName);
    void setIpAddress(const QString &ipAddress);
    void setNetmask(const QString &netmask);
    void setNetwork(const QString &network);
    void setOnBoot(const QString &onBoot);
    void setGateway(const QString &gateway);
    bool isActive();
    void setActive(bool active);
    void setMacAddress(const QString &addr);
    QString getMacAddress();
    void setDescription(const QString &desc);
    QString getDescription();
    void setType(const QString &type);
    QString getType();
    
  private: // Private attributes
    /** Interface name (i.e. eth0, eth1, etc). */
    QString deviceName;
    /** Boot protocol of the device (static, dhcp). */
    QString bootProto;
    /** IPv4 address of the interface. */
    QString ipAddress;
    /** The Netmask. */
    QString netmask;
    /** The network. */
    QString network;
    /** The broadcast. */
    QString broadcast;
    /** thetermines if this device is configured at boot time. */
    QString onBoot;
    /** default gateway for this device. */
    QString gateway;
    bool active;
    /** The MAC address. */
    QString hwAddress;
    /** A little description about the network device. */
    QString description;
    /** Type of interface (ethernet,wireless,loopback or modem)*/
    QString type;
};

#endif
