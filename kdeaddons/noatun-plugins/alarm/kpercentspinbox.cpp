/***************************************************************************
			kpercentspinbox.cpp  - QSpinbox with % 
                      -------------------
		begin    : Sun Apr 15 CEST 2001
		copyright: (C) 2001 by Mickael Marchand <mikmak@freenux.org>
		
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kpercentspinbox.h"
 
KPercentSpinBox::KPercentSpinBox(int _step, QWidget *_parent, const char *_name)
	        : QSpinBox(0, 100, _step, _parent, _name)
{
	setButtonSymbols(PlusMinus);
}
 
QString KPercentSpinBox::mapValueToText(int v)
{
	return QString::number(v)+'%';
}
#if 0
int KPercentSpinBox::mapTextToValue(bool *ok)
{
	QString mapText= text();
//	mapText.truncate(mapText.length()-1);
	
	return mapText.toInt(ok); 
}
#endif
#include "kpercentspinbox.moc"
