/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
	  
**************************************************************************** */


#include <klocale.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kdebug.h>

#include "pa_factory.h"
#include "poauxiliary.h"


extern "C"
{
    KDE_EXPORT void *init_kbabeldict_poauxiliary()
    {
        return new PaFactory;
    }
}


KInstance *PaFactory::s_instance = 0;
KAboutData *PaFactory::s_about = 0;


PaFactory::PaFactory( QObject *parent, const char *name)
        : KLibFactory(parent,name)
{
}

PaFactory::~PaFactory()
{
    if(s_instance)
    {
        delete s_instance;
        s_instance=0;
    }

    if(s_about)
    {
        delete s_about;
        s_about=0;
    }
}


QObject *PaFactory::createObject( QObject *parent, const char *name
                , const char *classname, const QStringList &)
{
    if(QCString(classname) != "SearchEngine")
    {
        kdError() << "not a SearchEngine requested" << endl;
        return 0;
    }
    
    return new PoAuxiliary(parent,name);
}


KInstance *PaFactory::instance()
{
    if(!s_instance)
    {
        s_about = new KAboutData( "poauxiliary", I18N_NOOP("PO Auxiliary")
                , "1.0"
                , I18N_NOOP("A simple module for exact searching in a PO file")
                , KAboutData::License_GPL
                , "Copyright 2000, Matthias Kiefer"
                ,0,0, "kiefer@kde.org");

        s_about->addAuthor("Matthias Kiefer",0,"kiefer@kde.org");
        
        s_instance = new KInstance(s_about);
    }

    return s_instance;
}

#include "pa_factory.moc"
