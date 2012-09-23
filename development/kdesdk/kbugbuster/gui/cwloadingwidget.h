/*
    cwloadingwidget.h  -  Widget to be shown while loading data

    begin       : sun march 18 17:12:24 CET 2001
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

#ifndef KBBMAINWINDOW_CWLOADINGWIDGET_H
#define KBBMAINWINDOW_CWLOADINGWIDGET_H

#include <qlabel.h>
#include <qframe.h>

class QPixmap;
class KPixmap;

namespace KBugBusterMainWindow
{

/**
 * @author Martijn Klingens
 */
class CWLoadingWidget : public QFrame
{
    Q_OBJECT

public:
    /**
     * Use WidgetMode to specify the layout for the background images
     * TopFrame loads and uses the logo and horizontal bars,
     * BottomFrame loads the tools and the translucent block.
     */
    enum WidgetMode { TopFrame = 0, BottomFrame };
    
    CWLoadingWidget( WidgetMode mode = TopFrame, QWidget* parent = 0,
                     const char* name = 0 );
    CWLoadingWidget( const QString &text, WidgetMode mode = TopFrame,
                     QWidget* parent = 0, const char* name = 0 );
    ~CWLoadingWidget();

    QString text() const { return m_text; }
    void setText( const QString &text );
    
protected:
    virtual void mouseReleaseEvent( QMouseEvent * );
    virtual void drawContents( QPainter *p );
    virtual void resizeEvent( QResizeEvent * );

signals:
    void clicked();
    
private:
    void init( WidgetMode mode );
    void updatePixmap();

    QString m_text;
    WidgetMode m_mode;

    // Pixmaps used
    QPixmap *m_toolsPixmap;
    QPixmap *m_logoPixmap;
    QPixmap *m_topRightPixmap;
    QPixmap *m_barsPixmap;

    // For performance reasons we apply the KPixmapEffect only once
    KPixmap *m_toolsPixmapEffect;
    
    QPixmap *m_buffer;
    
};

}   // namespace

#endif // KBBMAINWINDOW_CWLOADINGWIDGET_H

/* vim: set et ts=4 softtabstop=4 sw=4: */

