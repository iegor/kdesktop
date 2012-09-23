/***************************************************************************
                         kpercentmain.h  -  description
                            -------------------
   begin                : Fri Nov 16 2001
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

#ifndef KPERCENTMAIN_H
#define KPERCENTMAIN_H

// Qt includes

// KDE includes

// local includes
#include "kanswer.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QVBoxLayout;

/**
 * Dialog for the exercises
 * @author Matthias Messmer & Rober Gogolok & Carsten Niehaus
 */
class KPercentMain : public KDialog
{
    Q_OBJECT
    public:
        KPercentMain( QWidget *parent, const char *name = 0 );
        /** checks, if the task is solved right */
        bool validateTask();
        /** prepares the dialog for a new task */
        void newTask();
        /** gets the numbers from the editlines */
        int getPercentvalue();
        int getBasevalue();
        int getPercentage();
        /** inserts the numbers of a new task in the editlines */
        void showNumbers();
        /** called, if number tasks are managed */
        void finished();
        /** shows the kanswer dialog modal with an proper answer about the success. */
        void showAnswer( int modus );
        /** sets the total amount of tasks selekted in opening screen */
        void setNumber( int anumber );
        /** Prepares the exercise for number tasks
        * espacially fills the arrays with proper numbers */
        void initExercise();

        // Public attributes
        /** total number of exercises */
        int number;
        /** actual number of managed exercises */
        int count;

        /** actual amount of given answers */
        int total_answers;
        /** actual count of right answers */
        int right_answers;
        /** actual count of wrong answers */
        int wrong_answers;

        /** in KPercentage selected type of task */
        int selected_type;
        /** percentages to be calculated with */
        int percentage[ 10 ];
        /** basevalues to be calculated with */
        int basevalue[ 10 ];
        /** percentvalues to be calculated with */
        int percentvalue[ 10 ];

        QLabel *label_1;
        QLabel *label_2;
        QLabel *label_number;
        QLabel *label_count;

        /** editline for the actual task */
        QLineEdit *line_edit_input;
        /** editline for the percentage */
        QLineEdit *line_edit_percentage;
        /** editline for the basevalue */
        QLineEdit *line_edit_basevalue;
        /** editline for the percentvalue */
        QLineEdit *line_edit_percentvalue;

        /** showing the effort of the user */
        QProgressBar *progress_bar_count;
        /** button to apply the current input */
        QPushButton *push_button_apply;
        /** button to cancel the exercise series */
        QPushButton *push_button_cancel;
        
        QProgressBar *progress_bar_right_wrong;
        QLabel *label_right;
        QLabel *label_wrong;

        /** the answer window */
        KAnswer *answer;
        /** stores the level selected in KPercentage's combo_box_level */
        int selected_level;

	private:
		void keyPressEvent( QKeyEvent * e );


    public slots:  // Public slots
        /**
         * checks, if the input is a val. number
         * calls validateTask then and if succeeds causes a newTask
         *
         * @see kpercentmain#validateTask kpercetmain#newTask
         */
        void slotApplyInput();
        /** reimplemented: growing the progressbar step by step */
        void timerEvent( QTimerEvent * );
};

#endif
