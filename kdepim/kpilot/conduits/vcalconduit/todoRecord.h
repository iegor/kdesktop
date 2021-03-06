#ifndef _KPILOT_TODORECORD_H
#define _KPILOT_TODORECORD_H
/*
** Copyright (C) 2006 by Adriaan de Groot <groot@kde.org>
** Copyright (C) 2002-2003 by Reinhold Kainhofer
** Copyright (C) 2001 by Dan Pilone
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-pim@kde.org
*/

namespace KCal
{
	class Todo;
}

class PilotTodoEntry;

namespace KCalSync
{
	bool setTodo(KCal::Todo *e,
		const PilotTodoEntry *de,
		const CategoryAppInfo &info);
	bool setTodoEntry(PilotTodoEntry *de,
		const KCal::Todo *e,
		const CategoryAppInfo &info);
}

#endif

