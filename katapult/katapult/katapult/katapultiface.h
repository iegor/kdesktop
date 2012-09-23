/***************************************************************************
 *   Copyright (C) 2005 by Bastian Holst                                   *
 *   bastianholst@gmx.de                                                   *
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
// -*- c++ -*-

#ifndef KATAPULTDCOPIFACE_H
#define KATAPULTDCOPIFACE_H

#include <dcopobject.h>

class KatapultIface : virtual public DCOPObject
{
	K_DCOP
	k_dcop:
	
	virtual void showLauncher()  = 0;
	virtual void hideLauncher() = 0;
	virtual void setQuery(QString)  = 0;
	virtual void execute() = 0;
};

#endif // KATAPULTDCOPIFACE_H
