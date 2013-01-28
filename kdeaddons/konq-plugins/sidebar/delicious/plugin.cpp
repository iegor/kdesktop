//////////////////////////////////////////////////////////////////////////
// plugin.cpp                                                           //
//                                                                      //
// Copyright (C)  2005  Lukas Tinkl <lukas@kde.org>                     //
//                                                                      //
// This program is free software; you can redistribute it and/or        //
// modify it under the terms of the GNU General Public License          //
// as published by the Free Software Foundation; either version 2       //
// of the License, or (at your option) any later version.               //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA            //
// 02111-1307, USA.                                                     //
//////////////////////////////////////////////////////////////////////////

#include "plugin.h"

#include <qstring.h>

#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>

KonqSidebarDelicious::KonqSidebarDelicious( KInstance *instance, QObject *parent,
                                            QWidget *widgetParent, QString &desktopName_,
                                            const char* name )
    : KonqSidebarPlugin( instance, parent, widgetParent, desktopName_, name ),
      DCOPObject( "sidebar-delicious" )

{
    m_widget = new MainWidget( instance->config(), widgetParent );
    connect( m_widget, SIGNAL( signalURLClicked( const KURL &, const KParts::URLArgs & ) ),
             this, SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ) );
    connect( m_widget, SIGNAL( signalURLMidClicked( const KURL &, const KParts::URLArgs & ) ),
             this, SIGNAL( createNewWindow( const KURL &, const KParts::URLArgs & ) ) );
}

KonqSidebarDelicious::~KonqSidebarDelicious()
{

}

void * KonqSidebarDelicious::provides( const QString &  )
{
    return 0;
}

QWidget * KonqSidebarDelicious::getWidget()
{
    return m_widget;
}

void KonqSidebarDelicious::handleURL( const KURL & url )
{
    m_widget->setCurrentURL( url );
}

bool KonqSidebarDelicious::universalMode()
{
    return true;
}

extern "C"
{
    KDE_EXPORT void* create_konqsidebar_delicious( KInstance *instance, QObject *par, QWidget *widp,
                                                   QString &desktopname, const char *name )
    {
        KGlobal::locale()->insertCatalogue( "konqsidebar_delicious" );
        return new KonqSidebarDelicious( instance, par, widp, desktopname, name );
    }
}

extern "C"
{
   KDE_EXPORT bool add_konqsidebar_delicious( QString* fn, QString* /*param*/, QMap<QString,QString> *map )
   {
       map->insert("Type", "Link");
       map->insert("Icon", "konqsidebar_delicious");
       map->insert("Name", i18n( "del.icio.us Bookmarks" ) );
       map->insert("Open", "false");
       map->insert("X-KDE-KonqSidebarModule", "konqsidebar_delicious");
       fn->setLatin1("delicious%1.desktop");
       return true;
   }
}

QStringList KonqSidebarDelicious::tags() const
{
    return m_widget->tags();
}

QStringList KonqSidebarDelicious::bookmarks() const
{
    return m_widget->bookmarks();
}

void KonqSidebarDelicious::newBookmark()
{
    m_widget->slotNewBookmark();
}

#include "plugin.moc"
