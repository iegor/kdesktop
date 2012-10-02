/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002-2005	  by Stanislav Visnovsky
			    	<visnovsky@kde.org>

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


** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/


void KListEditor::addToList()
{
    _list->insertItem(_edit->text());
    _edit->clear();
    _removeButton->setEnabled(true);
    emit itemsChanged();
}

void KListEditor::downInList()
{
    int i=_list->currentItem();
    if( i< (int)_list->count()-1 ) {
	QString ci = _list->currentText();
	_list->removeItem(i);
	_list->insertItem(ci,i+1);
	_list->setCurrentItem(i+1);
    }
    emit itemsChanged();
}

void KListEditor::removeFromList()
{
    _list->removeItem(_list->currentItem());
    if( _list->count()==0 ) _edit->clear();
    _removeButton->setEnabled(_list->count()>0);
    emit itemsChanged();
}

void KListEditor::upInList()
{
    int i=_list->currentItem();
    if( i>0 ) {
	QString ci = _list->currentText();
	_list->removeItem(i);
	_list->insertItem(ci,i-1);
	_list->setCurrentItem(i-1);
    }
    emit itemsChanged();
}

void KListEditor::updateButtons( int newIndex )
{
    _upButton->setEnabled(newIndex>0);
    _downButton->setEnabled(newIndex+1 != (int)_list->count());
    _removeButton->setEnabled(true);
}

void KListEditor::updateList()
{
    int i=_list->currentItem();
    if( i==-1 ) addToList();
    else _list->changeItem(_edit->text(), i );
}

void KListEditor::setList( QStringList contents )
{
    _list->clear();
    _list->insertStringList(contents);
    _list->setCurrentItem(0);
    _removeButton->setEnabled(!contents.isEmpty());
}


void KListEditor::editChanged( const QString &s )
{
    _addButton->setEnabled(!s.isEmpty());
}


void KListEditor::setTitle( const QString &s )
{
    _frame->setTitle(s);
}


QStringList KListEditor::list()
{
    QStringList result;
    for( uint i=0; i<_list->count() ; i++ )
	result.append(_list->text(i));
    return result;
}
