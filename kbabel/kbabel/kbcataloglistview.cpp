/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2004 by Asgeir Frimannsson
                            <asgeirf@redhat.com>

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

**************************************************************************** */

#include "kbcataloglistview.h"
#include "kbcataloglistviewitem.h"
#include "kbcatalog.h"
#include "kbabel.h"
#include "editcmd.h"

#include <klocale.h>
#include <klistview.h>
#include <qcolor.h>
#include <qlayout.h>
#include <kglobalsettings.h>

using namespace KBabel;

KBCatalogListView::KBCatalogListView(KBCatalog* catalog, QWidget *parent, KBabel::Project::Ptr project)
 : QWidget(parent)
{
    m_catalog= catalog;
    QVBoxLayout* layout=new QVBoxLayout(this);
    
    m_listview = new KListView(this, "catalogListView");
    m_listview->addColumn(i18n("Id"));
    m_listview->addColumn(i18n("Original String"));
    m_listview->addColumn(i18n("Translated String"));
    m_listview->setAlternateBackground(KGlobalSettings::alternateBackgroundColor());
    m_listview->setFullWidth(true);
    m_listview->setAllColumnsShowFocus(true);
    m_listview->resize(this->size());

    layout->addWidget(m_listview);
    layout->setStretchFactor(m_listview,1);
    
    connect(m_listview,SIGNAL(selectionChanged(QListViewItem *)), this,SLOT(selectionChanged(QListViewItem *)));
}

KBCatalogListView::~KBCatalogListView()
{
}


void KBCatalogListView::selectionChanged ( QListViewItem * item)
{
    DocPosition pos;
    int number = m_items->find(reinterpret_cast<KBCatalogListViewItem*>(item));
    if(number<0) number = 0;
    
    pos.item=number;
    pos.form=0;
    
    emit signalSelectionChanged(pos);
}

void KBCatalogListView::setSelectedItem(int index)
{
    QListViewItem * item = m_items->at(index);

    // block signals - don't reemit the selected item signal
    blockSignals(true);
    m_listview->setSelected(item, true);
    blockSignals(false);
    
    m_listview->ensureItemVisible(item);    
}

void KBCatalogListView::update(EditCommand* cmd, bool undo)
{
    /*
   if((int)_currentIndex==cmd->index())
   {
      emitEntryState();

      if(cmd->part()==Msgstr)
      {
         msgstrEdit->processCommand(cmd,undo);
	 emit signalMsgstrChanged();
      }
   }
   */
}

void KBCatalogListView::msgstrChanged(const QString& str)
{
    KBCatalogListViewItem * item = reinterpret_cast<KBCatalogListViewItem *>(m_listview->selectedItem());
    item->setMsgStr(str);
}

void KBCatalogListView::slotNewFileOpened()
{
    m_listview->clear();
    KBCatalogListViewItem * tempItem;
    if(m_catalog)
    {
        m_items = new QPtrVector<KBCatalogListViewItem>(m_catalog->numberOfEntries());
        
        for(uint i=0;i<m_catalog->numberOfEntries();i++)
        {
            QString msgid = ( *m_catalog->msgid(i).at(0) );
            QString msgstr = ( *m_catalog->msgstr(i).at(0) );
            tempItem = new KBCatalogListViewItem(m_listview,0,i+1,
                                            msgid,
                                            msgstr);
           m_items->insert(i, tempItem);   
        }
    }
    
    int width = m_listview->columnWidth(1) + m_listview->columnWidth(2);
    m_listview->setColumnWidth(1,width/2);
    m_listview->setColumnWidth(2,width/2);


}


#include "kbcataloglistview.moc"
