/* This file is part of the KDE project
 * Copyright (C) 2001, 2002 Rolf Magnus <ramagnus@kde.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __KFILE_MP3_H__
#define __KFILE_MP3_H__

#include <kfilemetainfo.h>

class QStringList;

class KMp3Plugin: public KFilePlugin
{
    Q_OBJECT

public:
    KMp3Plugin(QObject *parent, const char *name, const QStringList &args);

    virtual bool readInfo( KFileMetaInfo& info, uint what );
    virtual bool writeInfo( const KFileMetaInfo& info) const;
    virtual QValidator *createValidator(const QString &mimetype,
                                        const QString &group,
                                        const QString &key,
                                        QObject *parent, const char *name) const;
};

#endif
