/***************************************************************************
                          kwirelessinterface  -  description
                             -------------------
    begin                : Thu oct 21 2004
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
#include "kwirelessinterface.h"

KWirelessInterface::KWirelessInterface()
 : KNetworkInterface()
{
}

KWirelessInterface::~KWirelessInterface()
{
}

QString KWirelessInterface::getEssid()
{
  return essid;
}

QString KWirelessInterface::getWepKey()
{
  return wepKey;
}

void KWirelessInterface::setWepKey(QString newKey)
{
  wepKey = newKey;
}
void KWirelessInterface::setEssid(QString newEssid)
{
  essid = newEssid;
}

QString KWirelessInterface::getKeyType()
{
  return keyType;
}

void KWirelessInterface::setKeyType(QString type)
{
    keyType = type;
}
