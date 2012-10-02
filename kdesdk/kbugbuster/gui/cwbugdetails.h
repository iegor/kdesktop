/*
    cwbugdetails.h  -  Details of a bug report
    
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

#ifndef KBBMAINWINDOW_CWBUGDETAILS_H
#define KBBMAINWINDOW_CWBUGDETAILS_H

#include "bug.h"
#include "bugdetails.h"

#include <qwidget.h>

#include <kparts/browserextension.h>

class KHTMLPart;

namespace KBugBusterMainWindow
{

/**
 * @author Martijn Klingens
 */
class CWBugDetails : public QWidget
{
    Q_OBJECT

  public:
    CWBugDetails( QWidget* parent = 0, const char* name = 0 );
    ~CWBugDetails();

    void setBug( const Bug &, const BugDetails & );

    QString source() const;
    QString selectedText() const;

  private slots:
    void handleOpenURLRequest( const KURL &url, const KParts::URLArgs & );

  private:
    QString textBugDetailsAttribute( const QString &value,
                                     const QString &name );

    KHTMLPart *m_bugDesc;

    QString mSource;
};
 
}   // namespace

#endif

/* vim: set et ts=4 softtabstop=4 sw=4: */

