/***************************************************************************
                          reportview.cpp  -  description
                             -------------------
    begin                : Sun Oct 15 2000
    copyright            : (C) 2000 by Ian Reinhart Geiser <geiseri@msoe.edu>
                         : ( C ) 2002-2004 Nadeem Hasan <nhasan@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kglobalsettings.h>

#include <qvbox.h>
#include <qpixmap.h>
#include <qapplication.h>

#include "reportview.h"
#include "weatherservice_stub.h"

reportView::reportView(const QString &reportLocation)
        : KDialogBase( (QWidget *)0, "report", false, QString::null, Close ),
	  m_locationCode(reportLocation)
{
    QVBox *vbox = makeVBoxMainWidget();
    m_reportView = new KHTMLPart(vbox, "m_reportView");

    KConfig config( "weather_panelappletrc" );
    config.setGroup( "General Options" );
    QSize defaultSize( 450, 325 );
    resize( config.readSizeEntry( "reportview_size", &defaultSize ) );

    centerOnScreen( this );

    m_weatherService = new WeatherService_stub( "KWeatherService", "WeatherService" );

    QPixmap icon = m_weatherService->icon( m_locationCode );
    setIcon( icon );

    render();
}

reportView::~reportView(){
    delete m_weatherService;
    // we do not have to delete m_reportView because this class is
    // the parent of the QVBox, and that is the parent of the KHTMLPart.

    KConfig config( "weather_panelappletrc" );
    config.setGroup( "General Options" );
    config.writeEntry( "reportview_size", size() );
}

/** Render the document */
void reportView::render(){
    QFont generalFont = KGlobalSettings::generalFont();
    QString fntFamily = generalFont.family();
    int fntSize = generalFont.pointSize();
    if (fntSize == -1)
       fntSize = QFontInfo(generalFont).pointSize();

    QString textColor = KGlobalSettings::textColor().name();
    QString baseColor = KGlobalSettings::baseColor().name();
    QColorGroup cg = palette().active();
    QString bgColor = cg.background().name();
    QString hlColor = cg.highlight().name();
    QString hlTextColor = cg.highlightedText().name();

    QString locationName = m_weatherService->stationName(m_locationCode);
    QString countryName = m_weatherService->stationCountry(m_locationCode);
    QString temp = m_weatherService->temperature(m_locationCode);
    QString dewPoint = m_weatherService->dewPoint( m_locationCode);
    QString relHumidity = m_weatherService->relativeHumidity(m_locationCode );
    QString heatIndex = m_weatherService->heatIndex(m_locationCode );
    QString windChill = m_weatherService->windChill(m_locationCode );
    QString pressure = m_weatherService->pressure(m_locationCode );
    QString wind = m_weatherService->wind(m_locationCode );
    QString sunRiseTime = m_weatherService->sunRiseTime(m_locationCode );
    QString sunSetTime = m_weatherService->sunSetTime(m_locationCode );
    QString date = m_weatherService->date(m_locationCode );
    QString icon = m_weatherService->iconFileName(m_locationCode );
    QStringList cover = m_weatherService->cover(m_locationCode );
    QStringList weather = m_weatherService->weather(m_locationCode );

    setCaption(i18n("Weather Report - %1").arg( locationName ) );

    QString weatherText = "<ul>\n";

    if ( m_weatherService->stationNeedsMaintenance( m_locationCode ) )
    {
        weatherText += "<li>" + i18n( "Station reports that it needs maintenance" ) + " \n";
    }
    for (QStringList::const_iterator it = cover.begin();
            it != cover.end(); ++it)
        weatherText += "<li>" + *it + "\n";

    for (QStringList::const_iterator it = weather.begin();
            it != weather.end(); ++it)
        weatherText += "<li>" + *it + "\n";

    weatherText += "</ul>\n";

    QString contents =
    "<html><head><style type=\"text/css\">" +
    QString("body { font-family: \"%1\"; font-size: %2pt; color: %3; background-color: %4; }\n")
    .arg(fntFamily).arg(fntSize).arg(textColor).arg(baseColor) +
    QString("div.headerTitle { background-color: %1; color: %2; padding: 4px; font-size: 120%; border: solid %3 1px; }\n")
    .arg(hlColor).arg(hlTextColor).arg(textColor) +
    QString("div.headerMsg { background-color: %1; color: %2; border-bottom: solid %3 1px; "
    "border-left: solid %4 1px; border-right: solid %5 1px; margin-bottom: 1em; padding: 2px; }\n")
    .arg(bgColor).arg(textColor).arg(textColor).arg(textColor).arg(textColor) +    
    QString("</style><title></title></head><body dir=\"%1\">").arg( QApplication::reverseLayout()?"rtl":"ltr") + 
    "<div class=\"headerTitle\"><b>" + i18n( "Weather Report - %1 - %2" ).arg( locationName ).arg( countryName ) +        
    "</b></div>\n";

    if ( ! date.isEmpty() )
      contents += "<div class=\"headerMsg\"><b>" + i18n( "Latest data from %1" ).arg(date) + "</b></div>\n";

    contents += QString(
    "<table><tr><td width=\"60\" style=\"text-align: center; border: dotted %1 1px;\">"
    "<img width=\"64\" height=\"64\" src=\"%2\" /></td>"
    "<td style=\"vertical-align: top\">%3</td></tr>")
    .arg(bgColor).arg(KURL(icon).url()).arg(weatherText) +
    "</table><table>" +
    QString("<tr><th style=\"text-align: right\">" + i18n( "Temperature:" )
    + "</th><td>%1</td>"
    "<td width=\"50\">&nbsp;</td>"
    "<th style=\"text-align: right\">" + i18n( "Dew Point:" )
    + "</th><td>%2</td></tr>"
    "<tr><th style=\"text-align: right\">" + i18n( "Air Pressure:" )
    + "</th><td>%3</td>"
    "<td width=\"50\">&nbsp;</td>"
    "<th style=\"text-align: right\">" + i18n( "Rel. Humidity:" )
    + "</th><td>%4</td></tr>"
    "<tr><th style=\"text-align: right\">" + i18n( "Wind Speed:" )
    + "</th><td>%5</td>")
    .arg(temp).arg(dewPoint).arg(pressure).arg(relHumidity)
    .arg(wind) + "<td width=\"50\">&nbsp;</td>";

    if (!heatIndex.isEmpty())
        contents += QString("<th style=\"text-align: right\">"
        + i18n( "Heat Index:" ) + "</th><td>%1</td>").arg(heatIndex);
    else if (!windChill.isEmpty())
        contents += QString("<th style=\"text-align: right\">"
        + i18n( "Wind Chill:" ) + "</th><td>%1</td>").arg(windChill);
    else
        contents += "<td>&nbsp;</td><td>&nbsp;</td>";
    contents += "</tr>";

    contents += QString("<tr><th style=\"text-align: right\">"
    + i18n( "Sunrise:" ) + "</th><td>%1</td>" +
    "<td width=\"50\">&nbsp;</td><th style=\"text-align: right\">"
    + i18n( "Sunset:" ) + "</th><td>%2</td>")
    .arg(sunRiseTime).arg(sunSetTime);

    contents += "</tr></table></body></html>";

    m_reportView->begin();
    m_reportView->write( contents );
    m_reportView->end();
	
    QScrollView *view = m_reportView->view();
    kdDebug() << "Size " << view->size().height() << "," << view->size().width() << endl;
    kdDebug() << "Size " << view->visibleHeight() << "," << view->visibleWidth() << endl;

    m_reportView->view()->resize(view->size().width(), view->size().height());

}

#include "reportview.moc"

/* vim: et sw=4 ts=4
*/

