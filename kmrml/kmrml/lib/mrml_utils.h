/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef MRML_UTILS_H
#define MRML_UTILS_H

#include <qobject.h>

#include <kurl.h>

#include "kmrml_config.h"

namespace KMrml
{
    class Util : public QObject
    {
    public:
        static Util * self();
        ~Util();

        bool requiresLocalServerFor( const KURL& url );
        bool startLocalServer( const Config& config );
        void unrequireLocalServer();
//         bool isLocalServerRunning();

    private:
        static Util *s_self;
        Util();
    };


}

#endif // MRML_UTILS_H
