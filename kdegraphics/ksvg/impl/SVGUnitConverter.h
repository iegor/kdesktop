/*
    Copyright (C) 2003 KSVG Team
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

#ifndef SVGUnitConverter_H
#define SVGUnitConverter_H

#include <qptrdict.h>

#include <dom/dom_string.h>

#include "SVGUnitTypes.h"
#include "SVGShapeImpl.h"
#include "SVGAnimatedLengthImpl.h"

namespace KSVG
{

class SVGUnitConverter
{
public:
	SVGUnitConverter() { m_dict.setAutoDelete(true); }
	~SVGUnitConverter() { }

	void add(SVGAnimatedLengthImpl *obj)
	{
		UnitData *data = new UnitData();
		data->valueAsString = QString::null;

		m_dict.insert(obj, data);
	}

	void modify(SVGAnimatedLengthImpl *obj, const QString &value)
	{
		UnitData *data = m_dict.find(obj);

		if(data)
			data->valueAsString = value;
	}

	void finalize(SVGShapeImpl *bboxContext, SVGShapeImpl *userContext, unsigned short unitType)
	{
		bool user = (unitType == SVGUnitTypes::SVG_UNIT_TYPE_USERSPACEONUSE);
		bool bbox = (unitType == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);

		if(!user && !bbox) // Invalid unit type
			return;

		QPtrDictIterator<UnitData> it(m_dict);
		for(; it.current(); ++it)
		{
			UnitData *data = it.current();

			if(!data)
				continue;

			SVGAnimatedLengthImpl *obj = static_cast<SVGAnimatedLengthImpl *>(it.currentKey());

			if(bbox)
				obj->baseVal()->setBBoxContext(bboxContext);
			else
				obj->baseVal()->setBBoxContext(userContext);

			if(user) // Just assign value, no conversion needed!
				obj->baseVal()->setValueAsString(data->valueAsString);
			else // Convert to objectBoundingBox
				obj->baseVal()->setValueAsString(SVGLengthImpl::convertValToPercentage(data->valueAsString));
		}
	}

private:
	typedef struct
	{
		QString valueAsString; // Original value
	} UnitData;

	QPtrDict<UnitData> m_dict;
};

}

#endif

// vim:ts=4:noet
