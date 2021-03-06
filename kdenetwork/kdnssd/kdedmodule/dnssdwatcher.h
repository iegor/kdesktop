/* This file is part of the KDE Project
   Copyright (c) 2004 Jakub Stachowski <qbast@go2.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _DNSSDWATCHER_H_
#define _DNSSDWATCHER_H_

#include <qdict.h>
#include <kdedmodule.h>
#include <qstring.h>
#include <qstringlist.h>
#include <kurl.h>

class Watcher;
class DNSSDWatcher : public KDEDModule
{
Q_OBJECT
K_DCOP
public:
	DNSSDWatcher(const QCString& obj);

k_dcop:
	QStringList watchedDirectories();
	void enteredDirectory(const KURL& dir);
	void leftDirectory(const KURL& dir);

private:
	QDict<Watcher> watchers;
	
	void createNotifier(const KURL& url);
	void dissect(const KURL& url,QString& name,QString& type,QString& domain);

};

#endif
