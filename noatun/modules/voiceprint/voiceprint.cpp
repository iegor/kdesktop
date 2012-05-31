#include "voiceprint.h"
#include <noatun/player.h>
#include <noatun/app.h>
#include <math.h>
#include <qpainter.h>
#include "prefs.h"
#include <klocale.h>
#include <stdio.h>

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new VoicePrint();
	}
}

VoicePrint *VoicePrint::voicePrint=0;

VoicePrint::VoicePrint() : QWidget(0,0,WRepaintNoErase), MonoFFTScope(50), Plugin()
{
	voicePrint=this;
	mOffset=0;
	mSegmentWidth=2;
	setCaption(i18n("Voiceprint"));
	resize(320, 240);
	MonoFFTScope::start();
	show();
	setMaximumHeight(1024);
}

VoicePrint::~VoicePrint()
{
}

void VoicePrint::init()
{
	Prefs *p=new Prefs(this); 
	p->reopen();
	p->save();
	resizeEvent(0);
}

void VoicePrint::setColors(const QColor &bg, const QColor &fg, const QColor &l)
{
	mProgress=l;
	mLowColor=bg.rgb();
	mHighColor=fg.rgb();
	setBackgroundColor(mLowColor);
}

void VoicePrint::closeEvent(QCloseEvent *)
{
	unload();
}

void VoicePrint::resizeEvent(QResizeEvent *)
{
	mOffset=0;
	mBuffer.resize(size());
	QPainter paint(&mBuffer);
	paint.fillRect(QRect(0,0, QWidget::width(), height()), QColor(mLowColor));
	setBands(magic(height()/mSegmentWidth));
}

#define COLOR(color, bgcolor, fgcolor, foctet) \
        (int)( color(bgcolor) + (foctet) * (color(fgcolor) - color(bgcolor)) )

inline static QRgb averageByIntensity(QRgb bgcolor, QRgb fgcolor, int octet)
{
	float foctet = octet / 255.0;

	return qRgb(COLOR(qRed, bgcolor, fgcolor, foctet),
	            COLOR(qGreen, bgcolor, fgcolor, foctet),
	            COLOR(qBlue, bgcolor, fgcolor, foctet)
	           );
}

#undef COLOR

void VoicePrint::paintEvent(QPaintEvent *e)
{
	bitBlt(this, e->rect().topLeft(), &mBuffer, e->rect(), Qt::CopyROP);
}

void VoicePrint::scopeEvent(float *data, int bands)
{
	// save cpu
	if(isHidden()) return;

	QPainter paint(&mBuffer);
	// each square has a width of mSegmentWidth
	float brightness = float(bands * mSegmentWidth);
	for (int i=0; i<bands ; i++)
	{
		float b=data[bands-i-1]+1.0;
		// the more bands there are, the dimmer each becomes
		b=log10(b)/log(2) * 16 * brightness;
		int band=int(b);
		if (band>255) band=255;
		else if (band<0) band=0;
		
		QColor area(averageByIntensity(mLowColor, mHighColor, band));

		int bandTop=i*height()/bands, bandBottom=(i+1)*height()/bands;
		paint.fillRect(mOffset, bandTop, mSegmentWidth,bandBottom-bandTop,area);
	}
	
	int newOffset = mOffset+mSegmentWidth;
	if (newOffset>QWidget::width()) newOffset=0;
	paint.fillRect(newOffset, 0, mSegmentWidth, height(), mProgress);

	// redraw changes with the minimum amount of work
	if(newOffset != 0)
	{
		repaint(mOffset,0,mSegmentWidth*2,height(),false);
	}
	else
	{
		repaint(mOffset,0,mSegmentWidth,height(),false);
		repaint(newOffset,0,mSegmentWidth,height(),false);
	}
	mOffset = newOffset;
}

#include "voiceprint.moc"
