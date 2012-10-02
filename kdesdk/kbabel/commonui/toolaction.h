/* This file is part of KBabel
   Copyright (C) 2002 Stanislav Visnovsky <visnovsky@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

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

*/

#ifndef TOOLACTION_H
#define TOOLACTION_H

#include <qobject.h>
#include <kaction.h>
#include <kdatatool.h>

class KShortcut;
class KActionCollection;

class KDE_EXPORT ToolAction : public KAction
{
    Q_OBJECT
public:
    ToolAction( const QString & text, const KShortcut& cut, const KDataToolInfo & info, const QString & command, QObject * parent = 0, const char * name = 0);

    /**
    * return the list of KActions created for a list of tools. @ref command
    * allows to specify rescriction of commands, for which the list should
    * or shouldn't be created according to the @ref excludeCommand flag.
    */
    static QPtrList<KAction> dataToolActionList( const QValueList<KDataToolInfo> & tools, const QObject *receiver, const char* slot, const QStringList& command, bool excludeCommand, KActionCollection* parent=0, const QString& namePrefix=QString::null );
    
    /**
    * returns information about all available validation tools (KDataTools with support for CatalogItem
    * and the "validate" command.
    */
    static QValueList<KDataToolInfo> validationTools();

signals:
    void toolActivated( const KDataToolInfo & info, const QString & command );

protected:
    virtual void slotActivated();

private:
    QString m_command;
    KDataToolInfo m_info;
};

#endif
