/***************************************************************************
                             domlistviewitem.cpp
                             -------------------

    author               : Andreas Schlapbach
    email                : schlpbch@iam.unibe.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "domlistviewitem.h"

#include <qpainter.h>
#include <qlistview.h>
#include <qapplication.h>

#include <kglobalsettings.h>

DOMListViewItem::DOMListViewItem( const DOM::Node &node, QListView *parent )
  : QListViewItem( parent ), m_node(node)
{
  init();
}

DOMListViewItem::DOMListViewItem( const DOM::Node &node, QListView *parent, QListViewItem *after)
  : QListViewItem( parent, after ), m_node(node)
{
  init();
}

DOMListViewItem::DOMListViewItem( const DOM::Node &node, QListViewItem *parent )
  : QListViewItem( parent ), m_node(node)
{
  init();
}

DOMListViewItem::DOMListViewItem( const DOM::Node &node, QListViewItem *parent, QListViewItem *after)
  : QListViewItem( parent, after ), m_node(node)
{
  init();
}

DOMListViewItem::~DOMListViewItem()
{
  //NOP
}

void DOMListViewItem::init()
{
  m_color = QApplication::palette().color( QPalette::Active, QColorGroup::Text );
  m_font = KGlobalSettings::generalFont();
  clos = false;
}

void DOMListViewItem::paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int alignment )
{
  QColorGroup _cg( cg );
  QColor c = _cg.text();

  p->setFont(m_font);
  _cg.setColor( QColorGroup::Text, m_color );
  QListViewItem::paintCell( p, _cg, column, width, alignment );
  _cg.setColor( QColorGroup::Text, c );
}


