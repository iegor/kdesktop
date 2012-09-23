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
#ifndef SPELLCATALOG_H
#define SPELLCATALOG_H

#include <kgenericfactory.h>

#include <qstring.h>

#include "spelling.h"
#include "katapultcatalog.h"

class QWidget;

/**
@author Jonathan Riddell
 */
class SpellCatalog : public KatapultCatalog
{
	Q_OBJECT

	public:

		SpellCatalog(QObject*, const char*, const QStringList&);
		virtual ~SpellCatalog();

		virtual void readSettings(KConfigBase*);
		virtual void writeSettings(KConfigBase*);
		virtual QWidget* configure();
		int triggerWordLength();

	protected:

		virtual void queryChanged();

	 private:
		bool accepts(const QString&) const;

		QString _triggerWord;

		Spelling _result; // The one result (there's always one).

		void reset();

        protected slots:
		void triggerWordChanged(const QString& triggerWord);

};

#endif
