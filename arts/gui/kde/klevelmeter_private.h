/*
    Copyright (  C ) 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#ifndef ARTS_KLEVELMETER_PRIVATE_H
#define ARTS_KLEVELMETER_PRIVATE_H

#include <qwidget.h>

#include "klevelmeter_impl.h"

class KLevelMeter_Template;
class QBoxLayout;
class KArtsWidget;

class KLevelMeter_Private : public QObject {
   Q_OBJECT
public:
	Arts::KLevelMeter_impl* _impl;
	QFrame* _frame;
	QBoxLayout *_layout;
	
	KLevelMeter_Template *_levelmeter;

	Arts::LevelMeterStyle _style;
	long _substyle;
	long _count;
	Arts::Direction _direction;
	long _peak;
	float _peakvalue;
	float _dbmin, _dbmax;

	KLevelMeter_Private( Arts::KLevelMeter_impl* impl, QFrame* frame, Arts::LevelMeterStyle defstyle, QObject* =0, const char* =0 );

	void createWidget();
};

#endif
// vim: sw=4 ts=4
