//////////////////////////////////////////////////////////////////////////////
// kwinqinxconfig.cc
// -------------------
// Config module for Qinx window decoration
// -------------------
// Copyright (c) 2002-2004 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
// Inspired by QNiX theme by Derek Greene <del@chek.com>
//////////////////////////////////////////////////////////////////////////////

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qwhatsthis.h>

#include "kwinqinxconfig.h"
#include "configdialog.h"

//////////////////////////////////////////////////////////////////////////////
// KwinQinxConfig Class                                                     //
//////////////////////////////////////////////////////////////////////////////

// TODO: consider options for using appicon somehow

//////////////////////////////////////////////////////////////////////////////
// KwinQinxConfig()
// ----------------
// Constructor

KwinQinxConfig::KwinQinxConfig(KConfig* config, QWidget* parent)
    : QObject(parent), config_(0), dialog_(0)
{
    config_ = new KConfig("kwinqinxrc");
    KGlobal::locale()->insertCatalogue("kwin_qinx_config");

    dialog_ = new ConfigDialog(parent);

    load(config_);
    dialog_->show();

    // connections
    connect(dialog_->titlealign, SIGNAL(clicked(int)),
            this, SLOT(selectionChanged()));
    connect(dialog_->mouseover, SIGNAL(clicked()),
            this, SLOT(selectionChanged()));
    connect(dialog_->cornerdetail, SIGNAL(clicked()),
            this, SLOT(selectionChanged()));
    connect(dialog_->flipgradient, SIGNAL(clicked()),
            this, SLOT(selectionChanged()));
}

//////////////////////////////////////////////////////////////////////////////
// ~KwinQinxConfig()
// -----------------
// Destructor

KwinQinxConfig::~KwinQinxConfig()
{
    if (dialog_) delete dialog_;
    if (config_) delete config_;
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void KwinQinxConfig::selectionChanged()
{
    emit changed();
}

//////////////////////////////////////////////////////////////////////////////
// load()
// ------
// Load configuration data

void KwinQinxConfig::load(KConfig*)
{
    config_->setGroup("General");
    
    QString value = config_->readEntry("TitleAlignment", "AlignHCenter");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->
        child((const char *)value.latin1());
    if (button) button->setChecked(true);

    dialog_->mouseover->setChecked
        (config_->readBoolEntry("MouseOver", true));
    dialog_->cornerdetail->setChecked
        (config_->readBoolEntry("CornerDetail", true));
    dialog_->flipgradient->setChecked
        (config_->readBoolEntry("FlipGradient", false));
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save configuration data

void KwinQinxConfig::save(KConfig*)
{
    config_->setGroup("General");
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->selected();
    if (button) config_->writeEntry("TitleAlignment",
                                    QString(button->name()));
    config_->writeEntry("MouseOver",
                        dialog_->mouseover->isChecked());
    config_->writeEntry("CornerDetail",
                        dialog_->cornerdetail->isChecked());
    config_->writeEntry("FlipGradient",
                        dialog_->flipgradient->isChecked());
    config_->sync();
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ------
// Set configuration defaults

void KwinQinxConfig::defaults()
{
    QRadioButton *button = (QRadioButton*)dialog_->titlealign->
        child("AlignHCenter");
    if (button) button->setChecked(true);
    dialog_->mouseover->setChecked(true);
    dialog_->cornerdetail->setChecked(true);
    dialog_->flipgradient->setChecked(false);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    QObject* allocate_config(KConfig* config, QWidget* parent) {
        return(new KwinQinxConfig(config, parent));
    }
}

#include "kwinqinxconfig.moc"
