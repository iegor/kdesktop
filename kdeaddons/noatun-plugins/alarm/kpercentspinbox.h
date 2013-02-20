/***************************************************************************
			kpercentspinbox.h  -  QSpinbox with %
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

#ifndef KPERCENT_SPINBOX_H
#define KPERCENT_SPINBOX_H

#include <qspinbox.h>

class KPercentSpinBox : public QSpinBox
{
	Q_OBJECT
	public:
		KPercentSpinBox (int _step=1, QWidget *_parent=0, const char *name=0);

	protected: 
		virtual QString mapValueToText(int v);
//		virtual int mapTextToValue(bool *ok);
};

#endif
