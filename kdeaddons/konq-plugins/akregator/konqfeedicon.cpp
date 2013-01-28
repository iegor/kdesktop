/*
    This file is part of Akregator.

    Copyright (C) 2004 Teemu Rytilahti <tpr@d5k.net>

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

#include "konqfeedicon.h"
#include "feeddetector.h"
#include "pluginbase.h"

#include <dcopref.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <khtml_part.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kparts/statusbarextension.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kurllabel.h>
#include <kurl.h>

#include <qcursor.h>
#include <qobjectlist.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qstylesheet.h>
#include <qtimer.h>
#include <qtooltip.h>

using namespace Akregator;

typedef KGenericFactory<KonqFeedIcon> KonqFeedIconFactory;
K_EXPORT_COMPONENT_FACTORY(libakregatorkonqfeedicon,
                           KonqFeedIconFactory("akregatorkonqfeedicon"))

KonqFeedIcon::KonqFeedIcon(QObject *parent, const char *name, const QStringList &)
    : KParts::Plugin(parent, name), PluginBase(), m_part(0), m_feedIcon(0), m_statusBarEx(0), m_menu(0)
{
    KGlobal::locale()->insertCatalogue("akregator_konqplugin");

    m_part = dynamic_cast<KHTMLPart*>(parent);
    if(!m_part) { kdDebug() << "couldn't get part" << endl; return; }
    // FIXME: need to do this because of a bug in khtmlpart, it's fixed now for 3.4 (and prolly backported for 3.3.3?)
    //connect(m_part->view(), SIGNAL(finishedLayout()), this, SLOT(addFeedIcon()));
    QTimer::singleShot(0, this, SLOT(waitPartToLoad()));
}

void KonqFeedIcon::waitPartToLoad()
{
    connect(m_part, SIGNAL(completed()), this, SLOT(addFeedIcon()));
    connect(m_part, SIGNAL(completed(bool)), this, SLOT(addFeedIcon())); // to make pages with metarefresh to work
    connect(m_part, SIGNAL(started(KIO::Job *)), this, SLOT(removeFeedIcon()));
    addFeedIcon();
}

KonqFeedIcon::~KonqFeedIcon()
{
    KGlobal::locale()->removeCatalogue("akregator_konqplugin");
    m_statusBarEx = KParts::StatusBarExtension::childObject(m_part);
    if (m_statusBarEx)
    {
        m_statusBarEx->removeStatusBarItem(m_feedIcon);
        // the feed icon is child of the statusbar extension, so if the statusbar is deleted, 
        // the icon was deleted by the status bar
        delete m_feedIcon;
    }
    // the icon is deleted in every case
    m_feedIcon = 0L;

    delete m_menu;
    m_menu = 0L;
}

bool KonqFeedIcon::feedFound()
{
	DOM::NodeList linkNodes = m_part->document().getElementsByTagName("link");
	
	if (linkNodes.length() == 0) 
        return false;

    unsigned int i;
    QString doc = "";

	for (i = 0; i < linkNodes.length(); i++) 
	{
        DOM::Node node = linkNodes.item(i);
        doc += "<link ";
        for (unsigned int j = 0; j < node.attributes().length(); j++)
        {
            doc += node.attributes().item(j).nodeName().string() + "=\"";
            doc += QStyleSheet::escape(node.attributes().item(j).nodeValue().string()).replace("\"", "&quot;");
            doc += "\" ";
        }
        doc += "/>";
    }

    m_feedList = FeedDetector::extractFromLinkTags(doc);
    return m_feedList.count() != 0;
}

void KonqFeedIcon::contextMenu()
{
    delete m_menu;
    m_menu = new KPopupMenu(m_part->widget());
    if(m_feedList.count() == 1) {
        m_menu->insertTitle(m_feedList.first().title());
        m_menu->insertItem(SmallIcon("bookmark_add"), i18n("Add Feed to Akregator"), this, SLOT(addFeeds()) );
    }
    else {
        m_menu->insertTitle(i18n("Add Feeds to Akregator"));
        connect(m_menu, SIGNAL(activated(int)), this, SLOT(addFeed(int)));
        int id = 0;
        for(FeedDetectorEntryList::Iterator it = m_feedList.begin(); it != m_feedList.end(); ++it) {
            m_menu->insertItem(SmallIcon("bookmark_add"), (*it).title(), id);
            id++;
        }
        //disconnect(m_menu, SIGNAL(activated(int)), this, SLOT(addFeed(int)));
        m_menu->insertSeparator();
        m_menu->insertItem(SmallIcon("bookmark_add"), i18n("Add All Found Feeds to Akregator"), this, SLOT(addFeeds()), 0, 50000 );
    }
    m_menu->popup(QCursor::pos());
}

void KonqFeedIcon::addFeedIcon()
{
    if(!feedFound() || m_feedIcon) {
        return;
    }

    m_statusBarEx = KParts::StatusBarExtension::childObject(m_part);
    if(!m_statusBarEx) return;

    m_feedIcon = new KURLLabel(m_statusBarEx->statusBar());

    // from khtmlpart's ualabel
    m_feedIcon->setFixedHeight(instance()->iconLoader()->currentSize(KIcon::Small));
    m_feedIcon->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    m_feedIcon->setUseCursor(false);
    //FIXME hackish
    m_feedIcon->setPixmap(QPixmap(locate("data", "akregator/pics/rss.png")));

    QToolTip::remove(m_feedIcon);
    QToolTip::add(m_feedIcon, i18n("Monitor this site for updates (using news feed)"));

    m_statusBarEx->addStatusBarItem(m_feedIcon, 0, true);

    connect(m_feedIcon, SIGNAL(leftClickedURL()), this, SLOT(contextMenu()));
}

void KonqFeedIcon::removeFeedIcon()
{
    m_feedList.clear();
    if(m_feedIcon) 
    {
        m_statusBarEx->removeStatusBarItem(m_feedIcon);
        delete m_feedIcon;
        m_feedIcon = 0L;
    }

    // Close the popup if it's open, otherwise we crash
    delete m_menu;
    m_menu = 0L;
}

void KonqFeedIcon::addFeed(int id)
{
    if(id == 50000) return;
    if(akregatorRunning())
        addFeedsViaDCOP(QStringList(fixRelativeURL(m_feedList[id].url(), m_part->baseURL())));
    else
        addFeedViaCmdLine(fixRelativeURL(m_feedList[id].url(), m_part->baseURL()));
}

// from akregatorplugin.cpp
void KonqFeedIcon::addFeeds()
{
    if(akregatorRunning()) 
    {
      	QStringList list;
        for ( FeedDetectorEntryList::Iterator it = m_feedList.begin(); it != m_feedList.end(); ++it )
            list.append(fixRelativeURL((*it).url(), m_part->baseURL()));
        addFeedsViaDCOP(list);
    }
    else {
        kdDebug() << "KonqFeedIcon::addFeeds(): use command line" << endl;
        KProcess *proc = new KProcess;
        *proc << "akregator" << "-g" << i18n("Imported Feeds");

        for ( FeedDetectorEntryList::Iterator it = m_feedList.begin(); it != m_feedList.end(); ++it ) {
            *proc << "-a" << fixRelativeURL((*it).url(), m_part->baseURL());
        }

        proc->start(KProcess::DontCare);
        delete proc;

    }
}

#include "konqfeedicon.moc"
// vim: ts=4 sw=4 et
