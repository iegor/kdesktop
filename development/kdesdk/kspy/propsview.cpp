/***************************************************************************
                          propsview.cpp  -  description
                             -------------------
    begin                : Tue May 1 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qobjectdict.h>
#include <qobjectlist.h>
#include <qmetaobject.h>
#include <qstrlist.h>
#include <qvariant.h>
#include <qcursor.h>

#include <klocale.h>

#include "propsview.h"

class KSpyItem : KListViewItem 
{
public:
  KSpyItem( QListView * parent, QString label1, QString label2 = QString::null, QString label3 = QString::null, QString label4 = QString::null, QString label5 = QString::null, QString label6 = QString::null )  
    : KListViewItem(parent, label1, label2, label3, label4, label5, label6)
  {
  }
protected:
  void paintCell( QPainter * p, const QColorGroup & cg,
                            int column, int width, int alignment )
  {
    if (column == 1 && text(2) == "QColor") {
      QColorGroup color_cg( cg.foreground(), cg.background(), 
                            cg.light(), cg.dark(), cg.mid(), 
                            QColor(text(1)), QColor(text(1)) );
      QListViewItem::paintCell(p, color_cg, column, width, alignment);
    } else {
      KListViewItem::paintCell(p, cg, column, width, alignment);
    }
  }
};

PropsView::PropsView(QWidget *parent, const char *name ) : KListView(parent,name)
{
  addColumn( i18n( "Name" ) );
  addColumn( i18n( "Value" ) );
  addColumn( i18n( "Type" ) );
  addColumn( i18n( "Access" ) );
  addColumn( i18n( "Designable" ) );
  addColumn( i18n( "Type Flags" ) );

  setAllColumnsShowFocus( true );
  setColumnAlignment( 3, AlignCenter );
  setColumnAlignment( 4, AlignCenter );
  setFullWidth( true );
}

PropsView::~PropsView()
{
}

void PropsView::buildList( QObject *o )
{
  QMetaObject *mo = o->metaObject();
  QStrList names = mo->propertyNames( true );

  for ( uint i = 0; i < names.count(); i++ ) {
    char *prop = names.at( i );
    QVariant v = o->property( prop );
    const QMetaProperty *mp = mo->property( mo->findProperty(prop, true), true );

    QString val( "????" );
    switch( v.type() ) {
       case QVariant::String:
       case QVariant::CString:
         val = v.toString();
         break;
       case QVariant::Bool:
         val = ( v.toBool() ? "True" : "False" );
         break;
       case QVariant::Color:
       {
         QColor c = v.toColor();
         val = c.name();
         break;
       }
       case QVariant::Cursor:
       {
         QCursor c = v.toCursor();
         val = QString("shape=%1").arg(c.shape());
         break;
       }
       case QVariant::Font:
       {
         QFont f = v.toFont();
         val = QString("family=%1, pointSize=%2, weight=%3, italic=%4, bold=%5, underline=%6, strikeOut=%7")
                       .arg(f.family())
                       .arg(f.pointSize())
                       .arg(f.weight())
                       .arg(f.italic())
                       .arg(f.bold())
                       .arg(f.underline())
                       .arg(f.strikeOut());
         break;
       }
       case QVariant::Int:
         val.setNum( v.toInt() );
         if (mp->isEnumType()) {
           QMetaObject * metaObject = *(mp->meta);
           val = QString("%1::%2").arg(metaObject->className()).arg(mp->valueToKey(val.toInt()));
         }
         break;
       case QVariant::Point:
       {
         QPoint p = v.toPoint();
         val = QString("x=%1, y=%2").arg(p.x()).arg(p.y());
         break;
       }
       case QVariant::Rect:
       {
         QRect r = v.toRect();
         val = QString("left=%1, right=%2, top=%3, bottom=%4")
                       .arg(r.left())
                       .arg(r.right())
                       .arg(r.top())
                       .arg(r.bottom());
         break;
       }
       case QVariant::Size:
       {
         QSize s = v.toSize();
         val = QString("width=%1, height=%2").arg(s.width()).arg(s.height());
         break;
       }
       case QVariant::SizePolicy:
       {
         QSizePolicy s = v.toSizePolicy();
         val = QString("horData=%1, verData=%2").arg(s.horData()).arg(s.verData());
         break;
       }
       case QVariant::Double:
         val.setNum( v.toDouble() );
         break;
       default:
         break;
    }

    QString ro("R/O");
    QString rw("R/W");
    QString st("Set");
    QString et("Enum");
    QString yes("Yes");
    QString no("No");

    QString writable = ( mp->writable() ? rw : ro );
    QString setType = ( mp->isSetType() ? st : QString::null );
    QString enumType = ( mp->isEnumType() ? et : QString::null );
    QString designable = ( mp->designable(o) ? yes : no );

    QString flags;
    bool first = true;
    if ( !setType.isNull() ) {
      if ( first )
        first = false;
      else
        flags += " | ";

      flags += setType;
    }
    if ( !enumType.isNull() ) {
      if ( first )
        first = false;
      else
        flags += " | ";

      flags += enumType;
    }

    new KSpyItem( this, prop, val, v.typeName(), writable, designable, flags );
  }
}

void PropsView::setTarget( QObject *o )
{
  clear();
  buildList( o );
}
#include "propsview.moc"
