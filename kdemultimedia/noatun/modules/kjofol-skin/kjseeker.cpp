/***************************************************************************
	kjseeker.cpp
	---------------------------------------------
	slider that lets the user jump inside the currently played file
	---------------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

#include "kjseeker.h"
#include "kjloader.h"

#include "helpers.cpp"
#include <noatun/player.h>

#include <kdebug.h>

KJSeeker::KJSeeker(const QStringList &i, KJLoader *l) : KJWidget(l), g(0)
{
	QString activeBg = backgroundPressed("bmp1");
	if(activeBg.isEmpty())
	{
		kdDebug(66666) << k_funcinfo << "No pressed background found for seeker," <<
			" using default background!" << endl;
		parent()->image(parser()["backgroundimage"][1]);
	}
	else
		mActive = parent()->image(activeBg);

	mScale = parent()->image(parser()["seekimage"][1]);
	QImage pixmapNoPress = parent()->image(parser()["backgroundimage"][1]);

	// generate transparent mask
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect(x,y,xs,ys);
	QImage transmask(xs, ys, 1, 2, QImage::LittleEndian);
#if QT_VERSION < 0x030300
	transmask.setColor(0, qRgb(0,0,0));
	transmask.setColor(1, qRgb(255,255,255));
#else
	transmask.setColor(1, qRgb(0,0,0));
	transmask.setColor(0, qRgb(255,255,255));
#endif

	// clear the pointers
	memset(barmodeImages, 0, 256*sizeof(QImage*));
	memset(barmode, 0, 256*sizeof(QPixmap*));

	// Now do the pixel føking
//	kdDebug(66666) << "creating Pixmaps for Seeker" << endl;
	for (int iy=y;iy<y+ys; iy++)
	{
		for (int ix=x;ix<x+xs; ix++)
		{
			QRgb checkmScale = mScale.pixel(ix, iy);
			// am I transparent?
			if (!isGray(checkmScale))
			{
				setPixel1BPP(transmask, ix-x, iy-y, 0);
				continue;
			}
			setPixel1BPP(transmask, ix-x, iy-y, 1);

			// what is the level
			int level=grayRgb(checkmScale)+1;
			if (level>255) level=255;
			// allocate the pixmap of the level proper
			// copy the color to the surface proper
			QRgb activeColor=mActive.pixel(ix,iy);
			QRgb inactiveColor=pixmapNoPress.pixel(ix,iy);
			// set this pixel and everything before it
			for(int i=0; i<level; i++)
			{
				if (!barmodeImages[i])
					barmodeImages[i]=new QImage(xs,ys, 32);
				QRgb *l=(QRgb*)barmodeImages[i]->scanLine(iy-y);
				l[ix-x]=inactiveColor;
			}

			do
			{
				if (!barmodeImages[level])
					barmodeImages[level]=new QImage(xs,ys, 32);
				QRgb *l=(QRgb*)barmodeImages[level]->scanLine(iy-y);
				l[ix-x]=activeColor;
			} while (level++<255);
		}
	}
//	kdDebug(66666) << "finished creating Pixmaps" << endl;

	// create the blank one
	barmode[0]=new QPixmap(xs, ys);
	QPixmap px=parent()->pixmap(parser()["backgroundimage"][1]);
	bitBlt(barmode[0], 0, 0, &px, x, y, xs, ys, Qt::CopyROP);
	px.convertFromImage(transmask);
	barModeMask=px;

//	kdDebug(66666) << "END KJSeeker constructor" << endl;
}

QPixmap *KJSeeker::toPixmap(int n)
{
	if (!barmodeImages[n]) return barmode[n];

	barmode[n]=new QPixmap(
			barmodeImages[n]->width(),
			barmodeImages[n]->height()
		);
	barmode[n]->convertFromImage(*barmodeImages[n]);

	delete barmodeImages[n];
	barmodeImages[n]=0;
	return barmode[n];
}


KJSeeker::~KJSeeker()
{
	for (uint i=0; i<256; i++)
	{
		if (barmode[i])
			delete barmode[i];
		if (barmodeImages[i])
			delete barmodeImages[i];
	}
}

void KJSeeker::paint(QPainter *p, const QRect &)
{
	closest();
	QPixmap *pixmap = toPixmap(g);
	pixmap->setMask(barModeMask);
	bitBlt(p->device(), rect().topLeft().x(), rect().topLeft().y(),
		pixmap, 0, 0, rect().width(), rect().height(), Qt::CopyROP);
}

bool KJSeeker::mousePress(const QPoint &pos)
{
	return (isGray(mScale.pixel(rect().topLeft().x()+pos.x(), rect().topLeft().y()+pos.y())));
}

void KJSeeker::mouseRelease(const QPoint &pos, bool in)
{
	int x = rect().topLeft().x()+pos.x();
	int y = rect().topLeft().y()+pos.y();

	if(napp->player()->isStopped())
		return;

	if(!mScale.valid(x, y))
	   return;

	QRgb color=mScale.pixel(x, y);

	// user released mousebutton outside of the seeker-area (which is gray)
	if ( (!isGray(color)) || (!in) )
		return;

	g = grayRgb(color);
	repaint();

//	kdDebug(66666) << "length : " << napp->player()->getLength() << endl;
//	kdDebug(66666) << "skip to: " << ((long long)g*(long long)napp->player()->getLength())/255 << endl;

	// g * titlelength can get REALLY HUGE, that's why I used (long long)
	napp->player()->skipTo( ((long long)g*(long long)napp->player()->getLength())/255 );

	return;
}

void KJSeeker::timeUpdate(int sec)
{
	int length = napp->player()->getLength() / 1000;
	if (length<1)
		length=1;

	if (sec > length)
		sec = length;
	else if ( sec < 0 )
		sec=0;

	g = sec * 255 / length;
	//kdDebug(66666) << "sec: " << sec << " len: " << length << " g: " << g << endl;
	QPainter p(parent());
	paint(&p, rect());
}

void KJSeeker::closest()
{
	int south=g, north=g;
	bool southtried=false, northtried=false;
	while (
			!barmode[south] && !barmodeImages[south]
			&& !barmode[north] && !barmodeImages[north])
	{
		if (southtried && northtried) { g=0; return; }
		south--;
		north++;
		if (north>255) {northtried=true; north=g;}
		if (south<0) {southtried=true; south=g;}
	}
	if (barmode[south] || barmodeImages[south])
		g=south;
	else if (barmode[north] || barmodeImages[north])
		g=north;
}
