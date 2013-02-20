/** 
 * Copyright (C) 2001-2002 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#ifndef KGVPAGEVIEW_H
#define KGVPAGEVIEW_H

#include <qscrollview.h>

/**
 * KGVPageView is a customized QScrollView, which can hold one page. This page
 * will be centered on the viewport. Furthermore it adds the ability to scroll
 * the page by dragging it using the mouse.
 */
class KGVPageView : public QScrollView
{
    Q_OBJECT

public:
    KGVPageView( QWidget* parent = 0, const char* name = 0 );
    ~KGVPageView() { ; }
    
    void setPage( QWidget* );
    QWidget* page() const { return _page; }
   
    /**
     * Return true if the top resp. bottom of the page is visible.
     */
    bool atTop()    const;
    bool atBottom() const;
    
    /**
     * Turn the scrollbars on/off.
     */
    void enableScrollBars( bool);
    
    /**
     * @reimplemented
     */
    bool eventFilter( QObject*, QEvent* );
    
public slots:
    bool readUp();
    bool readDown();
    void scrollUp();
    void scrollDown();
    void scrollRight();
    void scrollLeft();
    void scrollBottom();
    void scrollTop();
  
signals:
    void viewSizeChanged( const QSize& );
    void pageSizeChanged( const QSize& );
    void nextPage();
    void zoomOut();
    void zoomIn();
    void prevPage();
    void rightClick();
    void ReadUp();
    void ReadDown();

protected:
    virtual void keyPressEvent( QKeyEvent* );

    /**
     * Reimplemented to from QScrollView to make sure that the page is centered
     * when it fits in the viewport.
     */
    virtual void viewportResizeEvent( QResizeEvent* );

    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void wheelEvent( QWheelEvent * );

    /**
     * If the viewport is larger than the page, center the page on the
     * viewport.
     */
    void centerContents();

private:
    QPoint   _dragGrabPos;
    QWidget* _page;
};

#endif

// vim:sw=4:sts=4:ts=8:noet
