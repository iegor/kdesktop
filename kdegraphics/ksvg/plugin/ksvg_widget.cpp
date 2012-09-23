/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kcursor.h>
#include <kpopupmenu.h>
#include <kxmlguifactory.h>
#include "ksvg_widget.moc"

#include "ksvg_plugin.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

KSVGWidget::KSVGWidget(KSVGPlugin *part, QWidget *parent, const char *name)
: QWidget(parent, name), m_part(part)
{
	setMouseTracking(true);	
	setFocusPolicy(WheelFocus);

	setBackgroundMode(NoBackground);

	reset();
}

KSVGWidget::~KSVGWidget()
{
}

KSVGPlugin *KSVGWidget::part() const
{
	return m_part;
}

void KSVGWidget::reset()
{
	m_oldPanningPos.setX(0);
	m_oldPanningPos.setY(0);
	m_panningPos.setX(0);
	m_panningPos.setY(0);
}

void KSVGWidget::paintEvent(QPaintEvent *e)
{
	emit redraw(e->rect());
}

KSVG::SVGMouseEventImpl *KSVGWidget::newMouseEvent(KSVG::SVGEvent::EventId id, QMouseEvent *event)
{
	DOM::AbstractView temp;

	int clientX = event->x();
	int clientY = event->y();

	if(part()->docImpl() && part()->docImpl()->rootElement())
	{
		clientX = int(clientX / part()->docImpl()->rootElement()->currentScale());
		clientY = int(clientY / part()->docImpl()->rootElement()->currentScale());
	}

	int button = 0;
	if(event->stateAfter() & Qt::LeftButton)
		button = 1;
	else if(event->stateAfter() & Qt::MidButton)
		button = 2;
	else if(event->stateAfter() & Qt::RightButton)
		button = 3;

	KSVG::SVGMouseEventImpl *mev = new KSVG::SVGMouseEventImpl(id, // type
												   true, // canBubbleArg
												   true, // cancelableArg
												   temp, // view
												   0, // detail
												   event->globalX(), //screenXArg
												   event->globalY(), // screenYArg,
												   clientX, // clientXArg
												   clientY, // clientYArg
												   (event->state() & Qt::ControlButton), // ctrlKeyArg
												   (event->state() & Qt::AltButton), // altKeyArg
												   (event->state() & Qt::ShiftButton), // shiftKeyArg
												   (event->state() & Qt::MetaButton), // metaKeyArg
												   button, // buttonArg
												   0);

	mev->ref();

	return mev;
}

void KSVGWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(event->state() & QMouseEvent::ControlButton && event->state() & QMouseEvent::LeftButton)
	{
		if(m_panningPos.isNull())
			m_panningPos = event->pos();
		else
			part()->setPanPoint(m_oldPanningPos - (m_panningPos - event->pos()));

		return;
	}
	else if(event->state() & QMouseEvent::ControlButton)
		return;

	KSVG::SVGMouseEventImpl *mev = newMouseEvent(KSVG::SVGEvent::MOUSEMOVE_EVENT, event);

	if(part()->docImpl() && part()->docImpl()->rootElement())
		part()->docImpl()->rootElement()->prepareMouseEvent(event->pos(), event->pos(), mev);

	if(mev->target() && mev->url().string().isEmpty())
	{
		KSVG::SVGElementImpl *target = const_cast<KSVG::SVGElementImpl *>(mev->target());
		KSVG::SVGStylableImpl *style = dynamic_cast<KSVG::SVGStylableImpl *>(target);

		if(!style)
		{
			setCursor(KCursor::arrowCursor());
			return;
		}

		switch(style->getCursor())
		{
			case KSVG::CURSOR_CROSSHAIR:
				setCursor(KCursor::crossCursor());
				break;			
			case KSVG::CURSOR_POINTER:
				setCursor(KCursor::handCursor());
				break;
			case KSVG::CURSOR_MOVE:
				setCursor(KCursor::sizeAllCursor());
				break;
			case KSVG::CURSOR_E_RESIZE:
			case KSVG::CURSOR_W_RESIZE:
				setCursor(KCursor::sizeHorCursor());
				break;
			case KSVG::CURSOR_N_RESIZE:
			case KSVG::CURSOR_S_RESIZE:
				setCursor(KCursor::sizeVerCursor());
				break;
			case KSVG::CURSOR_NW_RESIZE:
			case KSVG::CURSOR_SE_RESIZE:
				setCursor(KCursor::sizeFDiagCursor());
				break;
			case KSVG::CURSOR_NE_RESIZE:
			case KSVG::CURSOR_SW_RESIZE:
				setCursor(KCursor::sizeBDiagCursor());
				break;
			case KSVG::CURSOR_TEXT:
				setCursor(KCursor::ibeamCursor());
				break;
			case KSVG::CURSOR_WAIT:
				setCursor(KCursor::waitCursor());
				break;
			case KSVG::CURSOR_HELP:
				setCursor(KCursor::whatsThisCursor());
				break;
			default:
				setCursor(KCursor::arrowCursor());
		}
	}
	else if(mev->url().string().isEmpty())
		setCursor(KCursor::arrowCursor());

	if(!mev->url().string().isEmpty())
		setCursor(KCursor::handCursor());

	mev->deref();
}

void KSVGWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->state() & QMouseEvent::ControlButton)
		return;

	if(event->button() == RightButton)
	{
		if(part() && part()->factory())
		{
			QPopupMenu *popup = static_cast<QPopupMenu *>(part()->factory()->container("popupmenu", part()));
			if(popup)	
				popup->popup(event->globalPos());
		}
	}

	KSVG::SVGMouseEventImpl *mev = newMouseEvent(KSVG::SVGEvent::MOUSEDOWN_EVENT, event);

	if(part()->docImpl() && part()->docImpl()->rootElement())
		part()->docImpl()->rootElement()->prepareMouseEvent(event->pos(), event->pos(), mev);

	mev->deref();
}

void KSVGWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(!m_panningPos.isNull())
	{
		m_oldPanningPos = m_oldPanningPos - (m_panningPos - event->pos());
		m_panningPos.setX(0);
		m_panningPos.setY(0);
	}

	if(event->state() & QMouseEvent::ControlButton)
		return;

	KSVG::SVGMouseEventImpl *mev = newMouseEvent(KSVG::SVGEvent::MOUSEUP_EVENT, event);

	if(part()->docImpl() && part()->docImpl()->rootElement())
		part()->docImpl()->rootElement()->prepareMouseEvent(event->pos(), event->pos(), mev);

	if(!mev->url().string().isEmpty())
	{
		QString url = mev->url().string();
		if(url.startsWith("#")) 
			url.prepend(part()->docImpl()->baseUrl().prettyURL());
		emit browseURL(url);
	}

 	mev->deref();
}

void KSVGWidget::keyPressEvent(QKeyEvent *ke)
{
	if(ke->stateAfter() & QMouseEvent::ControlButton)
	{
		setCursor(KCursor::sizeAllCursor());
		return;
	}

	if(part()->docImpl()->lastTarget())
		part()->docImpl()->lastTarget()->dispatchKeyEvent(ke);
}

void KSVGWidget::keyReleaseEvent(QKeyEvent *ke)
{
	if(ke->state() & QMouseEvent::ControlButton)
	{
		setCursor(KCursor::arrowCursor());
		return;
	}

	if(part()->docImpl()->lastTarget())
		part()->docImpl()->lastTarget()->dispatchKeyEvent(ke);
}

void KSVGWidget::resizeEvent(QResizeEvent *e)
{
	if(part()->docImpl() && part()->docImpl()->rootElement())
		part()->docImpl()->rootElement()->dispatchEvent(KSVG::SVGEvent::RESIZE_EVENT, true, false);

	emit redraw(QRect(0, 0, e->size().width(), e->size().height()));
}

// vim:ts=4:noet
