/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2004  by Stanislav Visnovsky
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

#include "charselectview.h"

#include <qlayout.h>
#include <qwhatsthis.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qscrollview.h>

#include <kconfig.h>
#include <kcursor.h>
#include <kdialog.h>
#include <klocale.h>
#include <kcharselect.h>

#include "kbcatalog.h"

using namespace KBabel;

CharacterSelectorView::CharacterSelectorView(KBCatalog* catalog,QWidget *parent, Project::Ptr project)
    : KBCatalogView(catalog,parent,project)
{
    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setResizeMode( QLayout::Minimum );

    layout->setSpacing( KDialog::spacingHint() );
    
    QHBox* bar = new QHBox(this);
    bar->setSpacing( KDialog::spacingHint() );
    layout->addWidget (bar);
    
    QLabel *lTable = new QLabel( i18n( "Table:" ), bar );
    _tableNum = new QSpinBox( 0, 255, 1, bar );
    lTable->setBuddy( _tableNum );
    bar->setStretchFactor( _tableNum, 1 );
    
    QScrollView* scroll = new QScrollView( this );    
    _table = new KCharSelectTable(scroll,"charselector","helvetica",' ',0);
    _table->setNumCols(16);
    _table->setNumRows(16);
    
    scroll->addChild(_table);
    layout->addWidget (scroll);
    
    connect( _table, SIGNAL( doubleClicked() ), this, SLOT( emitChar() ) );
    connect( _tableNum, SIGNAL( valueChanged(int) ), this, SLOT( setTab(int) ));
    
    connect( _catalog, SIGNAL( signalFileOpened(bool) ), this, SLOT (setDisabled (bool)));
    connect( _catalog, SIGNAL( signalFileOpened(bool) ), _table, SLOT (setDisabled (bool)));

    QWhatsThis::add(this,
       i18n("<qt><p><b>Character Selector</b></p>"
         "<p>This tool allows to insert special characters using "
         "double click.</p></qt>"));
}

void CharacterSelectorView::emitChar()
{
    emit characterDoubleClicked( _table->chr() );
}

void CharacterSelectorView::setTab(int value)
{
    _table->setTableNum( value );
}

void CharacterSelectorView::saveSettings(KConfig* config)
{
    KConfigGroupSaver saver(config, "KBCharSelector" );
	
    config->writeEntry( "TableNum", _tableNum->value() );
    config->writeEntry( "SelectedChar", QString(_table->chr()) );
}

void CharacterSelectorView::restoreSettings(KConfig* config)
{
    KConfigGroupSaver saver(config, "KBCharSelector" );
    
    _tableNum->setValue( config->readNumEntry("TableNum", 0 ));
    _table->setChar( config->readEntry("SelectedChar"," ").at(0));
}

#include "charselectview.moc"
