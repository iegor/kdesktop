#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include "searchengine.h"
#include "dbse2.h"

class KDB2PreferencesWidget : public PrefWidget
{
	Q_OBJECT

public:
	KDB2PreferencesWidget(QWidget *parent=0, const char* name=0);
	virtual ~KDB2PreferencesWidget();

	virtual void apply();
	virtual void cancel();
	virtual void standard();
	DBSearchEnginePrefWidget *dbpw;

signals:
     void applyNow();
     void restoreNow();

};

#endif
