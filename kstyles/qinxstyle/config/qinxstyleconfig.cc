//////////////////////////////////////////////////////////////////////////////
// qinxstyleconfig.cc
// -------------------
// Config module for Qinx widget style
// -------------------
// Copyright (c) 2002-2004 David Johnson <david@usermode.org>
// Please see the header file for copyright and license information.
//////////////////////////////////////////////////////////////////////////////
// Inspired by QNiX theme by Derek Greene <del@chek.com>
//////////////////////////////////////////////////////////////////////////////
 
#include <qsettings.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <klocale.h>
#include <kglobal.h>

#include "qinxstyleconfig.h"
#include "styledialog.h"

//////////////////////////////////////////////////////////////////////////////
// QinxStyleConfig Class                                                    //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// QinxStyleConfig()
// ----------------
// Constructor

QinxStyleConfig::QinxStyleConfig(QWidget* parent) : StyleDialog(parent)
{
    KGlobal::locale()->insertCatalogue("kstyle_qinx_config");

    QSettings settings;
    oldphotontabs =
        settings.readBoolEntry("/qinxstyle/Settings/photonTabs", false);
    photontabs->setChecked(oldphotontabs);
    oldphotonmenus =
        settings.readBoolEntry("/qinxstyle/Settings/photonMenus", true);
    photonmenus->setChecked(oldphotonmenus);
    oldusegradients =
        settings.readBoolEntry("/qinxstyle/Settings/useGradients", true);
    usegradients->setChecked(oldusegradients);
    oldhighlights =
        settings.readBoolEntry("/qinxstyle/Settings/highlights", true);
    highlights->setChecked(oldhighlights);

    // connections
    connect(photontabs, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(photonmenus, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(usegradients, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
    connect(highlights, SIGNAL(toggled(bool)),
            this, SLOT(updateChanged()));
}

//////////////////////////////////////////////////////////////////////////////
// ~QinxStyleConfig()
// -----------------
// Destructor

QinxStyleConfig::~QinxStyleConfig()
{
    KGlobal::locale()->removeCatalogue("kstyle_qinx_config");
}

//////////////////////////////////////////////////////////////////////////////
// selectionChanged()
// ------------------
// Selection has changed

void QinxStyleConfig::updateChanged()
{
    bool update = false;

    if ((photontabs->isChecked() != oldphotontabs) ||
        (photonmenus->isChecked() != oldphotonmenus) ||
        (usegradients->isChecked() != oldusegradients) ||
        (highlights->isChecked() != oldhighlights)) {
        update = true;
    }

    emit changed(update);
}

//////////////////////////////////////////////////////////////////////////////
// save()
// ------
// Save the settings

void QinxStyleConfig::save()
{
    QSettings settings;
    settings.writeEntry("/qinxstyle/Settings/photonTabs",
                        photontabs->isChecked());
    settings.writeEntry("/qinxstyle/Settings/photonMenus",
                        photonmenus->isChecked());
    settings.writeEntry("/qinxstyle/Settings/useGradients",
                        usegradients->isChecked());
    settings.writeEntry("/qinxstyle/Settings/highlights",
                        highlights->isChecked());
}

//////////////////////////////////////////////////////////////////////////////
// defaults()
// ----------
// Set to the defaults

void QinxStyleConfig::defaults()
{
   photontabs->setChecked(false); 
   photonmenus->setChecked(true); 
   usegradients->setChecked(true); 
   highlights->setChecked(true); 
}

//////////////////////////////////////////////////////////////////////////////
// Plugin Stuff                                                             //
//////////////////////////////////////////////////////////////////////////////

extern "C"
{
    KDE_EXPORT QObject* allocate_kstyle_config(QWidget* parent) {
        return(new QinxStyleConfig(parent));
    }
}

#include "qinxstyleconfig.moc"
