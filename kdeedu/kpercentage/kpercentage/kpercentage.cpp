/***************************************************************************
                         kpercentage.cpp  -  description
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

// C/C++ includes
#include <stdlib.h>

// Qt includes
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

// KDE includes
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

//local includes
#include "kpercentage.h"
#include "ksplashscreen.h"

KPercentage::KPercentage( const char *name ) :
        KDialog ( 0, name )
{
    // show splash screen
    KSplashScreen * splash_screen = new KSplashScreen( this, "splashscreen" );
    splash_screen->show();
    splash_screen->raise();

    // Icon loader for the button's icons
    KIconLoader icon_loader;

    // let's set a suitable, not too small font size
    QFont the_font( font() );
    the_font.setPointSize( 14 );
    //the_font.setBold(true);
    setFont( the_font );

    // prepare exercise window, that will use the fontsize above!
    percent_main = new KPercentMain( this, "KPercentage" );

    // fixed geometry bacause of background pixmap
    setFixedSize( QSize( 548, 248 ) );

    /** load and set background pixmap */
    QPixmap bgp( locate( "data", QApplication::reverseLayout() ? "kpercentage/pics/kpercentage_bg_rtl.png" : "kpercentage/pics/kpercentage_bg.png" ) );
    setBackgroundPixmap( bgp );


    QLabel *label_number = new QLabel( i18n( "Number of tasks:" ), this );
    QLabel *label_level = new QLabel( i18n( "Level:" ), this );
    QLabel *label_choose = new QLabel( i18n( "Choose an exercise type:" ), this );

    /** make labels transparent */
    label_number->setBackgroundPixmap( bgp );
    label_number->setBackgroundOrigin( QLabel::ParentOrigin );
    label_level->setBackgroundPixmap( bgp );
    label_level->setBackgroundOrigin( QLabel::ParentOrigin );
    label_choose->setBackgroundPixmap( bgp );
    label_choose->setBackgroundOrigin( QLabel::ParentOrigin );

    KPushButton *button_basevalue = new KPushButton( i18n( "x% &of ?? = y" ), this );
    KPushButton *button_percentvalue = new KPushButton( i18n( "x% of &y = ??" ), this );
    KPushButton *button_percentage = new KPushButton( i18n( "??% o&f x = y" ), this );
    KPushButton *button_random = new KPushButton( i18n( "??" ), this );
    KPushButton *button_help = new KPushButton( KStdGuiItem::help().text(), this );
    button_help->setIconSet( QIconSet( icon_loader.loadIcon( "help", KIcon::NoGroup, 32 ) ) );
	KHelpMenu *help_menu = new KHelpMenu( this, KGlobal::instance()->aboutData(), true );
	button_help->setPopup( ( QPopupMenu* ) ( help_menu->menu() ) );
    KPushButton *button_close = new KPushButton( i18n( "E&xit" ), this );
    button_close->setIconSet( QIconSet( icon_loader.loadIcon( "exit", KIcon::NoGroup, 32 ) ) );

    combo_box_level = new KComboBox( this );
    combo_box_level->insertItem( i18n( "Easy" ) );
    combo_box_level->insertItem( i18n( "Medium" ) );
    combo_box_level->insertItem( i18n( "Crazy" ) );

    spin_box_number = new QSpinBox( 1, 10, 1, this );
    spin_box_number->setValue( 5 );

    // connecting all the slots
    connect( button_basevalue, SIGNAL( clicked() ), this, SLOT( selBasevalue() ) );
    connect( button_percentvalue, SIGNAL( clicked() ), this, SLOT( selPercentvalue() ) );
    connect( button_percentage, SIGNAL( clicked() ), this, SLOT( selPercentage() ) );
    connect( button_random, SIGNAL( clicked() ), this, SLOT( selRandom() ) );
    connect( button_help, SIGNAL( clicked() ), this, SLOT( needHelp() ) );
    connect( button_close, SIGNAL( clicked() ), this, SLOT( accept() ) );

    ////////
    // begin layouting
    ////////
    QVBoxLayout *main_layout = new QVBoxLayout( this, 20, 20, "main_layout" );
    main_layout->setResizeMode( QLayout::FreeResize );

    QHBoxLayout *top_layout = new QHBoxLayout( main_layout, -1, "top_layout" );
    top_layout->addWidget( label_number );
    top_layout->addWidget( spin_box_number );
    top_layout->addSpacing( 20 );
    top_layout->addStretch();
    top_layout->addWidget( label_level );
    top_layout->addWidget( combo_box_level );

    main_layout->addSpacing( 40 );
    main_layout->addStretch();

    QHBoxLayout *bottom_layout = new QHBoxLayout( main_layout, -1, "bottom_layout" );
    QVBoxLayout *bLeftLayout = new QVBoxLayout( bottom_layout );
    bLeftLayout->addWidget( label_choose );
    bLeftLayout->addSpacing( 10 );

    QGridLayout *grid_layout = new QGridLayout( bLeftLayout, 2, 2, 10 );
    grid_layout->addWidget( button_basevalue, 0, 0 );
    grid_layout->addWidget( button_percentvalue, 1, 0 );
    grid_layout->addWidget( button_percentage, 0, 1 );
    grid_layout->addWidget( button_random, 1, 1 );

    bottom_layout->addStretch();

    QVBoxLayout *b_right_layout = new QVBoxLayout( bottom_layout );
    b_right_layout->addStretch();
    b_right_layout->addWidget( button_help );
    b_right_layout->addSpacing( 10 );
    b_right_layout->addWidget( button_close );

    main_layout->addStretch();
    ////////
    // end layouting
    ////////

	////////
	// Tooltips
	QToolTip::add( button_basevalue, i18n( "Exercises with base value omitted" ) );
	QToolTip::add( button_percentvalue, i18n( "Exercises with percent value omitted" ) );
	QToolTip::add( button_percentage, i18n( "Exercises with percentage omitted" ) );
	QToolTip::add( button_random, i18n( "Several exercise types in random" ) );
	QToolTip::add( spin_box_number, i18n( "Choose the number of exercises from 1 to 10." ) );
	QToolTip::add( combo_box_level, i18n( "Choose the level of difficulty." ) );
	QToolTip::add( button_close, i18n( "Close KPercentage." ) );
	QToolTip::add( button_help, i18n( "Get some help." ) );

	////////
	// WhatsThis
	QWhatsThis::add( button_basevalue, i18n( "Click here to start a sequence of exercises where the base value is omitted.") );
	QWhatsThis::add( button_percentvalue, i18n( "Click here to start a sequence of exercises where the percent value is omitted." ) );
	QWhatsThis::add( button_percentage, i18n( "Click here to start a sequence of exercises where the percentage is omitted." ) );
	QWhatsThis::add( button_random, i18n( "Click here to start a sequence of exercises where one value is omitted at random." ) );
	QWhatsThis::add( spin_box_number, i18n( "Here you can adjust the number of exercises from 1 to 10." ) );
	QWhatsThis::add( combo_box_level, i18n( "Choose one of the levels <i>easy</i>, <i>medium</i>, and <i>crazy</i>." ) );
	QWhatsThis::add( button_close, i18n( "Close KPercentage." ) );
	QWhatsThis::add( button_help, i18n( "Get some help." ) );
}

void KPercentage::selPercentage()
{
    // set the proper value for KPercentMain::selected_type
    percent_main->selected_type = SEL_PERCENTAGE;
    // and lets go!
    startExercise();
}

void KPercentage::selBasevalue()
{
    // set the proper value for KPercentMain::selected_type
    percent_main->selected_type = SEL_BASEVALUE;
    // and lets go!
    startExercise();
}

void KPercentage::selPercentvalue()
{
    // set the proper value for KPercentMain::selected_type
    percent_main->selected_type = SEL_PERCENTVALUE;
    // and lets go!
    startExercise();
}

void KPercentage::selRandom()
{
    // set the proper value for KPercentMain::selected_type
    percent_main->selected_type = SEL_RANDOM;
    // and lets go!
    startExercise();
}

/** No descriptions */
void KPercentage::startExercise()
{
    // copy the actual settings to the KPercentMain instance
    percent_main->setNumber( spin_box_number->value() );
    percent_main->selected_level = combo_box_level->currentItem();
    percent_main->initExercise();
    QPoint p = pos();
    hide();
    percent_main->move( p );
    percent_main->exec();
    move( p );
    show();
}

void KPercentage::needHelp()
{
    kapp->invokeHelp( "", "kpercentage" );
}

void KPercentage::closeEvent( QCloseEvent * )
{
    exit( 0 );
}

#include "kpercentage.moc"
