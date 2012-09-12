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
#ifndef CALCULATORCATALOG_H
#define CALCULATORCATALOG_H

#include <kgenericfactory.h>

#include <qmap.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qvaluevector.h>

#include "expression.h"
#include "katapultcatalog.h"

class QWidget;

typedef double (*FunPtr)(double);

/**
@author Tobi Vollebregt
 */
class CalculatorCatalog : public KatapultCatalog
{
	Q_OBJECT

	public:

		struct ParserControl {
			const char* expression;
			CalculatorCatalog* catalog;
			bool assignments; //are assignments enabled?
			double result;
		};

		struct Function {
			const char* name;
			int length;
			FunPtr fptr;
		};

		CalculatorCatalog(QObject*, const char*, const QStringList&);
		virtual ~CalculatorCatalog();

		//virtual void initialize();
		virtual void readSettings(KConfigBase*);
		virtual void writeSettings(KConfigBase*);
		virtual QWidget* configure();

		int getVarID(const char*);
		double getVar(int) const;
		double setVar(int, double);

		int fracDigits() const;
		bool scientific() const;
		bool degrees() const;
		bool clipboard() const;
		QString formatString() const;
		const Function* functionTable() const;

	protected:

		virtual void queryChanged();

	private:

		typedef QMap<QString, int> VarNameToIdMap;
		typedef QValueVector<double> VarIdToValueVector;

		static const Function radiansFunctionTable[];
		static const Function degreesFunctionTable[];

		void reset();
		bool accepts(const QString&) const;

		Expression _result; // The one result (there's always one).

		VarNameToIdMap varNameToId;      // Maps strings to IDs.
		VarIdToValueVector varIdToValue; // Maps IDs to values.
		QString _pendingVarName; // Pending while rest of assignment is parsed.

		int _fracDigits;   // Number of fractional digits.
		bool _bScientific; // Normal or scientific mode?
		bool _bDegrees;    // Radians or degrees?
		bool _bClipboard;  // Copy to clipboard?
		QString _formatString; // for clipboard copy

	private slots:

		void fracDigitsChanged(int);
		void scientificChanged(bool);
		void degreesChanged(bool);
		void clipboardChanged(bool);
		void formatStringChanged(const QString&);

};


/* from parser.y / parser.cpp: */
int yyparse(CalculatorCatalog::ParserControl*);

#endif
