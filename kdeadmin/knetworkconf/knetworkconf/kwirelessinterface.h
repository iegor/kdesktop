/***************************************************************************
                          kwirelessinterface  -  description
                             -------------------
    begin                : jue oct 21 2004
    copyright            : (C) 2004 by Juan Luis Baptiste
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
#ifndef KWIRELESSINTERFACE_H
#define KWIRELESSINTERFACE_H

#include <knetworkinterface.h>
#include <qstring.h>

/**
Class that represents a wireless network interface. It inherits from KNetworkInterface.

@author Juan Luis Baptiste
*/
class KWirelessInterface : public KNetworkInterface
{
  public:
    KWirelessInterface();

    ~KWirelessInterface();

    QString getEssid();
    QString getWepKey();  
    void setEssid(QString newEssid);
    void setWepKey(QString newKey);
    QString getKeyType();
    void setKeyType(QString type);
  
  private:
    QString essid;       
    QString wepKey;   
    QString keyType;
};

#endif
