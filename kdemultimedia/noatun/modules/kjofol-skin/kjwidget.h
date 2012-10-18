#ifndef KJWIDGET_H
#define KJWIDGET_H

#include "kjloader.h"

class KJWidget
{
public:
	KJWidget(KJLoader *);
	virtual ~KJWidget() {};
	// called when the widget should paint
	virtual void paint(QPainter *, const QRect &) {};
	// called to receive the rect this widget is in
	virtual QRect rect() const { return mRect; }
	// called when pressed in this widget
	virtual bool mousePress(const QPoint &) {return false; }
	// called when the mouse is released after clicked in this widget
	virtual void mouseRelease(const QPoint &, bool){}
	virtual void mouseMove(const QPoint &, bool) {}
	// called with the current time (mille)
	virtual void timeUpdate(int) {}
	// called when a new song is playing, player() is ready with it too
	virtual void newFile() {}
	// called when config-entries have to be read, is a TODO for most widgets
	virtual void readConfig() {}

	// called when the mouse is moved while clicked in this widget
	// repaint myself
	virtual void repaint(bool me=true, const QRect &rect=QRect(), bool clear=false);

	virtual QString tip() { return 0; }

public:
	static QBitmap getMask(const QImage &color, register QRgb=qRgb(255,0,255));

protected:
	const QString &backgroundPressed(const QString &bmp) const;
	KJLoader *parent() const {return mParent;}
	KJLoader &parser() const {return *mParent;}

	KJFont &textFont() const {return *mParent->mText;}
	KJFont &timeFont() const {return *mParent->mNumbers;}
	KJFont &volumeFont() const {return *mParent->mVolumeFont;}
	KJFont &pitchFont() const {return *mParent->mPitchFont;}

	void setRect(const QRect& rect) {mRect=rect;}
	void setRect(int x, int y, int xs, int ys) {mRect=QRect(x,y,xs,ys);}
private:
	KJLoader *mParent;
	QRect mRect;
};

#endif
