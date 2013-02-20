/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
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
#include "cmdedit.h"

#include <qlistbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtoolbutton.h>
#include <klocale.h>
#include <kdialog.h>


CmdEdit::CmdEdit(QWidget* parent, const char* name)
       : QWidget(parent,name)
{
    QGridLayout* layout = new QGridLayout( this , 1 , 1 );
    layout->setSpacing( KDialog::spacingHint() );

    QLabel* nameLabel = new QLabel( i18n("Command &Label:"), this);
    QLabel* cmdLabel = new QLabel( i18n("Co&mmand:"), this);
    layout->addWidget( nameLabel, 0 , 0 );
    layout->addWidget( cmdLabel, 0 , 1 );

    _cmdNameEdit = new QLineEdit( this , "cmdNameEdit" );
    _cmdNameEdit->setMaxLength(20);
    nameLabel->setBuddy(_cmdNameEdit);
    layout->addWidget( _cmdNameEdit , 1 , 0 );

    _cmdEdit = new QLineEdit( this , "cmdEdit" );
    cmdLabel->setBuddy(_cmdEdit);
    layout->addWidget( _cmdEdit , 1 , 1 );


    _addButton = new QPushButton( i18n("&Add"), this );
    _addButton->setEnabled(false);
    layout->addWidget( _addButton , 1 , 2 );

    _editButton = new QPushButton( i18n("&Edit"), this );
    _editButton->setEnabled(false);
    layout->addWidget( _editButton , 3 , 2 );

    _removeButton = new QPushButton( i18n("&Remove"), this );
    _removeButton->setEnabled(false);
    layout->addWidget( _removeButton , 4 , 2 );

    QHBoxLayout* hbox = new QHBoxLayout();
    layout->addLayout(hbox,5,2);

    _upButton = new QToolButton(UpArrow,this);
    _upButton->setFixedSize(20,20);
    _upButton->setEnabled(false);
    hbox->addWidget( _upButton );

    _downButton = new QToolButton(DownArrow,this);
    _downButton->setFixedSize(20,20);
    _downButton->setEnabled(false);
    hbox->addWidget( _downButton);

    _commandNames = new QListBox( this , "commandNamesBox" );
    _commandNames->setMinimumSize(100, 100);
    layout->addMultiCellWidget( _commandNames , 3 , 6 , 0 , 0);

    _commands = new QListBox( this , "commandsBox" );
    _commands->setMinimumSize(160, 100);
    layout->addMultiCellWidget( _commands , 3 , 6 , 1 ,1 );


    layout->setColStretch(0,1);
    layout->setColStretch(1,2);
    layout->setColStretch(2,0);

    layout->addRowSpacing(2, KDialog::spacingHint());
    layout->addRowSpacing(6, KDialog::spacingHint());

    setMinimumSize(layout->sizeHint());


    connect(_addButton , SIGNAL(clicked()) , this , SLOT(addCmd()) ) ;
    connect(_editButton , SIGNAL(clicked()) , this , SLOT(editCmd()) );
    connect(_removeButton , SIGNAL(clicked()) , this , SLOT(removeCmd()) );
    connect(_upButton , SIGNAL(clicked()) , this , SLOT(upCmd()) ) ;
    connect(_downButton , SIGNAL(clicked()) , this , SLOT(downCmd()) );

    connect(_commands , SIGNAL(highlighted(int)) , this, SLOT(cmdHighlighted(int)) );
    connect(_commandNames , SIGNAL(highlighted(int)) , this, SLOT(cmdNameHighlighted(int)) );
    connect(_commands , SIGNAL(selected(int)) , this, SLOT(editCmd()) );
    connect(_commandNames , SIGNAL(selected(int)) , this, SLOT(editCmd()) );

    connect(_cmdEdit, SIGNAL(textChanged(const QString&)) , this , SLOT(checkAdd()) );
    connect(_cmdNameEdit, SIGNAL(textChanged(const QString&)) , this , SLOT(checkAdd()) );
}

void CmdEdit::setCommands(const QStringList& commands,const QStringList& commandNames)
{
   _commands->clear();
   _commands->insertStringList(commands);

   _commandNames->clear();
   _commandNames->insertStringList(commandNames);
}

void CmdEdit::commands(QStringList& commands, QStringList& commandNames)
{
   commands.clear();
   commandNames.clear();

   int items=_commands->count();

   int i=0;
   for( i=0 ; i< items ; i++)
   {
      commands.append( _commands->text(i) );
      commandNames.append( _commandNames->text(i) );
   }
}

void CmdEdit::addCmd()
{
   QString cmd = _cmdEdit->text();
   QString cmdName = _cmdNameEdit->text();
   _cmdEdit->clear();
   _cmdNameEdit->clear();

   if(_commands->currentText() == cmd || _commandNames->currentText() == cmdName)
   {
      int current = _commands->currentItem();
      _commands->changeItem(cmd,current);
      _commandNames->changeItem(cmdName,current);
   }
   else
   {
      _commands->insertItem(cmd);
      _commandNames->insertItem(cmdName);
   }
   
   emit widgetChanged();
}

void CmdEdit::removeCmd()
{
   int current=_commands->currentItem();

   _commands->removeItem(current);
   _commandNames->removeItem(current);

   if(_commands->count() == 0)
   {
      _removeButton->setEnabled(false);
      _editButton->setEnabled(false);
      _upButton->setEnabled(false);
      _downButton->setEnabled(false);
   }
   else
   {
      if(current > (int)_commands->count()-1)
         current=_commands->count()-1;

      _commands->setSelected(current,true);
      _commandNames->setSelected(current,true);
      cmdHighlighted(current);
   }

   emit widgetChanged();
}

void CmdEdit::upCmd()
{
   QString cmd = _commands->currentText();
   QString cmdName = _commandNames->currentText();
   int index=_commands->currentItem();

   _commands->removeItem(index);
   _commandNames->removeItem(index);

   _commands->insertItem(cmd , index-1);
   _commandNames->insertItem(cmdName , index-1);

   _commands->clearSelection();
   _commandNames->clearSelection();

   _commands->setSelected(index-1,true);
   _commandNames->setSelected(index-1,true);

   cmdHighlighted(index-1);

   emit widgetChanged();
}

void CmdEdit::downCmd()
{
   QString cmd = _commands->currentText();
   QString cmdName = _commandNames->currentText();
   int index=_commands->currentItem();

   _commands->removeItem(index);
   _commandNames->removeItem(index);

   _commands->insertItem(cmd , index+1);
   _commandNames->insertItem(cmdName , index+1);

   _commands->clearSelection();
   _commandNames->clearSelection();

   _commands->setSelected(index+1,true);
   _commandNames->setSelected(index+1,true);

   cmdHighlighted(index+1);

   emit widgetChanged();
}

void CmdEdit::cmdHighlighted(int index)
{
   _commandNames->blockSignals(true);
   _commandNames->setCurrentItem(index);
   _commandNames->blockSignals(false);

   _removeButton->setEnabled(true);
   _editButton->setEnabled(true);

   if(index == (int)(_commands->count()-1))
      _downButton->setEnabled(false);
   else
      _downButton->setEnabled(true);

   if(index==0)
      _upButton->setEnabled(false);
   else
      _upButton->setEnabled(true);

}

void CmdEdit::cmdNameHighlighted(int index)
{
   _commands->blockSignals(true);
   _commands->setCurrentItem(index);
   _commands->blockSignals(false);

   _removeButton->setEnabled(true);
   _editButton->setEnabled(true);

   if(index == (int)(_commands->count()-1))
      _downButton->setEnabled(false);
   else
      _downButton->setEnabled(true);

   if(index==0)
      _upButton->setEnabled(false);
   else
      _upButton->setEnabled(true);
}

void CmdEdit::editCmd()
{
   _cmdEdit->setText(_commands->currentText());
   _cmdNameEdit->setText(_commandNames->currentText());

   emit widgetChanged();
}

void CmdEdit::checkAdd()
{
   _addButton->setEnabled( !(_cmdEdit->text().isEmpty() || _cmdNameEdit->text().isEmpty()) );
}

#include "cmdedit.moc"
