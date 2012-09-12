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

#include <kservicegroup.h>
#include <ksycocaentry.h>
#include <ksycocatype.h>
#include <kapplication.h>
#include <knuminput.h>
#include <kcombobox.h>

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qregexp.h>

#include <math.h>

#include "settings.h"
#include "calculatorcatalog.h"
#include "expression.h"
#include "actionregistry.h"
#include "actionevalexpr.h"
#include "status.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E  2.7182818284590452354
#endif


static double frac(double x)
{
	double nowhere;
	return modf(x, &nowhere);
}


static double deg_sin(double x)   { return sin(x * (M_PI / 180.0)); }
static double deg_cos(double x)   { return cos(x * (M_PI / 180.0)); }
static double deg_tan(double x)   { return tan(x * (M_PI / 180.0)); }
static double deg_asin(double x)  { return asin(x) * (180.0 / M_PI); }
static double deg_acos(double x)  { return acos(x) * (180.0 / M_PI); }
static double deg_atan(double x)  { return atan(x) * (180.0 / M_PI); }
static double deg_sinh(double x)  { return sinh(x * (M_PI / 180.0)); }
static double deg_cosh(double x)  { return cosh(x * (M_PI / 180.0)); }
static double deg_tanh(double x)  { return tanh(x * (M_PI / 180.0)); }
static double deg_asinh(double x) { return asinh(x) * (180.0 / M_PI); }
static double deg_acosh(double x) { return acosh(x) * (180.0 / M_PI); }
static double deg_atanh(double x) { return atanh(x) * (180.0 / M_PI); }


const CalculatorCatalog::Function CalculatorCatalog::radiansFunctionTable[] =
{
	{ "sin",   3, sin   },
	{ "cos",   3, cos   },
	{ "tan",   3, tan   },
	{ "asin",  4, asin  },
	{ "acos",  4, acos  },
	{ "atan",  4, atan  },
	{ "sinh",  4, sinh  },
	{ "cosh",  4, cosh  },
	{ "tanh",  4, tanh  },
	{ "asinh", 5, asinh },
	{ "acosh", 5, acosh },
	{ "atanh", 5, atanh },
	{ "sqrt",  4, sqrt  },
	{ "log",   3, log10 },
	{ "ln",    2, log   },
	{ "exp",   3, exp   },
	{ "abs",   3, fabs  },
	{ "frac",  4, frac  },
	{ "round", 5, round },
	{ "int",   3, trunc },
	{ 0, 0, 0 }
};

const CalculatorCatalog::Function CalculatorCatalog::degreesFunctionTable[] =
{
	{ "sin",   3, deg_sin   },
	{ "cos",   3, deg_cos   },
	{ "tan",   3, deg_tan   },
	{ "asin",  4, deg_asin  },
	{ "acos",  4, deg_acos  },
	{ "atan",  4, deg_atan  },
	{ "sinh",  4, deg_sinh  },
	{ "cosh",  4, deg_cosh  },
	{ "tanh",  4, deg_tanh  },
	{ "asinh", 5, deg_asinh },
	{ "acosh", 5, deg_acosh },
	{ "atanh", 5, deg_atanh },
	{ "sqrt",  4, sqrt  },
	{ "log",   3, log10 },
	{ "ln",    2, log   },
	{ "exp",   3, exp   },
	{ "abs",   3, fabs  },
	{ "frac",  4, frac  },
	{ "round", 5, round },
	{ "int",   3, trunc },
	{ 0, 0, 0 }
};


K_EXPORT_COMPONENT_FACTORY( katapult_calculatorcatalog,
                            KGenericFactory<CalculatorCatalog>( "katapult_calculatorcatalog" ) )

CalculatorCatalog::CalculatorCatalog(QObject*, const char*, const QStringList&): _result(this, QString::null)
{
	ActionRegistry::self()->registerAction(new ActionEvaluateExpression());

	setVar(getVarID("pi"), M_PI);
	setVar(getVarID("e"), M_E);
}

CalculatorCatalog::~CalculatorCatalog()
{
}

void CalculatorCatalog::queryChanged()
{
	int newStatus = 0;
	QString cmd = query();

	if (cmd.isEmpty()) {
		reset();
		setBestMatch(Match());
	} else {
		if (accepts(cmd)) {
			int i, origLength = cmd.length(), length = origLength;
			//autocomplete functions
			cmd = cmd.lower();
			for (i = length - 1; i >= 0 && cmd[i].isLetter(); --i) { }
			if (i != length - 1) {
				QString start = cmd.mid(i + 1);
				int lengthOfShortest = 9999, shortest = -1;
				for (int j = 0; radiansFunctionTable[j].name; ++j) {
					if (QString(radiansFunctionTable[j].name).startsWith(start)) {
						if (radiansFunctionTable[j].length < lengthOfShortest) {
							lengthOfShortest = radiansFunctionTable[j].length;
							shortest = j;
						}
					}
				}
				if (shortest != -1) {
					cmd = cmd.left(i + 1).append(radiansFunctionTable[shortest].name).append("(");
					length = cmd.length();
				}
			}
			//fix parse errors at end of expression,
			//ie. close open parentheses, convert operators into NOPs
			for (i = length - 1; i >= 0 && (cmd[i] == '(' || cmd[i] == ' '); --i) { }
			if (i < 0 || cmd[i] == '+' || cmd[i] == '-') {
				cmd.append("0");
				++length;
			} else if (cmd[i] == '*' || cmd[i] == '/' || cmd[i] == '^') {
				cmd.append("1");
				++length;
			} else if (cmd[i].isLetter() && (i < length - 1 && cmd[i + 1] == '(')) {
				//just add a 0 if it's a function: we don't bother to backpropagate
				//through the parse tree (if it existed at all) to figure out a NOP value
				//for this particular (chain of) function(s).
				cmd.append("0");
				++length;
			}
			int openParen = 0;
			//use cmd.length() here, it may be > than length.
			for (i = 0; i < length; ++i) {
				if (cmd[i] == '(') ++openParen;
				if (cmd[i] == ')') --openParen;
			}
			if (openParen > 0) {
				char* closeParen = new char[openParen + 1];
				memset(closeParen, ')', openParen);
				closeParen[openParen] = 0;
				cmd.append(closeParen);
				delete[] closeParen;
			}
			_result.setText(cmd);
			setBestMatch(Match(&_result, _result.parseError() ? 10 : 100, origLength));
			//set status.
			//add S_Multiple to make sure katapult doesn't auto-exec and close the window
			//add S_Active to make sure katapult doesn't start the hideTimer or clearTimer
			newStatus = S_HasResults | S_Multiple | S_Active;
		} else {
			newStatus = 0;
		}
	}
	setStatus(newStatus);
}

void CalculatorCatalog::reset()
{
	_result.setText(QString::null);
}

bool CalculatorCatalog::accepts(const QString& str) const
{
	//Heuristic to determine whether the string is an expression or not.
	//Accept anything containing [()+\\-/*^=.,0-9].
	return QRegExp("[()+\\-/*^=.,0-9]").search(str) >= 0;
}

int CalculatorCatalog::getVarID(const char* name)
{
	VarNameToIdMap::iterator it = varNameToId.find(QString(name));
	if (it == varNameToId.end()) {
		_pendingVarName = QString(name);
		return -1;
	}
	return *it;
}

double CalculatorCatalog::getVar(int id) const
{
	return varIdToValue[id];
}

double CalculatorCatalog::setVar(int id, double value)
{
	if (id == -1) {
		id = varIdToValue.count();
		varNameToId.insert(_pendingVarName, id);
		varIdToValue.push_back(value);
	} else {
		varIdToValue[id] = value;
	}
	return value;
}

/*
void CalculatorCatalog::initialize()
{
}
*/

void CalculatorCatalog::readSettings(KConfigBase* config)
{
	_fracDigits = config->readUnsignedNumEntry("FracDigits", 2);
	_bScientific = config->readBoolEntry("Scientific", false);
	_bDegrees = config->readBoolEntry("Degrees", false);
	_bClipboard = config->readBoolEntry("Clipboard", true);
	_formatString = config->readEntry("FormatString", "%1 = %2");
}

void CalculatorCatalog::writeSettings(KConfigBase* config)
{
	config->writeEntry("FracDigits", fracDigits());
	config->writeEntry("Scientific", scientific());
	config->writeEntry("Degrees", degrees());
	config->writeEntry("Clipboard", clipboard());
	config->writeEntry("FormatString", formatString());
}

QWidget * CalculatorCatalog::configure()
{
	CalculatorCatalogSettings* settings = new CalculatorCatalogSettings();

	settings->fracDigits->setValue(_fracDigits);
	connect(settings->fracDigits, SIGNAL(valueChanged(int)), this, SLOT(fracDigitsChanged(int)));

	settings->normal->setChecked(!scientific());
	settings->scientific->setChecked(scientific());
	connect(settings->scientific, SIGNAL(toggled(bool)), this, SLOT(scientificChanged(bool)));

	settings->radians->setChecked(!degrees());
	settings->degrees->setChecked(degrees());
	connect(settings->degrees, SIGNAL(toggled(bool)), this, SLOT(degreesChanged(bool)));

	settings->clipboard->setChecked(clipboard());
	connect(settings->clipboard, SIGNAL(toggled(bool)), this, SLOT(clipboardChanged(bool)));

	settings->formatString->setText(formatString());
	connect(settings->formatString, SIGNAL(textChanged(const QString&)), this, SLOT(formatStringChanged(const QString&)));

	return settings;
}

void CalculatorCatalog::fracDigitsChanged(int n)
{
	_fracDigits = n;
}

int CalculatorCatalog::fracDigits() const
{
	return _fracDigits;
}

void CalculatorCatalog::scientificChanged(bool en)
{
	_bScientific = en;
}

bool CalculatorCatalog::scientific() const
{
	return _bScientific;
}

void CalculatorCatalog::degreesChanged(bool en)
{
	_bDegrees = en;
}

bool CalculatorCatalog::degrees() const
{
	return _bDegrees;
}

void CalculatorCatalog::formatStringChanged(const QString& fmt)
{
	_formatString = fmt;
}

QString CalculatorCatalog::formatString() const
{
	return _formatString;
}

void CalculatorCatalog::clipboardChanged(bool en)
{
	_bClipboard = en;
}

bool CalculatorCatalog::clipboard() const
{
	return _bClipboard;
}

const CalculatorCatalog::Function* CalculatorCatalog::functionTable() const
{
	if (degrees()) {
		return degreesFunctionTable;
	} else {
		return radiansFunctionTable;
	}
}

#include "calculatorcatalog.moc"
