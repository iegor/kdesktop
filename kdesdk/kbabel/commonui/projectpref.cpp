/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2004-2005 by Stanislav Visnovsky
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
#include "projectpref.h"
#include "projectprefwidgets.h"
#include "diffpreferences.h"
#include "kbprojectsettings.h"

#include <qlayout.h>
#include <qwhatsthis.h>
#include <qvbox.h>

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>


#define PAGE_IDENTITY 		0
#define PAGE_SAVE     		1
#define PAGE_SPELL    		2
#define PAGE_SOURCE   		3
#define PAGE_MISC     		4
#define PAGE_CATMAN       	5
#define PAGE_DIRCOMMANDS  	6
#define PAGE_FILECOMMANDS 	7
#define PAGE_VIEW         	8
#define PAGE_DIFF         	9

using namespace KBabel;

ProjectDialog::ProjectDialog(Project::Ptr project)
    : KConfigDialog(0, "project dialog", project->settings(), 
                  IconList, Help|Default|Ok|Apply|Cancel)
    , _project( project )
{
    
    _identityPage = new IdentityPreferences(0, project->name());
    addPage(_identityPage, i18n("title of page in preferences dialog","Identity")
               , "pref_identity"
	       , i18n("Information About You and Translation Team")
               );

    _savePage = new SavePreferences(0);
    addPage(_savePage, i18n("title of page in preferences dialog","Save")
            , "filesave"
	    , i18n("Options for File Saving"));


    _spellPage = new SpellPreferences(0);
    addPage(_spellPage, i18n("title of page in preferences dialog","Spelling")
	    , "spellcheck"
            , i18n("Options for Spell Checking"));
    connect( _spellPage, SIGNAL( settingsChanged() )
	    , this, SLOT(updateButtons()) );

    _sourcePage = new SourceContextPreferences(0);
    addPage(_sourcePage, i18n("title of page in preferences dialog","Source")
		, "source"
                ,i18n("Options for Showing Source Context"));
    connect(_sourcePage, SIGNAL (itemsChanged())
	, this, SLOT (updateButtons()) );

    _miscPage = new MiscPreferences(0);
    addPage(_miscPage, i18n("title of page in preferences dialog","Miscellaneous")
		, "misc"
                ,i18n("Miscellaneous Settings"));

    _catmanPage = new CatmanPreferences(0);
    addPage(_catmanPage, i18n("title of page in preferences dialog","Folders")
		, "catalogmanager"
                , i18n("Paths to Message Catalogs & Catalog Templates"));

    _dirCommandsPage = new DirCommandsPreferences(0);
    addPage(_dirCommandsPage, i18n("title of page in preferences dialog","Folder Commands")
		, "folder"
                , i18n("User-Defined Commands for Folder Items"));
    connect( _dirCommandsPage, SIGNAL( settingsChanged() ), 
	this, SLOT(updateButtons()) );

    _fileCommandsPage = new FileCommandsPreferences(0);
    addPage(_fileCommandsPage, i18n("title of page in preferences dialog","File Commands")
		, "files"
                , i18n("User-Defined Commands for File Items"));
    connect( _fileCommandsPage, SIGNAL( settingsChanged() ), 
	this, SLOT(updateButtons()) );

    _viewPage = new ViewPreferences(0);
    addPage(_viewPage, i18n("title of page in preferences dialog","Catalog Manager")
		, "view_tree"
                , i18n("Catalog Manager View Settings"));

    _diffPage = new DiffPreferences(0);
    addPage(_diffPage, i18n("title of page in preferences dialog","Diff")
		, "diff"
                , i18n("Searching for Differences"));

    setHelp( "preferences-project-settings", "kbabel" );

    adjustSize();
}

void ProjectDialog::slotDefault()
{
    // redefine the KConfigDialog behavior to push default on the
    // current page only
    
    _project->settings()->useDefaults(true);
    
   switch(activePageIndex())
   {
      case PAGE_IDENTITY:
        _identityPage->defaults(_project->identitySettings());
        break;
      case PAGE_SAVE:
        _savePage->defaults(_project->saveSettings());
        break;
      case PAGE_MISC:
	  _miscPage->defaults(_project->miscSettings());
	  break;
      case PAGE_SPELL:
         _spellPage->defaults(_project->spellcheckSettings());
         break;
      case PAGE_SOURCE:
         _sourcePage->defaults(_project->sourceContextSettings());
         break;
      case PAGE_CATMAN:
         _catmanPage->defaults(_project->catManSettings());
         break;
      case PAGE_DIRCOMMANDS:
         _dirCommandsPage->defaults(_project->catManSettings());
         break;
      case PAGE_FILECOMMANDS:
         _fileCommandsPage->defaults(_project->catManSettings());
         break;
      case PAGE_VIEW:
         _viewPage->defaults(_project->catManSettings());
         break;
   }

    _project->settings()->useDefaults(false);
}

void ProjectDialog::updateSettings()
{
    KBabel::CatManSettings _CatManSettings;
    SourceContextSettings contextSettings;
    
   _spellPage->mergeSettings(_spellcheckSettings);
   _dirCommandsPage->mergeSettings(_CatManSettings);
   _fileCommandsPage->mergeSettings(_CatManSettings);
   _sourcePage->mergeSettings(contextSettings);
   
   _project->settings()->setDirCommands( _CatManSettings.dirCommands );
   _project->settings()->setDirCommandNames( _CatManSettings.dirCommandNames );
   _project->settings()->setFileCommands( _CatManSettings.fileCommands );
   _project->settings()->setFileCommandNames( _CatManSettings.fileCommandNames );
   
   _project->setSettings(_spellcheckSettings);
   
   _project->settings()->setPaths( contextSettings.sourcePaths );
}

void ProjectDialog::updateWidgets()
{
    _spellPage->updateWidgets(_project->spellcheckSettings());
    _dirCommandsPage->updateWidgets(_project->catManSettings());
    _fileCommandsPage->updateWidgets(_project->catManSettings());
    _sourcePage->updateWidgets(_project->sourceContextSettings());
}

void ProjectDialog::updateWidgetsDefault()
{
    _project->settings()->useDefaults( true );
    updateWidgets();
    _project->settings()->useDefaults( false );
}

bool ProjectDialog::isDefault()
{
    SourceContextSettings contextSettings, defaultContextSettings;

    // get the current values
   _spellPage->mergeSettings(_spellcheckSettings);
   _dirCommandsPage->mergeSettings(_CatManSettings);
   _fileCommandsPage->mergeSettings(_CatManSettings);
   _sourcePage->mergeSettings(defaultContextSettings);

    // get default values
    _project->settings()->useDefaults(true);    
    SpellcheckSettings defaultSpell = _project->spellcheckSettings();
    CatManSettings defaultCatMan = _project->catManSettings();
    defaultContextSettings = _project->sourceContextSettings();
    _project->settings()->useDefaults(false);    
    
    bool result = true;
    
    result &= _spellcheckSettings.noRootAffix == defaultSpell.noRootAffix;
    result &= _spellcheckSettings.runTogether == defaultSpell.runTogether;
    result &= _spellcheckSettings.spellClient == defaultSpell.spellClient;
    result &= _spellcheckSettings.spellDict == defaultSpell.spellDict;
    result &= _spellcheckSettings.spellEncoding == defaultSpell.spellEncoding;

    result &= _CatManSettings.dirCommandNames == defaultCatMan.dirCommandNames;
    result &= _CatManSettings.dirCommands == defaultCatMan.dirCommands;
    result &= _CatManSettings.fileCommandNames == defaultCatMan.fileCommandNames;
    result &= _CatManSettings.fileCommands == defaultCatMan.fileCommands;

    result &= contextSettings.sourcePaths != defaultContextSettings.sourcePaths;

    return result;
}

bool ProjectDialog::hasChanged()
{
    SourceContextSettings contextSettings, defaultContextSettings;
    
    // get the current values
   _spellPage->mergeSettings(_spellcheckSettings);
   _dirCommandsPage->mergeSettings(_CatManSettings);
   _fileCommandsPage->mergeSettings(_CatManSettings);
   _sourcePage->mergeSettings(contextSettings);

    // get project values
    SpellcheckSettings defaultSpell = _project->spellcheckSettings();
    CatManSettings defaultCatMan = _project->catManSettings();
    defaultContextSettings = _project->sourceContextSettings();
    
    bool result = false;
    
    result |= _spellcheckSettings.noRootAffix != defaultSpell.noRootAffix;
    result |= _spellcheckSettings.runTogether != defaultSpell.runTogether;
    result |= _spellcheckSettings.spellClient != defaultSpell.spellClient;
    result |= _spellcheckSettings.spellDict != defaultSpell.spellDict;
    result |= _spellcheckSettings.spellEncoding != defaultSpell.spellEncoding;

    result |= _CatManSettings.dirCommandNames != defaultCatMan.dirCommandNames;
    result |= _CatManSettings.dirCommands != defaultCatMan.dirCommands;
    result |= _CatManSettings.fileCommandNames != defaultCatMan.fileCommandNames;
    result |= _CatManSettings.fileCommands != defaultCatMan.fileCommands;

    result |= contextSettings.sourcePaths != defaultContextSettings.sourcePaths;

    return result;
}

#include "projectpref.moc"
