//////////////////////////////////////////////////////////////////////////////
// smoothblendconfig.cc
// -------------------
// Config module for Smooth Blend window decoration
// -------------------
// Copyright (c) 2005 Ryan Nickell <p0z3r@users.sourceforge.net>
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

#include "smoothblendconfig.h"
#include "configdialog.h"

//////////////////////////////////////////////////////////////////////////////
// smoothblendConfig()
// -------------
// Constructor

smoothblendConfig::smoothblendConfig(KConfig* config, QWidget* parent)
        : QObject(parent), config_(0), dialog_(0) {
    // create the configuration object
    config_ = new KConfig("kwinsmoothblendrc");
    KGlobal::locale()->insertCatalogue("kwin_smoothblend_config");

    // create and show the configuration dialog
    dialog_ = new ConfigDialog(parent);
    dialog_->show();

    // load the configuration
    load(config_);

    // setup the connections for title align
    connect(dialog_->titlealign, SIGNAL(clicked(int)),this, SLOT(selectionChanged(int)));
    // setup the connections for corner rounding
    connect(dialog_->roundCorners, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    // setup title shadow
    connect(dialog_->titleshadow, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    // setup button actions
    connect(dialog_->animatebuttons, SIGNAL(stateChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->btnComboBox, SIGNAL(activated(int)),this,SLOT(selectionChanged(int)));
    // setup the connections for spin boxes
    connect(dialog_->titlesize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->buttonsize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    connect(dialog_->framesize, SIGNAL(valueChanged(int)),this,SLOT(selectionChanged(int)));
    // double click the menu 
    connect(dialog_->menuClose, SIGNAL(stateChanged(int)),this, SLOT(selectionChanged(int)));
}

//////////////////////////////////////////////////////////////////////////////
// ~smoothblendConfig()
// --------------
// Destructor

smoothblendConfig::~smoothblendConfig() {
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

void smoothblendConfig::selectionChanged(int) {

    if(dialog_->buttonsize->value() + dialog_->framesize->value() > dialog_->titlesize->value())
    {
        dialog_->buttonsize->setValue(dialog_->titlesize->value()- dialog_->framesize->value());
    }
    // setting the framesize to less than 2 will lose the top gradient and look flat
    if(dialog_->framesize->value() < 2)
    {
        dialog_->framesize->setValue(2);
    }
    emit changed();
}

//////////////////////////////////////////////////////////////////////////////
// load()
// ------
// Load configuration data

void smoothblendConfig::load(KConfig*) {
    config_->setGroup("General");

    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child(value.latin1());
    if (button)
    {
        button->setChecked(true);
    }

    dialog_->titlesize->setValue( config_->readNumEntry("TitleSize",30 ) );
    dialog_->buttonsize->setValue( config_->readNumEntry("ButtonSize",26 ) );
    dialog_->framesize->setValue( config_->readNumEntry("FrameSize",4 ) );

    bool cornersFlag = config_->readBoolEntry("RoundCorners", true);
    dialog_->roundCorners->setChecked( cornersFlag );
    bool titleshadow = config_->readBoolEntry("TitleShadow", true);
    dialog_->titleshadow->setChecked(titleshadow);
    bool animatebuttons = config_->readBoolEntry("AnimateButtons", true);
    dialog_->animatebuttons->setChecked(animatebuttons);
    dialog_->btnComboBox->setCurrentItem(config_->readNumEntry("ButtonComboBox",0));
    bool menuClose = config_->readBoolEntry("CloseOnMenuDoubleClick");
    dialog_->menuClose->setChecked(menuClose);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save configuration data

void smoothblendConfig::save(KConfig*) {
    config_->setGroup("General");

    QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
    if (button)
    {
        config_->writeEntry("TitleAlignment", QString(button->name()));
    }
    config_->writeEntry("RoundCorners", dialog_->roundCorners->isChecked() );
    config_->writeEntry("TitleSize", dialog_->titlesize->value() );
    config_->writeEntry("ButtonSize", dialog_->buttonsize->value() );
    config_->writeEntry("FrameSize", dialog_->framesize->value() );
    config_->writeEntry("TitleShadow", dialog_->titleshadow->isChecked() );
    config_->writeEntry("AnimateButtons", dialog_->animatebuttons->isChecked() );
    config_->writeEntry("ButtonComboBox", dialog_->btnComboBox->currentItem());
    config_->writeEntry("CloseOnMenuDoubleClick", dialog_->menuClose->isChecked() );

    config_->sync();
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set configuration defaults

void smoothblendConfig::defaults() {
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->child("AlignHCenter");
    if (button)
    {
        button->setChecked(true);
    }
    dialog_->roundCorners->setChecked( true );
    dialog_->titlesize->setValue( 30 );
    dialog_->buttonsize->setValue( 26 );
    dialog_->framesize->setValue( 4 );
    dialog_->titleshadow->setChecked( true );
    dialog_->animatebuttons->setChecked( true );
    dialog_->btnComboBox->setCurrentItem( 0 );
    dialog_->menuClose->setChecked( false );
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C" {
    QObject* allocate_config(KConfig* config, QWidget* parent) {
        return (new smoothblendConfig(config, parent));
    }
}

#include "smoothblendconfig.moc"
