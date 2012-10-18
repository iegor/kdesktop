/*
 * drivescombo.h
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef DRIVESCOMBO_H
#define DRIVESCOMBO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kcombobox.h>

/**
Let user choose the disc drive. Takes presets from /etc/fstab.

@author Juergen  Kofler
*/
class DrivesCombo : public KComboBox
{
public:
    DrivesCombo(QWidget* parent, const char* name = 0);
    virtual ~DrivesCombo();

};

#endif /* DRIVESCOMBO_H */