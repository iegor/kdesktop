/***************************************************************************
                          kpercentage.h  -  description
                             -------------------
    begin                : Fre Nov 16 14:52:33 CET 2001
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

#ifndef KPERCENTAGE_H
#define KPERCENTAGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Qt includes
#include <qspinbox.h>

// KDE includes
#include <kapplication.h>
#include <kcombobox.h>

// local includes
#include "kpercentmain.h"

#define SEL_PERCENTAGE 0
#define SEL_BASEVALUE 1
#define SEL_PERCENTVALUE 2
#define SEL_RANDOM 3

#define KPERCENTAGE_VERSION "1.2"

/**
 * KPercentage is the base class of the project.
 */
class KPercentage : public KDialog
{
    Q_OBJECT
public:
    /** construtor */
    KPercentage( const char *name=0 );

    /** The exercise dialog */
    KPercentMain * percent_main;

public slots: // Public slots
    /** connected to PushButtonRandom */
    void selRandom();
    /** connected to PushButtonPercentvalue */
    void selPercentvalue();
    /** connected to PushButtonBasevalue */
    void selBasevalue();
    /** connected to PushButtonPercentage */
    void selPercentage();
    /** invokes the HelpCenter. doc must be installed */
    void needHelp();

protected:
    /** closes the application */
    void closeEvent(QCloseEvent *);

private:
    /** Choose the level: easy, medium, crazy */
    KComboBox *combo_box_level;
    /** Choose the number of exercises. */
    QSpinBox *spin_box_number;

    /** common preparing the exercise */
    void startExercise();
};

#endif
