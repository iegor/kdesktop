/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include <kdebug.h>

#include "pageRangeWidget.h"


PageRangeWidget::PageRangeWidget( Q_UINT16 _from, Q_UINT16 _to, Q_UINT16 _current, QWidget *parent, const char *name) : PageRangeWidget_base(parent, name)
{
  // Paranoid security checks
  if ((from == 0) || (to == 0))
    return;
  if (_from > _to) {
    kdError() << "PageRangeWidget::PageRangeWidget(..): from > to" << endl;
    _to = _from;
  }
  if (_current < _from) {
    kdError() << "PageRangeWidget::PageRangeWidget(..): _current < _from" << endl;
    _current = _from;
  }
  if (_current > _to) {
    kdError() << "PageRangeWidget::PageRangeWidget(..): _current > _to" << endl;
    _current = _to;
  }

  connect(from, SIGNAL(valueChanged(int)), this, SLOT(fromValueChanged(int)));
  connect(to, SIGNAL(valueChanged(int)), this, SLOT(toValueChanged(int)));

  from->setRange(_from, _to);
  from->setValue(_current);
  to->setRange(_from, _to);
  to->setValue(_current);
}


void PageRangeWidget::toValueChanged(int val)
{
  if (val < from->value())
    from->setValue(val);
}


void PageRangeWidget::fromValueChanged(int val)
{
  if (val > to->value())
    to->setValue(val);
}

#include "pageRangeWidget.moc"

