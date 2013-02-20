/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
 
**************************************************************************** */


#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include "kbabeldict.h"
#include "kbabeldictview.h"

#include <kdebug.h>

KBabelDict::KBabelDict()
          : KDialogBase(0,"kbabeldictmain",false,i18n("KBabelDict")
                         , Close|Help|User1|User2|User3|Ok, Ok, true
                         , i18n("About"), i18n("About Module")
                         , i18n("Hide Sett&ings"))
{
    connect(this, SIGNAL(closeClicked()),this,SLOT(saveConfig()));
    connect(this, SIGNAL(closeClicked()),this,SLOT(quit()));

    view = new KBabelDictView(this);
    connect(this, SIGNAL(user1Clicked()), view, SLOT(about()));
    connect(this, SIGNAL(user2Clicked()), view, SLOT(aboutModule()));
    connect(this, SIGNAL(user3Clicked()), this, SLOT(togglePref()));

    // HACK: hide default button, otherwise it would be Help button
    showButtonOK(false);

    // KBabelDict has not a separate help file, so point to the correct part of the KBabel documentation
    setHelp( "using-kbabeldict", "kbabel" );

    setMainWidget(view);

    readConfig();
}

KBabelDict::~KBabelDict()
{
    delete(view);
}

void KBabelDict::saveConfig()
{
    KConfig *config=KGlobal::config();
    KConfigGroupSaver gs(config,"KBabelDict");
    config->writeEntry("Preferences",view->prefVisible());
}

void KBabelDict::readConfig()
{
    KConfig *config=KGlobal::config();
    KConfigGroupSaver gs(config,"KBabelDict");
    bool pref=config->readBoolEntry("Preferences",true);

    if(view->prefVisible() != pref)
    {
        togglePref();
    }
}

void KBabelDict::quit()
{
    kapp->quit();
}

void KBabelDict::togglePref()
{
    view->togglePref();

    if(view->prefVisible())
    {
        setButtonText(User3,i18n("Hide Sett&ings"));
    }
    else
    {
        setButtonText(User3,i18n("Show Sett&ings"));
    }
}


#include "kbabeldict.moc"
