#ifndef KJBACKGROUND_H
#define KJBACKGROUND_H

#include "kjwidget.h"
//#include "kjloader.h"
class KJLoader;

#include <qpainter.h>

class KJBackground : public KJWidget
{
public:
	KJBackground(KJLoader *);
	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &) {return false;}
	virtual void mouseRelease(const QPoint &, bool) {}

private:
	QPixmap mBackground;
};
#endif
