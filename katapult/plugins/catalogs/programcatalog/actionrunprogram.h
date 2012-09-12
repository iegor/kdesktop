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
#ifndef ACTIONRUNPROGRAM_H
#define ACTIONRUNPROGRAM_H

#include "katapultaction.h"

class KatapultItem;

/**
@author Joe Ferris
*/
class ActionRunProgram : public KatapultAction
{

public:
	ActionRunProgram();
	~ActionRunProgram();
	
	virtual void execute(const KatapultItem *) const;
	virtual bool accepts(const KatapultItem *) const;
	virtual QString text() const;
	virtual QPixmap icon(int) const;

};

#endif
