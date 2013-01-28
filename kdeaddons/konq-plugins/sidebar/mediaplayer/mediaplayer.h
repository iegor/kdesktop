/***************************************************************************
                          mediaplayer.h - The real sidebar plugin
                             -------------------
    begin                : Sat June 23 13:35:30 CEST 2001
    copyright            : (C) 2001 Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _konq_sidebar_mediaplayer_h_
#define _konq_sidebar_mediaplayer_h_
#include <konqsidebarplugin.h>
#include <kparts/part.h>
#include <kparts/factory.h>
#include <kparts/browserextension.h>

class KSB_MediaWidget;

class KonqSidebar_MediaPlayer: public KonqSidebarPlugin
        {
                Q_OBJECT
                public:
                KonqSidebar_MediaPlayer(KInstance *instance,QObject *parent,QWidget *widgetParent, QString &desktopName_, const char* name=0);
                ~KonqSidebar_MediaPlayer();
                virtual void *provides(const QString &);
		void emitStatusBarText (const QString &);
                virtual QWidget *getWidget();
                protected:
                        virtual void handleURL(const KURL &url);
		private:
			KSB_MediaWidget *widget;
        };


#endif
