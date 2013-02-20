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

#include <kparts/factory.h>
#include <kparts/part.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include "kgv_view.h"

#include "kgvfactory.h"

KGVFactory::KGVFactory()
{
    if ( s_self )
        kdWarning() << "KGVFactory instantiated more than once!" << endl;
    s_self = this;
}

KGVFactory::~KGVFactory()
{
    delete s_aboutData;
    delete s_instance;
    s_aboutData = 0;
    s_instance = 0;
    s_self = 0;
}

KInstance *KGVFactory::createInstance()
{
    KInstance* res = new KInstance( aboutData() );
    return res;
}

KGVFactory *KGVFactory::s_self;
KInstance *KGVFactory::s_instance;
KAboutData *KGVFactory::s_aboutData;

KParts::Part *KGVFactory::createPartObject( QWidget *parentWidget, const char *widgetName,
        QObject *parent, const char *name,
        const char *className,
        const QStringList &args_ )
{
    QStringList args = args_;
    /* Below is the reason why we must
     * have our own factory instead of
     * typedef KParts::GenericFactory<KGVPart> KGVFactory
     *
     * as we did before.
     */
    args << QString::fromLatin1( className );
    if ( !strcmp( className, "Browser/View" ) ) {
	    className = "KParts::ReadOnlyPart";
    }
    KGVPart *part = KDEPrivate::ConcreteFactory<KGVPart>::create( parentWidget,
            widgetName,
            parent,
            name,
            className,
            args );

    if ( part && !qstrcmp( className, "KParts::ReadOnlyPart" ) )
    {
        KParts::ReadWritePart *rwp = dynamic_cast<KParts::ReadWritePart *>( part );
        if ( rwp )
            rwp->setReadWrite( false );
    }
    return part;
}

KInstance *KGVFactory::instance()
{
    if ( !s_instance )
    {
        if ( s_self )
            s_instance = s_self->createInstance();
        else
            s_instance = new KInstance( aboutData() );
    }
    return s_instance;
}

KAboutData *KGVFactory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = KGVPart::createAboutData();
    return s_aboutData;
}

