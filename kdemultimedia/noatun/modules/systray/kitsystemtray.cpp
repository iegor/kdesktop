// $Id: kitsystemtray.cpp 363781 2004-11-17 21:22:13Z adridg $
//
// Kit
//
// Copyright (C) 1999 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "yhconfig.h"

#include <noatun/app.h>
#include <noatun/player.h>
#include <qdragobject.h>

#include "kitsystemtray.h"
#include <kmainwindow.h>
#include <kpopupmenu.h>
#include <kxmlguifactory.h>
#include <kiconloader.h>
#include <kurldrag.h>

#include <fixx11h.h>

KitSystemTray::KitSystemTray(const QString &contextMenu, KMainWindow *parent, const char *name)
	: KSystemTray(parent, name)
{
	setAlignment(AlignHCenter | AlignVCenter);
	menu = (KPopupMenu *)parent->guiFactory()->container(contextMenu, parent);
	menu->insertTitle(SmallIcon("noatun"), QString::null, 0, 0);
	setAcceptDrops(true);
}

void KitSystemTray::changeTitle(const QPixmap &pixmap, const QString &title)
{
	menu->changeTitle(0, pixmap, title);
}

void KitSystemTray::showEvent(QShowEvent *)
{
	// empty
}

void KitSystemTray::mousePressEvent(QMouseEvent *event)
{
	switch(event->button())
	{
		case LeftButton:
			napp->toggleInterfaces();
			break;
		case MidButton:
			if (YHConfig::self()->middleMouseAction() == YHConfig::HideShowPlaylist)
				napp->playlist()->toggleList();
			else // play or pause
				napp->player()->playpause();
			break;
		default:
			menu->popup(event->globalPos());
			break;
	}
}

void KitSystemTray::dragEnterEvent(QDragEnterEvent * event)
{
	event->accept(KURLDrag::canDecode(event)); // accept uri drops only
}

void KitSystemTray::dropEvent(QDropEvent * event)
{
	KURL::List uris;
	if (KURLDrag::decode(event, uris))
	{
		KURL::List::ConstIterator it;
		for (it = uris.begin(); it != uris.end(); ++it)
			napp->player()->openFile(*it, false);
	}
}

void KitSystemTray::wheelEvent(QWheelEvent *event)
{
	YHConfig *c = YHConfig::self();

	int action = 0;
	if (event->state() & Qt::ShiftButton)
		action = c->mouseWheelAction(YHConfig::Shift);
	else if (event->state() & Qt::ControlButton)
		action = c->mouseWheelAction(YHConfig::Ctrl);
	else if (event->state() & Qt::AltButton)
		action = c->mouseWheelAction(YHConfig::Alt);
	else
		action = c->mouseWheelAction(YHConfig::None);

	switch(action)
	{
		case (YHConfig::ChangeVolume):
			napp->player()->setVolume(napp->player()->volume()+event->delta()/24);
			break;
		case (YHConfig::ChangeTrack):
			if (event->delta() > 0)
				napp->player()->forward(true);
			else
				napp->player()->back();
			break;
		default:
			break;
	}
}

#include "kitsystemtray.moc"
