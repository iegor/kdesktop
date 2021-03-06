/*
    This file is part of libkexif project
    Copyright (c) 2005 Laurent Montel <montel@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef _LIBKEXIF_EXPORT_H
#define _LIBKEXIF_EXPORT_H

#ifdef KDEMACROS_USABLE
#include <kdemacros.h>
#endif

#ifdef KDE_EXPORT
#define LIBKSIRTET_EXPORT KDE_EXPORT
#else
#define LIBKSIRTET_EXPORT
#endif

#endif /* _LIBKEXIF_EXPORT_H */

