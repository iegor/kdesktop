/*
    Copyright (C) 2003 Nikolas Zimmermann <wildfox@kde.org>
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

#ifndef T2P_BEZIERPATH_AGG_H
#define T2P_BEZIERPATH_AGG_H

#include <agg_curves.h>
#include <agg_conv_curve.h>
#include <agg_conv_stroke.h>
#include <agg_path_storage.h>
#include <agg_conv_transform.h>

#include "Affine.h"
#include "BezierPath.h"

typedef agg::conv_curve<agg::path_storage> curved;
typedef agg::conv_transform<curved> curved_trans;

namespace T2P
{
	class BezierPathAgg : public BezierPath
	{
	public:
		BezierPathAgg() : BezierPath(),
		m_curved(m_storage), m_curved_trans(m_curved, m_transform)
		{
			m_length = -1;
		}

		~BezierPathAgg() {}

		BezierPathAgg(const BezierPathAgg &other) : BezierPath(), m_storage(other.m_storage),
		 m_curved(m_storage), m_curved_trans(m_curved, m_transform)
		{
			m_transform = other.m_transform;
			m_length = other.m_length;
		}

		void copy_from(const agg::path_storage &ps, Affine &affine)
		{
		        for(unsigned i = 0; i < ps.total_vertices(); i++)
			{
				double x, y;
				unsigned cmd = ps.vertex(i, &x, &y);
				m_storage.add_vertex(affine.dx() + x * affine.m11() + y * affine.m21(),
								   affine.dy() + x * affine.m12() + y * affine.m22(), cmd);
			}
		}

		unsigned operator [](unsigned) { return 0; }

		virtual double length(double t = 1.0);
		virtual void pointTangentNormalAt(double t, Point *p = 0, Point *tn = 0, Point *n = 0);
		virtual void boundingBox(Point *topLeft, Point *bottomRight);

		agg::path_storage		m_storage;
		curved					m_curved;
		curved_trans			m_curved_trans;
		agg::trans_affine		m_transform;
		double					m_length;
	};
};

#endif

// vim:ts=4:noet
