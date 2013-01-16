/***************************************************************************
                          KKnownHostInfo.h  -  description
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

#ifndef KKNOWNHOSTINFO_H
#define KKNOWNHOSTINFO_H

#include <qstring.h>
#include <qstringlist.h>

/**Class that represents an entry in the /etc/hosts file.
  *@author Juan Luis Baptiste
  */

class KKnownHostInfo {
public: 
	KKnownHostInfo();
	~KKnownHostInfo();
  QStringList getAliases();
  QString getIpAddress();
  void setAliases(QStringList aliases);
  void addAlias(QString alias);
  void setIpAddress(QString ipAddress);
private: // Private attributes
  /** IP address of the known Host. */
  QString ipAddress;
  /** Lists of aliases of the known host. */
  QStringList aliases;
};

#endif
