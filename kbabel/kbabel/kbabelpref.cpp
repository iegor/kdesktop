/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2004-2005  by Stanislav Visnovsky
			    <visnovsky@kde.org>

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
#include "kbabelpref.h"
#include "kbabeldictbox.h"
#include "kbabelsettings.h"
#include "fontpreferences.h"
#include "editordiffpreferences.h"
#include "editorpreferences.h"
#include "searchpreferences.h"
#include "colorpreferences.h"
#include "toolaction.h"
#include "toolselectionwidget.h"
#include "qcombobox.h"

#include <klocale.h>
#include <kapplication.h>

using namespace KBabel;

KBabelPreferences::KBabelPreferences(QPtrList<ModuleInfo> ml)
    : KConfigDialog(0, "Preferences", KBabelSettings::self())
{
    _editorPage = new EditorPreferences(0, "editor");
    addPage( _editorPage
	, i18n("title of page in preferences dialog","Edit")
	, "edit"
        , i18n("Options for Editing"));

    // this contains a custom widget for tool selection, set it up	
    QValueList<KDataToolInfo> tools = ToolAction::validationTools();
    _editorPage->_kcfg_AutoCheckTools->loadTools( "validate", tools );
    connect( _editorPage->_kcfg_AutoCheckTools, SIGNAL( added( QListBoxItem * ) ),
	this, SLOT (updateButtons()));
    connect( _editorPage->_kcfg_AutoCheckTools, SIGNAL( removed( QListBoxItem * ) ),
	this, SLOT (updateButtons()));

    addPage(_searchPage = new SearchPreferences(0, "search")
	, i18n("title of page in preferences dialog","Search")
	, "transsearch"
        , i18n("Options for Searching Similar Translations"));

    // setup the dictionary combobox contents
    ModuleInfo *info;
    for(info = ml.first(); info != 0; info = ml.next())
    {
        _searchPage->_kcfg_DefaultModule->insertItem(info->name);
    }
    moduleList = ml;
    connect( _searchPage->_kcfg_DefaultModule, SIGNAL( activated(int) ),
	this, SLOT (updateButtons()));

    addPage(new EditorDiffPreferences(0, "diff") 
	,i18n("title of page in preferences dialog","Diff")
	, "diff"
        , i18n("Options for Showing Differences"));

    addPage(new FontPreferences(0, "fonts")
	, i18n("name of page in preferences dialog icon list","Fonts")
	, "font"
        , i18n("title of page in preferences dialog","Font Settings"));
    
    addPage(new ColorPreferences(0, "colors")
	, i18n("name of page in preferences dialog icon list","Colors")
	, "colorize"
        , i18n("title of page in preferences dialog","Color Settings"));
    
    adjustSize();
}


void KBabelPreferences::slotHelp()
{
   //TODO
   kapp->invokeHelp("Preferences","");
}

bool KBabelPreferences::hasChanged()
{
    ModuleInfo *info = moduleList.at(_searchPage->_kcfg_DefaultModule->currentItem());
    
    bool module_ret = true;
    if( info )
    {
	module_ret = info->id != KBabelSettings::defaultModule();
    }
    
    return KConfigDialog::hasChanged() 
	|| (_editorPage->_kcfg_AutoCheckTools->selectedTools() != KBabelSettings::autoCheckTools())
	|| (module_ret);
}

bool KBabelPreferences::isDefault()
{
    bool old_useDefault = KBabelSettings::self()->useDefaults(true);
    
    ModuleInfo *info = moduleList.at(_searchPage->_kcfg_DefaultModule->currentItem());
    bool module_ret = ( info && info->id == KBabelSettings::defaultModule() );

    bool ret = KConfigDialog::isDefault() 
	&& (_editorPage->_kcfg_AutoCheckTools->selectedTools().empty())
	&& (module_ret);
	
    KBabelSettings::self()->useDefaults(old_useDefault);
    return ret;
}

void KBabelPreferences::updateSettings()
{
    KConfigDialog::updateSettings();
    
    KBabelSettings::setAutoCheckTools(_editorPage->_kcfg_AutoCheckTools->selectedTools());

    int i=_searchPage->_kcfg_DefaultModule->currentItem();
    ModuleInfo *info = moduleList.at(i);

    if(info)
    {
	KBabelSettings::setDefaultModule(info->id);
    }
    
    emit settingsChanged();
}

void KBabelPreferences::updateWidgets()
{
    KConfigDialog::updateWidgets();
    _editorPage->_kcfg_AutoCheckTools->setSelectedTools(KBabelSettings::autoCheckTools());
    
    int i=0;
    ModuleInfo *info;
    for(info = moduleList.first(); info != 0; info = moduleList.next())
    {
        if(KBabelSettings::defaultModule() == info->id)
            break;

        i++;
    }
    _searchPage->_kcfg_DefaultModule->setCurrentItem(i);
}

void KBabelPreferences::updateWidgetsDefault()
{
    KConfigDialog::updateWidgetsDefault();
    
    bool old_useDefault = KBabelSettings::self()->useDefaults(true);

    kdDebug () << "Default tools: " << KBabelSettings::autoCheckTools() << endl;
    
    _editorPage->_kcfg_AutoCheckTools->setSelectedTools(KBabelSettings::autoCheckTools()); 

    int i=0;
    ModuleInfo *info;
    for(info = moduleList.first(); info != 0; info = moduleList.next())
    {
        if(KBabelSettings::defaultModule() == info->id)
            break;

        i++;
    }

    _searchPage->_kcfg_DefaultModule->setCurrentItem(i);
    
    KBabelSettings::self()->useDefaults(old_useDefault);
}

#include "kbabelpref.moc"
