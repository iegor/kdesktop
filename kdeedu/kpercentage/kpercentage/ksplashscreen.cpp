/***************************************************************************
                       splashscreen.h  -  description
                            -------------------
   begin                : Fri Nov 30 2001
   copyright            : (C) 2001 by Matthias Messmer &
                                      Carsten Niehaus &
                                      Robert Gogolok
   email                : bmlmessmer@web.de &
                          cniehaus@gmx.de &
                          mail@robert-gogolok.de
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Qt includes
#include <qcanvas.h>
#include <qtimer.h>

// KDE includes
#include <kstandarddirs.h>

// local includes
#include "kpercentage.h"
#include "ksplashscreen.h"

/* creates the splash window without titlebar and border */
KSplashScreen::KSplashScreen( KPercentage *percentage, const char *name ) :
        KDialog( 0L, name, FALSE, QWidget::WStyle_Customize | QWidget::WStyle_NoBorder )
{
    percentage_window = percentage;
    resize( 380, 300 );
    /* load the background pixmap */
    QPixmap bgp( locate( "data", "kpercentage/pics/splash.png" ) );
    /* wait some time and start the main window */
    QCanvas *canvas = new QCanvas( this );
    canvas->resize( 400, 300 );
    /* set the background pixmap */
    canvas->setBackgroundPixmap( bgp );
    //remove version number from splash annma 14/02/2005
    //QCanvasText *t = new QCanvasText( KPERCENTAGE_VERSION, canvas );
    //t->move( 330.0, 260.0 );
    //t->show();
    canvas->setAllChanged();
    canvas->update();
    QCanvasView *view = new QCanvasView( canvas, this, "", QCanvasView::WStyle_Customize | QCanvasView::WStyle_NoBorder );
    view->resize( 380, 300 );
    view->setVScrollBarMode( QCanvasView::AlwaysOff );
    view->setHScrollBarMode( QCanvasView::AlwaysOff );
    view->setFrameStyle( QCanvasView::NoFrame );
    view->show();
    QTimer::singleShot( 2000, this, SLOT( showPercentage() ) );
}

/* show the main window and close the splah window */
void KSplashScreen::showPercentage()
{
    percentage_window->show();
    delete( this );
}

#include "ksplashscreen.moc"
