#ifndef __GLOBALVIDEO_H
#define __GLOBALVIDEO_H

#include "noatun/video.h"


class GlobalVideo : public QWidget
{
Q_OBJECT
	QPopupMenu *menu;
	VideoFrame *video;

public:
	GlobalVideo();

public slots:
	void appear();
	void hide();
	void slotAdaptSize(int w, int h);

protected:
	void mouseReleaseEvent(QMouseEvent *e);
};


#endif
