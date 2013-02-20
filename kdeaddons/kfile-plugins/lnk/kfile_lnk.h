/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This plugin provides information about the content of a               *
 *   M$-Windoze .lnk file, which is useful if you work in a mixed          *
 *   Linux/Windoze environment.                                            *
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

#ifndef __KFILE_LNK_H__
#define __KFILE_LNK_H__

/**
 * Note: For further information look into <$KDEDIR/include/kfilemetainfo.h>
 */
#include <kfilemetainfo.h>

class QStringList;

class lnkPlugin: public KFilePlugin
{
    Q_OBJECT
    
public:
    lnkPlugin( QObject *parent, const char *name, const QStringList& args );
    
    virtual bool readInfo( KFileMetaInfo& info, uint what);
};

#endif // __KFILE_LNK_H__

