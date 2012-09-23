/***************************************************************************
                        kanswer.h  -  description
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

#ifndef KANSWER_H
#define KANSWER_H

// Qt includes

// KDE includes
#include <kdialog.h>

class QCanvas;
class QCanvasPixmapArray;
class QCanvasSprite;
class QCanvasView;
class QHBoxLayout;
class QLabel;

class KAnimation;

class KPushButton;

/**
 * A small dialog, which gives feedback, if the input was wrong or false.
 * It provides reading several sentences from file and selecting by chance.
 *
 * @author Matthias Messmer & Carsten Niehaus & Robert Gogolok
 */
class KAnswer : public KDialog
{
    Q_OBJECT
    public:
        /**
        * The constructor doesn't need any parameters but the parent,
        * because it will not be used in any other context.
        *
        * @param parent Parent widget for modal functionality
        */
        KAnswer( QWidget *parent );

        // public functions
        /** setting/changing feedback text and pic */
        void setAnswer( int modus );

    public slots:
        /** reimplemented: close the window after some time */
        void timerEvent( QTimerEvent * );
        void accept();
        
        int exec();

    private:
        // private functions
        /** Loads feadback sentences from file */
        void loadAnswers();
        /** Gets a answer by chance for well solved tasks. */
        QString getRightAnswer();
        /** Gets a answer by chance for not solved tasks. */
        QString getWrongAnswer();

        // GUI widgets
        /** Label for the answer text */
        QLabel *TextLabelAnswer;
        /** OK button */
        KPushButton *ButtonOK;

        /** List of feedback text for well solved tasks. */
        QStringList rightAnswerList;
        /** List of feedback text for not solved tasks. */
        QStringList wrongAnswerList;

        QHBoxLayout *mainLayout;

        KAnimation *right_animation;
        KAnimation *wrong_animation;
        
        int advPer;

        QCanvasView *canvas_view;
        QCanvasPixmapArray *pixs;
        QCanvas *canvas;

        void setupSprite();
};

#endif
