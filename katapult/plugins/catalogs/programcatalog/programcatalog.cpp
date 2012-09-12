/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kservicegroup.h>
#include <ksycocaentry.h>
#include <ksycocatype.h>
#include <kapplication.h>
#include <knuminput.h>
#include <kcombobox.h>

#include <qcheckbox.h>

#include "settings.h"
#include "programcatalog.h"
#include "program.h"
#include "actionregistry.h"
#include "actionrunprogram.h"

K_EXPORT_COMPONENT_FACTORY( katapult_programcatalog,
                            KGenericFactory<ProgramCatalog>( "katapult_programcatalog" ) )

ProgramCatalog::ProgramCatalog(QObject *, const char *, const QStringList&)
 : CachedCatalog()
{
	_minQueryLen = 1;
	_ignoreIconless = TRUE;
	_ignoreTerminal = TRUE;
	_useExecName = FALSE;
	ActionRegistry::self()->registerAction(new ActionRunProgram());
}

ProgramCatalog::~ProgramCatalog()
{
}

void ProgramCatalog::initialize()
{
	cacheProgramList(QString::null);
}

void ProgramCatalog::cacheProgramList(QString relPath)
{
	KServiceGroup::Ptr group = KServiceGroup::group(relPath);
	if(!group || !group->isValid())
		return;
	
	KServiceGroup::List list = group->entries();
	if(list.isEmpty())
		return;
	
	KServiceGroup::List::ConstIterator it = list.begin();
	for(; it != list.end(); ++it)
	{
		KSycocaEntry *e = *it;
		
		if(e != 0) {
			if(e->isType(KST_KServiceGroup))
			{
				KServiceGroup::Ptr g(static_cast<KServiceGroup *>(e));
				if(!g->noDisplay())
					cacheProgramList(g->relPath());
			} else if(e->isType(KST_KService))
			{
				KService::Ptr s(static_cast<KService *>(e));
				if(s->type() == "Application" &&
					(!_ignoreIconless || !s->icon().isEmpty()) &&
					(!_ignoreTerminal || !s->terminal()) && !s->noDisplay()
				) {
					addItem(new Program(s, _useExecName));
				}
			}
		}
	}
}

unsigned int ProgramCatalog::minQueryLen() const
{
	return _minQueryLen;
}

void ProgramCatalog::readSettings(KConfigBase *config)
{
	_minQueryLen = config->readUnsignedNumEntry("MinQueryLen", 1);
	_ignoreIconless = config->readBoolEntry("IgnoreIconless", TRUE);
	_useExecName = config->readBoolEntry("UseExecName", FALSE);
	_ignoreTerminal = config->readBoolEntry("IgnoreTerminal", TRUE);
}

void ProgramCatalog::writeSettings(KConfigBase *config)
{
	config->writeEntry("MinQueryLen", _minQueryLen);
	config->writeEntry("IgnoreIconless", _ignoreIconless);
	config->writeEntry("UseExecName", _useExecName);
	config->writeEntry("IgnoreTerminal", _ignoreTerminal);
}

QWidget * ProgramCatalog::configure()
{
	ProgramCatalogSettings *settings = new ProgramCatalogSettings();
	
	settings->minQueryLen->setValue(_minQueryLen);
	connect(settings->minQueryLen, SIGNAL(valueChanged(int)), this, SLOT(minQueryLenChanged(int)));
	
	settings->ignoreIconless->setChecked(_ignoreIconless);
	connect(settings->ignoreIconless, SIGNAL(toggled(bool)), this, SLOT(toggleIgnoreIconless(bool)));
	
	settings->useExecName->setChecked(_useExecName);
	connect(settings->useExecName, SIGNAL(toggled(bool)), this, SLOT(toggleUseExecName(bool)));
	
	settings->ignoreTerminal->setChecked(_ignoreTerminal);
	connect(settings->ignoreTerminal, SIGNAL(toggled(bool)), this, SLOT(toggleIgnoreTerminal(bool)));
	
	return settings;
}

void ProgramCatalog::minQueryLenChanged(int _minQueryLen)
{
	this->_minQueryLen = _minQueryLen;
}

void ProgramCatalog::toggleIgnoreIconless(bool _ignoreIconless)
{
	this->_ignoreIconless = _ignoreIconless;
}

void ProgramCatalog::toggleUseExecName(bool _useExecName)
{
	this->_useExecName = _useExecName;
}

void ProgramCatalog::toggleIgnoreTerminal(bool _ignoreTerminal)
{
	this->_ignoreTerminal = _ignoreTerminal;
}

#include "programcatalog.moc"
