/***************************************************************************
                          keducaprefs.h  -  description
                             -------------------
    begin                : Sat Jun 2 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEDUCAPREFS_H
#define KEDUCAPREFS_H

#include <kdialogbase.h>

#include <qradiobutton.h>
#include <qcheckbox.h>

/** Preferences for KEduca TestMaster (class KEduca)
 *@author Javier Campos
 */

class KEducaPrefs : public KDialogBase  {
    Q_OBJECT

public:
    KEducaPrefs(QWidget *parent=0, const char *name=0, bool modal=true);
    ~KEducaPrefs();

private:

    // Private methods

    /** Set page general */
    void setPageGeneral();
    /** Write settings */
    void configWrite();
    /** Read settings */
    void configRead();

    // Private attributes

    /** Show result after next */
    QRadioButton *_resultAfterNext;
    /** Show results after finish */
    QRadioButton *_resultAfterFinish;
    QCheckBox *_randomQuestions;
    QCheckBox *_randomAnswers;

private slots:
    /** Push ok button */
    void slotOk();
};

#endif
