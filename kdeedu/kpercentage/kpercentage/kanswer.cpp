/***************************************************************************
                       kanswer.cpp  -  description
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

// C/C++ includes
#include <stdlib.h>
#include <assert.h>

// Qt includes
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>

// KDE includes
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kstdguiitem.h>

// local includes
#include "kanimation.h"
#include "kanswer.h"

KAnswer::KAnswer( QWidget *parent ): KDialog( parent, "answer", TRUE )
{
    setFont( parent->font() );
    // fix size for the background pixmap
    setFixedSize( QSize( 430, 190 ) );

    // load background pixmap
    QPixmap bgp( locate( "data", "kpercentage/pics/kanswer_bg.png" ) );
    setBackgroundPixmap( bgp );

    setupSprite();

    ButtonOK = new KPushButton( KStdGuiItem::ok(), this );
    // for the bottons edges
    ButtonOK->setBackgroundOrigin( QPushButton::ParentOrigin );
    ButtonOK->setIconSet( QIconSet( DesktopIcon( "button_ok" ) ) );
//    ButtonOK->hide();

    TextLabelAnswer = new QLabel( this );
    // make the label transparent
    TextLabelAnswer->setBackgroundOrigin( QLabel::ParentOrigin );
    TextLabelAnswer->setBackgroundPixmap( bgp );

    canvas->setBackgroundPixmap( bgp );
    canvas_view->setBackgroundOrigin( QCanvasView::ParentOrigin );
    canvas_view->setBackgroundPixmap( bgp );

    ///////
    // begin layouting
    ///////
    mainLayout = new QHBoxLayout( this );
    mainLayout->setDirection(QBoxLayout::LeftToRight);

//    mainLayout->addSpacing( 20 );

    QVBoxLayout *leftLayout = new QVBoxLayout( mainLayout, -1, "main" );
//    leftLayout->addSpacing( 20 );
    leftLayout->addWidget( canvas_view );
    leftLayout->addSpacing( 20 );
    leftLayout->addStretch();

    mainLayout->addSpacing( 60 );

    QVBoxLayout *rightLayout = new QVBoxLayout( mainLayout, -1, "right" );

    rightLayout->addStretch( 2 );

    QHBoxLayout *topLayout = new QHBoxLayout( rightLayout, -1, "top" );
    topLayout->setDirection(QBoxLayout::LeftToRight);
    topLayout->addStretch();
    topLayout->addWidget( TextLabelAnswer );
    topLayout->addStretch();

    rightLayout->addStretch( 2 );

    QHBoxLayout *bottomLayout = new QHBoxLayout( rightLayout, -1, "bottom" );
    bottomLayout->setDirection(QBoxLayout::LeftToRight);
    bottomLayout->addStretch();
    bottomLayout->addWidget( ButtonOK );
    bottomLayout->addSpacing( 20 );

    rightLayout->addSpacing( 20 );
    ///////
    // end layouting
    ///////

    connect( ButtonOK, SIGNAL( clicked() ), this, SLOT( accept() ) );
    loadAnswers();
}

void KAnswer::loadAnswers()
{
    // helping to manage the IO
    QString s;
    QFile f;
    QTextStream t;

    //reading proper answers from file
    f.setName( locate( "data", "kpercentage/right.txt" ) );
    if ( f.open( IO_ReadOnly ) )        // file opened successfully
    {
        QTextStream t( &f );        // use a text stream
        while ( !t.eof() )            // until end of file...
        {
            s = t.readLine();       // line of text excluding '\n'
            rightAnswerList.append( s );
        }
    }
    f.close();

    // reading proper answers from file
    f.setName( locate( "data", "kpercentage/wrong.txt" ) );
    if ( f.open( IO_ReadOnly ) )        // file opened successfully
    {
        QTextStream t( &f );        // use a text stream
        while ( !t.eof() )            // until end of file...
        {
            s = t.readLine();       // line of text excluding '\n'
            wrongAnswerList.append( s );
        }
    }
    f.close();
}

void KAnswer::setAnswer( int modus )
{
    bool correct;
    switch ( modus )
    {
    case 1:
        setCaption( i18n( "Congratulations!" ) );
        TextLabelAnswer->setText( getRightAnswer() );
        correct = true;
        break;
    case 2:
        setCaption( i18n( "Error!" ) );
        TextLabelAnswer->setText( getWrongAnswer() );
        correct = false;
        break;
    case 3:
        setCaption( i18n( "Oops!" ) );
        TextLabelAnswer->setText( i18n( "Mistyped!" ) );
        correct = false;
        break;
    case 4:
        setCaption( i18n( "Congratulations!" ) );
        TextLabelAnswer->setText( i18n( "Great!\nYou managed all\nthe exercises!" ) );
        correct = true;
	break;
    default:
	assert(false);
	correct = false;
    }
    
    if (correct)
    {
        wrong_animation->hide();
        right_animation->show();
    }
    else
    {
        wrong_animation->show();
        right_animation->hide();
    }
    
    wrong_animation->setToStart();
    wrong_animation->setAnimated( !correct );
    right_animation->setToStart();
    right_animation->setAnimated( correct );
    canvas->setAllChanged();
    canvas->update();
    canvas->setAdvancePeriod( advPer );
    
	 // this seems to be needed for proper showing TextLabelAnswer :-(
    resize( width(), height() );
}

// reads one answer out of the list by chance
QString KAnswer::getRightAnswer()
{
    return i18n( rightAnswerList[ random() % rightAnswerList.count() ].utf8() );
}

// reads one answer out of the list by chance
QString KAnswer::getWrongAnswer()
{
    return i18n( wrongAnswerList[ random() % wrongAnswerList.count() ].utf8() );
}


// Animation stuff

void KAnswer::setupSprite()
{

    advPer = 80;
    
    canvas = new QCanvas( this );
    canvas->resize( size().width(), size().height() );
    pixs = new QCanvasPixmapArray( locate( "data", "kpercentage/pics/" )+"smily%1.png", 7 );

    right_animation = new KAnimation(  locate( "data", "kpercentage/story/right.story" ), pixs , canvas );
    right_animation->setAnimated( TRUE );
    wrong_animation = new KAnimation(  locate( "data", "kpercentage/story/wrong.story" ), pixs , canvas );
    wrong_animation->setAnimated( TRUE );

    canvas_view = new QCanvasView( canvas, this );
    canvas_view->resize( size() );
    canvas_view->setVScrollBarMode( QCanvasView::AlwaysOff );
    canvas_view->setHScrollBarMode( QCanvasView::AlwaysOff );
    canvas_view->setFrameStyle( QCanvasView::NoFrame );
}


void KAnswer::timerEvent( QTimerEvent *event )
{
    accept();
}

int KAnswer::exec()
{
    killTimers();
    startTimer( 3000 ); // 5 seconds
    return KDialog::exec();
}

void KAnswer::accept()
{
    canvas->setAdvancePeriod( -1 ); // stop all animation
    KDialog::accept();
}

#include "kanswer.moc"
