//////////////////////////////////////////////////////////////////////////////
// phasestyleconfig.cpp
// -------------------
// Config dialog for Phase widget style
// -------------------
// Copyright (c) 2004 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////

#include <qsettings.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <klocale.h>
#include <kglobal.h>

#include "phasestyleconfig.h"
#include "styledialog.h"

//////////////////////////////////////////////////////////////////////////////
// PhaseStyleConfig Class                                                    //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PhaseStyleConfig()
// ----------------
// Constructor

PhaseStyleConfig::PhaseStyleConfig(QWidget* parent) : StyleDialog(parent)
{
    KGlobal::locale()->insertCatalogue("kstyle_phase_config");

    QSettings settings;
    oldgradients =
        settings.readBoolEntry("/phasestyle/Settings/gradients", true);
    gradients->setChecked(oldgradients);
    oldhighlights =
        settings.readBoolEntry("/phasestyle/Settings/highlights", true);
    highlights->setChecked(oldhighlights);

    // connections
    connect(gradients, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(highlights, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
}

//////////////////////////////////////////////////////////////////////////////
// ~PhaseStyleConfig()
// -----------------
// Destructor

PhaseStyleConfig::~PhaseStyleConfig()
{
    KGlobal::locale()->removeCatalogue("kstyle_phase_config");
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void PhaseStyleConfig::updateChanged()
{
    bool update = false;

    if ((gradients->isChecked() != oldgradients) ||
        (highlights->isChecked() != oldhighlights)) {
        update = true;
    }

    emit changed(update);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save the settings

void PhaseStyleConfig::save()
{
    QSettings settings;
    settings.writeEntry("/phasestyle/Settings/gradients",
                        gradients->isChecked());
    settings.writeEntry("/phasestyle/Settings/highlights",
                        highlights->isChecked());
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set to the defaults

void PhaseStyleConfig::defaults()
{
    gradients->setChecked(true);
    highlights->setChecked(true);
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT QObject* allocate_kstyle_config(QWidget* parent) {
        return(new PhaseStyleConfig(parent));
    }
}

#include "phasestyleconfig.moc"
