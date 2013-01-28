/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This function reads the content of a M$-Windoze .lnk file             *
 *   and returns data in the given structure.                              *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef _READ_LNK_H_
#define _READ_LNK_H_

#include <qstring.h>
class KURL;

struct LNKInfo
{
  LNKInfo() : fileSize(0), isNetworkPath(false), isDirectory(false), isFileOrDir(false) {}

  Q_UINT32 fileSize;
  bool isNetworkPath;
  bool isDirectory;
  bool isFileOrDir;
  QString volumeName;
  QString driveName;
  QString path;
  QString description;
};

bool readLNK(const KURL &url, LNKInfo &info);

#endif
