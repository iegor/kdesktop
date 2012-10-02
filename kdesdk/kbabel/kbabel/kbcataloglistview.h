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

#ifndef KBCATALOGLISTVIEW_H
#define KBCATALOGLISTVIEW_H

#include <qwidget.h>
#include <catalogview.h>
#include <kconfig.h>
#include <qwidget.h>
//#include <qstrlist.h>
#include <resources.h>
#include "catalog.h"
#include "kbcatalog.h"
#include "kbproject.h"
#include "projectsettings.h"
#include "kbcataloglistviewitem.h"

namespace KBabel{
    class EditCommand;
}

class KBabelMW;
class KBCatalog;
class KListView;


/**
@author 
*/
class KBCatalogListView : public QWidget, public KBabel::CatalogView 
{
    Q_OBJECT
public:
    KBCatalogListView(KBCatalog* catalog, QWidget *parent, KBabel::Project::Ptr project);

    ~KBCatalogListView();
    
    /**
    * this is called from the catalog when updating his views.
    * reimplemented from @ref CatalogView
    * @param cmd the edit command that has been applied
    */
    virtual void update(KBabel::EditCommand* cmd, bool undo=false);    
    
    void setSelectedItem(int index);
signals:
    void signalSelectionChanged(const KBabel::DocPosition& pos);    
    
public slots:
    virtual void slotNewFileOpened();
    void msgstrChanged(const QString&);
        
private:
    KListView * m_listview;
    KBCatalog* m_catalog;
    QPtrVector<KBCatalogListViewItem>* m_items;

private slots:
    void selectionChanged ( QListViewItem * item);
};

#endif
