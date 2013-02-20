//////////////////////////////////////////////////////////////////////////
// plugin.h                                                             //
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

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include "mainWidget.h"

#include <konqsidebarplugin.h>
#include <kparts/part.h>
#include <kparts/factory.h>
#include <kparts/browserextension.h>
#include <kinstance.h>
#include <dcopobject.h>

class QString;

/**
 * @brief del.icio.us bookmarks plugin
 * @author Lukas Tinkl <lukas@kde.org>
 */
class KonqSidebarDelicious: public KonqSidebarPlugin, DCOPObject
{
    Q_OBJECT
    K_DCOP
public:
    KonqSidebarDelicious( KInstance * instance, QObject * parent, QWidget * widgetParent,
                          QString & desktopName_, const char * name = 0 );
    ~KonqSidebarDelicious();
    virtual void * provides( const QString & );
    /**
     * @return the main widget
     */
    virtual QWidget * getWidget();

k_dcop:
    QStringList tags() const;
    QStringList bookmarks() const;
    void newBookmark();

protected:
    /**
     * Called when the shell's @p url changes
     */
    virtual void handleURL( const KURL & url );

    /**
     * @return true if we are in universal (e.g. desktop) mode
     */
    bool universalMode();

signals:
    /**
     * Emitted in order to open @p url in the shell app
     */
    void openURLRequest( const KURL & url, const KParts::URLArgs & args = KParts::URLArgs() );

    /**
     * Emitted in order to open @p url in the shell app's new window
     */
    void createNewWindow( const KURL & url, const KParts::URLArgs & args = KParts::URLArgs() );

private:
    MainWidget * m_widget;
};

#endif
