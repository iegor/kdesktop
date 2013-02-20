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

#include <math.h>

#include "KSVGHelper.h"

using namespace KSVG;

bool KSVGHelper::m_initialised = false;

int KSVGHelper::m_linearRGBFromsRGB[256];
int KSVGHelper::m_sRGBFromLinearRGB[256];

// Force initialisation of the lookup tables
static KSVGHelper ksvgHelper;

KSVGHelper::KSVGHelper()
{
	if(!m_initialised)
	{
		initialise();
		m_initialised = true;
	}
}

void KSVGHelper::initialise()
{
	// Set up linearRGB - sRGB conversion tables
	for(int i = 0; i < 256; i++)
	{
		m_linearRGBFromsRGB[i] = calcLinearRGBFromsRGB(i);
		m_sRGBFromLinearRGB[i] = calcSRGBFromLinearRGB(i);
	}
}

int KSVGHelper::calcLinearRGBFromsRGB(int sRGB8bit)
{
	double sRGB = sRGB8bit / 255.0;
	double linearRGB;

	if(sRGB <= 0.04045)
		linearRGB = sRGB / 12.92;
	else
		linearRGB = pow((sRGB + 0.055) / 1.055, 2.4);

	return static_cast<int>(linearRGB * 255 + 0.5);
}

int KSVGHelper::calcSRGBFromLinearRGB(int linearRGB8bit)
{
	double linearRGB = linearRGB8bit / 255.0;
	double sRGB;

	if(linearRGB <= 0.0031308)
		sRGB = linearRGB * 12.92;
	else
		sRGB = 1.055 * pow(linearRGB, 1 / 2.4) - 0.055;

	return static_cast<int>(sRGB * 255 + 0.5);
}

extern "C"
int linearRGBFromsRGB(int sRGB8bit)
{
	return KSVGHelper::linearRGBFromsRGB(sRGB8bit);
}

extern "C"
int sRGBFromLinearRGB(int linearRGB8bit)
{
	return KSVGHelper::sRGBFromLinearRGB(linearRGB8bit);
}

// vim:ts=4:noet
