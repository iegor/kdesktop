#ifndef PREFS_H
#define PREFS_H

#include <qwidget.h>
#include <noatun/pref.h>

class KColorButton;

class Prefs : public CModule
{ 
Q_OBJECT
public:
	Prefs(QObject* parent);
	virtual void save();
	virtual void reopen();

private:
	KColorButton *mForeground, *mBackground, *mLine;
};

#endif

