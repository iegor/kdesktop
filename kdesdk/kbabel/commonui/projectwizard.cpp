/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2004 by StanislavVsinovsky
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

#include "projectwizard.h"
#include "projectwizardwidget.h"
#include "projectwizardwidget2.h"

#include "kbprojectmanager.h"

#include <qcombobox.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>
#include <kurlrequester.h>

using namespace KBabel;

ProjectWizard::ProjectWizard(QWidget *parent,const char *name)
        : KWizard(parent,name,true)
{
    _wizard = new ProjectStep1(this,"project wizard widget");
    
    // fill the known language codes
    KConfig all_languages("all_languages", true, false, "locale");
    QStringList lang_codes = KGlobal::locale()->allLanguagesTwoAlpha();
    for (QStringList::iterator it = lang_codes.begin();
	it != lang_codes.end(); ++it)
    {
	// we need untranslated entries here, because of Translation Robot!
	QString entry = (*it);
	const int i = entry.find('_');
	entry.replace(0, i, entry.left(i).lower());
	all_languages.setGroup(entry);
	entry = all_languages.readEntryUntranslated("Name");
	if( ! entry.isEmpty() )
	{
	    _wizard->_projectLanguage->insertItem( entry );
	    m_language_codes[entry] = (*it);
	}
    }	
   
    connect( _wizard->_projectName, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect( _wizard->_projectFile, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    connect( this, SIGNAL( helpClicked( void ) ), this, SLOT( slotHelpClicked( void ) ) );

    addPage(_wizard, i18n("Basic Project Information"));
    
    _wizard2 = new ProjectStep2(this,"project wizard widget2");
    _wizard2->_poDirEdit->setMode( KFile::Directory );
    _wizard2->_potDirEdit->setMode( KFile::Directory );
    addPage(_wizard2, i18n("Translation Files"));
    
    setFinishEnabled (_wizard2, true);
    setNextEnabled (_wizard, false);
}

QString ProjectWizard::url()
{
    return _wizard->_projectFile->url();
}

Project::Ptr ProjectWizard::project()
{
    Project::Ptr p = ProjectManager::open( _wizard->_projectFile->url() );
    p->setName( _wizard->_projectName->text() );
    
    enum type { KDE, GNOME, TP, Other };
    
    type project_type = (type) _wizard->_projectType->currentItem();
    
    KBabel::CatManSettings catman = p->catManSettings();
    catman.poBaseDir = _wizard2->_poDirEdit->url();
    catman.potBaseDir = _wizard2->_potDirEdit->url();
    p->setSettings (catman);
    
    KBabel::IdentitySettings identity = p->identitySettings();
    // Language
    identity.languageName = _wizard->_projectLanguage->currentText();
    // LanguageCode
    identity.languageCode = m_language_codes[identity.languageName];
    p->setSettings (identity);
    
    KBabel::SaveSettings save = p->saveSettings();
    // autochecksyntax (not for KDE - it uses incompatible plural forms formatting)
    if( project_type == KDE )
    {
	save.autoSyntaxCheck = false;
    }
    p->setSettings (save);
    
    KBabel::MiscSettings misc = p->miscSettings();
    if (project_type == GNOME)
    {
	misc.accelMarker = '_';
    }
    p->setSettings (misc);
    
    return p;
}

void ProjectWizard::next()
{
    // check if the file exists
    QFileInfo file(url());
	
    if( file.exists() )
    {
	if (KMessageBox::warningContinueCancel(0, i18n("The file '%1' already exists.\n"
	    "Do you want to replace it?").arg(url()), i18n("File Exists"), i18n("Replace") ) == KMessageBox::Cancel)
		return;
    }

    KWizard::next();
}

void ProjectWizard::textChanged(const QString &)
{
    setNextEnabled( _wizard, !_wizard->_projectName->text().isEmpty() && !_wizard->_projectFile->url().isEmpty() );
}

Project::Ptr ProjectWizard::newProject()
{
    ProjectWizard* dialog = new ProjectWizard();
    if( dialog->exec() == QDialog::Accepted )
    {
	Project::Ptr res = dialog->project();
	delete dialog;
	res->config()->sync();
	return res;
    }
    
    return 0;
}

void ProjectWizard::slotHelpClicked( void )
{
    kapp->invokeHelp( "preferences-project-wizard", "kbabel" );
}

#include "projectwizard.moc"
