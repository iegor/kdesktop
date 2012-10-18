/*
 * playlistimport.h
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

#ifndef PLAYLISTIMPORT_H
#define PLAYLISTIMPORT_H

#include "mrl.h"

class QWidget;

/*
 * static methods for playlist import
 */

class KDE_EXPORT PlaylistImport
{
public:

   static bool kaffeine(const QString&, QValueList<MRL>&);
   static bool noatun(const QString&, QValueList<MRL>&);
   static bool m3u(const QString&, QValueList<MRL>&);
   static bool pls(const QString&, QValueList<MRL>&);
   static bool ram(const MRL&, QValueList<MRL>&, QWidget*);
   static bool asx(const QString&, QValueList<MRL>&);
   static bool smil(const QString&, const MRL&, QValueList<MRL>&);
   /* helper */
   static QTime stringToTime(const QString&);

private:
   static uint extractIndex(const QString&);
};

#endif /* PLAYLISTIMPORT_H */
