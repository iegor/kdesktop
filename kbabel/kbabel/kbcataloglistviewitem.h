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

#ifndef KBCATALOGLISTVIEWITEM_H
#define KBCATALOGLISTVIEWITEM_H
#include <memory>
#include <klistview.h>
#include <qsimplerichtext.h>
#include "mymultilineedit.h"

/**
@author 
*/
class KBCatalogListViewItem : public KListViewItem
{
    typedef KListViewItem Super;
public:
    KBCatalogListViewItem(KListView* lv, KListViewItem* parent, uint id, QString msgid, QString msgstr);

    ~KBCatalogListViewItem();
    
    void setMsgId(const QString& st);
    void setMsgStr(const QString& st);
    void setId(const uint id);
    uint getId();

protected:

    virtual void setup();
    virtual QString key ( int column, bool ascending ) const;
    
    virtual void paintCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align);
        

private:
    void makeDocAvailable();
    void paintMsgIdCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align);
    void paintMsgStrCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align);
    
    QString formatMsg(const QString str);
    
    uint m_id;
    QString m_msgid;
    QString m_msgstr;
    
    std::auto_ptr<QSimpleRichText> m_doc_msgid;        
    std::auto_ptr<QSimpleRichText> m_doc_msgstr;       
};

#endif
