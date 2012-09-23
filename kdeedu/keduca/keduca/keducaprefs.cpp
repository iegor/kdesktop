/***************************************************************************
                          keducaprefs.cpp  -  description
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

#include "keducaprefs.h"
#include "keducaprefs.moc"

#include <klocale.h>
#include <kiconloader.h>
#include <kconfig.h>

#include <qlayout.h>
#include <qvbox.h>
#include <qbuttongroup.h>


KEducaPrefs::KEducaPrefs(QWidget *parent, const char *name, bool modal ) : KDialogBase(IconList, i18n("Configure"), Ok|Cancel, Ok, parent, name, modal, true)
{
    setPageGeneral();
    configRead();
}

KEducaPrefs::~KEducaPrefs(){}

/** Set page general */
void KEducaPrefs::setPageGeneral()
{
    QVBox *mainFrame = addVBoxPage( i18n("Miscellaneous"), i18n("Various Settings"), DesktopIcon("misc"));


    
    QButtonGroup *buttonGroup1 = new QButtonGroup( mainFrame, "ButtonGroup1" );
    buttonGroup1->setTitle( i18n( "General" ) );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 0 );
    buttonGroup1->layout()->setMargin( 0 );
    QVBoxLayout *buttonGroup1Layout = new QVBoxLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );
    buttonGroup1Layout->setSpacing( 6 );
    buttonGroup1Layout->setMargin( 11 );

    _resultAfterNext = new QRadioButton( buttonGroup1, "RadioButton1" );
    _resultAfterNext->setText( i18n( "Show results of the answer after press next" ) );
    buttonGroup1Layout->addWidget( _resultAfterNext );

    _resultAfterFinish = new QRadioButton( buttonGroup1, "RadioButton2" );
    _resultAfterFinish->setText( i18n( "Show results when finish the test" ) );
    buttonGroup1Layout->addWidget( _resultAfterFinish );

    QGroupBox *GroupBox1 = new QGroupBox( mainFrame, "GroupBox7" );
    GroupBox1->setTitle( i18n( "Order" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    QVBoxLayout *GroupBox1Layout = new QVBoxLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );
    GroupBox1Layout->setSpacing( 6 );
    GroupBox1Layout->setMargin( 11 );
    
    _randomQuestions = new QCheckBox( GroupBox1, "CheckBox1" );
    _randomQuestions->setText( i18n( "Show questions in random order" ) );
    GroupBox1Layout->addWidget( _randomQuestions );

    _randomAnswers = new QCheckBox( GroupBox1, "CheckBox2" );
    _randomAnswers->setText( i18n( "Show answers in random order" ) );
    GroupBox1Layout->addWidget( _randomAnswers );
}

/** Read settings */
void KEducaPrefs::configRead()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Options");
    QSize defaultSize(500,400);
    resize( config->readSizeEntry("Geometry", &defaultSize ) );
    config->readBoolEntry("ResultFinish", true ) ? _resultAfterFinish->setChecked(true) : _resultAfterNext->setChecked(true);
    config->readBoolEntry("RandomQuestions", false ) ? _randomQuestions->setChecked(true) : _randomQuestions->setChecked(false);
    config->readBoolEntry("RandomAnswers", false ) ? _randomAnswers->setChecked(true) : _randomAnswers->setChecked(false);
}

/** Write settings */
void KEducaPrefs::configWrite()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Options");
    config->writeEntry("Geometry", size() );
    config->writeEntry("ResultFinish", _resultAfterFinish->isChecked() );
    config->writeEntry("RandomQuestions", _randomQuestions->isChecked() );
    config->writeEntry("RandomAnswers", _randomAnswers->isChecked() );
    config->sync();
}

/** Push ok button */
void KEducaPrefs::slotOk()
{
    configWrite();
    accept();
}

