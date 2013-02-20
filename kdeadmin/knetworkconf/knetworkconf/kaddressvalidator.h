/***************************************************************************
                          kaddressvalidator.h  -  description
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

#ifndef KADDRESSVALIDATOR_H
#define KADDRESSVALIDATOR_H

/*
  Class for validating IP address and netmasks, and to calculate network and broadcast values.
  The functions to do the last two things where taken from the code of ipcalc.c, made by
  Erik Troan <ewt@redhat.com> and Preston Brown <pbrown@redhat.com> from Red Hat Inc. 
*/



/*!
  \def IPBITS
  \brief the number of bits in an IP address.
*/
#define IPBITS (sizeof(Q_UINT32) * 8)
/*!
  \def IPBYTES
  \brief the number of bytes in an IP address.
*/
#define IPBYTES (sizeof(Q_UINT32))

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <qstring.h>

/**Class that has simple methods for validating IP addresses, netmasks, calculating
   broadcast and network addresses.

  *@author Juan Luis Baptiste
  */

class KAddressValidator {
public: 
	KAddressValidator();
	~KAddressValidator();
  /** Validates if the address written by the user is a valid one. Return true if it is and false if not. */
  static bool isValidIPAddress(QString addr);
  /** Validates if the Netmask written by the user is a valid one. Return true if it is and false if not. */
  static bool isNetmaskValid(QString addr);
/** Validates if the Network written by the user is a valid one. Return true if it is and false if not. */
  static bool isNetworkValid(const QString &addr);
/** Validates if the Broadcast written by the user is a valid one. Return true if it is and false if not. */
  static bool isBroadcastValid(QString addr);

  /** Is a wrapper function to calc_network that receives the IP address and netsmask as QString and
  returns the network value also as a QString, or NULL if it couldn't be calculated. */
  static QString calculateNetwork(QString addr,QString netmask);
/** Is a wrapper function to calc_broadcast that receives the IP address and netsmask as QString and
returns the broadcast value also as a QString, or NULL if it couldn't be calculated. */
  static QString calculateBroadcast(QString addr, QString netmask);

private: // Private methods
/**   \fn unsigned long int calc_broadcast(unsigned long int addr, int prefix)

  \brief calculate broadcast address given an IP address and a prefix length.

  \param addr an IP address in network byte order.
  \param prefix a prefix length.

  \return the calculated broadcast address for the network, in network byte
  order. */
  static unsigned long int calc_broadcast(unsigned long int addr, int prefix);
  /** \fn unsigned long int calc_network(unsigned long int addr, int prefix)
  \brief calculates the network address for a specified address and prefix.

  \param addr an IP address, in network byte order
  \param prefix the network prefix
  \return the base address of the network that addr is associated with, in
  network byte order. */
  static unsigned long int calc_network(unsigned long int addr, int prefix);
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
  static unsigned long int prefix2mask(int prefix);
  /** \fn int mask2prefix(unsigned long int mask)
  \brief calculates the number of bits masked off by a netmask.

  This function calculates the significant bits in an IP address as specified by
  a netmask.  See also \ref prefix2mask.

  \param mask is the netmask, specified as an unsigned long integer in network byte order.
  \return the number of significant bits.   */
  static int mask2prefix(unsigned long int mask);
};

#endif
