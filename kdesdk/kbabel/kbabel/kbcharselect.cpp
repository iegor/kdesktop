/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Stanislav Visnovsky
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

**************************************************************************** */
#include "kbcharselect.h"

#include <kconfig.h>
#include <kcharselect.h>
#include <kdialog.h>
#include <klocale.h>

#include <qlabel.h>
#include <qspinbox.h>
#include <qscrollview.h>

KBCharSelect::KBCharSelect(QWidget* parent,const char* name)
             : QVBox(parent,name)
{
    setSpacing( KDialog::spacingHint() );
    
    QHBox* bar = new QHBox(this);
    bar->setSpacing( KDialog::spacingHint() );
    
    QLabel *lTable = new QLabel( i18n( "Table:" ), bar );
    _tableNum = new QSpinBox( 0, 255, 1, bar );
    lTable->setBuddy( _tableNum );
    bar->setStretchFactor( _tableNum, 1 );
    
    QScrollView* scroll = new QScrollView( this );    
    _table = new KCharSelectTable(scroll,"charselector","helvetica",' ',0);
    _table->setNumCols(16);
    _table->setNumRows(16);
    
    scroll->addChild(_table);
    
    connect( _table, SIGNAL( doubleClicked() ), this, SLOT( emitChar() ) );
    connect( _tableNum, SIGNAL( valueChanged(int) ), this, SLOT( setTab(int) ));
}

void KBCharSelect::emitChar()
{
    emit characterDoubleClicked( _table->chr() );
}

void KBCharSelect::setTab(int value)
{
    _table->setTableNum( value );
}

void KBCharSelect::saveSettings(KConfig* config)
{
    KConfigGroupSaver saver(config, "KBCharSelector" );
	
    config->writeEntry( "TableNum", _tableNum->value() );
    config->writeEntry( "SelectedChar", QString(_table->chr()) );
}

void KBCharSelect::restoreSettings(KConfig* config)
{
    KConfigGroupSaver saver(config, "KBCharSelector" );
    
    _tableNum->setValue( config->readNumEntry("TableNum", 0 ));
    _table->setChar( config->readEntry("SelectedChar"," ").at(0));
}

#include "kbcharselect.moc"
