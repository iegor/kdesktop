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

#include <qobject.h>
 
#include "match.h"

Match::Match()
{
	this->_rank = 0;
	this->_item = 0;
	this->_matched = 0;
	this->_null = TRUE;
}

Match::Match(const KatapultItem *_item, int _rank, unsigned int _matched)
{
	this->_rank = _rank;
	this->_item = _item;
	this->_matched = _matched;
	this->_null = FALSE;
}

int Match::rank() const
{
	return _rank;
}

unsigned int Match::matched() const
{
	return _matched;
}

const KatapultItem * Match::item() const
{
	return _item;
}

bool Match::isNull() const
{
	return _null;
}
