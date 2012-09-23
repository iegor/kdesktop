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

#include "toolselectionwidget.h"

#include <kdatatool.h>
#include <kdebug.h>

#include <qlistbox.h>

ToolSelectionWidget::ToolSelectionWidget( QWidget * parent, const char * name )
    : KActionSelector( parent, name )
{
}

void ToolSelectionWidget::loadTools( const QStringList &commands, 
    const QValueList<KDataToolInfo>& tools)
{
    if ( tools.isEmpty() ) return;
    
    _allTools = tools;
	
    QValueList<KDataToolInfo>::ConstIterator entry = tools.begin();
    for( ; entry != tools.end(); ++entry )
    {
	QStringList userCommands = (*entry).userCommands();
	QStringList toolCommands = (*entry).commands();
	Q_ASSERT(!toolCommands.isEmpty());
	if ( toolCommands.count() != userCommands.count() )
	        kdWarning() << "KDataTool desktop file error (" << (*entry).service()
	        << "). " << toolCommands.count() << " commands and "
	        << userCommands.count() << " descriptions." << endl;
	
	QStringList::ConstIterator uit = userCommands.begin();
	QStringList::ConstIterator cit = toolCommands.begin();
	for (; uit != userCommands.end() && cit != toolCommands.end(); ++uit, ++cit )
	{
	    if( commands.contains(*cit) )
	    {
		availableListBox()->insertItem( *uit );
	    }
        }
    }
}

void ToolSelectionWidget::setSelectedTools( const QStringList& tools )
{
    availableListBox()->clear();
    selectedListBox()->clear();
    QValueList<KDataToolInfo>::ConstIterator entry = _allTools.begin();
    for( ; entry != _allTools.end(); ++entry )
    {
	QString uic=*(*entry).userCommands().at((*entry).commands().findIndex("validate"));
	if( tools.contains((*entry).service()->library()) )
	    selectedListBox()->insertItem( uic );
	else
	    availableListBox()->insertItem( uic );
    }
}

QStringList ToolSelectionWidget::selectedTools()
{
    QStringList usedNames;
    for( uint i=0; i<selectedListBox()->count() ; i++ )
	usedNames += selectedListBox()->text(i);
	
    QStringList result;
    QValueList<KDataToolInfo>::ConstIterator entry = _allTools.begin();
    for( ; entry != _allTools.end(); ++entry )
    {
	if( usedNames.contains(*((*entry).userCommands().at((*entry).commands().findIndex("validate")))) )
	    result += (*entry).service()->library();
    }
    return result;
}

#include "toolselectionwidget.moc"
