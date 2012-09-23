/****************************************************************************
	Copyright (C) 2002 Charles Samuels
	this file is on the BSD license, sans advertisement clause
 *****************************************************************************/

#include <noatun/video.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/engine.h>

#include <qpopupmenu.h>
#include <kaction.h>
#include <klocale.h>

#include "globalvideo.h"

// sorry :)
QPtrList<VideoFrame> VideoFrame::frames;

VideoFrame *VideoFrame::whose=0;

struct VideoFrame::Private
{
};


VideoFrame::VideoFrame(KXMLGUIClient *clientParent, QWidget *parent, const char*name, WFlags f)
	: KVideoWidget(clientParent, parent, name, f)
{
	d = new Private;
	connect(napp->player(), SIGNAL(newSong()), SLOT(changed()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(stopped()));
	frames.append(this);
}

VideoFrame::VideoFrame(QWidget *parent, const char *name, WFlags f)
	 : KVideoWidget(parent, name, f)
{
	d = new Private;
	connect(napp->player(), SIGNAL(newSong()), SLOT(changed()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(stopped()));
	frames.append(this);
}

VideoFrame::~VideoFrame()
{
	if (whose == this)
	{
		embed(Arts::VideoPlayObject::null());
		whose=0;
	}

	frames.removeRef(this);
	VideoFrame *l = frames.last();
	if (l) l->give();
	else whose=0;
	delete d;
}

VideoFrame *VideoFrame::playing()
{
	return whose;
}

QPopupMenu *VideoFrame::popupMenu(QWidget *parent)
{
    QPopupMenu *view = new QPopupMenu(parent);
    action( "half_size" )->plug( view );
    action( "normal_size" )->plug( view );
    action( "double_size" )->plug( view );
    view->insertSeparator();
    action( "fullscreen_mode" )->plug( view );
	return view;
}

void VideoFrame::give()
{
	VideoFrame *old=whose;
	whose = this;
	
	if (whose != old && old != 0)
	{
		old->embed(Arts::VideoPlayObject::null());
		emit old->lost();
	}

	Arts::PlayObject po = napp->player()->engine()->playObject();
	if (po.isNull()) return;
	
	Arts::VideoPlayObject vpo = Arts::DynamicCast(po);
	if (!vpo.isNull())
	{
		embed(vpo);
		emit acquired();
	}
}

void VideoFrame::changed()
{
	if (whose==this)
		give();
}

void VideoFrame::stopped()
{
	if (whose==this)
	{
		embed(Arts::VideoPlayObject::null());
		emit lost();
	}
}

#include <qlayout.h>


GlobalVideo::GlobalVideo()
    : QWidget( 0, 0, WType_TopLevel | WStyle_Customize | WStyle_DialogBorder | WStyle_Title )
{
	setCaption(i18n("Video - Noatun"));
	(new QVBoxLayout(this))->setAutoAdd(true);
	video = new VideoFrame(this);
	menu = video->popupMenu(this);

	// FIXME: How to obtain minimum size for top-level widgets?
//	video->setMinimumSize(minimumSizeHint());
//	video->setMinimumSize(101,35);
	video->setMinimumSize(128,96);

	connect(video, SIGNAL(acquired()), SLOT(appear()));
	connect(video, SIGNAL(lost()), SLOT(hide()));
	connect(video, SIGNAL(adaptSize(int,int)), this, SLOT(slotAdaptSize(int,int)));

	video->setNormalSize();
	video->give();
}

void GlobalVideo::slotAdaptSize(int w, int h)
{
    resize(w, h);
}

void GlobalVideo::appear()
{
	QWidget::show();
}

void GlobalVideo::hide()
{
	QWidget::hide();
}

void GlobalVideo::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == RightButton)
	{
		menu->exec(mapToGlobal(e->pos()));
	}
}

#include "globalvideo.moc"
#include "video.moc"

