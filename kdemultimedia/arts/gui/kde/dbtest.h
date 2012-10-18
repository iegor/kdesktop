
#ifndef DBTESTWIDGET_H
#define DBTESTWIDGET_H

#include <qwidget.h>

#include "dbvolcalc.h"

class dBTestWidget : public QWidget, public dB2VolCalc {
   Q_OBJECT
public:
	dBTestWidget( QWidget*, const char* =0 );
	~dBTestWidget();
};

#endif
