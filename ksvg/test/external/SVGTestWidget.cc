// Very small test app to look at ksvg performance without konqueror overhead (Rob)

#include "SVGTestWidget.moc"
#include "DocumentFactory.h"
#include "CanvasFactory.h"
#include "SVGCircleElement.h"
#include "SVGAnimatedLength.h"
#include "SVGLength.h"
#include "SVGSVGElement.h"
#include "SVGEventImpl.h"
#include "KSVGCanvas.h"
#include "CanvasItem.h"
#include <kcursor.h>

using namespace KSVG;

SVGTestWidget::SVGTestWidget(const KURL &url)
{
	setBackgroundColor(Qt::white);
	setMouseTracking(true);	
	setFocusPolicy(WheelFocus);

	m_doc = DocumentFactory::self()->requestDocument(this, SLOT(slotRenderingFinished()));
	if(!m_doc)
		return;	

	resize(450, 450);
	m_canvas = CanvasFactory::self()->loadCanvas(450, 450);
	if(!m_canvas)
		return;

	m_canvas->setup(this, this);

	if(!DocumentFactory::self()->attachCanvas(m_canvas, m_doc))
		return;

	if(!DocumentFactory::self()->startParsing(m_doc, url))
		return;
}

SVGTestWidget::~SVGTestWidget()
{
	delete m_canvas;
	delete m_doc;
}

void SVGTestWidget::slotRenderingFinished()
{
	QRect rect(0, 0, width(), height());
	m_canvas->blit(rect, true);

	SVGElement test = m_doc->rootElement().getElementById("test");
	if(test.nodeName() == "circle")
	{
		SVGCircleElement c = dom_cast(SVGCircleElement, test);
		c.r().baseVal().setValue(150);
		c.setAttribute("fill", "blue");
	}
}

void SVGTestWidget::paintEvent(QPaintEvent *event)
{
	m_canvas->update();
	m_canvas->blit(event->rect(), true);
}

void SVGTestWidget::resizeEvent(QResizeEvent *event)
{
	int w = event->size().width();
	int h = event->size().height();

	m_canvas->resize(w, h);
	resize(w, h);
	m_canvas->blit();
}
/*
KSVG::SVGMouseEventImpl *newMouseEvent(SVGDocument *doc, KSVG::SVGEventImpl::EventId id, QMouseEvent *event)
{
	DOM::AbstractView temp;

	int clientX = event->x();
	int clientY = event->y();

	if(doc && doc->rootElement())
	{
		clientX = int(clientX / doc->rootElement()->currentScale());
		clientY = int(clientY / doc->rootElement()->currentScale());
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

void SVGTestWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(event->state() & QMouseEvent::ControlButton && event->state() & QMouseEvent::LeftButton)
	{
		if(m_panningPos.isNull())
			m_panningPos = event->pos();
		else
		{
			QPoint panPoint = m_oldPanningPos - (m_panningPos - event->pos());
			m_doc->rootElement()->setCurrentTranslate(panPoint);
//			m_doc->syncCachedMatrices(); FIXME
			m_canvas->update(panPoint);
		}
		return;
	}
	else if(event->state() & QMouseEvent::ControlButton)
		return;
}

void SVGTestWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->state() & QMouseEvent::ControlButton)
		return;

	KSVG::SVGMouseEventImpl *mev = newMouseEvent(m_doc, KSVG::SVGEventImpl::MOUSEDOWN_EVENT, event);

	if(m_doc && m_doc->rootElement())
		m_doc->rootElement()->prepareMouseEvent(event->pos(), event->pos(), mev);
	mev->deref();
}

void SVGTestWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(!m_panningPos.isNull())
	{
		m_oldPanningPos = m_oldPanningPos - (m_panningPos - event->pos());
		m_panningPos.setX(0);
		m_panningPos.setY(0);
	}

	if(event->state() & QMouseEvent::ControlButton)
		return;

	// only simulate mouse clicks for now
	KSVG::SVGMouseEventImpl *mev = newMouseEvent(m_doc, KSVG::SVGEventImpl::MOUSEUP_EVENT, event);

	if(m_doc && m_doc->rootElement())
		m_doc->rootElement()->prepareMouseEvent(event->pos(), event->pos(), mev);
	mev->deref();
}

void SVGTestWidget::keyPressEvent(QKeyEvent *event)
{
	if(event->stateAfter() & QMouseEvent::ControlButton)
	{
		setCursor(KCursor::sizeAllCursor());
		return;
	}
}

void SVGTestWidget::keyReleaseEvent(QKeyEvent *event)
{
	if(event->state() & QMouseEvent::ControlButton)
	{
		setCursor(KCursor::arrowCursor());
		return;
	}
	SVGSVGElementImpl *root = m_doc->rootElement();
	if(!root) return;
	if(event->key() == Qt::Key_Minus)
	{
		erase(0, 0, m_canvas->width(), m_canvas->height());
		float zoomFactor = root->currentScale() / 1.2;
		root->setCurrentScale(zoomFactor);
		m_doc->syncCachedMatrices();
		m_canvas->update(zoomFactor);
	}
	else if(event->key() == Qt::Key_Plus)
	{
		float zoomFactor = root->currentScale() * 1.2;
		root->setCurrentScale(zoomFactor);
		m_doc->syncCachedMatrices();
		m_canvas->update(zoomFactor);
	}
}
*/

// vim:ts=4:noet
