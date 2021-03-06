/* This file is part of the KDE project
 * Copyright (C) 2002 Wilco Greven <greven@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *  $Id: kfile_ps.h 465369 2005-09-29 14:33:08Z mueller $
 */

#ifndef __KFILE_PS_H__
#define __KFILE_PS_H__

#include <kfilemetainfo.h>

#include "dscparse_adapter.h"

class QStringList;

class KPSPlugin: public KFilePlugin, public KDSCCommentHandler
{
    Q_OBJECT
public:
    KPSPlugin( QObject *parent, const char *name,
                const QStringList& preferredItems );
    
    virtual bool readInfo( KFileMetaInfo& info, uint what);

    void comment( Name );
    void makeMimeTypeInfo(  const char* mimeType );
    
private:
    KFileMetaInfo _info;
    KFileMetaInfoGroup _group;
    KDSC* _dsc;
    bool _endComments;
    int _setData;
};

#endif
