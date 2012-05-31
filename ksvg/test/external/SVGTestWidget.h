#ifndef _SVGTESTWIDGET_H
#define _SVGTESTWIDGET_H

#include <qwidget.h>
#include <qobject.h>
#include "SVGDocument.h"
#include "KSVGCanvas.h"

class SVGTestWidget : public QWidget
{
Q_OBJECT
public:
	SVGTestWidget(const KURL &url);
	~SVGTestWidget();

private slots:
	void slotRenderingFinished();

protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
/*	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
*/
private:
	QPoint m_panningPos;
	QPoint m_oldPanningPos;
	
	KSVG::SVGDocument *m_doc;
	KSVG::KSVGCanvas *m_canvas;
};

#endif

