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
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <katapultitem.h>

class CalculatorCatalog;

/**
@author Tobi Vollebregt
*/
class Expression : public KatapultItem
{
	Q_OBJECT
	public:
		Expression(CalculatorCatalog*, const QString&);

		virtual QPixmap icon(int) const;
		virtual QString text() const;

		void setText(const QString&);
		double result() const;
		bool parseError() const;

		//evaluate() must be const, or ActionEvaluateExpression::execute() can't call it.
		//It makes sense because evaluate() does _not_ change the expression,
		//it just calculates the result and remembers that.
		void evaluate(bool assignments = false) const;

		CalculatorCatalog* catalog() const;

	private:
		CalculatorCatalog* const _catalog;
		QString _text;
		mutable double _result;
		mutable bool _parseError;
};

#endif
