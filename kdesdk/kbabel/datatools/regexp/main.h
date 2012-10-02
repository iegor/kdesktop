/*
	Copyright (C) 2005	Albert Cervera i Areny <albertca at hotpop dot com>

	Based on Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
		    2002 	Stanislav Visnovsky <visnovsky@kde.org>
			2003	Dwayne Bailey <dwayne@translate.org.za>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public License
	along with this library; see the file COPYING.LIB.  If not, write to
	the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA 02111-1307, USA.
*/

#ifndef __main_h__
#define __main_h__

#include <kdatatool.h>
#include <qvaluelist.h>

class QDomElement;

class Expression
{
public:
	Expression() {};
	Expression( const QString& name, const QRegExp& regExp )
	{
		_name = name;
		_regExp = regExp;
	}
	const QString& name() const
	{
		return _name;
	}
	const QRegExp& regExp() const
	{
		return _regExp;
	}

private:
	QString _name;
	QRegExp _regExp;
};

typedef QValueList<Expression> ExpressionList;

class RegExpTool : public KDataTool
{
	Q_OBJECT

public:
	RegExpTool( QObject* parent, const char* name, const QStringList & );
	virtual bool run( const QString& command, void* data, const QString& datatype, const QString& mimetype);
	
private:
	void loadExpressions();
	void elementToExpression( const QDomElement& e );

	ExpressionList _list;
	QString _error;
};

#endif
