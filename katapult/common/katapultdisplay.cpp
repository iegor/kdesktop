/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "katapultdisplay.moc"

#include "katapultdisplay.h"

KatapultDisplay::KatapultDisplay(const char *name, WFlags f)
	: QWidget(0, name, f)
{
	_query = "";
	_item = 0;
	_action = 0;
	_status = 0;
	_selectionLength = 0;
}

KatapultDisplay::~KatapultDisplay()
{
}

void KatapultDisplay::focusOutEvent(QFocusEvent *)
{
	emit focusOut();
}

void KatapultDisplay::keyPressEvent(QKeyEvent *e)
{
	emit keyReleased(e);
}

// Deprecated - added for backwards compatibility
void KatapultDisplay::keyReleaseEvent(QKeyEvent *e)
{
}

void KatapultDisplay::setQuery(QString _query)
{
	this->_query = _query;
}

void KatapultDisplay::setSelected(unsigned int length)
{
	_selectionLength = length;
}

unsigned int KatapultDisplay::selected() const
{
	return _selectionLength;
}

void KatapultDisplay::setItem(const KatapultItem *_item)
{
	this->_item = _item;
}

void KatapultDisplay::setAction(const KatapultAction *_action)
{
	this->_action = _action;
}

void KatapultDisplay::setStatus(int _status)
{
	this->_status = _status;
}

QString KatapultDisplay::query() const
{
	return _query;
}

const KatapultItem * KatapultDisplay::item() const
{
	return _item;
}

const KatapultAction * KatapultDisplay::action() const
{
	return _action;
}

int KatapultDisplay::status() const
{
	return _status;
}

void KatapultDisplay::readSettings(KConfigBase *)
{
}

void KatapultDisplay::writeSettings(KConfigBase *)
{
}

QWidget * KatapultDisplay::configure()
{
	return 0;
}

