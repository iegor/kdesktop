/***************************************************************************
                          glowbutton.cpp  -  description
                             -------------------
    begin                : Thu Sep 6 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <math.h>
#include <iostream>
#include <vector>
#include <qmap.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <kdecoration.h>
#include <kiconeffect.h>
#include "glowbutton.h"

namespace Glow
{

//-----------------------------------------------------------------------------
// PixmapCache
//-----------------------------------------------------------------------------

QMap<QString, const QPixmap*> PixmapCache::m_pixmapMap;

const QPixmap* PixmapCache::find(const QString& key)
{
	QMap<QString, const QPixmap*>::const_iterator it =
		m_pixmapMap.find(key);
	if( it != m_pixmapMap.end() )
		return *it;
	else
		return 0;
}

void PixmapCache::insert(const QString& key, const QPixmap *pixmap)
{
	m_pixmapMap[key] = pixmap;
}

void PixmapCache::erase(const QString& key)
{
	QMap<QString, const QPixmap*>::iterator it =
		m_pixmapMap.find(key);
	if (it != m_pixmapMap.end())
	{
		delete *it;
		m_pixmapMap.erase(it);
	}
}

void PixmapCache::clear()
{
	// delete all pixmaps in the cache
	QMap<QString, const QPixmap*>::const_iterator it
		= m_pixmapMap.begin();
	for(; it != m_pixmapMap.end(); ++it)
		delete *it;
	m_pixmapMap.clear();
}

//-----------------------------------------------------------------------------
// GlowButton
//-----------------------------------------------------------------------------

GlowButton::GlowButton(QWidget *parent, const char *name,
	const QString& tip, const int realizeBtns)
	: QButton(parent, name)
{
	m_realizeButtons = realizeBtns;

	_steps = 0;
	m_updateTime = 50;
	m_pixmapName = QString::null;

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
	m_pos = 0;
	m_timerStatus = Stop;

	setTipText (tip);
	setCursor(arrowCursor);
}

GlowButton::~GlowButton()
{
}

void GlowButton::setTipText( const QString& tip )
{
	if (KDecoration::options()->showTooltips())
	{
	    QToolTip::remove( this );
	    QToolTip::add( this, tip );
	}
}

QString GlowButton::getPixmapName() const
{
	return m_pixmapName;
}

Qt::ButtonState GlowButton::lastButton() const
{
	return _last_button;
}

void GlowButton::setPixmapName(const QString& pixmapName)
{
	m_pixmapName = pixmapName;

	const QPixmap *pixmap = PixmapCache::find(pixmapName);
	if( ! pixmap )
		return;

	// set steps
	_steps = pixmap->height()/pixmap->width() - 1;

	repaint(false);
}

void GlowButton::paintEvent( QPaintEvent *e )
{
 	QWidget::paintEvent(e);
	const QPixmap *pixmap = PixmapCache::find(m_pixmapName);
	if( pixmap != 0 )
	{
		int pos = m_pos>=0?m_pos:-m_pos;
		QPainter p;
		QPixmap pm (pixmap->size());
		p.begin(&pm);
		const QPixmap * bg_pixmap = PixmapCache::find(
				QString::number(parentWidget()->winId()));
		p.drawPixmap (0, 0, *bg_pixmap, x(), y(), width(), height());
		p.drawPixmap (0, 0, *pixmap, 0, pos*height(), width(), height());
		p.end();
		p.begin(this);
		p.drawPixmap (0, 0, pm);
		p.end();
	}
}

void GlowButton::enterEvent( QEvent *e )
{
	if( m_pos<0 )
		m_pos=-m_pos;
	m_timerStatus = Run;
	if( ! m_timer->isActive() )
		m_timer->start(m_updateTime);
	QButton::enterEvent(e);
}

void GlowButton::leaveEvent( QEvent *e )
{
	m_timerStatus = Stop;
	if( ! m_timer->isActive() )
		m_timer->start(m_updateTime);
	QButton::leaveEvent(e);
}

void GlowButton::mousePressEvent( QMouseEvent *e )
{
	_last_button = e->button();
	if( m_timer->isActive() )
		m_timer->stop();
	m_pos = _steps;
	repaint(false);
	// without pretending LeftButton, clicking on the button with MidButton
	// or RightButton would cause unwanted titlebar action
	QMouseEvent me (e->type(), e->pos(), e->globalPos(),
			(e->button()&m_realizeButtons)?LeftButton:NoButton, e->state());
	QButton::mousePressEvent(&me);
}

void GlowButton::mouseReleaseEvent( QMouseEvent *e )
{
	_last_button = e->button();
	QPoint p = mapToParent(mapFromGlobal(e->globalPos()));
	if( ! m_timer->isActive() ) {
		m_timer->start(m_updateTime);
	}
	if( ! geometry().contains(p) ) {
		m_timerStatus = Stop;
	}
	QMouseEvent me (e->type(), e->pos(), e->globalPos(),
			(e->button()&m_realizeButtons)?LeftButton:NoButton, e->state());
	QButton::mouseReleaseEvent(&me);
}

void GlowButton::slotTimeout()
{
	repaint(false);

	if( m_pos>=_steps-1 ) {
		m_pos = -m_pos;
	}
	if( m_timerStatus==Stop ) {
		if( m_pos==0 ) {
			m_timer->stop();
			return;
		} else if( m_pos>0 ) {
			m_pos = -m_pos;
		}
	}

	m_pos++;
}

//-----------------------------------------------------------------------------
// GlowButtonFactory
//-----------------------------------------------------------------------------

GlowButtonFactory::GlowButtonFactory()
{
	_steps = 20;
}

int GlowButtonFactory::getSteps()
{
	return _steps;
}

void GlowButtonFactory::setSteps(int steps)
{
	_steps = steps;
}

QPixmap * GlowButtonFactory::createGlowButtonPixmap(
				const QImage & bg_image,
				const QImage & fg_image,
				const QImage & glow_image,
				const QColor & color,
				const QColor & glow_color)
{
		if (bg_image.size() != fg_image.size()
			|| fg_image.size() != glow_image.size()) {
				std::cerr << "Image size error" << std::endl;
				return new QPixmap();
		}

		QImage colorized_bg_image = bg_image.copy();
		KIconEffect::colorize (colorized_bg_image, color, 1.0);

		int w = colorized_bg_image.width();
		int h = colorized_bg_image.height();

		QImage image (w, (_steps+1)*h, 32);
		image.setAlphaBuffer (true);
		for (int i=0; i<_steps+1; ++i) {
			for (int y=0; y<h; ++y) {
				uint * src1_line = (uint*) colorized_bg_image.scanLine (y);
				uint * src2_line = (uint*) fg_image.scanLine (y);
				uint * dst_line = (uint*) image.scanLine (i*h+y);
				for (int x=0; x<w; ++x) {
					int r = qRed (*(src1_line+x));
					int g = qGreen (*(src1_line+x));
					int b = qBlue (*(src1_line+x));
					int a = QMAX (qAlpha(*(src1_line+x)),qGray(*(src2_line+x)));
					*(dst_line+x) = qRgba (r, g, b, a);
				}
			}
		}
		QPixmap * pixmap = new QPixmap (image);
		QPainter painter (pixmap);

		bool dark = (qGray(color.rgb()) <= 127);
		QImage fg_img (w, h, 32);
		fg_img.setAlphaBuffer (true);
		for (int y=0; y<h; ++y) {
			uint * src_line = (uint*) fg_image.scanLine (y);
			uint * dst_line = (uint*) fg_img.scanLine (y);
			for (int x=0; x<w; ++x) {
				int alpha = qGray (*(src_line+x));
				if (dark)
						*(dst_line+x) = qRgba (255, 255, 255, alpha);
				else
						*(dst_line+x) = qRgba (0, 0, 0, alpha);
			}
		}

		int r = glow_color.red();
		int g = glow_color.green();
		int b = glow_color.blue();
		QImage glow_img (w, h, 32);
		glow_img.setAlphaBuffer (true);
		for (int i=0; i<_steps; ++i) {
			painter.drawImage (0, i*h, fg_img);
			for (int y=0; y<h; ++y) {
				uint * src_line = (uint*) glow_image.scanLine(y);
				uint * dst_line = (uint*) glow_img.scanLine(y);
				for (int x=0; x<w; ++x) {
					int alpha =
							(int) (qGray (*(src_line+x)) * ((double) i/_steps));
					*(dst_line+x) = qRgba (r, g, b, alpha);
				}
			}
			painter.drawImage (0, i*h, glow_img);
		}
		painter.drawImage (0, _steps*h, fg_img);
		for (int y=0; y<h; ++y) {
				uint * src_line = (uint*) glow_image.scanLine (y);
				uint * dst_line = (uint*) glow_img.scanLine (y);
				for (int x=0; x<w; ++x) {
					int alpha = qGray (*(src_line+x));
					*(dst_line+x) = qRgba (r, g, b, alpha);
				}
		}
		painter.drawImage (0, _steps*h, glow_img);

		return pixmap;
}

GlowButton* GlowButtonFactory::createGlowButton(
	QWidget *parent, const char* name, const QString& tip, const int realizeBtns)
{
	GlowButton *glowButton = new GlowButton(parent, name, tip, realizeBtns);
	return glowButton;
}

}

#include "glowbutton.moc"

