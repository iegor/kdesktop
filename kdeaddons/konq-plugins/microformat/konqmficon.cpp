/*
    Copyright (C) 2004 Teemu Rytilahti <tpr@d5k.net>
    Copyright (C) 2005 George Staikos <staikos@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "konqmficon.h"

#include <dcopref.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kiconloader.h>
#include <kparts/statusbarextension.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kurllabel.h>

#include <qcursor.h>
#include <qstylesheet.h>
#include <qtimer.h>
#include <qtooltip.h>

typedef KGenericFactory<KonqMFIcon> KonqMFIconFactory;
K_EXPORT_COMPONENT_FACTORY(libmfkonqmficon,
                           KonqMFIconFactory("mfkonqmficon"))

KonqMFIcon::KonqMFIcon(QObject *parent, const char *name, const QStringList &)
: KParts::Plugin(parent, name), PluginBase(), m_part(0), m_mfIcon(0), m_statusBarEx(0), m_menu(0) {
	KGlobal::locale()->insertCatalogue("mf_konqplugin");

	m_part = dynamic_cast<KHTMLPart*>(parent);
	if (!m_part) { 
		kdDebug() << "couldn't get part" << endl;
		return;
	}
	QTimer::singleShot(0, this, SLOT(waitPartToLoad()));
}


void KonqMFIcon::waitPartToLoad() {
	connect(m_part, SIGNAL(completed()), this, SLOT(addMFIcon()));
	connect(m_part, SIGNAL(completed(bool)), this, SLOT(addMFIcon())); // to make pages with metarefresh to work
	connect(m_part, SIGNAL(started(KIO::Job *)), this, SLOT(removeMFIcon()));
}


KonqMFIcon::~KonqMFIcon() {
	KGlobal::locale()->removeCatalogue("mf_konqplugin");
	delete m_menu;
	m_menu = 0L;
}


static QString textForNode(DOM::Node node) {
	QString rc;
	DOM::NodeList nl = node.childNodes();
	for (unsigned int i = 0; i < nl.length(); ++i) {
		DOM::Node n = nl.item(i);
		if (n.nodeType() == DOM::Node::TEXT_NODE) {
			rc += n.nodeValue().string();
		}
	}
	// FIXME: entries need to be escaped for vcard/vevent
	return rc.stripWhiteSpace();
}


static QString extractAddress(DOM::Node node) {
	QString rc = ";;";
	QMap<QString,QString> entry;
	DOM::NodeList nodes = node.childNodes();
	unsigned int n = nodes.length();
	for (unsigned int i = 0; i < n; ++i) {
		DOM::Node node = nodes.item(i);
		DOM::NamedNodeMap map = node.attributes();
		for (unsigned int j = 0; j < map.length(); ++j) {
			if (map.item(j).nodeName().string() != "class") {
				continue;
			}
			QString a = map.item(j).nodeValue().string();
			if (a == "street-address") {
				entry["street-address"] = textForNode(node);
			} else if (a == "locality") {
				entry["locality"] = textForNode(node);
			} else if (a == "region") {
				entry["region"] = textForNode(node);
			} else if (a == "postal-code") {
				entry["postal-code"] = textForNode(node);
			}
		}
	}

	rc += entry["street-address"] + ";" + entry["locality"] + ";" + entry["region"] + ";" + entry["postal-code"] + ";" + entry["country"];
	return rc.stripWhiteSpace();
}


void KonqMFIcon::extractCard(DOM::Node node) {
	QString name, value;
	DOM::NodeList nodes = node.childNodes();
	unsigned int n = nodes.length();
	value += "BEGIN:VCARD\nVERSION:3.0\n";
	for (unsigned int i = 0; i < n; ++i) {
		DOM::Node node = nodes.item(i);
		DOM::NamedNodeMap map = node.attributes();
		for (unsigned int j = 0; j < map.length(); ++j) {
			if (map.item(j).nodeName().string() != "class") {
				continue;
			}
			QStringList l = QStringList::split(' ', map.item(j).nodeValue().string());
			for (QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
				if (*it == "photo") {
				} else if (*it == "adr") {
					value += "ADR:" + extractAddress(node) + "\n";
				} else if (*it == "tel") {
					value += "TEL;TYPE=VOICE:" + textForNode(node) + "\n";
				} else if (*it == "fn") {
					name = textForNode(node);
					value += "FN:" + name + "\n";
				} else if (*it == "url") {
					DOM::Node at = node.attributes().getNamedItem("href");
					if (!at.isNull()) {
						value += "URL:" + at.nodeValue().string().stripWhiteSpace() + "\n";
					}
				} else if (*it == "email") {
					DOM::Node at = node.attributes().getNamedItem("href");
					if (!at.isNull()) {
						QString v = at.nodeValue().string();
						if (v.startsWith("mailto:")) {
							v = v.mid(7);
						}
						value += "EMAIL:" + v.stripWhiteSpace() + "\n";
					}
				} else if (*it == "org") {
					value += "ORG:" + textForNode(node) + "\n";
				}
			}
		}
	}

	if (!name.isEmpty()) {
		value += "END:VCARD\n";
		_cards.append(qMakePair(name, value));
	}
}


void KonqMFIcon::extractEvent(DOM::Node node) {
	QString name, value = "BEGIN:VCALENDAR\nPRODID:-//Konqueror//EN\nVERSION:2.0\nBEGIN:VEVENT\n";
	DOM::NodeList nodes = node.childNodes();
	unsigned int n = nodes.length();
	for (unsigned int i = 0; i < n; ++i) {
		DOM::Node node = nodes.item(i);
		DOM::NamedNodeMap map = node.attributes();
		for (unsigned int j = 0; j < map.length(); ++j) {
			if (map.item(j).nodeName().string() != "class") {
				continue;
			}
			QStringList l = QStringList::split(' ', map.item(j).nodeValue().string());
			for (QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
				if (*it == "url") {
					DOM::Node at = node.attributes().getNamedItem("href");
					if (!at.isNull()) {
						value += "URL:" + at.nodeValue().string().stripWhiteSpace() + "\n";
					}
				} else if (*it == "dtstart") {
					DOM::Node at = node.attributes().getNamedItem("title");
					if (!at.isNull()) {
						value += "DTSTART:" + at.nodeValue().string().stripWhiteSpace() + "\n";
					}
				} else if (*it == "dtend") {
					DOM::Node at = node.attributes().getNamedItem("title");
					if (!at.isNull()) {
						value += "DTEND:" + at.nodeValue().string().stripWhiteSpace() + "\n";
					}
				} else if (*it == "summary") {
					name = textForNode(node);
					value += "SUMMARY:" + name + "\n";
				} else if (*it == "location") {
					value += "LOCATION:" + textForNode(node) + "\n";
				}
			}
		}
	}

	if (!name.isEmpty()) {
		value += "END:VEVENT\nEND:VCALENDAR\n";
		_events.append(qMakePair(name, value));
	}
}


bool KonqMFIcon::hasMicroFormat(DOM::NodeList nodes) {
	bool ok = false;
	unsigned int n = nodes.length();
	for (unsigned int i = 0; i < n; ++i) {
		DOM::Node node = nodes.item(i);
		DOM::NamedNodeMap map = node.attributes();
		for (unsigned int j = 0; j < map.length(); ++j) {
			if (map.item(j).nodeName().string() != "class") {
				continue;
			}
			if (map.item(j).nodeValue().string() == "vevent") {
				ok = true;
				extractEvent(node);
				break;
			}
			if (map.item(j).nodeValue().string() == "vcard") {
				ok = true;
				extractCard(node);
				break;
			}
		}
		if (hasMicroFormat(node.childNodes())) {
			ok = true;
		}
	}
	return ok;
}


bool KonqMFIcon::mfFound() {
	_events.clear();
	_cards.clear();
	return hasMicroFormat(m_part->document().childNodes());
}


void KonqMFIcon::contextMenu() {
	delete m_menu;
	m_menu = new KPopupMenu(m_part->widget());
	m_menu->insertTitle(i18n("Microformats"));
	connect(m_menu, SIGNAL(activated(int)), this, SLOT(addMF(int)));
	int id = 0;
	for (QValueList<QPair<QString, QString> >::ConstIterator it = _events.begin(); it != _events.end(); ++it) {
		m_menu->insertItem(SmallIcon("bookmark_add"), (*it).first, id);
		id++;
	}
	for (QValueList<QPair<QString, QString> >::ConstIterator it = _cards.begin(); it != _cards.end(); ++it) {
		m_menu->insertItem(SmallIcon("bookmark_add"), (*it).first, id);
		id++;
	}
	m_menu->insertSeparator();
	m_menu->insertItem(SmallIcon("bookmark_add"), i18n("Import All Microformats"), this, SLOT(addMFs()), 0, 50000 );
	m_menu->popup(QCursor::pos());
}


void KonqMFIcon::addMFIcon() {
	if (!mfFound() || m_mfIcon) {
		return;
	}

	m_statusBarEx = KParts::StatusBarExtension::childObject(m_part);
	if (!m_statusBarEx) {
		return;
	}

	m_mfIcon = new KURLLabel(m_statusBarEx->statusBar());
	m_mfIcon->setFixedHeight(instance()->iconLoader()->currentSize(KIcon::Small));
	m_mfIcon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_mfIcon->setUseCursor(false);
	//FIXME hackish
	m_mfIcon->setPixmap(QPixmap(locate("data", "microformat/pics/microformat.png")));

	QToolTip::remove(m_mfIcon);
	QToolTip::add(m_mfIcon, i18n("This site has a microformat entry", "This site has %n microformat entries", _events.count() + _cards.count()));

	m_statusBarEx->addStatusBarItem(m_mfIcon, 0, true);

	connect(m_mfIcon, SIGNAL(leftClickedURL()), this, SLOT(contextMenu()));
}


void KonqMFIcon::removeMFIcon() {
	_events.clear();
	_cards.clear();
	if (m_mfIcon) {
		m_statusBarEx->removeStatusBarItem(m_mfIcon);
		delete m_mfIcon;
		m_mfIcon = 0;
	}

	// Close the popup if it's open, otherwise we crash
	delete m_menu;
	m_menu = 0L;
}


void KonqMFIcon::addMF(int id) {
	if (id < int(_events.count())) {
	} else if (id < int(_cards.count())) {
		id -= _cards.count() - 1;
		addVCardViaDCOP(_cards[id].second);
	}
}


void KonqMFIcon::addMFs() {
	int n = _events.count() + _cards.count();
	for (int i = 0; i < n; ++i) {
		addMF(i);
	}
}

#include "konqmficon.moc"
