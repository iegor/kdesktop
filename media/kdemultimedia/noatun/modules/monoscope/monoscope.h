#ifndef MONOSCOPE_H
#define MONOSCOPE_H

#include <noatun/plugin.h>

class KToggleAction;

class Monoscope : public QWidget,  public MonoScope, public Plugin
{
Q_OBJECT
NOATUNPLUGIND

public:
	Monoscope();
	virtual ~Monoscope();

	void init();

public slots:
	void toggle(void);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void scopeEvent(float *data, int bands);

	virtual void resizeEvent(QResizeEvent *);

private:
	QRgb mHighColor, mLowColor;
	KToggleAction *mAction;
};

#endif

