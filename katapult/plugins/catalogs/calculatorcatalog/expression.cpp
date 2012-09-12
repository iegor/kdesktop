/***************************************************************************
 *   Copyright (C) 2005  Tobi Vollebregt                                   *
 *   tobivollebregt@gmail.com                                              *
 *                                                                         *
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

#include <kservice.h> 
#include <kglobal.h>
#include <kiconloader.h>

#include "calculatorcatalog.h"
#include "expression.h"


Expression::Expression(CalculatorCatalog* catalog, const QString& text): KatapultItem(), _catalog(catalog), _text(text)
{
	evaluate();
}

QPixmap Expression::icon(int size) const
{
	const char* icon = "checkmark";
	if (_parseError) {
		icon = "no";
	}
	return KGlobal::iconLoader()->loadIcon(icon, KIcon::NoGroup, size);
}

QString Expression::text() const
{
	return _text;
}

void Expression::setText(const QString& text)
{
	_text = text;
	evaluate();
}

double Expression::result() const
{
	return _result;
}

bool Expression::parseError() const
{
	return _parseError;
}

CalculatorCatalog* Expression::catalog() const
{
	return _catalog;
}

void Expression::evaluate(bool assignments) const
{
	if (!_text.isEmpty()) {
		QString t = _text;
		CalculatorCatalog::ParserControl cntrl;
		cntrl.expression = t.replace(',', '.').ascii();
		cntrl.catalog = _catalog;
		cntrl.assignments = assignments;
		if (yyparse(&cntrl) == 0) {
			_result = cntrl.result;
			_parseError = false;
			if (assignments) {
				_catalog->setVar(_catalog->getVarID("ans"), _result);
			}
		} else {
			_parseError = true;
		}
	} else {
		_parseError = true;
	}
}

#include "expression.moc"
