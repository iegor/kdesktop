/***************************************************************************
                          kweather.h  -  description
                             -------------------
    begin                : Wed Jul  5 23:09:02 CDT 2000
    copyright            : (C) 2000 by Ian Reinhart Geiser
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

#ifndef KWEATHER_H
#define KWEATHER_H

#include <kpanelapplet.h>
#include <dcopref.h>

#include "weatherIface.h"

class QTimer;
class dockwidget;
class reportView;
class KPopupMenu;
class WeatherService_stub;
class QPalette;

class kweather : public KPanelApplet, public weatherIface
{
    Q_OBJECT

public:
    kweather(const QString& configFile, Type t = Normal, int actions = 0, QWidget *parent = 0, const char *name = 0);
    ~kweather();
    void help();
    void resizeEvent(QResizeEvent*);
    int heightForWidth(int i) const;
    int widthForHeight(int i) const;
    void refresh(QString);
    void setBackground();
    void setLabelColor();

public slots: // Public slots
    void preferences();
    void about();

protected:
    void paletteChange(const QPalette &);

protected slots:
    void doReport();
    void timeout();
    void slotPrefsAccepted();
    void slotReportFinished();
    void slotUpdateNow();

private: // Private methods
    void initContextMenu();
    void initDCOP();
    void loadPrefs();
    void savePrefs();
    void showWeather();
    void writeLogEntry();
    void mousePressEvent(QMouseEvent *e);
    
    bool attach();
    
    QString reportLocation;
    QString fileName;
    QString metarData;
    bool logOn;
    bool mFirstRun;
    int mViewMode;
    QTimer *timeOut;
    dockwidget *dockWidget;
    reportView *mReport;
    DCOPClient *mClient;
    //DCOPRef *mWeatherService;
    KPopupMenu *mContextMenu;
    WeatherService_stub *mWeatherService;
    KCMultiDialog *settingsDialog;
    QColor mTextColor;
};

#endif
