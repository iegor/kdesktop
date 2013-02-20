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

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

// KDE includes

class QWidget;
class KPercentage;

/**
 * Spashscreen
 *
 * @author Matthias Messmer & Robert Gogolok
 */
class KSplashScreen : public KDialog
{
    Q_OBJECT

    public:
        /** constructor */
        KSplashScreen( KPercentage *percentage, const char *name );

    public slots:   // public slots
        /* called after some time to hide the splash and show the main window */
        void showPercentage();

    private:
        /** The KPercentage central window */
        KPercentage *percentage_window;
};

#endif
