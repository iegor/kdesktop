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
#ifndef MOZILLABOOKMARK_H
#define MOZILLABOOKMARK_H

#include <qpixmap.h>

#include "katapultitem.h"

/**
@author Joe Ferris
*/
class MozillaBookmark : public KatapultItem
{
	Q_OBJECT
public:
	MozillaBookmark(QString, QString, QPixmap);
	
	virtual QPixmap icon(int) const;
	virtual QString text() const;
	
	QString url() const;
	
private:
	QString _url, _title;
	QPixmap _icon;
};

#endif
