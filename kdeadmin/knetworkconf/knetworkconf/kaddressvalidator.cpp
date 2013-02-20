/***************************************************************************
                          kaddressvalidator.cpp  -  description
                             -------------------
    begin                : Wed Jan 22 2003
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

#include "kaddressvalidator.h"

/*
  Class for validating IP address and netmasks, and to calculate network and broadcast values.
  The functions to do the last two things where taken from the code of ipcalc.c, made by
  Erik Troan <ewt@redhat.com> and Preston Brown <pbrown@redhat.com> from Red Hat Inc. .
*/

KAddressValidator::KAddressValidator(){
}
KAddressValidator::~KAddressValidator(){
}
/** Validates if the address written by the user is a valid one. Return true if it is and false if not. */
bool KAddressValidator::isValidIPAddress(QString addr){
  QString s = "";
  int i;
  int number;
  bool ok;
  if ((addr.contains('.') > 3) || (addr.length() > 15))
    return false;
  for (i = 0; i < 4; i++)
  {
    s = addr.section('.',i,i);
    number = s.toInt(&ok);
    if (!ok)
      return false;
    if ((i == 0) && (number == 0))
      return false;
    if ((number < 0) || (number > 255))
      return false;
    if ((i == 3) && (number == 0))
      return false;      
  }
  if (i == 4)
    return true;
  else
    return false;
}
bool KAddressValidator::isNetmaskValid(QString addr){
  QString s = "";
  int i;
  int number;
  bool ok;
  for (i = 0; i < 4; i++)
  {
    s = addr.section('.',i,i);
    number = s.toInt(&ok);
    if (!ok)
      return false;
    if ((i == 0) && (number == 0))
      return false;
    if ((number < 0) || (number > 255))
      return false;
  }
  if (i == 4)
    return true;
  else
    return false;

}

bool KAddressValidator::isNetworkValid(const QString &addr){
  QString s = "";
  int i;
  int number;
  bool ok;
  for (i = 0; i < 4; i++)
  {
    s = addr.section('.',i,i);
    number = s.toInt(&ok);
    if (!ok)
      return false;
    if ((i == 0) && (number == 0))
      return false;
    if ((number < 0) || (number >= 255))
      return false;
  }
  if (i == 4)
    return true;
  else
    return false;

}
bool KAddressValidator::isBroadcastValid(QString addr){
  QString s = "";
  int i;
  int number;
  bool ok;
  for (i = 0; i < 4; i++)
  {
    s = addr.section('.',i,i);
    number = s.toInt(&ok);
    if (!ok)
      return false;
    if ((i == 0) && (number == 0))
      return false;
    if ((number < 0) || (number > 255))
      return false;
    if ((i == 3) && (number == 0))
      return false;
  }
  if (i == 4)
    return true;
  else
    return false;

}

/**   \fn unsigned long int prefix2mask(int bits)
  \brief creates a netmask from a specified number of bits

  This function converts a prefix length to a netmask.  As CIDR (classless
  internet domain internet domain routing) has taken off, more an more IP
  addresses are being specified in the format address/prefix
  (i.e. 192.168.2.3/24, with a corresponding netmask 255.255.255.0).  If you
  need to see what netmask corresponds to the prefix part of the address, this
  is the function.  See also \ref mask2prefix.

  \param prefix is the number of bits to create a mask for.
  \return a network mask, in network byte order.
 */
unsigned long int KAddressValidator::prefix2mask(int prefix){
  return htonl(~((2 << (31 - prefix)) - 1));
}
/** \fn int mask2prefix(unsigned long int mask)
  \brief calculates the number of bits masked off by a netmask.

  This function calculates the significant bits in an IP address as specified by
  a netmask.  See also \ref prefix2mask.

  \param mask is the netmask, specified as an unsigned long integer in network byte order.
  \return the number of significant bits.   */
int KAddressValidator::mask2prefix(unsigned long int mask){
  unsigned i;
  int count = IPBITS;

  for (i = 0; i < IPBITS; i++)
  {
	  if (!(ntohl(mask) & ((2 << i) - 1)))
	    count--;
  }

  return count;
}
/*!
   \fn unsigned long int calc_broadcast(unsigned long int addr, int prefix)

  \brief calculate broadcast address given an IP address and a prefix length.

  \param addr an IP address in network byte order.
  \param prefix a prefix length.

  \return the calculated broadcast address for the network, in network byte
  order. */
unsigned long int KAddressValidator::calc_broadcast(unsigned long int addr, int prefix){
  return (addr & prefix2mask(prefix)) | ~prefix2mask(prefix);
}
/** \fn unsigned long int calc_network(unsigned long int addr, int prefix)
  \brief calculates the network address for a specified address and prefix.

  \param addr an IP address, in network byte order
  \param prefix the network prefix
  \return the base address of the network that addr is associated with, in
  network byte order. */
unsigned long int KAddressValidator::calc_network(unsigned long int addr, int prefix){
  return (addr & prefix2mask(prefix));
}
/** Is a wrapper function to calc_network that receives the IP address and netsmask as QString and
returns the network value also as a QString, or NULL if it couldn't be calculated. */
QString KAddressValidator::calculateNetwork(QString addr,QString netmask){
  struct in_addr _addr, _netmask, _network;
  int prefix = 0;
  QString s;  
  if (addr.isNull() || netmask.isNull())
    return NULL; //bad address
  if (!inet_pton(AF_INET,addr.latin1(),&_addr))
    return NULL; //bad address
  else
  {
    if (!inet_pton(AF_INET,netmask.latin1(),&_netmask))
      return NULL; //bad address
    else
    {
      prefix = mask2prefix(_netmask.s_addr);
      _network.s_addr = calc_network(_addr.s_addr, prefix);
      char * char_network = new char[20];
      if (!inet_ntop(AF_INET,&_network,char_network,20))
        return NULL;
      else
        s = char_network;
    }
  }
  return s;      
}

/** Is a wrapper function to calc_broadcast that receives the IP address and netsmask as QString and
returns the broadcast value also as a QString, or NULL if it couldn't be calculated. */
QString KAddressValidator::calculateBroadcast(QString addr, QString netmask){
  struct in_addr _addr, _netmask, _network;
  int prefix = 0;
  QString s;
  if (addr.isNull() || netmask.isNull())
    return NULL; //bad address
  if (!inet_pton(AF_INET,addr.latin1(),&_addr))
    return NULL; //bad address
  else
  {
    if (!inet_pton(AF_INET,netmask.latin1(),&_netmask))
      return NULL; //bad address
    else
    {
      prefix = mask2prefix(_netmask.s_addr);
      _network.s_addr = calc_broadcast(_addr.s_addr, prefix);
      char * char_network = new char[20];
      if (!inet_ntop(AF_INET,&_network,char_network,20))
        return NULL;
      else
        s = char_network;
    }
  }
  return s;

}
