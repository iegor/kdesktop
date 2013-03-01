//////////////////////////////////////////////////////////////////////////////
// polyesterconfig.cc
// -------------------
// Config module for Polyester window decoration
// -------------------
// Copyright (c) 2005 Marco Martin
// -------------------
// derived from Smooth Blend
// Copyright (c) 2005 Ryan Nickell
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qwhatsthis.h>
#include <qcombobox.h>

#include "polyesterconfig.h"
#include "configdialog.h"

//////////////////////////////////////////////////////////////////////////////
// polyesterConfig()
// -------------
// Constructor

polyesterConfig::polyesterConfig(KConfig* config, QWidget* parent)
        : QObject(parent), config_(0), dialog_(0) {
    // create the configuration object
    config_ = new KConfig("kwinpolyesterrc");
    KGlobal::locale()->insertCatalogue("kwin_polyester_config");

    // create and show the configuration dialog
    dialog_ = new ConfigDialog(parent);

    dialog_->show();
    // load the configuration
    load(config_);

    // setup the connections for title align
    connect(dialog_->titlealign, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
    // setup the connections for corner rounding
    connect(dialog_->roundCorners, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->titleBarStyle, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->buttonStyle, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    // setup title shadow
    connect(dialog_->titleshadow, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    // setup button actions
    connect(dialog_->animatebuttons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->nomodalbuttons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->btnComboBox, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->lightBorder, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    // setup the connections for spin boxes
    connect(dialog_->titlesize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->buttonsize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->framesize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->squareButton, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    // double click the menu
    connect(dialog_->menuClose, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
}

//////////////////////////////////////////////////////////////////////////////
// ~polyesterConfig()
// --------------
// Destructor

polyesterConfig::~polyesterConfig() {
    if (dialog_)
    {
        delete dialog_;
    }
    if (config_)
    {
        delete config_;
    }
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void polyesterConfig::selectionChanged(int) {

    if(dialog_->buttonsize->value() + dialog_->framesize->value() > dialog_->titlesize->value())
    {
        dialog_->buttonsize->setValue(dialog_->titlesize->value()- dialog_->framesize->value());
    }
    // setting the framesize to less than 2 will lose the top gradient and look flat
   /* if(dialog_->framesize->value() < 2)
    {
        dialog_->framesize->setValue(2);
    }*/
    emit changed();
}

//////////////////////////////////////////////////////////////////////////////
// load()
// ------
// Load configuration data

void polyesterConfig::load(KConfig*) {
    config_->setGroup("General");

    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value.latin1());
    if (button)
    {
        button->setChecked(true);
    }

    dialog_->titlesize->setValue( config_->readNumEntry("TitleSize",20 ) );
    dialog_->buttonsize->setValue( config_->readNumEntry("ButtonSize",18 ) );
    dialog_->squareButton->setChecked( config_->readBoolEntry("SquareButton", false ) );
    dialog_->framesize->setValue( config_->readNumEntry("FrameSize",2 ) );

    bool cornersFlag = config_->readBoolEntry("RoundCorners", true);
    dialog_->roundCorners->setChecked( cornersFlag );
    bool titleshadow = config_->readBoolEntry("TitleShadow", true);
    dialog_->titleshadow->setChecked(titleshadow);
    bool animatebuttons = config_->readBoolEntry("AnimateButtons", true);
    dialog_->animatebuttons->setChecked(animatebuttons);
    bool lightBorder = config_->readBoolEntry("LightBorder", true);
    dialog_->lightBorder->setChecked(lightBorder);
    dialog_->titleBarStyle->setCurrentItem(config_->readNumEntry("TitleBarStyle",0));
    dialog_->buttonStyle->setCurrentItem(config_->readNumEntry("ButtonStyle",1));
    bool nomodalbuttons = config_->readBoolEntry("NoModalButtons", false);
    dialog_->nomodalbuttons->setChecked(nomodalbuttons);
    dialog_->btnComboBox->setCurrentItem(config_->readNumEntry("ButtonComboBox",0));
    bool menuClose = config_->readBoolEntry("CloseOnMenuDoubleClick");
    dialog_->menuClose->setChecked(menuClose);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save configuration data

void polyesterConfig::save(KConfig*) {
    config_->setGroup("General");

    QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
    if (button)
    {
        config_->writeEntry("TitleAlignment", QString(button->name()));
    }
    config_->writeEntry("RoundCorners", dialog_->roundCorners->isChecked() );
    config_->writeEntry("TitleSize", dialog_->titlesize->value() );
    config_->writeEntry("ButtonSize", dialog_->buttonsize->value() );
    config_->writeEntry("SquareButton", dialog_->squareButton->isChecked() );
    config_->writeEntry("FrameSize", dialog_->framesize->value() );
    config_->writeEntry("TitleShadow", dialog_->titleshadow->isChecked() );
    config_->writeEntry("TitleBarStyle", dialog_->titleBarStyle->currentItem());
    config_->writeEntry("ButtonStyle", dialog_->buttonStyle->currentItem());
    config_->writeEntry("AnimateButtons", dialog_->animatebuttons->isChecked() );
    config_->writeEntry("LightBorder", dialog_->lightBorder->isChecked() );
    config_->writeEntry("NoModalButtons", dialog_->nomodalbuttons->isChecked() );
    config_->writeEntry("ButtonComboBox", dialog_->btnComboBox->currentItem());
    config_->writeEntry("CloseOnMenuDoubleClick", dialog_->menuClose->isChecked() );

    config_->sync();
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set configuration defaults

void polyesterConfig::defaults() {
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child("AlignHCenter");
    if (button)
    {
        button->setChecked(true);
    }
    dialog_->roundCorners->setChecked( true );
    dialog_->titlesize->setValue( 20 );
    dialog_->squareButton->setChecked( false );
    dialog_->buttonsize->setValue( 18 );
    dialog_->framesize->setValue( 2 );
    dialog_->titleBarStyle->setCurrentItem( 0 );
    dialog_->buttonStyle->setCurrentItem( 1 );
    dialog_->titleshadow->setChecked( true );
    dialog_->animatebuttons->setChecked( true );
    dialog_->nomodalbuttons->setChecked( true );
    dialog_->lightBorder->setChecked( true );
    dialog_->btnComboBox->setCurrentItem( 0 );
    dialog_->menuClose->setChecked( false );
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C" {
    QObject* allocate_config(KConfig* config, QWidget* parent) {
        return (new polyesterConfig(config, parent));
    }
}

#include "polyesterconfig.moc"
