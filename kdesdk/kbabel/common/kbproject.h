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
#ifndef KBPROJECT_H
#define KBPROJECT_H

#include <qstring.h>

#include <kconfig.h>
#include "catalogsettings.h"
#include "projectsettings.h"

namespace KBabel
{
    class ProjectSettingsBase;

    class KDE_EXPORT Project : public QObject, public KShared
    {
	    Q_OBJECT
	public:
	    typedef KSharedPtr <Project> Ptr;

	    Project( const QString& file );
	    virtual ~Project ();
	    
	    QString filename () const { return _filename; }
	    QString name () const { return _name; }
	    
	    void setName( const QString& name ) { _name = name; }

	    KConfig* config ();
            /**
             * Returns the KSharedConfig pointer of the project data
             * @since 1.11.2 (KDE 3.5.2)
             */
            KSharedConfig* sharedConfig( void );
            
	    ProjectSettingsBase* settings ();
	    
	    bool valid () { return _valid; }
	    
	    IdentitySettings identitySettings ();
	    SaveSettings saveSettings ();
	    MiscSettings miscSettings ();
	    SpellcheckSettings spellcheckSettings ();
	    SourceContextSettings sourceContextSettings ();
	    CatManSettings catManSettings ();
	    
	    void setSettings(KBabel::CatManSettings newSettings);
	    void setSettings(KBabel::SaveSettings newSettings);
	    void setSettings(KBabel::IdentitySettings newSettings);
	    void setSettings(KBabel::MiscSettings newSettings);
	    void setSettings(KBabel::SpellcheckSettings newSettings);
	    void setSettings(KBabel::SourceContextSettings newSettings);
	    
	signals:
	    void signalIdentitySettingsChanged();
	    void signalSaveSettingsChanged();
	    void signalMiscSettingsChanged();
	    void signalSpellcheckSettingsChanged();
	    void signalSourceContextSettingsChanged();
	    void signalCatManSettingsChanged();

	    /**
	    * This is general purpose signal emitted additionally
	    * to the signals above.
	    */	    
	    void signalSettingsChanged();

	private:
	    QString _filename;
	    QString _name;
	    bool _valid;
	    KSharedConfig::Ptr _config;
	    ProjectSettingsBase* _settings;
    };

}

#endif // KBPROJECT_H
