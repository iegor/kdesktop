/***************************************************************************
                          mediaplayer.cpp - The real sidebar plugin
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

#include "mediaplayer.h"
#include <klocale.h>
#include "mediaplayer.moc"
#include <kdebug.h>
#include <ksimpleconfig.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kdemacros.h>
#include "mediawidget.h"

KonqSidebar_MediaPlayer::KonqSidebar_MediaPlayer(KInstance *instance,QObject *parent,QWidget *widgetParent, QString &desktopName_, const char* name):
                   KonqSidebarPlugin(instance,parent,widgetParent,desktopName_,name)
	{
		widget=new KSB_MediaWidget(widgetParent);
        }


KonqSidebar_MediaPlayer::~KonqSidebar_MediaPlayer(){;}

void* KonqSidebar_MediaPlayer::provides(const QString &) {return 0;}

void KonqSidebar_MediaPlayer::emitStatusBarText (const QString &) {;}

QWidget *KonqSidebar_MediaPlayer::getWidget(){return widget;}

void KonqSidebar_MediaPlayer::handleURL(const KURL &/*url*/)
    {
    }




extern "C"
{
    KDE_EXPORT void* create_konqsidebar_mediaplayer(KInstance *instance,QObject *par,QWidget *widp,QString &desktopname,const char *name)
    {
	KGlobal::locale()->insertCatalogue("konqsidebar_mediaplayer");
        return new KonqSidebar_MediaPlayer(instance,par,widp,desktopname,name);
    }
}

extern "C"
{
   KDE_EXPORT bool add_konqsidebar_mediaplayer(QString* fn, QString* /*param*/, QMap<QString,QString> *map)
   {
		        map->insert("Type","Link");
			map->insert("Icon","konqsidebar_mediaplayer");
			map->insert("Name",i18n("Media Player"));
		 	map->insert("Open","false");
			map->insert("X-KDE-KonqSidebarModule","konqsidebar_mediaplayer");
			fn->setLatin1("mplayer%1.desktop");
			return true;
   }
}

