#ifndef REGIONWIDGET_H
#define REGIONWIDGET_H

#include <qframe.h>

/**
 * show a widget with a field rectangle, and a mini-region inside
 * 
 * text boxes allow the region to be resized and moved, along with
 * click-n-drag
 **/
class RegionWidget : public QFrame
{
Q_OBJECT
public:
	RegionWidget(QWidget *parent);
	RegionWidget(const QSize &viewsize, QWidget *parent);
	RegionWidget(int x, int y, int w, int h, const QSize &viewsize, 
	             QWidget *parent);
	RegionWidget(const QRect &region, const QSize &viewsize, QWidget *parent);
	
	~RegionWidget();
	
	QRect region() const;
	
public slots:
	void setX(int x);
	void setY(int y);
	void setWidth(int w);
	void setHeight(int h);
	
	void setRegion(const QRect &region);
	
	void setViewSize(const QSize &size);

signals:
	void changed();
	void changed(int x, int y, int w, int h);
	void changed(const QRect &region);

protected:
	virtual void moved(int x, int y);
	virtual void resized(int w, int h);
};


#endif
