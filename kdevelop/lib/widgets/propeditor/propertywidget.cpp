/***************************************************************************
 *   Copyright (C) 2002-2004 by Alexander Dymo                             *
 *   cloudtemple@mskat.net                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "propertywidget.h"

#include <qpainter.h>

namespace PropertyLib{

PropertyWidget::PropertyWidget(MultiProperty *property, QWidget *parent, const char *name)
    :QWidget(parent, name), m_property(property)
{
}

QString PropertyWidget::propertyName() const
{
    return m_property->name();
}

void PropertyWidget::setProperty(MultiProperty *property)
{
    m_property = property;
}

void PropertyWidget::drawViewer(QPainter *p, const QColorGroup &cg, const QRect &r, const QVariant &value)
{
    p->setPen(Qt::NoPen);
    p->setBrush(cg.background());
    p->drawRect(r);
    p->drawText(r, Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, value.toString());
}

void PropertyWidget::setValueList(const QMap<QString, QVariant> &// valueList
                                  )
{
    //this does nothing
}

void PropertyWidget::undo()
{
    m_property->undo();
}

}

#ifndef PURE_QT
#include "propertywidget.moc"
#endif
