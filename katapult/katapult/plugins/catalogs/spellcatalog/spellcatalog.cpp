/***************************************************************************
 *   Copyright (C) 2006  Jonathan Riddell                                  *
 *   jriddell@ubuntu.com                                                   *
 *                                                                         *
 *   Copyright (C) 2005  Tobi Vollebregt                                   *
 *   tobivollebregt@gmail.com                                              *
 *                                                                         *
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

#include <qlineedit.h>
#include <qlabel.h>

#include "settings.h"
#include "spellcatalog.h"
#include "actionregistry.h"
#include "actioncopyspelling.h"
#include "status.h"

K_EXPORT_COMPONENT_FACTORY( katapult_spellcatalog,
                            KGenericFactory<SpellCatalog>( "katapult_spellcatalog" ) )

SpellCatalog::SpellCatalog(QObject*, const char*, const QStringList&): _result(this, QString::null)
{
	ActionRegistry::self()->registerAction(new ActionCopySpelling());
}

SpellCatalog::~SpellCatalog()
{
}

void SpellCatalog::queryChanged()
{
	int newStatus = 0;
	QString cmd = query();
	int origLength = cmd.length();

	if (cmd.isEmpty()) {
		reset();
		setBestMatch(Match());
	} else {
		if (accepts(cmd)) {
			_result.setText(cmd);

			setBestMatch(Match(&_result, _result.parseError() ? 10 : 100, origLength));
			//set status.
			//add S_Multiple to make sure katapult doesn't auto-exec and close the window
			//add S_Active to make sure katapult doesn't start the hideTimer or clearTimer
			newStatus = S_HasResults | S_Multiple | S_Active;
		} else {
			newStatus = 0;
		}
	}
	setStatus(newStatus);
}

bool SpellCatalog::accepts(const QString& str) const
{
	//accept if we begin with the triggerWord
	int length = _triggerWord.length();
	return str.left(length + 1) == _triggerWord + " ";
}

void SpellCatalog::readSettings(KConfigBase* config)
{
        _triggerWord = config->readEntry("TriggerWord", i18n("Should be short, easy and quick to type", "spell"));
}

void SpellCatalog::writeSettings(KConfigBase* config)
{
        config->writeEntry("TriggerWord", _triggerWord);
}

QWidget * SpellCatalog::configure()
{
	SpellCatalogSettings* settings = new SpellCatalogSettings();

	settings->triggerWordLE->setText(_triggerWord);
	connect(settings->triggerWordLE, SIGNAL(textChanged(const QString&)), this, SLOT(triggerWordChanged(const QString&)));

	settings->introLabel->setText(i18n("Use with \"%1 myword\"").arg(_triggerWord));

	return settings;
}

void SpellCatalog::triggerWordChanged(const QString& triggerWord)
{
	_triggerWord = QString(triggerWord);
}

int SpellCatalog::triggerWordLength()
{
	return _triggerWord.length();
}

void SpellCatalog::reset()
{
	_result.setText(QString::null);
}

#include "spellcatalog.moc"
