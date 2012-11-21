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
 *  $Id: weatherbar.cpp 465369 2005-09-29 14:33:08Z mueller $
 */

#include "weatherbar.h"
#include "dockwidget.h"
#include "sidebarwidget.h"

#include <qlabel.h>
#include <qfont.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qgroupbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <dcopref.h>


KonqSidebarWeather::KonqSidebarWeather(KInstance* inst, QObject* parent,
                                         QWidget* widgetParent,
                                         QString& desktopName, const char* name)

    : KonqSidebarPlugin(inst, parent, widgetParent, desktopName, name),
    DCOPObject(name)
{

	m_container = new sidebarwidget(widgetParent,"sidebarwidget");

	kdDebug() << "Calling Get Widget" << endl;

	kdDebug() << "Get weatherstation list... " << endl;

	if (!connectDCOPSignal(0,0,
		"fileUpdate(QString)",
		"refresh(QString)",false))
		kdDebug() << "Could not attach signal..." << endl;
	else
		kdDebug() << "attached dcop signals..." << endl;

	DCOPRef dcopCall( "KWeatherService", "WeatherService" );
	DCOPReply reply = dcopCall.call("listStations()", true );
	if ( reply.isValid() ) {
		QStringList replyList = reply;
		for(int i = 0; i < replyList.size(); i++)
		{
			dockwidget *d = new dockwidget(m_container->viewport(), replyList[i].latin1());
			m_container->addWidget(d, replyList[i].latin1());
			d->resizeView(80,48);
			d->show();
			m_widgets.insert(replyList[i], d);
			dcopCall.send("update(QString)", replyList[i]);
		}
	}

	timeOut = new QTimer(this, "timeOut" );
	timeOut->changeInterval(15*60000);
	connect(timeOut, SIGNAL(timeout()), this, SLOT(update()));
   // m_widgets.append(new dockwidget(widgetParent));
}


KonqSidebarWeather::~KonqSidebarWeather()
{
}

void* KonqSidebarWeather::provides(const QString&)
{
    return 0;
}

void KonqSidebarWeather::emitStatusBarText(const QString& s)
{
}

QWidget* KonqSidebarWeather::getWidget()
{
	return m_container;
}

void KonqSidebarWeather::refresh(QString stationID)
{
	kdDebug() << "refresh " << stationID << endl;
	if(m_widgets.find(stationID))
	{
		DCOPRef dcopCall( "KWeatherService", "WeatherService" );
		m_widgets[stationID]->setWeatherIcon(dcopCall.call("currentIcon(QString)", stationID ,true ));
		m_widgets[stationID]->setTemperature(dcopCall.call("temperature(QString)", stationID,true ));
		m_widgets[stationID]->setPressure(dcopCall.call("pressure(QString)", stationID,true ));
		m_widgets[stationID]->setWind(dcopCall.call("wind(QString)", stationID,true ));
		m_widgets[stationID]->showWeather();
	}
	else
		update();
}

void KonqSidebarWeather::handleURL(const KURL& url)
{
}

void KonqSidebarWeather::handlePreview(const KFileItemList& items)
{
}

void KonqSidebarWeather::handlePreviewOnMouseOver(const KFileItem& item)
{
}

void KonqSidebarWeather::update()
{
	timeOut->stop();
	DCOPRef dcopCall( "KWeatherService", "WeatherService" );
	DCOPReply reply = dcopCall.call("listStations()", true );
	if ( reply.isValid() ) {
		QStringList replyList = reply;
		for(int i = 0; i < replyList.size(); i++)
		{
			if(!m_widgets.find(replyList[i]))
			{
				dockwidget *d = new dockwidget(m_container->viewport(), replyList[i].latin1());
				m_container->addWidget(d, replyList[i].latin1());
				d->show();
				m_widgets.insert(replyList[i], d);
			}
			dcopCall.send("update(QString)", replyList[i]);
		}
	}
    	timeOut->start(15*60000);
}

extern "C"
{
    KDE_EXPORT void* create_weather_sidebar(KInstance* inst, QObject* par, QWidget*widp,
                                      QString& desktopname, const char* name)
    {
        return new KonqSidebarWeather(inst, par, widp, desktopname, name);
    }

    KDE_EXPORT bool add_weather_sidebar(QString* fn, QString* /*param*/,
                                  QMap<QString, QString>* map)
    {
                    map->insert("Type","Link");
                        map->insert("Icon","weather_sidebar");
                        map->insert("Name",i18n("Sidebar Weather Report"));
                        map->insert("Open","false");
                          map->insert("X-KDE-KonqSidebarModule","weather_sidebar");
                        fn->setLatin1("weatherbar%1.desktop");
                        return true;
    }
}

#include "weatherbar.moc"

