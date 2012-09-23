#ifndef PREFERENCESWIDGET_H
#define PREFERENCESWIDGET_H

#include "searchengine.h"
#include "dbseprefwidget.h"

class PreferencesWidget : public PrefWidget
{
	Q_OBJECT

public:
	PreferencesWidget(QWidget *parent=0, const char* name=0);
	virtual ~PreferencesWidget();

	virtual void apply();
	virtual void cancel();
	virtual void standard();
	DBSearchEnginePref *dbpw;
public slots:
     void setName(QString);
     void setEntries(int);     
signals:
     void applyNow();
     void restoreNow();

};

#endif
