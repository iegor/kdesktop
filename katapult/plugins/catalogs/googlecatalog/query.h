/***************************************************************************
 *   Copyright (C) 2007  Martin Meredith                                   *
 *   mez@ubuntu.com                                                        *
 *                                                                         *
 *   Copyright (C) 2006  Jonathan Riddell                                  *
 *   jriddell@ubuntu.com                                                   *
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
#ifndef QUERY_H
#define QUERY_H

#include <kspell.h>

#include <katapultitem.h>

class GoogleCatalog;

/**
@author Tobi Vollebregt
*/
class SearchQuery : public KatapultItem
{
	Q_OBJECT
	public:
		SearchQuery(GoogleCatalog*, const QString&);
		~SearchQuery();

		virtual QPixmap icon(int) const;
		virtual QString text() const;

		void setText(const QString&);

		GoogleCatalog* catalog() const;

	private:
	        GoogleCatalog* const _catalog;
		QString _text;


};

#endif
