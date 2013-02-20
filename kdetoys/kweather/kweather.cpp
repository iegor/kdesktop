/***************************************************************************
                          kweather.cpp  -  description
                             -------------------
    begin                : Wed Jul  5 23:09:02 CDT 2000
    copyright            : (C) 2000-2003 by Ian Reinhart Geiser
                         : (C) 2002-2003 Nadeem Hasan <nhasan@kde.org>
    email                : geiseri@msoe.edu
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kaboutapplication.h>
#include <kapplication.h>
#include <kcmultidialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kconfig.h>
#include <ksettings/dialog.h>
#include <ksettings/dispatcher.h>
#include <dcopclient.h>

#include <qfile.h>
#include <qtimer.h>
#include <qpalette.h>

#include "kweather.h"
#include "reportview.h"
#include "dockwidget.h"
#include "weatherservice_stub.h"

extern "C"
{
    KDE_EXPORT KPanelApplet* init(QWidget *parent, const QString& configFile)
    {
        KGlobal::locale()->insertCatalogue("kweather");
        kweather *theApplet = new kweather(configFile, KPanelApplet::Normal,
                0, parent, "kweather");
        return theApplet;
    }
}

kweather::kweather(const QString& configFile, Type t, int actions,
        QWidget *parent, const char *name):
        KPanelApplet(configFile, t, actions, parent, name), weatherIface(),
        mFirstRun( false ), mReport( 0 ), mClient( 0 ),
        mContextMenu( 0 ), mWeatherService( 0 ), settingsDialog( 0 ), mTextColor(Qt::black)
{
    kdDebug(12004) << "Constructor " << endl;
    setObjId("weatherIface");
    
    setBackgroundOrigin( QWidget::AncestorOrigin );
    loadPrefs();
    initContextMenu();
    initDCOP();
    
    dockWidget = new dockwidget(reportLocation, this, "dockwidget");
    connect(dockWidget, SIGNAL(buttonClicked()), SLOT(doReport()));
    dockWidget->setViewMode(mViewMode);
    setLabelColor();
    
    timeOut = new QTimer(this, "timeOut" );
    connect(timeOut, SIGNAL(timeout()), SLOT(timeout()));
    timeOut->start(10*60*1000);
    
    if(mFirstRun)
        preferences();
    else
        timeout();
}

kweather::~kweather()
{
    delete mWeatherService;
}

void kweather::initContextMenu()
{
    mContextMenu = new KPopupMenu(this);
    mContextMenu->insertTitle(i18n("KWeather - %1").arg( reportLocation ), -1, 0);
    mContextMenu->insertItem(SmallIcon("viewmag"), i18n("Show &Report"),
        this, SLOT(doReport()), 0, -1, 1);
    mContextMenu->insertItem(SmallIcon("reload"), i18n("&Update Now"),
        this, SLOT(slotUpdateNow()), 0, -1, 2);
    mContextMenu->insertSeparator();
    mContextMenu->insertItem(SmallIcon("kweather"), i18n("&About KWeather"), this, SLOT(about()));
    mContextMenu->insertItem(SmallIcon("configure"),
        i18n("&Configure KWeather..."), this, SLOT(preferences()));
    setCustomMenu(mContextMenu);
}

void kweather::initDCOP()
{
    if ( !mClient )
        mClient = KApplication::dcopClient();
    
    if(!mClient->isAttached())
        mClient->attach();
    
    if(!attach())
        return;
    
    kdDebug(12004) << "attached to the server..." << endl;
    
    if ( mWeatherService )
        delete mWeatherService;
    
    mWeatherService = new WeatherService_stub( "KWeatherService", "WeatherService" );
    
    if (!connectDCOPSignal(0, 0, "fileUpdate(QString)",
        "refresh(QString)",false))
        kdDebug(12004) << "Could not attach dcop signal..." << endl;
    else
        kdDebug(12004) << "attached dcop signals..." << endl;
}

/** about box */
void kweather::about(){
    kdDebug(12004) << "Telling about" << endl;
    KAboutData aboutData("KWeather", "KWeather", "2.1.0",
            I18N_NOOP("Weather applet for the Kicker"), KAboutData::License_GPL_V2);
    aboutData.addAuthor("Ian Reinhart Geiser", "", "geiseri@kde.org",
            "http://www.kde.org/");
    //aboutData.addAuthor("Nadeem Hasan",  "", "nhasan@nadmm.com", "http://www.nadmm.com/");
    aboutData.addCredit("Nadeem Hasan", I18N_NOOP("Lots of bugfixes,"
            " improvements and cleanups."), "nhasan@nadmm.com");
    aboutData.addCredit("Will Andrews", I18N_NOOP("Fixed for BSD port"),
            "wca@users.sourceforge.net", "");
    aboutData.addCredit("Ben Burton", I18N_NOOP("Debian fixes"), "benb@acm.org");
    aboutData.addCredit("Otto Bruggeman", I18N_NOOP("Fixed the i18n stuff and"
            " made sure the indentation was consistent :P"), "bruggie@home.nl");
    aboutData.addCredit( "Carles Carbonell Bernado",
            I18N_NOOP( "Great new weather icons" ), "mail@carlitus.net" );
    aboutData.addCredit( "John Ratke",
            I18N_NOOP( "Improvements and more code cleanups" ), "jratke@comcast.net" );

    KAboutApplication about(&aboutData, this, 0);
    about.setProgramLogo( KGlobal::instance()->iconLoader()->iconPath( "kweather", -KIcon::SizeLarge ) );
    about.exec();
}

/** prefs */
void kweather::preferences()
{
    kdDebug(12004) << "prefs" << endl;

    savePrefs();

    if ( settingsDialog == 0 )
    {
      settingsDialog = new KCMultiDialog( this );
      connect( settingsDialog, SIGNAL( configCommitted() ), SLOT( slotPrefsAccepted() ) );

      settingsDialog->addModule( "kcmweather.desktop" );
      settingsDialog->addModule( "kcmweatherservice.desktop" );
    }

    settingsDialog->show();
    settingsDialog->raise();
}

/** The help handler */
void kweather::help()
{
    kapp->invokeHelp(QString::null, QString::fromLatin1("kweather"));
}

/** Display the current weather report. */
void kweather::doReport()
{
    if ( reportLocation.isEmpty() )
    {
        // no station to display defined -> open settings dialog
        preferences();
        return;
    }

    kdDebug(12004) << "Showing out the report" << endl;
    if ( mReport == 0 )
    {
        mReport = new reportView(reportLocation);
    
        connect( mReport, SIGNAL( finished() ), SLOT( slotReportFinished() ) );
    }
    
    mReport->show();
    mReport->raise();
}

void kweather::slotReportFinished(){
    mReport->delayedDestruct();
    mReport = 0;
}

/** load the application */
void kweather::loadPrefs(){
    kdDebug(12004) << "Loading Prefs" << endl;
    KConfig *kcConfig = config();
    kcConfig->reparseConfiguration();

    if (!kcConfig->hasGroup ("General Options") )
        mFirstRun = true;

    kcConfig->setGroup("General Options");
    logOn = kcConfig->readBoolEntry("logging", false);
    fileName = kcConfig->readPathEntry("log_file_name");
    reportLocation = kcConfig->readEntry("report_location");
    mViewMode = kcConfig->readNumEntry("smallview_mode", dockwidget::ShowAll);

    static QColor black(Qt::black);
    mTextColor = kcConfig->readColorEntry("textColor", &black);
}

/** Save the application mPrefs. */
void kweather::savePrefs(){
    kdDebug(12004) << "Saving Prefs..." << endl;
    KConfig *kcConfig = config();
    
    kcConfig->setGroup("General Options");
    kcConfig->writeEntry("logging", logOn);
    kcConfig->writeEntry("report_location", reportLocation);
    kcConfig->writeEntry("smallview_mode", mViewMode);
    kcConfig->writePathEntry("log_file_name", fileName );
    kcConfig->sync();
}

void kweather::showWeather()
{
    kdDebug(12004) << "Show weather" << endl;
    dockWidget->showWeather();
    emit updateLayout();
}

void kweather::writeLogEntry()
{
    // Write data line in the CSV format
    if (logOn && !fileName.isEmpty())
    {
        kdDebug(12004)<< "Try log file:" << fileName << endl;
        QFile logFile(fileName);
        QTextStream logFileStream(&logFile);
        if (logFile.open(IO_Append | IO_ReadWrite))
        {
            QString temperature = mWeatherService->temperature(reportLocation );
            QString wind        = mWeatherService->wind(reportLocation );
            QString pressure    = mWeatherService->pressure(reportLocation );
            QString date        = mWeatherService->date(reportLocation );
            QStringList weather = mWeatherService->weather(reportLocation );
            QStringList cover   = mWeatherService->cover(reportLocation );
            QString visibility  = mWeatherService->visibility(reportLocation );
            logFileStream << date << ",";
            logFileStream << wind << ",";
            logFileStream << temperature << ",";
            logFileStream << pressure << ",";
            logFileStream << cover.join(";") << ",";
            logFileStream << visibility << ",";
            logFileStream << weather.join(";");
            logFileStream << endl;
        }
        else
        {
            KMessageBox::sorry( this,
                    i18n("For some reason the log file could not be written to.\n"
                    "Please check to see if your disk is full or if you "
                    "have write access to the location you are trying to "
                    "write to."),
                    i18n("KWeather Error"));
        }
        logFile.close();
    }
}

/** get new data */
void kweather::timeout()
{
    if ( !mWeatherService )
        initDCOP();
    
    if ( mWeatherService )
    {
        // isEmtpy is true for null and 0 length strings
        if ( !reportLocation.isEmpty() )
        {
        kdDebug(12004)<< "Requesting new data for " << reportLocation << endl;
        mWeatherService->update(reportLocation);
        }
    }
}

int kweather::widthForHeight(int h) const
{
    //kdDebug(12004) << "widthForHeight " << h << endl;
    dockWidget->setOrientation(Horizontal);
    int w = dockWidget->widthForHeight(h);
    return w;
}

int kweather::heightForWidth(int w) const
{
    kdDebug(12004) << "heightForWidth " << w<< endl;
    dockWidget->setOrientation(Vertical);
    int h = dockWidget->heightForWidth( w );
    return h;
}

void kweather::refresh(QString stationID)
{
    kdDebug(12004) << "refresh " << stationID << endl;
    if( stationID == reportLocation)
    {
        showWeather();
        writeLogEntry();
    }
}

void kweather::slotPrefsAccepted()
{
    // Preferences have been saved in the config file by the KCModule,
    // so read them out.
    loadPrefs();

    dockWidget->setLocationCode(reportLocation);
    dockWidget->setViewMode(mViewMode);
    setLabelColor();
    emit updateLayout();

    if (logOn && !fileName.isEmpty())
    {
        QFile logFile(fileName);
        // Open the file, create it if not already exists
        if (logFile.open(IO_ReadWrite))
        {
            if (logFile.size() == 0)
            {
                // Empty file, put the header
                QTextStream logFileStream(&logFile);
                logFileStream << "Date,Wind Speed & Direction,Temperature,Pressure,Cover,Visibility,Current Weather" << endl;
            }
            logFile.close();
        }
        else
        {
            kdDebug(12004) << "There was an error opening the file...." << endl;
            KMessageBox::sorry( this,
                    i18n("For some reason a new log file could not be opened.\n"
                    "Please check to see if your disk is full or if you have "
                    "write access to the location you are trying to write to."),
                    i18n("KWeather Error"));
        }
    }

    timeout();
}

void kweather::mousePressEvent(QMouseEvent *e)
{
    if ( e->button() != RightButton )
    {
        KPanelApplet::mousePressEvent( e );
        return;
    }

    mContextMenu->exec(e->globalPos());
}

void kweather::slotUpdateNow()
{
    if ( !mWeatherService )
         initDCOP();

    mWeatherService->forceUpdate(reportLocation );
}

bool kweather::attach()
{
    QString error;
    kdDebug(12004) << "Looking for dcop service..." << endl;
    if (!mClient->isApplicationRegistered("KWeatherService"))
    {
        kdDebug(12004) << "Could not find service so I am starting it..."
            << endl;
        if (!KApplication::startServiceByDesktopName("kweatherservice",
            QStringList(), &error))
        {
            kdDebug(12004) << "Starting KWeatherService failed with message: "
                << error << endl;
            return false;
        }

        kdDebug (12004) << "Service Started..." << endl;
    }
    else
    {
        kdDebug(12004) << "Found weather service..." << endl;
    }

    return true;
}

void kweather::resizeEvent(QResizeEvent *e)
{
    kdDebug(12004) << "KWeather Resize event " << e->size() << endl;
    dockWidget->resizeView(e->size());
}

void kweather::setLabelColor()
{
    setPaletteForegroundColor(mTextColor);
}

void kweather::paletteChange(const QPalette &)
{
    setLabelColor();
}

#include "kweather.moc"
