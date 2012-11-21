/*
 *  This file is part of the KDE project
 *  Copyright (C) 2002 Ian Reinhart Geiser <geiseri@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2.0 as published by the Free Software Foundation.
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
 *
 *  $Id: weatherbar.h 465369 2005-09-29 14:33:08Z mueller $
 */

#ifndef __KONQSIDEBARWEATHER_H__
#define __KONQSIDEBARWEATHER_H__

#include <konqsidebarplugin.h>
#include <kparts/part.h>
#include <kparts/factory.h>
#include <kparts/browserextension.h>
#include <qdict.h>
#include <dcopobject.h>
#include <qlayout.h>
#include <qtimer.h>

class dockwidget;
class sidebarwidget;

class KonqSidebarWeather: public KonqSidebarPlugin, virtual public DCOPObject
{
    Q_OBJECT
    K_DCOP
public:
    KonqSidebarWeather(KInstance* inst, QObject* parent, QWidget* widgetParent,
                        QString& desktopName_, const char* name = 0);

    ~KonqSidebarWeather();
    virtual void* provides(const QString&);
    void emitStatusBarText(const QString&);
    virtual QWidget *getWidget();

    k_dcop:
    virtual void refresh(QString);
        
protected:
    virtual void handleURL(const KURL &url);
    virtual void handlePreview(const KFileItemList& items);
    virtual void handlePreviewOnMouseOver(const KFileItem& item);

private slots:
	void update();
	
private:
    QDict <dockwidget> m_widgets;
    sidebarwidget *m_container;
    QTimer *timeOut;
};

#endif

