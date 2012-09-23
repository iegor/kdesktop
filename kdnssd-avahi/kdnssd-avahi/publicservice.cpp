/* This file is part of the KDE project
 *
 * Copyright (C) 2004, 2005 Jakub Stachowski <qbast@go2.pl>
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

#include "config.h"

#include "publicservice.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netinet/in.h>
#include <sys/socket.h>
#include <qapplication.h>
#include <ksocketaddress.h>
#include <kurl.h>
#include <unistd.h>
#include <avahi-client/client.h>
#ifdef AVAHI_API_0_6
#include <avahi-client/publish.h>
#endif
#include <avahi-common/alternative.h>
#include <avahi-common/strlst.h>
#include "sdevent.h"
#include "responder.h"
#include "servicebrowser.h"
#include "settings.h"

namespace DNSSD
{
static unsigned long publicIP();

void publish_callback (AvahiEntryGroup*, AvahiEntryGroupState s,  void *context);

class PublicServicePrivate 
{
public:
	PublicServicePrivate() : m_published(false), m_running(false), m_collision(false)
	{}
	bool m_published;
	bool m_running;
	bool m_collision;
	AvahiEntryGroup* m_group;
	void commit()
	{
	    if (!m_collision) avahi_entry_group_commit(m_group);
	}    
	
};

PublicService::PublicService(const QString& name, const QString& type, unsigned int port,
			      const QString& domain)
  		: QObject(), ServiceBase(name, type, QString::null, domain, port)
{
	d = new PublicServicePrivate;
	if (Responder::self().client()) d->m_group = avahi_entry_group_new(Responder::self().client(),
	    publish_callback,this);
	connect(&Responder::self(),SIGNAL(stateChanged(AvahiClientState)),this,SLOT(clientState(AvahiClientState)));
	if (domain.isNull())
		if (Configuration::publishType()==Configuration::EnumPublishType::LAN) m_domain="local.";
		else m_domain=Configuration::publishDomain();
}


PublicService::~PublicService()
{
	if (d->m_group) avahi_entry_group_free(d->m_group);
	delete d;
}

void PublicService::tryApply()
{
    if (fillEntryGroup()) d->commit();
    else {
	stop();
	emit published(false);
    }
}

void PublicService::setServiceName(const QString& serviceName)
{
	m_serviceName = serviceName;
	if (d->m_running) {
	    avahi_entry_group_reset(d->m_group);
	    tryApply();
	} 
}

void PublicService::setDomain(const QString& domain)
{
	m_domain = domain;
	if (d->m_running) {
	    avahi_entry_group_reset(d->m_group);
	    tryApply();
	} 
}


void PublicService::setType(const QString& type)
{
	m_type = type;
	if (d->m_running) {
	    avahi_entry_group_reset(d->m_group);
	    tryApply();
	} 
}

void PublicService::setPort(unsigned short port)
{
	m_port = port;
	if (d->m_running) {
	    avahi_entry_group_reset(d->m_group);
	    tryApply();
    	} 
}

void PublicService::setTextData(const QMap<QString,QString>& textData)
{
	m_textData = textData;
	if (d->m_running) {
	    avahi_entry_group_reset(d->m_group);
	    tryApply();
	} 
}

bool PublicService::isPublished() const
{
	return d->m_published;
}

bool PublicService::publish()
{
	publishAsync();
	while (d->m_running && !d->m_published) Responder::self().process();
	return d->m_published;
}

void PublicService::stop()
{
    if (d->m_group) avahi_entry_group_reset(d->m_group);
    d->m_published = false;
}
bool PublicService::fillEntryGroup()
{
    AvahiStringList *s=0;
    QMap<QString,QString>::ConstIterator itEnd = m_textData.end();
    for (QMap<QString,QString>::ConstIterator it = m_textData.begin(); it!=itEnd ; ++it) 
	s = avahi_string_list_add_pair(s, it.key().utf8(),it.data().utf8());
#ifdef AVAHI_API_0_6
    bool res = (!avahi_entry_group_add_service_strlst(d->m_group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0, 
	m_serviceName.isNull() ? avahi_client_get_host_name(Responder::self().client()) : m_serviceName.utf8().data(),
	m_type.ascii(),domainToDNS(m_domain),m_hostName.utf8(),m_port,s));
#else
    bool res = (!avahi_entry_group_add_service_strlst(d->m_group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 
	m_serviceName.isNull() ? avahi_client_get_host_name(Responder::self().client()) : m_serviceName.utf8().data(),
	m_type.ascii(),m_domain.utf8(),m_hostName.utf8(),m_port,s));
#endif
    avahi_string_list_free(s);
    return res;
}

void PublicService::clientState(AvahiClientState s)
{
    if (!d->m_running) return;
    switch (s) {
#ifdef AVAHI_API_0_6
	case AVAHI_CLIENT_FAILURE:
#else
	case AVAHI_CLIENT_S_INVALID:
	case AVAHI_CLIENT_DISCONNECTED:
#endif
	    stop();
	    emit published(false);
	    break;
	case AVAHI_CLIENT_S_REGISTERING:
	case AVAHI_CLIENT_S_COLLISION:
	    avahi_entry_group_reset(d->m_group);
	    d->m_collision=true;
	    break;
	case AVAHI_CLIENT_S_RUNNING:
	    if (d->m_collision) {
		d->m_collision=false;
		tryApply();
	    }
    }
}				    

void PublicService::publishAsync()
{
	if (d->m_running) stop();
	
	if (!d->m_group) {
	    emit published(false);
	    return;
	}
	AvahiClientState s=avahi_client_get_state(Responder::self().client());
	d->m_running=true; 
	d->m_collision=true; // make it look like server is getting out of collision to force registering
	clientState(s);
}

void publish_callback (AvahiEntryGroup*, AvahiEntryGroupState s,  void *context)
{
	QObject *obj = reinterpret_cast<QObject*>(context);
	if (s!=AVAHI_ENTRY_GROUP_ESTABLISHED && s!=AVAHI_ENTRY_GROUP_COLLISION) return;
	PublishEvent* pev=new PublishEvent(s==AVAHI_ENTRY_GROUP_ESTABLISHED);
	QApplication::postEvent(obj, pev);
}

const KURL PublicService::toInvitation(const QString& host)
{
	KURL url;
	url.setProtocol("invitation");
	if (host.isEmpty()) { // select best address
		unsigned long s_address = publicIP();
		if (!s_address) return KURL();
		KNetwork::KIpAddress addr(s_address);
		url.setHost(addr.toString());
	} else 	url.setHost(host);
	//FIXME: if there is no public interface, select any non-loopback
	url.setPort(m_port);
	url.setPath("/"+m_type+"/"+KURL::encode_string(m_serviceName));
	QString query;
	QMap<QString,QString>::ConstIterator itEnd = m_textData.end();
	for (QMap<QString,QString>::ConstIterator it = m_textData.begin(); it!=itEnd ; ++it)
		url.addQueryItem(it.key(),it.data());;
	return url;
}

void PublicService::customEvent(QCustomEvent* event)
{
	if (event->type()==QEvent::User+SD_PUBLISH) {
		if (!static_cast<PublishEvent*>(event)->m_ok) {
		    setServiceName(QString::fromUtf8(avahi_alternative_service_name(m_serviceName.utf8())));
		    return;
		}
		d->m_published=true;
		emit published(true);
	}
}

void PublicService::virtual_hook(int, void*)
{
}

static unsigned long publicIP()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	if (sock == -1) return 0;
	addr.sin_family = AF_INET;
	addr.sin_port = 1;	// Not important, any port and public address will do
	addr.sin_addr.s_addr = 0x11111111;
	if ((connect(sock,(const struct sockaddr*)&addr,sizeof(addr))) == -1) { close(sock); return 0; }
	if ((getsockname(sock,(struct sockaddr*)&addr, &len)) == -1) { close(sock); return 0; }
	::close(sock);
	return addr.sin_addr.s_addr;
}


}

#include "publicservice.moc"
