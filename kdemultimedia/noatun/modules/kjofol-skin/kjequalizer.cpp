/***************************************************************************
	kjequalizer.cpp  -  links noatun VEqualizer and KJofol
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

#include "kjequalizer.h"
#include "kjequalizer.moc"

#include <qpainter.h>
#include <qtimer.h>

#include <kdebug.h>
#include <kpixmap.h>

#include <noatun/vequalizer.h>

KJEqualizer::KJEqualizer(const QStringList &l, KJLoader *p)
	: QObject(0), KJWidget(p), mBack(0), mView(0), mInterpEq(0)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	setRect(x,y,xs,ys);

	mBars = p->pixmap(parser()["equalizerbmp"][3]);

	mBands = l[6].toInt();
	mXSpace = l[7].toInt();

	// background under equalizer
	// needed to only blit onto screen ONCE and not for every band
	QPixmap tmp = p->pixmap(p->item("backgroundimage")[1]);
	mBack = new KPixmap ( QSize(xs,ys) );
	bitBlt( mBack, 0, 0, &tmp, x, y, xs, ys, Qt::CopyROP );

	// buffer for view
	mView = new QPixmap ( xs, ys );

	mBandWidth=parser()["EqualizerBmp"][1].toInt();
	mBandHalfHeight=parser()["EqualizerBmp"][2].toInt();
	
	kdDebug(66666) << "[KJEqualizer] mBands=" << mBands << ", mXSpace=" << mXSpace << ", mBandWidth=" << mBandWidth << ", mBandHalfHeight=" <<  mBandHalfHeight << "." << endl;

	kdDebug(66666) << "[KJEqualizer] creating VInterpolation for " << mBands << " bands..." << endl;
	mInterpEq = new VInterpolation(mBands);
//	napp->vequalizer()->setBands(mBands); // FIXME: hack because spline sucks :P
	connect(napp->vequalizer(), SIGNAL(changed()), this, SLOT(slotUpdateBuffer()));

	slotUpdateBuffer(); // fill mView pixmap with valid data
}

KJEqualizer::~KJEqualizer(void)
{
	delete mInterpEq;
	delete mView;
	delete mBack;
}

int KJEqualizer::barNum(const QPoint &pos) const
{
	int x = pos.x();
	x = x / mXSpace;
	return mInterpEq->bands() * x / mBands;
}

int KJEqualizer::level(const QPoint &pos) const
{
	int y = ((-pos.y()) + mBandHalfHeight+1) * (200/mBandHalfHeight);
	return y;
}

void KJEqualizer::paint(QPainter *p, const QRect &)
{
	QPixmap temp(rect().width(), rect().height());
	// draw background into buffer
	bitBlt ( &temp, 0, 0, mBack, 0, 0, -1, -1, Qt::CopyROP );
	// draw band sliders into buffer
	bitBlt( &temp, 0, 0, mView, 0, 0, rect().width(), rect().height(), Qt::CopyROP);
	// and draw it on screen
	bitBlt(p->device(), rect().topLeft(), &temp, QRect(0,0,-1,-1), Qt::CopyROP);
}

void KJEqualizer::slotUpdateBuffer()
{
//	kdDebug(66666) << "[KJEqualizer] slotUpdateBuffer() called." << endl;

	QBitmap regionMask( rect().width(), rect().height(), true); // fully transparent mask
	QPainter mask( &regionMask );

	QPoint destX = QPoint(0, 0);

	for (int band=0; band<mBands; band++)
	{
		int level = mInterpEq->level(band);
		if (level>200) level=200;
		if (level<-200) level=-200;
		int picNum = ((int)(level+200)*(mBandHalfHeight-1) / 400) + 1;
		int xPos = (picNum * mBandWidth) - mBandWidth;

//		kdDebug(66666) << "[KJEqualizer] band=" << band << ", level=" << level << ", picNum=" << picNum << " @ xpos=" << xPos << "." << endl;

		bitBlt(mView, destX, &mBars, QRect(xPos,0,mBandWidth,rect().height()), Qt::CopyROP);
		// make slider opaque in mask so you see something on screen
		mask.fillRect ( destX.x(), 0, mBandWidth, rect().height(), Qt::color1 );
		destX += QPoint(mXSpace,0);

	} // for()
	// whole thingy has been drawn, now set the mask
	mView->setMask( regionMask );
	repaint();
}

void KJEqualizer::mouseMove(const QPoint &p, bool in)
{
	if (!in) return;
	mousePress(p);
}

bool KJEqualizer::mousePress(const QPoint &p)
{
	kdDebug(66666) << "[KJEqualizer] setting band " << mBands << "/" << barNum(p)+1 << " to level " << level(p) << endl;
	VBand b = mInterpEq->band( barNum(p) );
	b.setLevel( level(p) );
//	mouseMove(p, true);
	return true;
}
