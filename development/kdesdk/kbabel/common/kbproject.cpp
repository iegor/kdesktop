/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2004 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <goutte@kde.org>

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

#include "resources.h"
#include "kbproject.h"
#include "kbprojectmanager.h"
#include "kbprojectsettings.h"

#include <qfileinfo.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kspell.h>

namespace KBabel
{

Project::Project( const QString& file ) :
    QObject ()
    , _filename (file)
    , _name (i18n("unnamed"))
    , _valid (false)
    , _config (NULL)
    , _settings (NULL)
{
    QFileInfo info(file);

    if(info.isDir())
      return;

    if(info.exists())
    {
        // ### TODO: can a setting file only be a text/plain file?
        // ### TODO: why not use KMimeType::findByPath if the fileis always local?
	// first, for existing file check the MIME type
	// it has to be text file
	KMimeType::Ptr mime = KMimeType::findByURL( KURL::fromPathOrURL( file ) );
        
	if ( mime->name() == "text/plain" )
        {
            // Plain text, no problem! 
        }
        else if ( mime->name() == "application/x-zerosize" )
        {
            // Empty files are allowed too
            kdWarning() << "Empty KBabel project file!" << endl;
        }
	else if ( mime->name() == "application/octet-stream" )
        {
	    // this can happen if the file has some very long lines
	    kdWarning() << "KBabel project file detected as octet-stream! Perhaps there are very long lines in it!" << endl;
        }
        else
	{
            // Unknown mime type, abort!
            // ### TODO: should the user not be asked instead?
	    kdWarning () << "File type is " << mime->name() << endl;
	    return;
	}
    }
        
    _config = KSharedConfig::openConfig (_filename, false, false);
    kdDebug (KBABEL) << "Opened project " << _filename << endl;
    
    // read the project name
    _config->setGroup( "Project" );
    _name = _config->readEntry( "Name", QString() );
    if ( _name.isEmpty() )
	_name = "Default-Project"; // set default project name

    // ### FIXME: why is the Version number not written to the project file?
    if ( _config->readEntry( "Version", QString() ) != "1.0.1" )
    {
	kdWarning() << "Old project format assumed" << endl;
	
	// transform old entries to the new ones
	_config->setGroup( "Header");

	// TODO remove obsolete entries as well	
	if (_config->hasKey("Author-Email") && !_config->hasKey("AuthorEmail"))
	{
	    _config->writeEntry ("AuthorEmail", _config->readEntry ("Author-Email", ""));
	    _config->deleteEntry ("Author-Email");
	}

	if (_config->hasKey("Author-Name") && !_config->hasKey("AuthorName"))
	{
	    _config->writeEntry ("AuthorName", _config->readEntry ("Author-Name", ""));
	    _config->deleteEntry ("Author-Name");
	}    

	if (_config->hasKey("Local-Author-Name") && !_config->hasKey("LocalAuthorName"))
	{
	    _config->writeEntry ("LocalAuthorName", _config->readEntry ("Local-Author-Name", ""));
	    _config->deleteEntry ("Local-Author-Name");
	}    

	if (_config->hasKey("Update-Charset") && !_config->hasKey("UpdateCharset"))
	{
	    _config->writeEntry ("UpdateCharset", _config->readEntry ("Update-Charset", ""));
	    _config->deleteEntry ("Update-Charset");
	}    

	if (_config->hasKey("Update-Encoding") && !_config->hasKey("UpdateEncoding"))
	{
	    _config->writeEntry ("UpdateEncoding", _config->readEntry ("Update-Encoding", ""));
	    _config->deleteEntry ("Update-Encoding");
	}    

	if (_config->hasKey("Update-Language-Team") && !_config->hasKey("UpdateLanguageTeam"))
	{
	    _config->writeEntry ("UpdateLanguageTeam", _config->readEntry ("Update-Language-Team", ""));
	    _config->deleteEntry ("Update-Language-Team");
	}    

	if (_config->hasKey("Update-Last-Translator") && !_config->hasKey("UpdateLastTranslator"))
	{
	    _config->writeEntry ("UpdateLastTranslator", _config->readEntry ("Update-Last-Translator", ""));
	    _config->deleteEntry ("Update-Last-Translator");
	}    

	if (_config->hasKey("Update-Project") && !_config->hasKey("UpdateProject"))
	{
	    _config->writeEntry ("UpdateProject", _config->readEntry ("Update-Project", ""));
	    _config->deleteEntry ("Update-Project");
	}    

	if (_config->hasKey("Update-Revision") && !_config->hasKey("UpdateRevision"))
	{
	    _config->writeEntry ("UpdateRevision", _config->readEntry ("Update-Revision", ""));
	    _config->deleteEntry ("Update-Revision");
	}    

	_config->sync();
    }
    
    _valid = true;
    
    _settings = new KBabel::ProjectSettingsBase( _config );
    _settings->readConfig();
}

Project::~Project ()
{
    if (_settings) 
    {
	// store the project name
	_settings->setVersion( "1.0.1" );
	_settings->setName(_name);
	
	kdDebug () << "Writing configuration" << endl;
	_settings->writeConfig();
    
	delete _settings;
    }
    
    // unregister in project manager
    kdDebug () << "Freeing project " << _filename << endl;
    ProjectManager::remove (this);
}
	    
KConfig* Project::config ()
{
    return _config;
}

KSharedConfig* Project::sharedConfig( void )
{
    return _config;
}

ProjectSettingsBase* Project::settings ()
{
    return _settings;
}

IdentitySettings Project::identitySettings ()
{    
    IdentitySettings settings;
    
    settings.authorName=_settings->authorName();
    settings.authorLocalizedName=_settings->localAuthorName();
    settings.authorEmail=_settings->authorEmail();
    settings.languageName=_settings->language();
    settings.languageCode=_settings->languageCode();
    settings.mailingList=_settings->mailinglist();
    settings.timeZone=_settings->timezone();

    settings.numberOfPluralForms=_settings->pluralForms();
	    
    if( settings.numberOfPluralForms < -1 )
    {
	kdWarning() << "Invalid number of plural forms, ignoring: " << settings.numberOfPluralForms << endl;
	settings.numberOfPluralForms = 2;
    }
    settings.checkPluralArgument=_settings->checkPluralArgument();
    settings.gnuPluralFormHeader=_settings->pluralFormsHeader();

    return settings;
}

SaveSettings Project::saveSettings ()
{
    SaveSettings settings;
    
    settings.autoUpdate=_settings->autoUpdate();
    settings.updateLastTranslator=_settings->updateLastTranslator();
    settings.updateRevisionDate=_settings->updateRevisionDate();
    settings.updateLanguageTeam=_settings->updateLanguageTeam();
    settings.updateCharset=_settings->updateCharset();
    settings.updateEncoding=_settings->updateEncoding();
    settings.encoding=_settings->encoding();
    settings.useOldEncoding=_settings->useOldEncoding();

    settings.updateProject=_settings->updateProject();
    settings.projectString=_settings->projectString();

    settings.autoSyntaxCheck = _settings->autoSyntaxCheck();
    settings.saveObsolete = _settings->saveObsolete();
    settings.customDateFormat = _settings->customDateFormat();
    settings.dateFormat = (Qt::DateFormat)_settings->dateFormat();
    settings.updateDescription = _settings->updateDescription();
    settings.descriptionString = _settings->descriptionString();
    settings.updateTranslatorCopyright = _settings->updateTranslatorCopyright();
    settings.FSFCopyright = _settings->fSFCopyright();

    settings.autoSaveDelay=_settings->autoSaveDelay();
    
    return settings;
}


MiscSettings Project::miscSettings ()
{
    MiscSettings settings;

    QString temp=_settings->accelMarker();
    if(temp.length() > 0)
    {
	settings.accelMarker=temp[0];
    }
    
    temp = _settings->contextInfo();

    settings.contextInfo.setPattern(temp);

    temp = _settings->singularPlural();
    settings.singularPlural.setPattern(temp);

    settings.useBzip = _settings->bZipCompression();
    settings.compressSingleFile = _settings->compressSingleFile();
    
    return settings;
}

SpellcheckSettings Project::spellcheckSettings ()
{
    SpellcheckSettings settings;
    
    settings.noRootAffix=_settings->noRootAffix();
    settings.runTogether=_settings->runTogether();
    settings.spellEncoding=_settings->spellEncoding();
    settings.spellClient=_settings->spellClient();
    settings.spellDict=_settings->spellDictionary();
    settings.rememberIgnored=_settings->rememberIgnored();
    settings.ignoreURL=_settings->ignoreURL();
    settings.onFlySpellcheck=_settings->onFlySpellCheck();

    settings.valid=true;
    
    return settings;
}

SourceContextSettings Project::sourceContextSettings ()
{
    SourceContextSettings settings;
    
    settings.codeRoot=_settings->codeRoot();
    settings.sourcePaths=_settings->paths();
	
    return settings;
}

CatManSettings Project::catManSettings ()
{
    CatManSettings settings;
    
    settings.poBaseDir=_settings->poBaseDir();
    settings.potBaseDir=_settings->potBaseDir();
    settings.openWindow=_settings->openWindow();

    settings.killCmdOnExit=_settings->killCmdOnExit();
    settings.indexWords=_settings->indexWords();
    settings.msgfmt=_settings->msgfmt();


    settings.dirCommands = _settings->dirCommands();
    settings.dirCommandNames = _settings->dirCommandNames();

    settings.fileCommands = _settings->fileCommands();
    settings.fileCommandNames = _settings->fileCommandNames();

    settings.ignoreURL=_settings->ignoreURL();

    settings.flagColumn=_settings->showFlagColumn();
    settings.fuzzyColumn=_settings->showFuzzyColumn();
    settings.untranslatedColumn=_settings->showUntranslatedColumn();
    settings.totalColumn=_settings->showTotalColumn();
    settings.cvsColumn=_settings->showCVSColumn();
    settings.revisionColumn=_settings->showRevisionColumn();
    settings.translatorColumn=_settings->showTranslatorColumn();

    return settings;
}
	    
void Project::setSettings (IdentitySettings settings)
{    
    _settings->setAuthorName(settings.authorName);
    _settings->setLocalAuthorName(settings.authorLocalizedName);
    _settings->setAuthorEmail(settings.authorEmail);
    _settings->setAuthorEmail(settings.authorEmail);
    _settings->setLanguage(settings.languageName);
    _settings->setLanguageCode(settings.languageCode);
    _settings->setMailinglist(settings.mailingList);
    _settings->setTimezone(settings.timeZone);
    _settings->setPluralForms(settings.numberOfPluralForms);
    _settings->setCheckPluralArgument(settings.checkPluralArgument);
    _settings->setPluralFormsHeader(settings.gnuPluralFormHeader);
	
    _settings->writeConfig();
    
    emit signalIdentitySettingsChanged();
    emit signalSettingsChanged();
}

void Project::setSettings (SaveSettings settings)
{
    _settings->setAutoUpdate(settings.autoUpdate);
    _settings->setUpdateLastTranslator(settings.updateLastTranslator);
    _settings->setUpdateRevisionDate(settings.updateRevisionDate);
    _settings->setUpdateLanguageTeam(settings.updateLanguageTeam);
    _settings->setUpdateCharset(settings.updateCharset);
    _settings->setUpdateEncoding(settings.updateEncoding);
    _settings->setEncoding(settings.encoding);
    _settings->setUseOldEncoding(settings.useOldEncoding);

    _settings->setUpdateProject(settings.updateProject);
    _settings->setProjectString(settings.projectString);

    _settings->setAutoSyntaxCheck(settings.autoSyntaxCheck);
    _settings->setSaveObsolete(settings.saveObsolete);
    _settings->setCustomDateFormat(settings.customDateFormat);
    _settings->setDateFormat(settings.dateFormat);
    _settings->setUpdateDescription(settings.updateDescription);
    _settings->setDescriptionString(settings.descriptionString);
    _settings->setUpdateTranslatorCopyright(settings.updateTranslatorCopyright);
    _settings->setFSFCopyright(settings.FSFCopyright);

    _settings->setAutoSaveDelay(settings.autoSaveDelay);

    _settings->writeConfig();    
    
    emit signalSaveSettingsChanged();
    emit signalSettingsChanged();
}


void Project::setSettings (MiscSettings settings)
{
    _settings->setAccelMarker(settings.accelMarker);
    _settings->setContextInfo(settings.contextInfo.pattern());
    _settings->setSingularPlural(settings.singularPlural.pattern());
    _settings->setBZipCompression(settings.useBzip);
    _settings->setCompressSingleFile(settings.compressSingleFile);
    
    _settings->writeConfig();
    
    emit signalMiscSettingsChanged();
    emit signalSettingsChanged();
}

void Project::setSettings (SpellcheckSettings settings)
{
    _settings->setNoRootAffix(settings.noRootAffix);
    _settings->setRunTogether(settings.runTogether);
    _settings->setSpellEncoding(settings.spellEncoding);
    _settings->setSpellClient(settings.spellClient);
    _settings->setSpellDictionary(settings.spellDict);
    _settings->setRememberIgnored(settings.rememberIgnored);
    _settings->setIgnoreURL(settings.ignoreURL);
    _settings->setOnFlySpellCheck(settings.onFlySpellcheck);
    
    _settings->writeConfig();
    
    emit signalSpellcheckSettingsChanged();
    emit signalSettingsChanged();
}

void Project::setSettings (SourceContextSettings settings)
{
    KConfigGroupSaver saver(_config,"SourceContext");

    _settings->setCodeRoot(settings.codeRoot);
    _settings->setPaths(settings.sourcePaths);

    _settings->writeConfig();
    
    emit signalSourceContextSettingsChanged();
    emit signalSettingsChanged();
}

void Project::setSettings (CatManSettings settings)
{
    _settings->setPoBaseDir(settings.poBaseDir);
    _settings->setPotBaseDir(settings.potBaseDir);
    _settings->setOpenWindow(settings.openWindow);

    _settings->setKillCmdOnExit(settings.killCmdOnExit);
    _settings->setIndexWords(settings.indexWords);

    _settings->setDirCommands(settings.dirCommands);
    _settings->setDirCommandNames(settings.dirCommandNames);

    _settings->setFileCommands(settings.fileCommands);
    _settings->setFileCommandNames(settings.fileCommandNames);

    _settings->setValidationIgnoreURL(settings.ignoreURL);

    _settings->setShowFlagColumn(settings.flagColumn);
    _settings->setShowFuzzyColumn(settings.fuzzyColumn);
    _settings->setShowUntranslatedColumn(settings.untranslatedColumn);
    _settings->setShowTotalColumn(settings.totalColumn);
    _settings->setShowCVSColumn(settings.cvsColumn);
    _settings->setShowRevisionColumn(settings.revisionColumn);
    _settings->setShowTranslatorColumn(settings.translatorColumn);
	
    _settings->writeConfig();
    
    emit signalCatManSettingsChanged();
    emit signalSettingsChanged();
}
    
}

#include "kbproject.moc"
