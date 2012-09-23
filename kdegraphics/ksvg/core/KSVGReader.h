/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

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
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KSVGReader_H
#define KSVGReader_H

#include <qxml.h>
#include <qobject.h>

namespace KSVG
{

class SVGDocumentImpl;
class KSVGReader : public QObject
{
Q_OBJECT
public:
	struct ParsingArgs
	{
		bool fit;
		bool getURLMode;

		QString SVGFragmentId;
	};

	KSVGReader(SVGDocumentImpl *doc, KSVGCanvas *canvas, ParsingArgs args);
	virtual ~KSVGReader();

	void parse(QXmlInputSource *source);
	void finishParsing(bool, const QString &);

signals:
	void finished(bool, const QString &);

protected:
	friend class Helper;

	SVGDocumentImpl *doc();
	KSVGCanvas *canvas();

	void setFinished(bool error, const QString &errorDesc = 0);

private:
	struct Private;
	Private *d;
};

}

#endif
