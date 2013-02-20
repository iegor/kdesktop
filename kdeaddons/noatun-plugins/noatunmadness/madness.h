#ifndef MADNESS_H
#define MADNESS_H

#include <noatun/plugin.h>
#include <string.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <kwinmodule.h>

class Madness : public QObject, public MonoFFTScope, public Plugin
{
Q_OBJECT
public:
	Madness();
	virtual ~Madness();

	void init();

private slots:
	void update();

protected:
	virtual void scopeEvent(float *data, int bands);

private:
	KWinModule mWm;
	QValueList<WId> mWindowList;
	QRect mWorkArea;
	QMap<WId, QPoint> mOriginalPositions;
};

#endif
