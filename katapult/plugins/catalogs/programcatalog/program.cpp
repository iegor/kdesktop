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

#include <kservice.h> 
#include <kglobal.h>
#include <kiconloader.h>

#include "program.h"

Program::Program(KService::Ptr s, bool useExecName)
	: KatapultItem()
{
	if(useExecName)
		_name = s->exec();
	else
		_name = s->name();
	_icon = s->icon();
	if(_icon == "")
		_icon = s->name().lower();
	_service = s;
}

QPixmap Program::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon(_icon, KIcon::NoGroup, size);
}

QString Program::text() const
{
	return _name;
}

KService * Program::service() const
{
	return _service;
}

#include "program.moc"
