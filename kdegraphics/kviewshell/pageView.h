// -*- C++ -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Wilco Greven <greven@kde.org>
   Copyright (C) 2004-2005 Wilfried Huss <Wilfried.Huss@gmx.at>
   Copyright (C) 2005 Stefan Kebekus <kebekus@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef PAGEVIEW_H
#define PAGEVIEW_H

#include "documentWidget.h"

#include <qptrvector.h>
#include <qscrollview.h>

class PageNumber;

/*
 * PageView is a customized QScrollView, which can hold one
 * page. This page will be centered on the viewport.
 */
class PageView : public QScrollView
{
    Q_OBJECT

public:
    PageView( QWidget* parent = 0, const char* name = 0 );
    ~PageView() {}

    void addChild( QPtrVector<DocumentWidget> *wdgList );

    /** Sets the number of columns into which the widgets are
        aligned. If nothing is set, '2' is the default. */
    void setNrColumns( Q_UINT8 cols );
    void setNrRows( Q_UINT8 rows );

    void setContinuousViewMode(bool continuous);
    bool fullScreenMode() { return fullScreen; }
    bool singlePageFullScreenMode();
    bool overviewMode() { return !continuousViewmode && (nrRows > 1 || nrCols > 1); }

    /** Returns the number of columns into which the widgets are aligned.

    This method returns the number of colums actually used to display
    the widgets. 

    @warning This method need not return the number columns set in the
    setViewMode() method. For instance, if the viewmode
    KVSPrefs::EnumViewMode::ContinuousFacing is set, but there is only
    one widget, then only one column is used, and the method returns
    the number one.

    If there aren't any widgets, the number 1 is returned.

    @returns Number of columns used, or 1 if there aren't any
    widgets. The number i returned always satisfies 1 <= i <= nrCols,
    where nrCols is the private variable of the same nane.
    */
    Q_UINT8 getNrColumns() const { return  (widgetList==0) ? 1 : QMIN(nrCols, QMAX(1, widgetList->size())); }

    Q_UINT8 getNrRows() const { return nrRows; }
    bool isContinuous() const { return continuousViewmode; }

    /** Return true if the top resp. bottom of the page is visible. */
    bool atTop()    const;
    bool atBottom() const;

    /** Distance between pages in pixels (this is independent of
        the zoom level). */
    int distanceBetweenPages() { return distanceBetweenWidgets; }

    /** Moves the viewport so that the widget is at the top left corner. */
    void moveViewportToWidget(QWidget* widget, int y = 0);

    bool isMoveToolEnabled() const { return moveTool; }

public slots:
    void calculateCurrentPageNumber();

    bool readUp();
    bool readDown();
    void scrollUp();
    void scrollDown();
    void scrollRight();
    void scrollLeft();
    void scrollBottom();
    void scrollTop();

    void setFullScreenMode(bool fullScreen);
    /** Turn the scrollbars on/off. */
    void slotShowScrollbars(bool);

    /** Set layout of the page widgets according to the current viewmode and zoomlevel.
        Set zoomChanged = true if the the layout needs updateing because the zoomlevel has changed. */
    void layoutPages(bool zoomChanged = false);

    void slotEnableMoveTool(bool enable);

signals:
    void viewSizeChanged(const QSize& size);
    void pageSizeChanged(const QSize& size);

    void currentPageChanged(const PageNumber&);

    /** This signal is emitted when the scrollView receives a mouse
        wheel event. */
    void wheelEventReceived( QWheelEvent * );

protected:
    void keyPressEvent( QKeyEvent* );

    /** Reimplemented from QScrollView to make sure that the page is
        centered when it fits in the viewport. */
    void viewportResizeEvent( QResizeEvent* );
    void viewportPaintEvent(QPaintEvent*);

    /** Reimplemented from QScrollView, because the kviepart needs to
        handle the wheel events itself. The wheel event is passed on by 
        emitting the singal "wheelEventReceived". Nothing else is done. */
    void contentsWheelEvent ( QWheelEvent * );

    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseReleaseEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);

private slots:
    void calculateCurrentPageNumber(int x, int y);

private:
    /** Stores the mouse position between two mouse events. This is used
        to implement the "grab and drag the viewport contents" feature. */
    QPoint   dragGrabPos;

    QPtrVector<DocumentWidget>* widgetList;

    /** Used internally by the centerContents()-method. Set with the
        setNrColumns() method */
    Q_UINT8  nrCols;
    Q_UINT8  nrRows;

    bool continuousViewmode;
    bool fullScreen;

    /** This int remembers the style of the frame of the centering
        scrollview when fullscreen mode is switched on. It is then
        restored when it is switched off. */
    int oldFrameStyle;

    /** color of the viewport's background. This is also
        stored on entering the fullscreen mode. */
    QColor backgroundColor;

    /** Distance between pages in pixels 
        (this is independent of the zoom level). */
    static const int distanceBetweenWidgets=10;

    bool moveTool;
};

#endif
