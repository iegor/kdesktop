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

#include "kbcataloglistviewitem.h"
#include <assert.h>

KBCatalogListViewItem::KBCatalogListViewItem(KListView* lv, KListViewItem* parent,     uint id, QString msgid, QString msgstr)
 : Super(lv, parent, "","",""), m_id(id), m_msgid(msgid), m_msgstr(msgstr)
{
    setText(0,QString::number(id));  
}


KBCatalogListViewItem::~KBCatalogListViewItem()
{
}

void KBCatalogListViewItem::setMsgId(const QString& st)
{
    m_msgid = st;
    setup();
    repaint();
}

void KBCatalogListViewItem::setMsgStr(const QString& st)
{
    m_msgstr = st;
    setup();
    repaint();
}

uint KBCatalogListViewItem::getId()
{
    return m_id;
}

void KBCatalogListViewItem::setId(const uint id)
{
    m_id = id;
    setup();
    repaint();
}

QString KBCatalogListViewItem::key ( int column, bool ascending ) const{
    
    if(column==0)
        return QString().sprintf("%.8u", m_id);
    else if(column==1)
        return m_msgid;
    else if(column==2)
        return m_msgstr;
    else
        return Super::key(column, ascending);
}


void KBCatalogListViewItem::setup()
{
    assert(listView());

    widthChanged();

    m_doc_msgid.reset(0);
    m_doc_msgstr.reset(0);
    
    makeDocAvailable();

    QListView* lv = listView();

    m_doc_msgid->setWidth(std::max(1, int(lv->columnWidth(1))));
    m_doc_msgstr->setWidth(std::max(1, int(lv->columnWidth(2))));
    
    if(m_doc_msgid->height() > m_doc_msgstr->height())
        setHeight(m_doc_msgid->height());
    else
        setHeight(m_doc_msgstr->height());
    
}


void KBCatalogListViewItem::paintMsgIdCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align)
{
    // check width
    
    int widthPrepared = m_doc_msgid->width();
    
    if (width != widthPrepared)
        m_doc_msgid->setWidth(p, std::max(1, int(width)));
    
    // check height
    
    if (height() < m_doc_msgid->height() )
    {
        // invalid height, don't draw
    
        setHeight(m_doc_msgid->height());
        return;
    }
    
    // draw appropriate background
    
    Super::paintCell(p, cg, column, width, align);
    
    // draw it
    
    m_doc_msgid->draw(p, 0, 0, QRect(0,0, width, height()), cg);

}

void KBCatalogListViewItem::paintMsgStrCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align)
{
    // check width
    
    int widthPrepared = m_doc_msgstr->width();
    
    if (width != widthPrepared)
        m_doc_msgstr->setWidth(p, std::max(1, int(width)));
    
    // check height
    
    if (height() < m_doc_msgstr->height() )
    {
        // invalid height, don't draw
    
        setHeight(m_doc_msgstr->height());
        return;
    }
    else if(height() > m_doc_msgstr->height() && height() > m_doc_msgid->height())
    {
        if(m_doc_msgstr->height() > m_doc_msgid->height())
            setHeight(m_doc_msgstr->height());
        else
            setHeight(m_doc_msgid->height());
            
        return;
    } 
    
    // draw appropriate background
    
    Super::paintCell(p, cg, column, width, align);
    
    // draw it
    
    m_doc_msgstr->draw(p, 0, 0, QRect(0,0, width, height()), cg);

}
        
void KBCatalogListViewItem::paintCell(QPainter* p, const QColorGroup& cg,
        int column, int width, int align)
{
     assert(m_doc_msgid.get() && m_doc_msgstr.get() && p);
        
    // paint background (empty text)
    
    
    switch(column)
    {
        case 0: // id
            Super::paintCell(p, cg, column, width, align);
            return;
        case 1: // msgid
            paintMsgIdCell(p,cg,column,width,align);
            return;
        case 2: // msgstr        
            paintMsgStrCell(p,cg,column,width,align);
            return;
    }
}
 
void KBCatalogListViewItem::makeDocAvailable()
{
    if (m_doc_msgid.get() && m_doc_msgstr.get())
        return;

    assert(listView());
    
    m_doc_msgid.reset(new QSimpleRichText(
            formatMsg(m_msgid), listView()->font()));
    m_doc_msgstr.reset(new QSimpleRichText(
            formatMsg(m_msgstr), listView()->font()));
}

QString KBCatalogListViewItem::formatMsg(const QString str)
{
    // TODO: Use KBHighlighting for this
    QString tmp_msgid = str;
    tmp_msgid = tmp_msgid.replace( "\\n", "<br>" );
    tmp_msgid = tmp_msgid.replace( "<", "&lt;" );
    tmp_msgid = tmp_msgid.replace( ">", "&gt;" );

    return tmp_msgid;
}
