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

#include <kapplication.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <krun.h>
#include <kurl.h>
#include <klocale.h>

#include <qclipboard.h>

#include "calculatorcatalog.h"
#include "expression.h"
#include "katapultitem.h"
#include "actionevalexpr.h"


// Conditional compilation to not bloat the lib if
// KDE already has the following functions.
// Prevents a deprecated function warning too.

#if !KDE_IS_VERSION(3, 5, 0)

// Copied some code from KDE 3.5 to make it compile on 3.4.

// insert (thousands)-"separator"s into the non-fractional part of str
static void _insertSeparator(QString &str, const QString &separator, const QString &decimalSymbol)
{
	// leave fractional part untouched
	QString mainPart = str.section(decimalSymbol, 0, 0);
	QString fracPart = str.section(decimalSymbol, 1, 1, QString::SectionIncludeLeadingSep);
	if (fracPart==decimalSymbol) fracPart=QString();
	for (int pos = mainPart.length() - 3; pos > 0; pos -= 3)
		mainPart.insert(pos, separator);
	str = mainPart + fracPart;
}

// This was KLocale::formatNumber(const QString&, bool, int)

static QString formatNumber(const QString &numStr)
{
	QString tmpString = numStr;

	// Skip the sign (for now)
	bool neg = (tmpString[0] == '-');
	if (neg  ||  tmpString[0] == '+') tmpString.remove(0, 1);

	// Split off exponential part (including 'e'-symbol)
	QString mantString = tmpString.section('e', 0, 0, QString::SectionCaseInsensitiveSeps);
	QString expString = tmpString.section('e', 1, 1, QString::SectionCaseInsensitiveSeps | QString::SectionIncludeLeadingSep);
	if (expString.length()==1) expString=QString();

	// Replace dot with locale decimal separator
	mantString.replace(QChar('.'), KGlobal::locale()->decimalSymbol());

	// Insert the thousand separators
	_insertSeparator(mantString, KGlobal::locale()->thousandsSeparator(), KGlobal::locale()->decimalSymbol());

	// How can we know where we should put the sign?
	mantString.prepend(neg?KGlobal::locale()->negativeSign():KGlobal::locale()->positiveSign());

	return mantString +  expString;
}

#else
// KDE_VERSION >= 3.5.0

static QString formatNumber(const QString& numStr)
{
	return KGlobal::locale()->formatNumber(numStr, false, 0);
}

#endif


ActionEvaluateExpression::ActionEvaluateExpression()
	: KatapultAction(), _expr(0)
{
}

ActionEvaluateExpression::~ActionEvaluateExpression()
{
}

QString ActionEvaluateExpression::text() const
{
	if (_expr->parseError()) {
		return i18n("Evaluate Expression");
	} else {
		// Format result.
		int digits = _expr->catalog()->fracDigits();
		QChar f = _expr->catalog()->scientific() ? 'g' : 'f';
		QString num = QString::number(_expr->result(), f, digits);
		// Strip trailing zeroes.
		if (f == 'f' && digits != 0) {
			while (num.endsWith("0")) {
				num = num.left(num.length() - 1);
			}
			if (num.endsWith(".")) {
				num = num.left(num.length() - 1);
			}
		}
		// Localize (choose right implementation based on KDE version, see above).
		return formatNumber(num);
	}
}

QPixmap ActionEvaluateExpression::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("xcalc", KIcon::NoGroup, size);
}

bool ActionEvaluateExpression::accepts(const KatapultItem* item) const
{
	bool accept = strcmp(item->className(), "Expression") == 0;
	if (accept) {
		_expr = (const Expression*)item;
	}
	return accept;
}

void ActionEvaluateExpression::execute(const KatapultItem* item) const
{
	if (strcmp(item->className(), "Expression") == 0) {
		_expr = (const Expression*)item;

		//evaluate expression with assignments enabled
		_expr->evaluate(true);

		// Copy calculation and result into clipboard (unless there's a parse error).
		if (!_expr->parseError()) {
			QClipboard *cb = QApplication::clipboard();
			QString s = _expr->catalog()->formatString();
			s.replace("%1", _expr->text());
			s.replace("%2", text());
			cb->setText(s, QClipboard::Clipboard);
			cb->setText(s, QClipboard::Selection);
		}
	}
}
