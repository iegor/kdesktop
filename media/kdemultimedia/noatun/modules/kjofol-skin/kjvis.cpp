/***************************************************************************
	kjvis.cpp  -  Visualizations used in the KJöfol-GUI
	--------------------------------------
	Maintainer: Stefan Gehn <metz AT gehn.net>

 ***************************************************************************/

// local includes
#include "kjvis.h"
#include "kjprefs.h"

// system includes
#include <math.h>

//qt includes
#include <qpainter.h>
#include <qsize.h>

//kde includes
#include <kdebug.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kpixmapeffect.h>
#include <kpixmap.h>

// noatun includes
#include <noatun/player.h>

#define _KJ_GRADIENT_DIFF 130

/*******************************************
 * KJFFTScope
 *******************************************/

void KJVisScope::swapScope(Visuals newOne)
{
	//kdDebug(66666) << k_funcinfo << endl;
	QStringList line = parent()->item("analyzerwindow");
	KJLoader *p=parent();
	p->removeChild(this);
	delete this;

	KJLoader::kjofol->prefs()->setVisType ( newOne );

	KJWidget *w = 0;
	switch (newOne)
	{
	case Null:
		w = new KJNullScope(line, p);
		break;
	case FFT:
		w = new KJFFT(line, p);
		break;
	case StereoFFT:
		w = new KJStereoFFT(line, p);
		break;
	case Mono:
		w = new KJScope(line, p);
		break;
	};

	p->addChild(w);
}

/*******************************************
 * KJNullScope
 *******************************************/

KJNullScope::KJNullScope(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent)
{
	int x  = l[1].toInt();
	int y  = l[2].toInt();
	int xs = l[3].toInt() - x;
	int ys = l[4].toInt() - y;

	// background under vis
	QPixmap tmp = parent->pixmap(parent->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );
	setRect ( x, y, xs, ys );
	repaint();
}

void KJNullScope::paint(QPainter *p, const QRect &)
{
	// just redraw the background
	bitBlt ( p->device(), rect().topLeft(), mBack, QRect(0,0,-1,-1), Qt::CopyROP );
}

bool KJNullScope::mousePress(const QPoint &)
{
	return true;
}

void KJNullScope::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;

	parent()->repaint(rect(), false);
	swapScope(FFT);
}

void KJNullScope::readConfig()
{
//	kdDebug(66666) << "[KJNullScope] readConfig()" << endl;
	Visuals v = (Visuals) KJLoader::kjofol->prefs()->visType();
	if ( v != Null )
	{
		parent()->repaint(rect(), false);
		swapScope ( v );
	}
}


/*************************************************
 * KJFFT - Analyzer like visualization, mono
 *************************************************/

KJFFT::KJFFT(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent), MonoFFTScope(50), mGradient(0)
{
	int x = l[1].toInt();
	int y = l[2].toInt();
	int xs = l[3].toInt()-x;
	int ys = l[4].toInt()-y;

	// each bar will be 1px wide
	mMultiples=1;

	if ( parent->exist("analyzercolor") )
	{
		QStringList &col = parser()["analyzercolor"];
		mColor.setRgb ( col[1].toInt(), col[2].toInt(), col[3].toInt() );
	}
	else // TODO: what should be default colors for Vis?
	{
		mColor.setRgb ( 255, 255, 255 ); // white is default
	}

	// background under vis
	QPixmap tmp  = parent->pixmap(parent->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	mAnalyzer = new KPixmap ( QSize(xs,ys) );
	bitBlt( mAnalyzer, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	// create a gradient for the bars going from 30% lighter to 30% darker than mColor
	mGradient = new KPixmap ( QSize(xs,ys) );
	KPixmapEffect::gradient ( *mGradient, mColor.light(_KJ_GRADIENT_DIFF),
		mColor.dark(_KJ_GRADIENT_DIFF), KPixmapEffect::VerticalGradient );

	setRect (x,y,xs,ys);
	setBands(magic(xs/mMultiples));
	readConfig(); // read our config settings
	start();
}

void KJFFT::scopeEvent(float *d, int size)
{
	if ( !napp->player()->isPlaying() ) // don't draw if we aren't playing (either paused or stopped)
	{
		if ( napp->player()->isStopped() ) // clear vis-window if playing has been stopped
			parent()->repaint(rect(), false);
		return;
	}

	int x = 0;
	int h = rect().height();

	QBitmap mGradientMask ( rect().width(), h, true );
	QPainter mask( &mGradientMask );

	float *start = d ;
	float *end = d + size /*- 1*/;

	// loop creating the mask for vis-gradient
	for ( ; start < end; ++start )
	{
		// 5 has been 8 before and I have no idea how this scaling works :/
		// FIXME: somebody please make it scale to 100% of height,
		//        I guess that would be nicer to look at
//		float n = log((*start)+1) * (float)h * 5;
		float n = log((*start)+1) * (float)h * 5;
		int amp=(int)n;

		// range check
		if ( amp < 0 ) amp = 0;
		else if ( amp > h ) amp = h;

		// make a part of the analyzer-gradient visible
		mask.fillRect ( x, (h-amp), mMultiples, amp, Qt::color1 );
		x += mMultiples;
	}
	// done creating our mask

	// draw background of vis into it
	bitBlt ( mAnalyzer, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );

	// draw the analyzer
	mGradient->setMask(mGradientMask);
	bitBlt ( mAnalyzer, 0, 0, mGradient, 0, 0, -1, -1, Qt::CopyROP );

	repaint();
}

void KJFFT::paint(QPainter *p, const QRect &)
{
	// put that thing on screen
	if ( !napp->player()->isStopped() )
		bitBlt ( p->device(), rect().topLeft(), mAnalyzer, QRect(0,0,-1,-1), Qt::CopyROP );
}


bool KJFFT::mousePress(const QPoint &)
{
	return true;
}

void KJFFT::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;

	stop();
	parent()->repaint(rect(), false);
	swapScope(Mono);
}

void KJFFT::readConfig()
{
//	kdDebug(66666) << "[KJFFT] readConfig()" << endl;
	Visuals v = (Visuals) KJLoader::kjofol->prefs()->visType();
	if ( v != FFT )
	{
		stop();
		parent()->repaint(rect(), false);
		swapScope ( v );
		return;
	}

	mTimerValue = KJLoader::kjofol->prefs()->visTimerValue();
	setInterval( mTimerValue );
}


/*************************************************
 * KJStereoFFT - Analyzer like visualization, stereo
 *************************************************/

KJStereoFFT::KJStereoFFT(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent), StereoFFTScope(50), mGradient(0)
{
	//kdDebug(66666) << k_funcinfo << endl;

	int x = l[1].toInt();
	int y = l[2].toInt();
	int xs = l[3].toInt()-x;
	int ys = l[4].toInt()-y;

	// each bar will be 1px wide
	mMultiples=1;

	if ( parent->exist("analyzercolor") )
	{
		QStringList &col = parser()["analyzercolor"];
		mColor.setRgb ( col[1].toInt(), col[2].toInt(), col[3].toInt() );
	}
	else // TODO: what should be default colors for Vis?
	{
		mColor.setRgb ( 255, 255, 255 ); // white is default
	}

	// background under vis
	QPixmap tmp  = parent->pixmap(parent->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	mAnalyzer = new KPixmap ( QSize(xs,ys) );
	bitBlt( mAnalyzer, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	// create a gradient for the bars going from 30% lighter to 30% darker than mColor
	mGradient = new KPixmap ( QSize(xs,ys) );
	KPixmapEffect::gradient ( *mGradient, mColor.light(_KJ_GRADIENT_DIFF),
		mColor.dark(_KJ_GRADIENT_DIFF), KPixmapEffect::VerticalGradient );

	setRect (x,y,xs,ys);
	setBands(magic(xs/mMultiples));
	readConfig(); // read our config settings
	start();
}

void KJStereoFFT::scopeEvent(float *left, float *right, int len)
{
	if ( !napp->player()->isPlaying() ) // don't draw if we aren't playing (either paused or stopped)
	{
		if ( napp->player()->isStopped() ) // clear vis-window if playing has been stopped
			parent()->repaint(rect(), false);
		return;
	}

	unsigned int h = rect().height();
	int hh = (int)(rect().height()/2);

	QBitmap mGradientMask ( rect().width(), h, true );
	QPainter mask( &mGradientMask );

	float *start = left;
	float *end = left + len;
	float n = 0.0;
	int amp = 0;
	int x = 0;

	// loop creating the mask for vis-gradient
	for ( ; start < end; ++start )
	{
		n = log((*start)+1) * (float)hh * 5;
		amp = (int)n;

		// range check
		if ( amp < 0 ) amp = 0;
		else if ( amp > hh ) amp = hh;

		// make a part of the analyzer-gradient visible
		mask.fillRect ( x, (h-amp), mMultiples, amp, Qt::color1 );
		x += mMultiples;
	}
	// done creating our mask


	start = right;
	end = right + len;
	x = 0;
	// loop creating the mask for vis-gradient
	for ( ; start < end; ++start )
	{
		n = log((*start)+1) * (float)hh * 5;
		amp = (int)n;

		// range check
		if ( amp < 0 ) amp = 0;
		else if ( amp > hh ) amp = hh;

		// make a part of the analyzer-gradient visible
		mask.fillRect ( x, 0, mMultiples, amp, Qt::color1 );
		x += mMultiples;
	}


	// draw background of vis into it
	bitBlt ( mAnalyzer, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );

	// draw the analyzer
	mGradient->setMask(mGradientMask);
	bitBlt ( mAnalyzer, 0, 0, mGradient, 0, 0, -1, -1, Qt::CopyROP );

	repaint();
}

void KJStereoFFT::paint(QPainter *p, const QRect &)
{
	// put that thing on screen
	if ( !napp->player()->isStopped() )
		bitBlt ( p->device(), rect().topLeft(), mAnalyzer, QRect(0,0,-1,-1), Qt::CopyROP );
}

bool KJStereoFFT::mousePress(const QPoint &)
{
	return true;
}

void KJStereoFFT::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;
	stop();
	parent()->repaint(rect(), false);
	swapScope(Null);
}

void KJStereoFFT::readConfig()
{
	//kdDebug(66666) << k_funcinfo << endl;
	Visuals v = (Visuals) KJLoader::kjofol->prefs()->visType();
	if ( v != StereoFFT )
	{
		stop();
		parent()->repaint(rect(), false);
		swapScope ( v );
		return;
	}
	setInterval(KJLoader::kjofol->prefs()->visTimerValue());
}


/*************************************************
 * KJScope - oscilloscope like visualization
 *************************************************/

KJScope::KJScope(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent), MonoScope(50)/*, blurnum(0), mOsci(0)*/
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs = mWidth = l[3].toInt()-x;
	int ys = mHeight = l[4].toInt()-y;

	blurnum = 0;

//	kdDebug(66666) << "Analyzer Window " << x << "," << y << " " << mWidth << "," << mHeight << endl;

	if ( parent->exist("analyzercolor") )
	{
		QStringList &col = parser()["analyzercolor"];
		mColor.setRgb ( col[1].toInt(), col[2].toInt(), col[3].toInt() );
	}
	else // FIXME: what should be default colors for Vis?
		mColor.setRgb ( 255, 255, 255 );

	// background under vis
	QPixmap tmp  = parent->pixmap(parent->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	mOsci = new KPixmap ( QSize(xs,ys) );
	bitBlt( mOsci, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	// create a gradient
	mGradient = new KPixmap ( QSize(xs,ys) );
	KPixmapEffect::gradient ( *mGradient, mColor.light(_KJ_GRADIENT_DIFF),
		mColor.dark(_KJ_GRADIENT_DIFF), KPixmapEffect::VerticalGradient );

	setRect ( x, y, xs, ys );

	// set the samplewidth to the largest integer divisible by mWidth
	setSamples ( xs );

	readConfig();
	start();
}

void KJScope::scopeEvent(float *d, int size)
{
	if ( !napp->player()->isPlaying() )
	{
		if ( napp->player()->isStopped() )
		{
			bitBlt ( mOsci, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
			repaint();
		}
		return;
	}

	float *start = d;
	float *end = d + size;

	int heightHalf = rect().height()/2 /* -1 */;
	int x = 0;

	QPainter tempP( mOsci );

	if ( blurnum == 3 )
	{  // clear whole Vis
		bitBlt ( mOsci, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
		tempP.setPen( mColor.light(110) ); // 10% lighter than mColor
		blurnum=0;
	}
	else
	{
		blurnum++;
		// reduce color for blur-effect
		tempP.setPen( mColor.dark(90+(10*blurnum)) ); // darken color
	}

	for ( ; start < end; ++start )
	{
		float n = (*start) * (float)heightHalf;
		int amp = (int)n;

		// range check
		if ( amp > heightHalf ) amp = heightHalf;
		else if ( amp < -heightHalf) amp = -heightHalf;

		// draw
//		tempP.drawLine(x, heightHalf, x, heightHalf+amp);
		if ( amp > 0 )
		{
			bitBlt ( tempP.device(), QPoint(x,heightHalf), mGradient, QRect(x,heightHalf,1,amp), Qt::CopyROP );
		}
		else
		{
			amp = -amp;
			bitBlt ( tempP.device(), QPoint(x,heightHalf-amp), mGradient, QRect(x,(heightHalf-amp),1,amp), Qt::CopyROP );
		}
		x++;
	}

	repaint();
}

void KJScope::paint(QPainter *p, const QRect &)
{
	// put that thing on screen
	bitBlt ( p->device(), rect().topLeft(), mOsci, QRect(0,0,-1,-1), Qt::CopyROP );
}

bool KJScope::mousePress(const QPoint &)
{
	return true;
}

void KJScope::mouseRelease(const QPoint &, bool in)
{
	if (!in) // only do something if users is still inside the button
		return;

	stop();
	parent()->repaint(rect(), false);
	swapScope(/*Null*/ StereoFFT);
}

void KJScope::readConfig()
{
//	kdDebug(66666) << "[KJScope] readConfig()" << endl;
	Visuals v = (Visuals) KJLoader::kjofol->prefs()->visType();
	if ( v != Mono )
	{
		stop();
		parent()->repaint(rect(), false);
		swapScope ( v );
		return;
	}

	mTimerValue = KJLoader::kjofol->prefs()->visTimerValue();
	setInterval( mTimerValue );
}
