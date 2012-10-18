#ifndef KJEQUALIZER_H
#define KJEQUALIZER_H

#include "kjwidget.h"
//#include "kjloader.h"
class KJLoader;
class VInterpolation;

#include <qobject.h>

class KJEqualizer : public QObject, public KJWidget
{
Q_OBJECT
public:
	KJEqualizer(const QStringList &, KJLoader *parent);
	~KJEqualizer(void);

	virtual void mouseMove(const QPoint &pos, bool);
	virtual bool mousePress(const QPoint&);
	virtual void paint(QPainter *p, const QRect &rect);
	int barNum(const QPoint &pos) const;
	int level(const QPoint &pos) const;

public slots:
	void slotUpdateBuffer();

private:
	int mBands;
	int mXSpace;

	int mBandWidth;
	int mBandHalfHeight;
	QPixmap mBars; // holds all slider images
	QPixmap *mBack; // holds background of EQ for easy repaint
	QPixmap *mView; // holds prepared img of all sliders
	VInterpolation *mInterpEq;
};

#endif
