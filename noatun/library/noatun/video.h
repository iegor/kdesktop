#ifndef NOATUN__VIDEO_H
#define NOATUN__VIDEO_H

#include <kvideowidget.h>
#include <arts/kmedia2.h>

class QPopupMenu;

/**
 * a widget that contains the video being played
 **/
class VideoFrame : public KVideoWidget
{
Q_OBJECT
	struct Private;
	VideoFrame::Private *d;
	
	static QPtrList<VideoFrame> frames;
	static VideoFrame *whose;

public:
	VideoFrame(KXMLGUIClient *clientParent, QWidget *parent=0, const char *name=0, WFlags f=0);
	VideoFrame(QWidget *parent = 0, const char *name=0, WFlags f=0);
	~VideoFrame();
	
	/**
	 * which one has the video (or will have it next, if no video is playing)
	 **/
	static VideoFrame *playing();
	
	QPopupMenu *popupMenu(QWidget *parent);
	QPopupMenu *popupMenu() { return popupMenu(this); }
	
public slots:
	/**
	 * only one VideoFrame can be playing a video, make this the one
	 **/
	void give();

private slots:
	void stopped();
	void changed();
	
signals:
	/**
	 * signaled when video is playing in here, when
	 * (width*height) != 0
	 **/
	void acquired();
	/**
	 * signaled when video is no longer playing
	 * here, when (width*heoght) == 0
	 */
	void lost();
};



#endif

