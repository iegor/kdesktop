#ifndef VOICEPRINT_H
#define VOICEPRINT_H

#include <noatun/plugin.h>

class VoicePrint : public QWidget,  public MonoFFTScope, public Plugin
{
Q_OBJECT

public:
	VoicePrint();
	virtual ~VoicePrint();

	void setColors(const QColor &bg, const QColor &fg, const QColor &l);
	void init();

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void scopeEvent(float *data, int bands);
	virtual void resizeEvent(QResizeEvent *);
	virtual void paintEvent(QPaintEvent *);

public:
	static VoicePrint* voicePrint;
	
private:
	QColor mProgress;
	QPixmap mBuffer;
	QRgb mLowColor, mHighColor;
	int mOffset, mSegmentWidth;
};
#endif

