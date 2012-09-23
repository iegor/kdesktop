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
#include "dictionarymenu.h"
#include <kpopupmenu.h>
#include <kaction.h>
#include <kdebug.h>
#include <kshortcut.h>

#include <qsignalmapper.h>

DictionaryMenu::DictionaryMenu(KPopupMenu *popupMenu, KActionCollection *collection
        , QObject *parent)
              : QObject(parent, "dictionarymenu")
              , popup(popupMenu)
              , actionCollection(collection)
	      , maxId(10)
{
   num2id.setAutoDelete(true);
   accel2id.setAutoDelete(true);

   if(popup)
   {
      connect(popup,SIGNAL(activated(int)),this,SLOT(activated(int)));
   }
   
   dictionaryMapper = new QSignalMapper( this );
   connect( dictionaryMapper, SIGNAL( mapped( int ) ),
            this, SLOT( activated( int ) ) );

}

DictionaryMenu::~DictionaryMenu()
{
   clear();
}

void DictionaryMenu::clear()
{
   if(popup)
   {
      QIntDictIterator<QString> it( num2id ); // iterator for dict
      while ( it.current() )
      {
         popup->removeItem(it.currentKey());
         ++it;
      }
   }

   num2id.clear();
   
   // create new mapper
   delete dictionaryMapper;
   dictionaryMapper = new QSignalMapper( this );
   connect( dictionaryMapper, SIGNAL( mapped( int ) ),
            this, SLOT( activated( int ) ) );
}

void DictionaryMenu::add(const QString& name, const QString& moduleId)
{
   if(popup)
   {
      KAction* dictionaryAction = new KAction( name, 0, dictionaryMapper, SLOT(map()), actionCollection, moduleId.utf8() );
      
      uint id = maxId++;
      dictionaryAction->plug(popup, id);

      dictionaryMapper->setMapping( dictionaryAction, id );
   
      QString *idString = new QString(moduleId);
      num2id.insert(id,idString);
   }
   
}

void DictionaryMenu::add(const QString& n, const QString& moduleId
           , const QString& key)
{
    if(popup)
    {
        QString name=n;
	
        QString keyString=key;
        if(keyString.contains("%1"))
        {
            keyString=key.arg(accel2id.count()+1);
    	}
        KShortcut k(keyString);

    	KAction* dictionaryAction = new KAction( name, k, dictionaryMapper, SLOT(map()), actionCollection, key.arg(moduleId).utf8() );
	uint id = maxId++;
        dictionaryAction->plug(popup,id);

        if(!k.isNull())
        {
            QString *idString = new QString(moduleId);
            accel2id.insert(id,idString);

	    dictionaryMapper->setMapping( dictionaryAction, id );
	    
            name+='\t';
            name+=k.toString();
        }
        else
        {
            kdWarning() << "key not valid" << endl;
        }   
   
        QString *idString = new QString(moduleId);
        num2id.insert(id,idString);
   }
}


void DictionaryMenu::activated(int id)
{
   QString *idString = num2id[id];
   
   if(idString)
   {
      emit activated(*idString);
   }
}

#include "dictionarymenu.moc"
