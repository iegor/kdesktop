/* This file is part of the KDE libraries
 *  Copyright (C) 2000 Yves Arrouye <yves@realnames.com>
 *  Copyright (C) 2000 Dawit Alemayehu <adawit at kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include <config.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <ktrader.h>
#include <kmimetype.h>
#include <klibloader.h>
#include <kstaticdeleter.h>
#include <kparts/componentfactory.h>

#include "kurifilter.h"

template class QPtrList<KURIFilterPlugin>;

KURIFilterPlugin::KURIFilterPlugin( QObject *parent, const char *name, double pri )
                 :QObject( parent, name )
{
    m_strName = QString::fromLatin1( name );
    m_dblPriority = pri;
}

void KURIFilterPlugin::setFilteredURI( KURIFilterData& data, const KURL& uri ) const
{
    if ( data.uri() != uri )
    {
        data.m_pURI = uri;
        data.m_bChanged = true;
    }
}

class KURIFilterDataPrivate
{
public:
    KURIFilterDataPrivate() {};
    QString abs_path;
    QString args;
    QString typedString;
};

KURIFilterData::KURIFilterData( const KURIFilterData& data )
{
    m_iType = data.m_iType;
    m_pURI = data.m_pURI;
    m_strErrMsg = data.m_strErrMsg;
    m_strIconName = data.m_strIconName;
    m_bChanged = data.m_bChanged;
    m_bCheckForExecutables = data.m_bCheckForExecutables;
    d = new KURIFilterDataPrivate;
    d->abs_path = data.absolutePath();
    d->typedString = data.typedString();
    d->args = data.argsAndOptions();
}

KURIFilterData::~KURIFilterData()
{
    delete d;
    d = 0;
}

void KURIFilterData::init( const KURL& url )
{
    m_iType = KURIFilterData::UNKNOWN;
    m_pURI = url;
    m_strErrMsg = QString::null;
    m_strIconName = QString::null;
    m_bCheckForExecutables = true;
    m_bChanged = true;
    d = new KURIFilterDataPrivate;
    d->typedString = url.url();
}

void KURIFilterData::init( const QString& url )
{
    m_iType = KURIFilterData::UNKNOWN;
    m_pURI = url;
    m_strErrMsg = QString::null;
    m_strIconName = QString::null;
    m_bCheckForExecutables = true;
    m_bChanged = true;
    d = new KURIFilterDataPrivate;
    d->typedString = url;
}

void KURIFilterData::reinit(const KURL &url)
{
    delete d;
    init(url);
}

void KURIFilterData::reinit(const QString &url)
{
    delete d;
    init(url);
}

QString KURIFilterData::typedString() const
{
    return d->typedString;
}

void KURIFilterData::setCheckForExecutables( bool check )
{
    m_bCheckForExecutables = check;
}

bool KURIFilterData::hasArgsAndOptions() const
{
    return !d->args.isEmpty();
}

bool KURIFilterData::hasAbsolutePath() const
{
    return !d->abs_path.isEmpty();
}

bool KURIFilterData::setAbsolutePath( const QString& absPath )
{
    // Since a malformed URL could possibly be a relative
    // URL we tag it as a possible local resource...
    if( (!m_pURI.isValid() || m_pURI.isLocalFile()) )
    {
        d->abs_path = absPath;
        return true;
    }
    return false;
}

QString KURIFilterData::absolutePath() const
{
    return d->abs_path;
}

QString KURIFilterData::argsAndOptions() const
{
    return d->args;
}

QString KURIFilterData::iconName()
{
    if( m_bChanged )
    {
        switch ( m_iType )
        {
            case KURIFilterData::LOCAL_FILE:
            case KURIFilterData::LOCAL_DIR:
            case KURIFilterData::NET_PROTOCOL:
            {
                m_strIconName = KMimeType::iconForURL( m_pURI );
                break;
            }
            case KURIFilterData::EXECUTABLE:
            {
                QString exeName = m_pURI.url();
                exeName = exeName.mid( exeName.findRev( '/' ) + 1 ); // strip path if given
                KService::Ptr service = KService::serviceByDesktopName( exeName );
                if (service && service->icon() != QString::fromLatin1( "unknown" ))
                    m_strIconName = service->icon();
                // Try to find an icon with the same name as the binary (useful for non-kde apps)
                else if ( !KGlobal::iconLoader()->loadIcon( exeName, KIcon::NoGroup, 16, KIcon::DefaultState, 0, true ).isNull() )
                    m_strIconName = exeName;
                else
                    // not found, use default
                    m_strIconName = QString::fromLatin1("exec");
                break;
            }
            case KURIFilterData::HELP:
            {
                m_strIconName = QString::fromLatin1("khelpcenter");
                break;
            }
            case KURIFilterData::SHELL:
            {
                m_strIconName = QString::fromLatin1("konsole");
                break;
            }
            case KURIFilterData::ERROR:
            case KURIFilterData::BLOCKED:
            {
                m_strIconName = QString::fromLatin1("error");
                break;
            }
            default:
                m_strIconName = QString::null;
                break;
        }
        m_bChanged = false;
    }
    return m_strIconName;
}

//********************************************  KURIFilterPlugin **********************************************
void KURIFilterPlugin::setArguments( KURIFilterData& data, const QString& args ) const
{
    data.d->args = args;
}

//********************************************  KURIFilter **********************************************
KURIFilter *KURIFilter::s_self;
static KStaticDeleter<KURIFilter> kurifiltersd;

KURIFilter *KURIFilter::self()
{
    if (!s_self)
        s_self = kurifiltersd.setObject(s_self, new KURIFilter);
    return s_self;
}

KURIFilter::KURIFilter()
{
    m_lstPlugins.setAutoDelete(true);
    loadPlugins();
}

KURIFilter::~KURIFilter()
{
}

bool KURIFilter::filterURI( KURIFilterData& data, const QStringList& filters )
{
    bool filtered = false;
    KURIFilterPluginList use_plugins;

    // If we have a filter list, only include the once
    // explicitly specified by it. Otherwise, use all available filters...
    if( filters.isEmpty() )
        use_plugins = m_lstPlugins;  // Use everything that is loaded...
    else
    {
        //kdDebug() << "Named plugins requested..."  << endl;
        for( QStringList::ConstIterator lst = filters.begin(); lst != filters.end(); ++lst )
        {
            QPtrListIterator<KURIFilterPlugin> it( m_lstPlugins );
            for( ; it.current() ; ++it )
            {
                if( (*lst) == it.current()->name() )
                {
                    //kdDebug() << "Will use filter plugin named: " << it.current()->name() << endl;
                    use_plugins.append( it.current() );
                    break;  // We already found it ; so lets test the next named filter...
                }
            }
        }
    }

    QPtrListIterator<KURIFilterPlugin> it( use_plugins );
    //kdDebug() << "Using " << use_plugins.count() << " out of the "
    //          << m_lstPlugins.count() << " available plugins" << endl;
    for (; it.current() && !filtered; ++it)
    {
        //kdDebug() << "Using a filter plugin named: " << it.current()->name() << endl;
        filtered |= it.current()->filterURI( data );
    }
    return filtered;
}

bool KURIFilter::filterURI( KURL& uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    bool filtered = filterURI( data, filters );
    if( filtered ) uri = data.uri();
    return filtered;
}

bool KURIFilter::filterURI( QString& uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    bool filtered = filterURI( data, filters );
    if( filtered )  uri = data.uri().url();
    return filtered;

}

KURL KURIFilter::filteredURI( const KURL &uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    filterURI( data, filters );
    return data.uri();
}

QString KURIFilter::filteredURI( const QString &uri, const QStringList& filters )
{
    KURIFilterData data = uri;
    filterURI( data, filters );
    return data.uri().url();
}

QPtrListIterator<KURIFilterPlugin> KURIFilter::pluginsIterator() const
{
    return QPtrListIterator<KURIFilterPlugin>(m_lstPlugins);
}

QStringList KURIFilter::pluginNames() const
{
    QStringList list;
    for(QPtrListIterator<KURIFilterPlugin> i = pluginsIterator(); *i; ++i)
        list.append((*i)->name());
    return list;
}

void KURIFilter::loadPlugins()
{
    KTrader::OfferList offers = KTrader::self()->query( "KURIFilter/Plugin" );

    KTrader::OfferList::ConstIterator it = offers.begin();
    KTrader::OfferList::ConstIterator end = offers.end();

    for (; it != end; ++it )
    {
      KURIFilterPlugin *plugin = KParts::ComponentFactory::createInstanceFromService<KURIFilterPlugin>( *it, 0, (*it)->desktopEntryName().latin1() );
      if ( plugin )
        m_lstPlugins.append( plugin );
    }

    // NOTE: Plugin priority is now determined by
    // the entry in the .desktop files...
    // TODO: Config dialog to differentiate "system"
    // plugins from "user-defined" ones...
    // m_lstPlugins.sort();
}

void KURIFilterPlugin::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

#include "kurifilter.moc"
