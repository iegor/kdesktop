#ifndef KJSLIDERS_H
#define KJSLIDERS_H

#include "kjwidget.h"
#include <qpainter.h>

class KJLoader;
class KJPitchText;
class KJVolumeText;


class KJVolumeBMP : public KJWidget
{
public:
	KJVolumeBMP(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void timeUpdate(int);
	virtual void mouseMove(const QPoint &pos, bool);

	virtual QString tip();

	void setText(KJVolumeText *t) { mText=t; }

private:
	QPixmap mImages;
	QImage mPos;
	int mVolume, mOldVolume;
	int mWidth, mCount;
	KJVolumeText *mText;
};


class KJVolumeBar : public KJWidget
{
public:
	KJVolumeBar(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void timeUpdate(int);
	virtual void mouseMove(const QPoint &pos, bool);

	virtual QString tip();

	void setText(KJVolumeText *t) { mText=t; }

private:
	QPixmap mSlider;
	QPixmap mBack;
	int mVolume;
	KJVolumeText *mText;
};


class KJPitchBMP : public KJWidget
{
public:
	KJPitchBMP(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void timeUpdate(int);
	virtual void newFile();
	virtual void mouseMove(const QPoint &pos, bool);
	virtual void readConfig();
	
	virtual QString tip();

	void setText(KJPitchText *t) { mText=t; }

private:
	QPixmap mImages;
	QImage mPos;
	int mWidth, mCount;
	float mCurrentPitch;
	float mOldPitch;
	float mMinPitch;
	float mMaxPitch;

	KJPitchText *mText;
};

#endif
