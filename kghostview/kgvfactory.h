/**
 * Copyright (C) 2003, Luís Pedro Coelho,
 * based on kdelibs/kparts/genericfactory.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KGVPart_H_INCLUDE_GUARD_
#define KGVPart_H_INCLUDE_GUARD_

#include <kparts/factory.h>
#include <kparts/part.h>

class KInstance;
class KAboutData;

class KDE_EXPORT KGVFactory : public KParts::Factory
{
    public:
        KGVFactory();
        virtual ~KGVFactory();
        static KInstance *instance();
        static KAboutData *aboutData();

        virtual KParts::Part *createPartObject( QWidget *parentWidget, const char *widgetName,
                QObject *parent, const char *name,
                const char *className,
                const QStringList &args );

    protected:
        virtual KInstance *createInstance();
    private:
        static KGVFactory*s_self;
        static KInstance *s_instance;
        static KAboutData *s_aboutData;
};

#endif

