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
#ifndef DICTIONARYMENU_H
#define DICTIONARYMENU_H

#include <qintdict.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qguardedptr.h>
#include <kdemacros.h>

class QSignalMapper;
class KPopupMenu;
class KActionCollection;

class KDE_EXPORT DictionaryMenu : public QObject
{
   Q_OBJECT

public:
   DictionaryMenu(KPopupMenu *menu, KActionCollection *collection, QObject *parent=0);
   ~DictionaryMenu();
   
   void add(const QString& name,const QString& id);

   /**
    * Adds an item to this menu and to the keyboard accelerator.
    * If key contains %1 it is replaced with a number 1-9 
    */
   void add(const QString& name,const QString& id, const QString& key);

   void clear();
   
signals:
   void activated(const QString moduleId);
   
private slots:
   void activated(int);
   
private:
   QGuardedPtr<KPopupMenu> popup;
   QGuardedPtr<KActionCollection> actionCollection;
   QIntDict<QString> num2id;
   QIntDict<QString> accel2id;
   QSignalMapper* dictionaryMapper;
   
   uint maxId;
};

#endif

