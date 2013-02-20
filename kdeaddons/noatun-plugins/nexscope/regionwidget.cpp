
#include "regionwidget.h"

RegionWidget::RegionWidget(QWidget *parent)
{

}

RegionWidget::RegionWidget(const QSize &viewsize, QWidget *parent)
{

}
RegionWidget::RegionWidget(int x, int y, int w, int h, const QSize &viewsize,
                           QWidget *parent)
{

}

RegionWidget::RegionWidget(const QRect &region, const QSize &viewsize,
                           QWidget *parent)
{

}

RegionWidget::~RegionWidget()
{

}


QRect RegionWidget::region() const
{
	return QRect(0,0,0,0);
}

void RegionWidget::setX(int x)
{

}

void RegionWidget::setY(int y)
{

}

void RegionWidget::setWidth(int w)
{

}

void RegionWidget::setHeight(int h)
{

}

void RegionWidget::setRegion(const QRect &region)
{

}

void RegionWidget::setViewSize(const QSize &size)
{

}

void RegionWidget::moved(int x, int y)
{

}

void RegionWidget::resized(int w, int h)
{

}

#include "regionwidget.moc"

