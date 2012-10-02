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
#ifndef CMDEDIT_H
#define CMDEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <qstringlist.h>

class QListBox;
class QLineEdit;
class QPushButton;
class QToolButton;

class CmdEdit : public QWidget
{
   Q_OBJECT
public:
   CmdEdit(QWidget* parent=0,const char* name=0);

   void setCommands(const QStringList& commands,const QStringList& commandNames);
   void commands(QStringList& commands, QStringList& commandNames);
   
signals:
   void widgetChanged();

private slots:
   /**
   * reads command and commandName from the line edits and
   * inserts them at the end of the listbox
   */
   void addCmd();
   /**
   * removes the currently selected command and commandName from the listbox
   */
   void removeCmd();

   void upCmd();
   void downCmd();

   void editCmd();
   void cmdHighlighted(int index);
   void cmdNameHighlighted(int index);

   void checkAdd();

private:
   QListBox* _commands;
   QListBox* _commandNames;

   QLineEdit* _cmdEdit;
   QLineEdit* _cmdNameEdit;

   QPushButton* _addButton;
   QPushButton* _editButton;
   QPushButton* _removeButton;
   QToolButton* _upButton;
   QToolButton* _downButton;
};

#endif // CMDEDIT_H
