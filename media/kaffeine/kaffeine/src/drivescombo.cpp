/*
 * drivescombo.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "drivescombo.h"

#include <kdebug.h>

#include <qfile.h>

DrivesCombo::DrivesCombo(QWidget* parent, const char* name)
 : KComboBox(parent, name)
{
  setEditable(true);

  QFile file("/etc/fstab");
  if (file.open(IO_ReadOnly))
  {
    QTextStream stream(&file);
    QString line;
    QStringList drives;

    while (!stream.eof())
    {
      line = stream.readLine();
      if (line.contains("cdfss"))
      {
        kdDebug() << "DrivesCombo: found disc drive: " << line.section(' ', 0, 0) << endl;
	drives.append(line.section(' ', 0, 0));
      }
    }
    insertStringList(drives);
  }
  file.close();
}


DrivesCombo::~DrivesCombo()
{
}
