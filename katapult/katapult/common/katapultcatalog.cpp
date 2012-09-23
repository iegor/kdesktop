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

#include "katapultcatalog.moc"
#include "katapultcatalog.h"
#include "match.h"

KatapultCatalog::KatapultCatalog()
{
	_bestMatch = Match();
	_status = 0;
	_query = "";
}


KatapultCatalog::~KatapultCatalog()
{
}

void KatapultCatalog::queryChanged()
{
}

void KatapultCatalog::initialize()
{
}

void KatapultCatalog::setQuery(QString _query)
{
	if(this->_query != _query) {
		this->_query = _query;
		queryChanged();
	}
}

void KatapultCatalog::setStatus(int _status)
{
	this->_status = _status;
}

void KatapultCatalog::setBestMatch(Match _bestMatch)
{
	this->_bestMatch = _bestMatch;
}

QString KatapultCatalog::query() const
{
	return _query;
}

int KatapultCatalog::status() const
{
	return _status;
}

Match KatapultCatalog::bestMatch() const
{
	return _bestMatch;
}

void KatapultCatalog::readSettings(KConfigBase *)
{
}

void KatapultCatalog::writeSettings(KConfigBase *)
{
}

QWidget * KatapultCatalog::configure()
{
	return 0;
}

