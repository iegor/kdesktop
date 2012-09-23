/*
 *   This file is part of ScalixAdmin.
 *
 *   Copyright (C) 2007 Trolltech ASA. All rights reserved.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef OTHERUSERMANAGER_H
#define OTHERUSERMANAGER_H

#include <qobject.h>
#include <qstringlist.h>

class OtherUserManager : public QObject
{
  Q_OBJECT

  public:
    OtherUserManager();
    ~OtherUserManager();

    void addOtherUser( const QString &email );
    void clear();

    QStringList otherUsers() const;

  signals:
    void changed();

  private:
    QStringList mOtherUsers;
};

#endif