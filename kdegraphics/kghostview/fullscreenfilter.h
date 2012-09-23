/**
 * Copyright (C) 2003, Luís Pedro Coelho
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef DB_FULLSCREENFILTER_H1055977622_INCLUDE_GUARD_
#define DB_FULLSCREENFILTER_H1055977622_INCLUDE_GUARD_

#include <qobject.h>
class KGVShell;

/* @class FullScreenFilter
 *
 * @short This class is to have in one place the special event all the special
 * key/mouse handling related to full-screen mode without bloating further KGVPart
 */
class FullScreenFilter : public QObject {
	Q_OBJECT
	public:
		FullScreenFilter( KGVShell& parent );

		/** 
		 * @reimplemented
		 */
		virtual bool eventFilter( QObject*, QEvent* );
	private:
		KGVShell& parent;
};



#endif /* DB_FULLSCREENFILTER_H1055977622_INCLUDE_GUARD_ */
