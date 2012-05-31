#ifndef KJSEEKER_H
#define KJSEEKER_H

#include "kjwidget.h"
//#include "kjloader.h"
class KJLoader;

#include <qpainter.h>

class KJSeeker : public KJWidget
{
public:
	KJSeeker(const QStringList &i, KJLoader *);
	~KJSeeker();


	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);

	void timeUpdate(int mille);

	void closest();

private:
	QPixmap *toPixmap(int n);
	
private:
	QImage mScale;
	QImage mActive;
	QPixmap *barmode[256];
	QImage *barmodeImages[256];
	QBitmap barModeMask;
	int g;
};

#endif
