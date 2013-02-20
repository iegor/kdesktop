/***************************************************************************
                              domlistviewitem.h
                             -------------------

    author               : Andreas Schlapbach
    email                : schlpbch@iam.unibe.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DOMLISTVIEWITEMS_H
#define DOMLISTVIEWITEMS_H
 
#include <dom/dom_node.h>

#include <qlistview.h>
#include <qcolor.h>
#include <qfont.h>

class DOMListViewItem : public QListViewItem
{

 public:
   DOMListViewItem( const DOM::Node &, QListView *parent );
   DOMListViewItem( const DOM::Node &, QListView *parent, QListViewItem *after ); 
   DOMListViewItem( const DOM::Node &, QListViewItem *parent );
   DOMListViewItem( const DOM::Node &, QListViewItem *parent, QListViewItem *after );
   virtual ~DOMListViewItem();
 
  virtual void paintCell( QPainter *p, const QColorGroup &cg,
			  int column, int width, int alignment );  

  void setColor( const QColor &color) { m_color = color; }

  void setFont( const QFont &font) { m_font = font;}
  void setItalic( bool b) {m_font.setItalic(b);}
  void setBold( bool b) {m_font.setBold(b);}
  void setUnderline(bool b) {m_font.setUnderline(b);}
  
  bool isClosing() const { return clos; }
  void setClosing(bool s) { clos = s; }

  DOM::Node node() const { return m_node; }

 private:
  void init();
  QColor m_color;
  QFont m_font;
  DOM::Node m_node;
  bool clos;
};
#endif
