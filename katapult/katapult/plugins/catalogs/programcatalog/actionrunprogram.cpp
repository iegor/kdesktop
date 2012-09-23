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

#include <kglobal.h>
#include <kiconloader.h>
#include <krun.h>
#include <kurl.h>
#include <klocale.h>

#include "program.h"
#include "katapultitem.h"
#include "actionrunprogram.h"

ActionRunProgram::ActionRunProgram()
 : KatapultAction()
{
}


ActionRunProgram::~ActionRunProgram()
{
}

QString ActionRunProgram::text() const
{
	return i18n("Run Program");
}

QPixmap ActionRunProgram::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("exec", KIcon::NoGroup, size);
}

void ActionRunProgram::execute(const KatapultItem *item) const
{
	if(strcmp(item->className(), "Program") == 0) {
		const Program *program = (const Program *) item;
		KService *service = program->service();
		if(service != 0)
			KRun::run(*service, KURL::List());
		//TODO: report error running program
	}
}

bool ActionRunProgram::accepts(const KatapultItem *item) const
{
	return strcmp(item->className(), "Program") == 0;
}
