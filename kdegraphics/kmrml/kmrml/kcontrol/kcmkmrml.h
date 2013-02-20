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

#ifndef KCMKMRML_H
#define KCMKMRML_H

#include <kcmodule.h>

class KAboutData;
class KURLRequester;

namespace KMrmlConfig
{
    class MainPage;

    class KCMKMrml : public KCModule
    {
        Q_OBJECT
    
    public:
        KCMKMrml(QWidget *parent, const char *name, const QStringList &);
        virtual ~KCMKMrml();

        virtual void defaults();
        virtual void load();
        virtual void save();
        virtual QString quickHelp() const;

    private:
        void checkGiftInstallation();

        MainPage *m_mainPage;
    };

}

#endif
