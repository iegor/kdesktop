/*
    cwsearchwidget.h  -  Search Pane
    
    copyright   : (c) 2001 by Martijn Klingens
    email       : klingens@kde.org

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef KBBMAINWINDOW_CWSEARCHWIDGET_H
#define KBBMAINWINDOW_CWSEARCHWIDGET_H
 
#include "cwsearchwidget_base.h"

namespace KBugBusterMainWindow
{

/**
 * @author Martijn Klingens
 */
class CWSearchWidget : public CWSearchWidget_Base
{
    Q_OBJECT

public:
    CWSearchWidget( QWidget* parent = 0, const char* name = 0 );
    ~CWSearchWidget();

public slots:
    void textNumberChanged ( const QString & );
    void textDescriptionChanged ( const QString & );
};
 
}   // namespace

#endif

/* vim: set et ts=4 softtabstop=4 sw=4: */

