/***************************************************************************
	kjwidget.cpp  -  Base Class for all widgets
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "kjwidget.h"
//#include <kdebug.h>

// ugly static functions and stuff
#include "helpers.cpp"

#include <qpainter.h>

KJWidget::KJWidget(KJLoader *p) : mParent(p)
{
}

QBitmap KJWidget::getMask(const QImage &_rect, register QRgb transparent)
{
	QImage result(_rect.width(), _rect.height(), 1,2, QImage::LittleEndian);
#if QT_VERSION < 0x030300
	result.setColor(0, qRgb(0,0,0));  //TODO: maybe use Qt::color0 and Qt::color1
	result.setColor(1, qRgb(255,255,255));
#else
	result.setColor(1, qRgb(0,0,0));
	result.setColor(0, qRgb(255,255,255));
#endif

	for(int height=0;height<_rect.height(); height++)
	{
		for(int width=0; width<_rect.width(); width++)
			setPixel1BPP(result, width, height, _rect.pixel(width, height)!=transparent);
	}
	QBitmap bm;
	bm.convertFromImage(result);
	return bm;
}

void KJWidget::repaint(bool me, const QRect &r, bool clear)
{
	QPainter p(parent());
	if (me)
		paint(&p, r.isValid() ? r : rect());
	else
		parent()->repaint(r.isValid() ? r : rect(), clear);
}

const QString &KJWidget::backgroundPressed(const QString &bmp) const
{
	if(bmp.isEmpty()) // play safe
	{
//		kdDebug(66666) << k_funcinfo << "empty argument 'bmp', returning QString::null!" << endl;
		return QString::null;
	}

//	kdDebug(66666) << k_funcinfo << "Returning pressed pixmap for '" << bmp.latin1() << "'" << endl;

	// make absolutely sure the wanted backgroundimagepressedX line is there
	QStringList item = parser()["backgroundimagepressed"+QString::number(bmp.mid(3).toInt())];
	if(item.count() < 2)
	{
//		kdDebug(66666) << k_funcinfo << "backgroundimagepressed doesn't have enough keys in its line!" << endl;
		return QString::null;
	}
	else
		return item[1];
}
