/*
   Copyright (C) 1998 Jürgen Hochwald <juergen.hochwald@privat.kkf.net>
   Copyright (C) 2003 Charles Samuels <charles@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/



#include <stdio.h>
#include "spline.h"

const bool Spline::natural=false;

Spline::Spline()
{
	yp1=0.0;
	ypn=0.0;
	mRecalc = true;
}

Spline::Spline(const Spline &copy)
{
	operator=(copy);
}

Spline &Spline::operator =(const Spline &copy)
{
	mPoints = copy.mPoints;
	mRecalc = copy.mRecalc;
	yp1 = copy.yp1;
	ypn = copy.ypn;
	return *this;
}

Spline::~Spline()
{

}

void Spline::add(double x, double y)
{
	Group g = { x, y, 0.0 };
	mPoints.push_back(g);
	mRecalc=true;
}

std::vector<double> Spline::points(int count) const
{
	std::vector<double> points;
	if (count == numPoints())
	{
		for (int i=0; i < count; ++i)
		{
			points.push_back(mPoints[i].y);
		}
	}
	else
	{
		double min = mPoints[0].x;
		double max = mPoints[numPoints()-1].x;
		double dcount = double(count);

		for (int i=0; i<count; ++i)
		{
			points.push_back(spline( (max-min)/dcount*i + min));
		}
	}

	return points;
}

void Spline::calcSpline()
{
	const int np=numPoints();
	double *u = new double[np];

	if (natural)
	{
		mPoints[0].y2 = u[0] = 0.0;
	}
	else
	{
		mPoints[0].y2 = -0.5;
		u[0] =
			(3.0/(mPoints[1].x-mPoints[0].x))*((mPoints[1].y-mPoints[0].y)
			/ (mPoints[1].x-mPoints[0].x)-yp1);
	}

	double sig,p,qn,un;
	for (int i=1; i<=np-2; i++)
	{
		sig=(mPoints[i].x-mPoints[i-1].x) / (mPoints[i+1].x-mPoints[i-1].x);
		p = sig*mPoints[i-1].y2+2.0;
		mPoints[i].y2 = (sig-1.0)/p;
		u[i] =
			(mPoints[i+1].y - mPoints[i].y) / (mPoints[i+1].x-mPoints[i].x)
			 - (mPoints[i].y - mPoints[i-1].y) / (mPoints[i].x - mPoints[i-1].x);
		u[i] = (6.0*u[i] / (mPoints[i+1].x - mPoints[i-1].x) - sig *u[i-1])/p;
	}

	if (natural)
	{
		qn = un = 0.0;
	}
	else
	{
		qn = 0.5;
		un =
			(3.0 / (mPoints[np-1].x - mPoints[np-2].x))
			* (ypn - (mPoints[np-1].y - mPoints[np-2].y)
			/ (mPoints[np-1].x - mPoints[np-2].x));
	}
	mPoints[np-1].y2 = (un - qn * u[np-2]) / (qn*mPoints[np-2].y2 +1.0 );

	for (int i=np-2; i>=0; i--)
	{
		mPoints[i].y2 = mPoints[i].y2 * mPoints[i+1].y2+u[i];
	}
	mRecalc = false;
	delete [] u;
}

double Spline::spline(double xarg) const
{
	if (numPoints()==0) return 0.0;
	if (numPoints()==1) return mPoints[0].y;

	if (mRecalc) calcSpline();

	int klo=0;
	int khi=numPoints()-1;
	int k;
	while (khi-klo > 1)
	{
		k = khi+klo;
		if (k % 2)
			k = (k+1) / 2;
		else
			k = k/2;

		if (mPoints[k].x > xarg) khi=k;
		else klo=k;
	}

	double h = mPoints[khi].x - mPoints[klo].x;
	if (h==0)
	{
		// failed
		return 0.0;
	}

	double a = (mPoints[khi].x - xarg) / h;
	double b = (xarg - mPoints[klo].x) / h;
	return
		a * mPoints[klo].y + b*mPoints[khi].y
		+ ((a*a*a-a) * mPoints[klo].y2
		+ (b*b*b-b) * mPoints[khi].y2) * (h*h) / 6.0;
}

double Spline::x(int num) const
{
	if (numPoints()<num) return 0.0;
	return mPoints[num].x;
}

double Spline::y(int num) const
{
	if (numPoints()<num) return 0.0;
	return mPoints[num].y;
}

void Spline::clear()
{
	mPoints.resize(0);
	ypn=0.0;
	yp1=0.0;
	mRecalc=true;
}


