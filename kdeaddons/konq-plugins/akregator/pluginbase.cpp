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

#include <kurl.h>
#include <kprocess.h>
#include <dcopref.h>
#include <khtml_part.h>
#include <klocale.h>

#include "feeddetector.h"
#include "pluginbase.h"
#include <qstringlist.h>

#include <kdebug.h>

using namespace Akregator;

PluginBase::PluginBase()
{}

PluginBase::~PluginBase()
{}

bool PluginBase::akregatorRunning()
{
    DCOPRef akr("akregator", "akregator");
    DCOPReply reply = akr.call("interfaces"); // FIXME hackish but works :) -tpr 20041203
    return reply.isValid();
}

void PluginBase::addFeedsViaDCOP(const QStringList& urls)
{
    kdDebug() << "PluginBase::addFeedsViaDCOP" << endl;
    DCOPRef akr("akregator", "AkregatorIface");
    akr.send("addFeedsToGroup", urls, i18n("Imported Feeds") );
    /*if(!reply.isValid()) {
    KMessageBox::error( 0, i18n( "Akregator feed icon - DCOP Call failed" ),
    i18n( "The DCOP call addFeedToGroup failed" ));
}*/
}

void PluginBase::addFeedViaCmdLine(QString url)
{
    KProcess *proc = new KProcess;
    *proc << "akregator" << "-g" << i18n("Imported Feeds");
    *proc << "-a" << url;
    proc->start(KProcess::DontCare);
    delete proc;
}

// handle all the wild stuff that KURL doesn't handle
QString PluginBase::fixRelativeURL(const QString &s, const KURL &baseurl)
{
    QString s2=s;
    KURL u;
    if (KURL::isRelativeURL(s2))
    {
        if (s2.startsWith("//"))
        {
            s2=s2.prepend(baseurl.protocol()+":");
            u=s2;
        }
        else if (s2.startsWith("/"))
        {
            KURL b2(baseurl);
            b2.setPath(QString()); // delete path and query, so that only protocol://host remains
            b2.setQuery(QString());
            u = KURL(b2, s2.remove(0,1)); // remove leading "/" 
        }
        else
        {
            u = KURL(baseurl, s2);
        }
    }
    else
        u=s2;

    u.cleanPath();
    //kdDebug() << "AKREGATOR_PLUGIN_FIXURL: " << "url=" << s << " baseurl=" << baseurl.url() << " fixed=" << u.url() << endl;
    return u.url();
}
