/***************************************************************************
	kjsliders.cpp
	---------------------------------------------
	Sliders for Volume and Pitch
	---------------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "kjsliders.h"
#include "kjtextdisplay.h"
#include "kjprefs.h"

#include "helpers.cpp"

// kde includes
#include <klocale.h>
#include <kdebug.h>

// arts-includes, needed for pitch
#include <artsmodules.h>
#include <reference.h>
#include <soundserver.h>
#include <kmedia2.h>

// noatun includes
#include <noatun/player.h>
#include <noatun/engine.h>

/*******************************************
 * KJVolumeBar
 *******************************************/

KJVolumeBar::KJVolumeBar(const QStringList &i, KJLoader *p)
	: KJWidget(p), mVolume(0), mText(0)
{
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect ( x, y, xs, ys );

//	kdDebug(66666) << "x: " << x << " y: " << y << " w: " << xs << " h: " << ys << endl;

	mBack   = parent()->pixmap(parser()["backgroundimage"][1]);
	mSlider = parent()->pixmap(parser()["volumecontrolimage"][1]);
}

QString KJVolumeBar::tip()
{
	return i18n("Volume");
}

void KJVolumeBar::paint(QPainter *p, const QRect &)
{
//	kdDebug(66666) << "x: " << rect().x() << " y: " << rect().y() << endl;
//	kdDebug(66666) << "vol x: " << rect().x()+(mVolume*rect().width())/100 << endl;

	// center of that slider-pixmap
//	QPoint hotSpot = QPoint( mSlider.width()/2, mSlider.height()/2 );

	// draw our background
	bitBlt(
		p->device(),
			rect().x() /*- hotSpot.x()*/,
			rect().y() /*- hotSpot.y()*/,
		&mBack,
			rect().x() /*- hotSpot.x()*/,
			rect().y() /*- hotSpot.y()*/,
			rect().width()  /*+ (2*hotSpot.x())*/,
			rect().height() /*+ (2*hotSpot.y())*/,
		Qt::CopyROP);

	// draw our slider
	bitBlt(
		p->device(),
			rect().x() + ((mVolume*rect().width())/100) /*- hotSpot.x()*/,
			rect().y() /*- hotSpot.y()*/,
		&mSlider,
			0,
			0,
			mSlider.width(),
			mSlider.height(),
		Qt::CopyROP);

	if (mText)
		mText->repaint();
}

bool KJVolumeBar::mousePress(const QPoint &pos)
{
	mVolume = (pos.x()*100) / rect().width();
//	kdDebug(66666) << "volume: " << mVolume << endl;
	repaint();
	napp->player()->setVolume(mVolume);
	return true;
}

void KJVolumeBar::mouseRelease(const QPoint &, bool)
{
}

void KJVolumeBar::mouseMove(const QPoint &pos, bool in)
{
	if (!in)
		return;
	mousePress(pos);
}

void KJVolumeBar::timeUpdate(int)
{
	mVolume = napp->player()->volume();
	repaint();
}


/*******************************************
 * KJVolumeBMP
 *******************************************/

KJVolumeBMP::KJVolumeBMP(const QStringList &i, KJLoader *p)
	: KJWidget(p), mVolume(0), mOldVolume(0), mText(0)
{
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect ( x, y, xs, ys );

	mWidth = parser()["volumecontrolimagexsize"][1].toInt();
	mCount = parser()["volumecontrolimagenb"][1].toInt()-1;

	mImages = parent()->pixmap(parser()["volumecontrolimage"][1]);
	mPos = parent()->image(parser()["volumecontrolimageposition"][1]);
	timeUpdate(0);
}

QString KJVolumeBMP::tip()
{
	return i18n("Volume");
}

void KJVolumeBMP::paint(QPainter *p, const QRect &)
{
	QRect from(mVolume*mCount/100*mWidth, 0, mWidth, mImages.height());
	bitBlt(p->device(), rect().topLeft(), &mImages, from, Qt::CopyROP);
	if (mText)
		mText->repaint();
}

bool KJVolumeBMP::mousePress(const QPoint &pos)
{
	QRgb color = mPos.pixel ( rect().topLeft().x()+pos.x(), rect().topLeft().y()+pos.y() );

	if (!isGray(color))
		return false;

	mVolume = grayRgb(color)*100/255;
//	kdDebug(66666) << "gray  : " << grayRgb(color) << endl;
//	kdDebug(66666) << "volume: " << mVolume << endl;

	repaint();

	napp->player()->setVolume(mVolume);

	return true;
}

void KJVolumeBMP::mouseRelease(const QPoint &, bool)
{}

void KJVolumeBMP::mouseMove(const QPoint &pos, bool in)
{
	if (!in) return;
	mousePress(pos);
}

void KJVolumeBMP::timeUpdate(int)
{
	mVolume = napp->player()->volume();

	if ( mVolume == mOldVolume ) // dont redraw if nothing changed
		return;

	mOldVolume = mVolume;

	repaint();
}


/*******************************************
 * KJPitchBMP
 *******************************************/

KJPitchBMP::KJPitchBMP(const QStringList &i, KJLoader *p)
	: KJWidget(p), mText(0)
{
	int x  = i[1].toInt();
	int y  = i[2].toInt();
	int xs = i[3].toInt() - x;
	int ys = i[4].toInt() - y;
	
	setRect ( x, y, xs, ys );

	mWidth = parser()["pitchcontrolimagexsize"][1].toInt();
	mCount = parser()["pitchcontrolimagenb"][1].toInt()-1;

	mImages = parent()->pixmap(parser()["pitchcontrolimage"][1]);
	mPos = parent()->image(parser()["pitchcontrolimageposition"][1]);

	// makes all pixels with rgb(255,0,255) transparent
	QImage ibackground;
	ibackground = parent()->image(parser()["pitchcontrolimage"][1]);
	mImages.setMask( getMask(ibackground) );

	Arts::PlayObject playobject = napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if ( pitchable.isNull() )
		mCurrentPitch = 1.0;
	else
		mCurrentPitch = pitchable.speed();

//	kdDebug() << "[KJPitchBMP] starting with pitch: " << mCurrentPitch << endl;
/*
	mMinPitch = 0.5;
	mMaxPitch = 2.0;

	mMinPitch = 0.8;
	mMaxPitch = 1.2;
*/

	readConfig();

	if (mText)
		mText->repaint();
}

QString KJPitchBMP::tip()
{
	return i18n("Pitch");
}

void KJPitchBMP::paint(QPainter *p, const QRect &)
{
	float xPos = (int)((mCurrentPitch-mMinPitch)*100.0) * mCount / (int)((mMaxPitch-mMinPitch)*100.0) * mWidth;

	QRect from( (int)xPos, 0, mWidth, mImages.height());

	bitBlt(p->device(), rect().topLeft(), &mImages, from, Qt::CopyROP);

	if (mText)
		mText->repaint();
}

bool KJPitchBMP::mousePress(const QPoint &pos)
{
	QRgb color = mPos.pixel ( rect().topLeft().x()+pos.x(), rect().topLeft().y()+pos.y() );

	if (!isGray(color))
		return false;

	mCurrentPitch = mMinPitch + ( (grayRgb(color)*(mMaxPitch-mMinPitch)) / 255 );
//	kdDebug(66666) << "[KJPitchBMP] mousePress()  mCurrentPitch: " << mCurrentPitch << endl;

	repaint();

	newFile(); // wrong naming, in fact it just sets pitch

	return true;
}

void KJPitchBMP::mouseRelease(const QPoint &, bool)
{}

void KJPitchBMP::mouseMove(const QPoint &pos, bool in)
{
	if (!in) return;
	mousePress(pos);
}

void KJPitchBMP::timeUpdate(int)
{
//	kdDebug(66666) << "[KJPitchBMP] :timeUpdate(int)" << endl;

	Arts::PlayObject playobject = napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if ( !pitchable.isNull() )
	{
		mCurrentPitch = pitchable.speed();
//		kdDebug(66666) << "[KJPitchBMP] mCurrentPitch: " << mCurrentPitch << endl;
	}

	if ( mCurrentPitch == mOldPitch ) // dont redraw if nothing changed
		return;

	mOldPitch = mCurrentPitch;

	repaint();
}

void KJPitchBMP::newFile()
{
//	kdDebug(66666) << "[KJPitchBMP] newFile()" << endl;

	Arts::PlayObject playobject = napp->player()->engine()->playObject();
	Arts::PitchablePlayObject pitchable = Arts::DynamicCast(playobject);

	if (!pitchable.isNull())
	{
//		kdDebug(66666) << "[KJPitchBMP] new speed: " << mCurrentPitch << endl;
		pitchable.speed( mCurrentPitch );
	}
}

void KJPitchBMP::readConfig()
{
//	kdDebug(66666) << "KJPitchBMP::readConfig()" << endl;

	mMinPitch = KJLoader::kjofol->prefs()->minimumPitch() / 100.0;
	mMaxPitch = KJLoader::kjofol->prefs()->maximumPitch() / 100.0;

	// Now comes the range checking if the user changed the setting :)
	if ( mCurrentPitch < mMinPitch || mCurrentPitch > mMaxPitch )
	{
		if ( mCurrentPitch < mMinPitch  )
			mCurrentPitch = mMinPitch;
		if ( mCurrentPitch > mMaxPitch )
			mCurrentPitch = mMaxPitch;
		newFile(); // wrong naming, in fact it just sets pitch
	}
}
