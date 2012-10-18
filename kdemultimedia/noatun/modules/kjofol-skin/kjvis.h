#ifndef KJVIS_H
#define KJVIS_H

#include "kjwidget.h"
class KJLoader;
class KPixmap;

class KJVisScope : public KJWidget
{
public:
	KJVisScope(KJLoader *parent) : KJWidget(parent) {};
	enum Visuals { Null=0, FFT, Mono, StereoFFT };
	void swapScope(Visuals newOne);
//	virtual void readConfig();
};


// dummy-scope displaying nothing
class KJNullScope : public KJVisScope
{
public:
	KJNullScope(const QStringList &, KJLoader *parent);
	virtual void paint(QPainter *p, const QRect &);
	virtual bool mousePress(const QPoint&);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void readConfig(void);

private:
	KPixmap *mBack;

};


// analyzer-like scope
class KJFFT : public KJVisScope, public MonoFFTScope
{
public:
	KJFFT(const QStringList &, KJLoader *parent);
	virtual void paint(QPainter *p, const QRect &);
	virtual void scopeEvent(float *d, int size);

	virtual bool mousePress(const QPoint&);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void readConfig(void);

private:
	QColor mColor;
	KPixmap *mGradient;
	KPixmap *mBack;
	KPixmap *mAnalyzer;
	int mMultiples;
	int mTimerValue;
};


// analyzer-like scope, stereo version
class KJStereoFFT : public KJVisScope, public StereoFFTScope
{
public:
	KJStereoFFT(const QStringList &, KJLoader *parent);
	virtual void paint(QPainter *p, const QRect &);
	virtual void scopeEvent(float *left, float *right, int len);

	virtual bool mousePress(const QPoint&);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void readConfig(void);

private:
	QColor mColor;
	KPixmap *mGradient;
	KPixmap *mBack;
	KPixmap *mAnalyzer;
	int mMultiples;
	int mTimerValue;
};


// oscilloscope showing waveform
class KJScope : public KJVisScope, public MonoScope
{
public:
	KJScope ( const QStringList &, KJLoader *parent);
	virtual void paint(QPainter *p, const QRect &);
	virtual void scopeEvent(float *d, int size);

	virtual bool mousePress(const QPoint&);
	virtual void mouseRelease(const QPoint &, bool in);
	virtual void readConfig(void);

private:
	QColor mColor;
	KPixmap *mGradient;
	KPixmap *mBack;
	KPixmap *mOsci;
	int mMultiples;
	int mWidth;
	int mHeight;
	unsigned int blurnum;
	int mTimerValue;
};

#endif
