/* This file is part of the KDE project
 *
 * Copyright (C) 2004 Jakub Stachowski <qbast@go2.pl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <qstringlist.h>
#include "domainbrowser.h"
#include "settings.h"
#include "sdevent.h"
#include "responder.h"
#include "remoteservice.h"
#include "query.h"
#include "servicebrowser.h"
#include <kapplication.h>
#ifdef AVAHI_API_0_6
#include <avahi-client/lookup.h>
#endif

namespace DNSSD
{

#ifdef AVAHI_API_0_6
void domains_callback(AvahiDomainBrowser*,  AvahiIfIndex, AvahiProtocol, AvahiBrowserEvent event, const char* replyDomain,
     AvahiLookupResultFlags, void* context);
#else
void domains_callback(AvahiDomainBrowser*,  AvahiIfIndex, AvahiProtocol, AvahiBrowserEvent event, const char* replyDomain,
     void* context);
#endif


class DomainBrowserPrivate 
{
public:
	DomainBrowserPrivate(DomainBrowser* owner) : m_browseLAN(false), m_started(false), 
	    m_browser(0), m_owner(owner) {}
	~DomainBrowserPrivate() { if (m_browser) avahi_domain_browser_free(m_browser); }
	QStringList m_domains;
	virtual void customEvent(QCustomEvent* event);
	bool m_browseLAN;
	bool m_started;
	AvahiDomainBrowser* m_browser;
	DomainBrowser* m_owner;
};		

void DomainBrowserPrivate::customEvent(QCustomEvent* event)
{
	if (event->type()==QEvent::User+SD_ADDREMOVE) {
		AddRemoveEvent *aev = static_cast<AddRemoveEvent*>(event);
		if (aev->m_op==AddRemoveEvent::Add) m_owner->gotNewDomain(aev->m_domain);
			else m_owner->gotRemoveDomain(aev->m_domain);
	}
}

	
DomainBrowser::DomainBrowser(QObject *parent) : QObject(parent)
{
	d = new DomainBrowserPrivate(this);
 	d->m_domains = Configuration::domainList();
	if (Configuration::browseLocal()) {
		d->m_domains+="local.";
		d->m_browseLAN=true;
	}
 	connect(KApplication::kApplication(),SIGNAL(kipcMessage(int,int)),this,
 	        SLOT(domainListChanged(int,int)));
}

DomainBrowser::DomainBrowser(const QStringList& domains, bool recursive, QObject *parent) : QObject(parent)
{
	d = new DomainBrowserPrivate(this);
	d->m_browseLAN = recursive;
	d->m_domains=domains;
}


DomainBrowser::~DomainBrowser()
{
	delete d;
}


void DomainBrowser::startBrowse()
{
	if (d->m_started) return;
	d->m_started=true;
	if (ServiceBrowser::isAvailable()!=ServiceBrowser::Working) return;
 	QStringList::const_iterator itEnd = d->m_domains.end();
	for (QStringList::const_iterator it=d->m_domains.begin(); it!=itEnd; ++it ) emit domainAdded(*it);
	if (d->m_browseLAN) 
#ifdef AVAHI_API_0_6
	    d->m_browser = avahi_domain_browser_new(Responder::self().client(), AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
		"local.", AVAHI_DOMAIN_BROWSER_BROWSE, (AvahiLookupFlags)0, domains_callback, this);
#else
	    d->m_browser = avahi_domain_browser_new(Responder::self().client(), AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC,
		"local.", AVAHI_DOMAIN_BROWSER_BROWSE, domains_callback, this);
#endif
}

void DomainBrowser::gotNewDomain(const QString& domain)
{
	if (d->m_domains.contains(domain)) return;
	d->m_domains.append(domain);
	emit domainAdded(domain);
}

void DomainBrowser::gotRemoveDomain(const QString& domain)
{
	d->m_domains.remove(domain);
	emit domainRemoved(domain);
}

void DomainBrowser::domainListChanged(int message,int)
{
	if (message!=KIPCDomainsChanged) return;

	bool was_started = d->m_started;
	if (d->m_browser) { 
	    avahi_domain_browser_free(d->m_browser);  // LAN query
	    d->m_browser=0;
	}
	d->m_started = false;

	// remove all domains and resolvers
	if (was_started) {
		QStringList::const_iterator itEnd = d->m_domains.end();
		for (QStringList::const_iterator it=d->m_domains.begin(); it!=itEnd; ++it )
			emit domainRemoved(*it);
	}
	d->m_domains.clear();
	// now reread configuration and add domains
	Configuration::self()->readConfig();
	d->m_browseLAN = Configuration::browseLocal();
	d->m_domains = Configuration::domainList();
	if (Configuration::browseLocal()) d->m_domains+="local";
	// this will emit domainAdded() for every domain if necessary
	if (was_started) startBrowse();
}

const QStringList& DomainBrowser::domains() const
{
	return d->m_domains;
}

bool DomainBrowser::isRunning() const
{
	return d->m_started;
}

void DomainBrowser::virtual_hook(int, void*)
{}

#ifdef AVAHI_API_0_6
void domains_callback(AvahiDomainBrowser*,  AvahiIfIndex, AvahiProtocol, AvahiBrowserEvent event, const char* replyDomain,
     AvahiLookupResultFlags,void* context)
#else
void domains_callback(AvahiDomainBrowser*,  AvahiIfIndex, AvahiProtocol, AvahiBrowserEvent event, const char* replyDomain,
     void* context)
#endif
{
	QObject *obj = reinterpret_cast<QObject*>(context);
	AddRemoveEvent* arev=new AddRemoveEvent((event==AVAHI_BROWSER_NEW) ? AddRemoveEvent::Add :
			AddRemoveEvent::Remove, QString::null, QString::null, 
			DNSToDomain(replyDomain));
		QApplication::postEvent(obj, arev);
}


}
#include "domainbrowser.moc"
