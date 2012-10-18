    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#include "drawutils.h"

QString DrawUtils::cropText(QPainter *p, QString text, int maxlen, int& textwidth)
{
	QString label = text;

	while(p->fontMetrics().width(label) > maxlen && label.length() > 0) {
		int i = label.find('_');

		if(i != -1)
			label = label.mid(i+1);
		else
			label = label.left(label.length() - 1);
	}

	textwidth = p->fontMetrics().width(label);
	return label;
}
