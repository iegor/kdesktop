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

#include "toolaction.h"

#include <kdebug.h>

ToolAction::ToolAction( const QString & text, const KShortcut& cut, const KDataToolInfo & info, const QString & command,
                                    QObject * parent, const char * name )
  : KAction( text, info.iconName() == "unknown" ? QString::null : info.iconName(), cut, parent, name ),
      m_command( command ),
      m_info( info )
{
}

void ToolAction::slotActivated()
{
    emit toolActivated( m_info, m_command );
}

QPtrList<KAction> ToolAction::dataToolActionList( const QValueList<KDataToolInfo> & tools, const QObject *receiver, const char* slot, const QStringList& command, bool excludeCommand, KActionCollection* parent, const QString& namePrefix )
{
    QPtrList<KAction> actionList;
    if ( tools.isEmpty() )
	return actionList;
	
    QValueList<KDataToolInfo>::ConstIterator entry = tools.begin();
    for( ; entry != tools.end(); ++entry )
    {
	QStringList userCommands = (*entry).userCommands();
	QStringList commands = (*entry).commands();
	QStringList shortcuts = (*entry).service()->property("Shortcuts").toStringList();
	Q_ASSERT(!commands.isEmpty());
	if ( commands.count() != userCommands.count() )
	        kdWarning() << "KDataTool desktop file error (" << (*entry).service()
	        << "). " << commands.count() << " commands and "
	        << userCommands.count() << " descriptions." << endl;
	
	QStringList::ConstIterator uit = userCommands.begin();
	QStringList::ConstIterator cit = commands.begin();
	QStringList::ConstIterator sit = shortcuts.begin();
	for (; uit != userCommands.end() && cit != commands.end(); ++uit, ++cit)
	{
	    if( !excludeCommand == command.contains(*cit) )
	    {
		QString sc=*sit;

		ToolAction * action = new ToolAction( *uit, (sc.isEmpty()?QString::null:sc), *entry, *cit
		    , parent
		    , QString(namePrefix+(*entry).service()->library()+"_"+(*cit)).utf8() );
		connect( action, SIGNAL( toolActivated( const KDataToolInfo &, const QString & ) ),
                     receiver, slot );
		
        	actionList.append( action );
	    }
	    if( sit != shortcuts.end() ) sit++;
        }
    }

    return actionList;
}

QValueList<KDataToolInfo> ToolAction::validationTools()
{
    QValueList<KDataToolInfo> result;
    
    QValueList<KDataToolInfo> tools = KDataToolInfo::query("CatalogItem", "application/x-kbabel-catalogitem", KGlobal::instance());
    
    for( QValueList<KDataToolInfo>::ConstIterator entry = tools.begin(); entry != tools.end(); ++entry )
    {
	if( (*entry).commands().contains("validate") ) 
	{
	    result.append( *entry );
	}
    }
    
    return result;
}

#include "toolaction.moc"
