/***************************************************************************
                         kpercentmain.cpp  -  description
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

// C/C++ includes
#include <stdlib.h>
#include <time.h>

// Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtooltip.h>

// KDE includes
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kstdguiitem.h>
#include <knumvalidator.h>

// local includes
#include "kpercentmain.h"
#include "kpercentage.h"


KPercentMain::KPercentMain( QWidget *parent, const char *name ) :
        KDialog( parent, name, TRUE )  // TRUE for modal dialog
{
    setFont( parent->font() );
    /** load and set the background pixmap */
    QPixmap bgp( locate( "data", QApplication::reverseLayout() ? "kpercentage/pics/kpercentmain_bg_rtl.png" : "kpercentage/pics/kpercentmain_bg.png" ) );
//    resize( 520, 220 );
    setFixedSize( QSize( 520, 275 ) );
    setBackgroundPixmap( bgp );
    setBackgroundOrigin( QDialog::ParentOrigin );
    KIntValidator *validLine = new KIntValidator( this );

    line_edit_percentage = new QLineEdit( this, "line_edit_percentage" );
    line_edit_percentage->setAlignment( int( QLineEdit::AlignHCenter ) );
    line_edit_percentage->setValidator( validLine );

    line_edit_basevalue = new QLineEdit( this, "line_edit_basevalue" );
    line_edit_basevalue->setAlignment( int( QLineEdit::AlignHCenter ) );
    line_edit_basevalue->setValidator( validLine );

    line_edit_percentvalue = new QLineEdit( this, "line_edit_percentvalue" );
    line_edit_percentvalue->setAlignment( int( QLineEdit::AlignHCenter ) );
    line_edit_percentvalue->setValidator( validLine );

    progress_bar_count = new QProgressBar( this, "progress_bar_count" );
    progress_bar_count->setFrameShape( QProgressBar::Panel );
    progress_bar_count->setFrameShadow( QProgressBar::Sunken );
//    progress_bar_count->setFixedWidth( 250 );

    progress_bar_right_wrong = new QProgressBar( this, "progress_bar_right" );
    progress_bar_right_wrong->setFrameShape( QProgressBar::Panel );
    progress_bar_right_wrong->setFrameShadow( QProgressBar::Sunken );
//    progress_bar_right_wrong->setFixedWidth( 250 );
    progress_bar_right_wrong->setPercentageVisible( FALSE );
    progress_bar_right_wrong->setPaletteBackgroundColor( QColor( "red" ) );

    label_right = new QLabel( this, "" );
    label_right->setBackgroundPixmap( bgp );
    label_right->setBackgroundOrigin( QDialog::ParentOrigin );
    label_wrong = new QLabel( this, "" );
    label_wrong->setBackgroundPixmap( bgp );
    label_wrong->setBackgroundOrigin( QDialog::ParentOrigin );

    push_button_apply = new QPushButton( this, "push_button_apply" );
    push_button_apply->setText( KStdGuiItem::apply().text() );
    push_button_apply->setDefault( TRUE );
    push_button_apply->setBackgroundPixmap( bgp);
    push_button_apply->setBackgroundOrigin( QPushButton::ParentOrigin );

    push_button_cancel = new QPushButton( this, "push_button_cancel" );
    push_button_cancel->setText( KStdGuiItem::cancel().text() );

    KIconLoader icon_loader;
    push_button_apply->setIconSet( QIconSet( icon_loader.loadIcon( "button_ok", KIcon::NoGroup, 32 ) ) );
    push_button_cancel->setIconSet( QIconSet( icon_loader.loadIcon( "button_cancel", KIcon::NoGroup, 32 ) ) );

    label_1 = new QLabel( this, "label_1" );
    label_1->setText( i18n( " % of " ) );
    label_1->setBackgroundPixmap( bgp );
    label_1->setBackgroundOrigin( QDialog::ParentOrigin );

    label_2 = new QLabel( this, "label_2" );
    label_2->setText( i18n( " = " ) );
    label_2->setBackgroundPixmap( bgp );
    label_2->setBackgroundOrigin( QDialog::ParentOrigin );

    label_number = new QLabel( this, "label_number" );
    label_number->setBackgroundPixmap( bgp );
    label_number->setBackgroundOrigin( QDialog::ParentOrigin );
    label_number->setText( i18n( "Task no. MM:" ) ); // for the right width

    label_count = new QLabel( this, "label_count" );
    label_count->setText( i18n( "You got MM of MM." ) ); // for the right width
    label_count->setBackgroundPixmap( bgp );
    label_count->setBackgroundOrigin( QDialog::ParentOrigin );

    //////
    // begin layouting
    //////
    QVBoxLayout *main_layout = new QVBoxLayout ( this, 0, -1, "main_layout" );
    main_layout->setMargin( 20 );
    main_layout->setSpacing( 10 );

    QHBoxLayout *label_number_layout = new QHBoxLayout ( main_layout, -1, "label_number_layout" );
    label_number_layout->addWidget( label_number );
    label_number_layout->addStretch();

    QHBoxLayout *line_edit_layout = new QHBoxLayout ( main_layout, -1, "line_edit_layout" );

    line_edit_layout->addWidget( line_edit_percentage );
    line_edit_layout->addWidget( label_1 );
    line_edit_layout->addWidget( line_edit_basevalue );
    line_edit_layout->addWidget( label_2 );
    line_edit_layout->addWidget( line_edit_percentvalue );
    line_edit_layout->addStretch();

    main_layout->addStretch();

    QHBoxLayout *bottom_layout = new QHBoxLayout( main_layout, -1, "bottom_layout" );

    QVBoxLayout *current_state_layout = new QVBoxLayout ( bottom_layout, -1, "current_state_layout" );

    QHBoxLayout *label_count_layout = new QHBoxLayout ( current_state_layout, -1, "label_count_layout" );
    label_count_layout->addWidget( label_count );
    label_count_layout->addStretch();

    current_state_layout->addSpacing( 10 );
    current_state_layout->addWidget( progress_bar_count );

    QHBoxLayout *right_wrong_layout = new QHBoxLayout( current_state_layout, -1, "right_wrong_layout" );

    right_wrong_layout->addWidget( label_right );
    right_wrong_layout->addWidget( progress_bar_right_wrong );
    right_wrong_layout->addWidget( label_wrong );

    bottom_layout->addSpacing( 40 );

    QVBoxLayout *button_layout = new QVBoxLayout ( bottom_layout, -1, "button_layout" );

    button_layout->addWidget( push_button_apply );
    button_layout->addSpacing( 10 );
    button_layout->addWidget( push_button_cancel );

    // END LAYOUT
    // --------------------------------------------------------

    // tab order
    setTabOrder( line_edit_percentvalue, push_button_apply );
    setTabOrder( push_button_apply, push_button_cancel );

	///////////
	// ToolTips

	QToolTip::add( progress_bar_count, i18n( "Number of managed exercises" ) );
	QToolTip::add( progress_bar_right_wrong, i18n( "Relation of right to wrong inputs" ) );
	QToolTip::add( push_button_apply, i18n( "Check your answer" ) );
	QToolTip::add( push_button_cancel, i18n( "Back to the main window" ) );

    // signals and slots connections
    connect( push_button_apply, SIGNAL( clicked() ), this, SLOT( slotApplyInput() ) );
    connect( push_button_cancel, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // build up the answer window
    answer = new KAnswer( this );


    /** this is to tell rand() to be randomizing */
    srandom( time( 0 ) );

}

/** No descriptions */
void KPercentMain::newTask()
{
    // no Editline can be focused
    line_edit_percentage->setFocusPolicy( NoFocus );
    line_edit_basevalue->setFocusPolicy( NoFocus );
    line_edit_percentvalue->setFocusPolicy( NoFocus );

    // set the number of managed tasks into a label
    label_count->setText( QString( i18n("You got %1 of %2 exercises.").arg(count).arg(number) ) ); // update Label for the Progress

//    progress_bar_count->setProgress( count*100 ); // updating Progrssbar
    startTimer( 10 );
    // set string which task is just workin on (Task x of y)
    QString taskNumber( i18n( "Exercise no. %1:" ).arg(count+1) );

    label_number->setText( taskNumber );
    //    label_number->adjustSize();

    int selection; // which task is to be solved ?
     if ( count == number )  // all tasks managed ?
     {
//         finished();
     }
    else
    {
        if ( selected_type == SEL_RANDOM )
        {
            selection = rand() % 3;  // select one task by random
        }
        else
        {
            selection = selected_type; // or take the preselected one
        }
        // set the right line_edit_layout for input
        switch ( selection )
        {
        case SEL_PERCENTAGE:
            line_edit_input = line_edit_percentage;
            break;
        case SEL_BASEVALUE:
            line_edit_input = line_edit_basevalue;
            break;
        case SEL_PERCENTVALUE:
            line_edit_input = line_edit_percentvalue;
            break;
        }
        // show the numbers
        showNumbers();
        // delete this one
        line_edit_input->clear();
        // make the line_edit_layout accessable
        line_edit_input->setFocusPolicy( StrongFocus );
        // set the focus on it
        line_edit_input->setFocus();
    }
}

/** checks, if the task is solved right */
bool KPercentMain::validateTask()
{
    // here is some mathematics :)
    return ( getPercentage() * getBasevalue() == 100 * getPercentvalue() );
}

/** gets the numbers from the line_edit_layouts */
int KPercentMain::getPercentage()
{
    return line_edit_percentage->text().toInt(); // check for valid input is omitted (slotApplyInput)
}
int KPercentMain::getBasevalue()
{
    return line_edit_basevalue->text().toInt(); // check for valid input is omitted (slotApplyInput)
}
int KPercentMain::getPercentvalue()
{
    return line_edit_percentvalue->text().toInt(); // check for valid input is omitted (slotApplyInput)
}

/* checks, if the input is a val. number
 * calls validateTask then and if succeeds causes a newTask */
void KPercentMain::slotApplyInput()
{
    bool ok; // conversion to int successful ?
    line_edit_input->text().toInt( &ok );
    if ( !ok )
    {
        showAnswer( 3 );
    }
    else
    {
        if ( validateTask() )  // are the numbers right ?
        {
            showAnswer( 1 );
            count++; // one more solved
            right_answers++; // one more right answer
            total_answers++;
            progress_bar_right_wrong->setTotalSteps( total_answers );
            progress_bar_right_wrong->setProgress( right_answers );
            label_right->setText( i18n( "%1%\nright" ).arg( (100*right_answers)/total_answers ) );
            label_wrong->setText( i18n( "%1%\nwrong" ).arg( 100-(100*right_answers)/total_answers ) );

            newTask(); // next task, perhaps
        }
        else
        {
            showAnswer( 2 );
            wrong_answers++; // one more right answer
            total_answers++;
            progress_bar_right_wrong->setTotalSteps( total_answers );
            progress_bar_right_wrong->setProgress( right_answers );
            label_right->setText( i18n( "%1%\nright" ).arg( (100*right_answers)/total_answers ) );
            label_wrong->setText( i18n( "%1%\nwrong" ).arg( 100-(100*right_answers)/total_answers ) );
        }
    }
}

/** inserts the numbers of a new task in the line_edit_layouts */
void KPercentMain::showNumbers()
{
    line_edit_percentage->setText( QString::number( percentage[ count ] ) );
    line_edit_basevalue->setText( QString::number( basevalue[ count ] ) );
    line_edit_percentvalue->setText( QString::number( percentvalue[ count ] ) );
}

/** called, if number tasks are managed */
void KPercentMain::finished()
{
    showAnswer( 4 );
    accept(); // hides the dialog
}
/** shows the kanswer dialog modal with an proper answer about the success. */
void KPercentMain::showAnswer( int modus )
{
    answer->setAnswer( modus );
    answer->exec();
}
/** sets the total amount of tasks selekted in opening screen */
void KPercentMain::setNumber( int anumber )
{
    number = anumber;
    progress_bar_count->setTotalSteps( number*100 );
}

/** Prepares the exercise of number tasks */
void KPercentMain::initExercise()
{
    count = 0;
    progress_bar_count->setProgress( 0 );
    total_answers = 0;
    right_answers = 0;
    wrong_answers = 0;

    progress_bar_right_wrong->setProgress( 0 );
    label_right->setText( "" );
    label_wrong->setText( "" );
    switch ( selected_level )
    {
        /** easy level */
    case 0:
        {
       	    /** easy percentages to calculate with */
            const int values[] = { 1, 10, 25, 50, 75, 100, 10, 25, 50, 75};
            ::memcpy(percentage, values, sizeof(values));
		}


		int num;
        if ( number < 6 )    /* for only few tasks, chose from the first 6 numbers */
        {
            num = 6;
        }
        else
        {
            num = 10;     /* else take them all! */
        }
        /** swap 20 times in the array */
        for ( int i = 0; i < 20; i++ )
        {
            int m = rand() % num;
            int n = rand() % num;
            if ( n == m )  /* not a swap */
            {
                n = ( n + 1 ) % num;
            }
            int h = percentage[ m ];
            percentage[ m ] = percentage[ n ];
            percentage[ n ] = h;
        }
        for ( int i = 0; i < number; i++ )
        {
            /** basevalue 100..2000 by chance */
            basevalue[ i ] = 100 * ( 1 + rand() % 20 );
            /** calc percentvalue */
            percentvalue[ i ] = percentage[ i ] * basevalue[ i ] / 100;
        }
        break;
    case 1:
        {
            /** some more tricky percentages */
            const int values[] = { 0, 5, 10, 20, 60, 90, 100, 110, 150, 200};
            ::memcpy(percentage, values, sizeof(values));
	    }
		/** swap some times in this array */
        for ( int i = 0; i < 20; i++ )
        {
            int m = rand() % 10;
            int n = rand() % 10;
            if ( n == m )  /** not a swap */
            {
                n = ( n + 1 ) % 10;
            }
            int h = percentage[ m ];
            percentage[ m ] = percentage[ n ];
            percentage[ n ] = h;
        }
        for ( int i = 0; i < number; i++ )
        {
            /** basevalue 0..2000 */
            basevalue[ i ] = 100 * ( rand() % 21 );
            /** calc percentvalue */
            percentvalue[ i ] = percentage[ i ] * basevalue[ i ] / 100;
        }
        break;
    case 2:
        int factor;
        for ( int i = 0; i < number; i++ )
        {
            /** this lines give you 3 integer by chance. percentage will be somewhere
              * between 0 and 200. We use factorisation to have "almost" random numbers
              * and _always_ valid numbers. */
            factor = 1;
            if ( rand() % 2 == 1 )
                factor *= 2;
            if ( rand() % 2 == 1 )
                factor *= 2;
            if ( rand() % 2 == 1 )
                factor *= 5;
            if ( rand() % 2 == 1 )
                factor *= 5;
            percentage[ i ] = rand() % ( 200 / factor );
            basevalue[ i ] = rand() % ( 10 * factor );
            percentvalue[ i ] = percentage[ i ] * basevalue[ i ];
            percentage[ i ] *= factor;
            basevalue[ i ] *= 100 / factor;
        }
        break;
    }
    newTask(); /** prepare the window for the first task */
}

void KPercentMain::timerEvent( QTimerEvent *event )
{
    if( progress_bar_count->progress()<count*100 )
    {
        progress_bar_count->setProgress(progress_bar_count->progress()+1);
    }
    else
    {
        killTimer( event->timerId() );
    }
    if( progress_bar_count->progress()==number*100 ) finished();
}

void KPercentMain::keyPressEvent( QKeyEvent * e )
{
	 if ( e->key() == Key_Enter ) slotApplyInput();
    else
        KDialog::keyPressEvent( e );
}

#include "kpercentmain.moc"
