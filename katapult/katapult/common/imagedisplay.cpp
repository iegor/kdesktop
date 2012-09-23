/***************************************************************************
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <imagedisplay.moc>

#include <kglobal.h>
#include <kglobalsettings.h>
#include <kiconloader.h>
#include <kwin.h>
#include <kimageeffect.h>
#include <kgenericfactory.h>
#include <kconfig.h>
#include <kfontcombo.h>
#include <klocale.h>

#include <qpainter.h>
#include <qapplication.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qcursor.h>

#include <status.h>
 
#include "imagedisplay.h"
#include "imagedisplaysettings.h"

#define FADEINTERVAL 20
#define FADECONST 2000

ImageDisplay::ImageDisplay(QObject *, const char *name, const QStringList&)
	: KatapultDisplay(name, WType_TopLevel | WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WNoAutoErase | WDestructiveClose)
{
	KWin::setType(winId(), NET::Dock);
	setBackgroundMode(NoBackground);
	setFocusPolicy(QWidget::StrongFocus);
	
        singlebg = 0;
        doublebg = 0;
	
	singlesize[0] = 0;
	singlesize[1] = 0;
	doublesize[0] = 0;
	doublesize[1] = 0;
	padding = 0;
	margin[0] = 0;
	margin[1] = 0;
	margin[2] = 0;
	margin[3] = 0;
	iconsize = 0;
        
	bgSngl = 0;
	bgDbl = 0;
	
	fadeImg = 0;
	
	alpha = 0;
	fadeTimer = new QTimer(this);
	connect(fadeTimer, SIGNAL(timeout()), this, SLOT(continueFade()));
	
	desktopSize = qApp->desktop()->availableGeometry(QCursor::pos());
	
	fadeTime = 250;
	updateFadeStep();
}

ImageDisplay::~ImageDisplay()
{
	if(singlebg != 0)
		delete singlebg;
	if(doublebg != 0)
		delete doublebg;
	if(bgSngl != 0)  
		delete bgSngl;
	if(bgDbl != 0)
		delete bgDbl;
	if(fadeImg != 0)
		delete fadeImg;
}

void ImageDisplay::continueFade()
{
	if(fadeOut)
	{
		if(fadeStep == 0)
			alpha = 0;
		else
			alpha -= fadeStep;
		if(alpha <= 0)
		{
			alpha = 0;
			fadeTimer->stop();
			hide();
		}
	} else {
		if(fadeStep == 0)
			alpha = 100;
		else
			alpha += fadeStep;
		if(alpha > 100)
		{
			alpha = 100;
			fadeTimer->stop();
			if(fadeImg != 0)
			{
				delete fadeImg;
				fadeImg = 0;
			}
		}
	}
	update();
}

void ImageDisplay::showEvent(QShowEvent *)
{
	// set back the display-size, because if it wouldn't be done this could be a problem-source.
	displaySize = 0; 
	
	if(bgSngl != 0)
		delete bgSngl;
	if(bgDbl != 0)
		delete bgDbl;
	
	bgSngl = new QPixmap(QPixmap::grabWindow(qt_xrootwin(), (desktopSize.width() - singlesize[0])/2 + desktopSize.left(), (desktopSize.height() - singlesize[1])/2 + desktopSize.top(), singlesize[0], singlesize[1]));
	bgDbl = new QPixmap(QPixmap::grabWindow(qt_xrootwin(), (desktopSize.width() - doublesize[0])/2 + desktopSize.left(), (desktopSize.height() - doublesize[1])/2 + desktopSize.top(), doublesize[0], doublesize[1]));
	
	if(fadeImg != 0)
		delete fadeImg;
	
	if(fadeTime <= 1) {
		alpha = 100;
		placeWindow(1);
	} else {
		fadeOut = false;
		alpha = 0;
		fadeImg = new QImage(getDisplay().convertToImage());
		fadeTimer->start(FADEINTERVAL, FALSE);
	}
}

void ImageDisplay::hide()
{
	if(alpha == 0)
	{
		if(fadeImg != 0)
		{
			delete fadeImg;
			fadeImg = 0;
		}
		KatapultDisplay::hide();
	} else {
		if(fadeImg != 0)
			delete fadeImg;
		fadeImg = new QImage(getDisplay().convertToImage());
		fadeOut = TRUE;
		if(!fadeTimer->isActive())
			fadeTimer->start(FADEINTERVAL, FALSE);
	}
}

void ImageDisplay::drawText(QPixmap & pixmap, int x, int width, QString text, int hilight) const
{
	int fontSize = maxFontSize;
	QFont font(fontFace, fontSize);
	QFontMetrics metrics(font);
	
	while(fontSize > minFontSize && metrics.width(text) > width) {
		fontSize--;
		font.setPointSize(fontSize);
		metrics = QFontMetrics(font);
	}

	while(hilight > 1 && metrics.width(text) > width) {
		text = text.remove(0, 1);
		hilight--;
	}
	
	int useChars = text.length();
	
	while(metrics.width(text, useChars) > width) {
		useChars--;
	}
	text = text.left(useChars);
	
	QString hilighted = text.left(hilight);
	QString remaining = text.right(text.length() - hilighted.length());
	
	x += (width-metrics.width(text))/2;
	
	QPainter painter(&pixmap);
	painter.setFont(font);

	painter.setPen(colorGroup().color(QColorGroup::Link));
	painter.drawText(x, singlesize[1]-offset[3], hilighted);
	painter.setPen(QColor(255, 255, 255));
	painter.drawText(x+metrics.width(hilighted), singlesize[1]-offset[3], remaining);
}

QPixmap ImageDisplay::getDisplay()
{
	if(status() & S_HasResults)
	{
		// show best match
		QPixmap pixmap(*bgDbl);
		QPainter painter(&pixmap);
		
		painter.drawPixmap(0, 0, *doublebg);
		
		QFontMetrics metrics = painter.fontMetrics();
		QRect bounds;
		QPixmap icon;
		
		int itemSpace = (doublesize[0]-offset[0]-offset[2]-padding)/2;
		
		const KatapultItem *_item = item();
		if(_item != 0)
		{
			icon = _item->icon(iconsize);
			painter.drawPixmap(offset[0]+(itemSpace-iconsize)/2, offset[1], icon);
			drawText(pixmap, offset[0], itemSpace, _item->text(), selected());
		}
			
		const KatapultAction *_action = action();
		if(_action != 0)
		{
			int x = offset[0]+itemSpace+padding+padding;
			icon = _action->icon(iconsize);
			painter.drawPixmap(x+(itemSpace-iconsize)/2, offset[1], icon);
			drawText(pixmap, x, itemSpace, _action->text(), 0);
		}
		painter.end();
		
		if(displaySize != 2)
		{
			placeWindow(2);
			displaySize = 2;
		}
		
		return pixmap;
	} else {
		// show splash or error
		QPixmap pixmap(*bgSngl);
		QPainter painter(&pixmap);
		
		painter.drawPixmap(0, 0, *singlebg);
		
		QString label;
		QPixmap icon;
		
		if(status() & S_Active)
		{
			icon = KGlobal::iconLoader()->loadIcon("unknown", KIcon::NoGroup, 128);
			if(query().isEmpty())
				label = i18n("No items matched.");
			else
				label = query();
		} else {
			icon = KGlobal::iconLoader()->loadIcon("katapult", KIcon::NoGroup, 128);
			if(query().isEmpty())
				label = "Katapult";
			else {
				label = query();
				painter.setPen(QColor(16, 48, 254));
			}
		}
		
		painter.drawPixmap(offset[0]+(singlesize[0]-offset[0]-offset[2]-128)/2, offset[1], icon);
		
		drawText(pixmap, offset[0], singlesize[0]-offset[0]-offset[2], label, 0);
		
/*		painter.drawText(offset[0], singlesize[1]-offset[3]-30, singlesize[0]-offset[0]-offset[2], 30,
			Qt::AlignHCenter | Qt::AlignBottom | Qt::SingleLine, label);*/
		painter.end();
		
		if(displaySize != 1)
		{
			placeWindow(1);
			displaySize = 1;
		}
		
		return pixmap;
	}
}

void ImageDisplay::placeWindow(int size)
{
	if(size == 2) {
		move((desktopSize.width() - doublesize[0])/2 + desktopSize.left(), (desktopSize.height() - doublesize[1])/2 + desktopSize.top());
		resize(doublesize[0], doublesize[1]);
	} else {
		move((desktopSize.width() - singlesize[0])/2 + desktopSize.left(), (desktopSize.height() - singlesize[1])/2 + desktopSize.top());
		resize(singlesize[0], singlesize[1]);
	}
}

void ImageDisplay::paintEvent(QPaintEvent *)
{
	if(alpha == 0)
	{
		if(displaySize == 2)
			bitBlt(this, 0, 0, bgDbl);
		else
			bitBlt(this, 0, 0, bgSngl);
	} else if(fadeImg != 0) {
		QImage buffer;
		if(displaySize == 2) 
			buffer = bgDbl->convertToImage();
		else
			buffer = bgSngl->convertToImage();
		
		KImageEffect::blend(*fadeImg, buffer, (float)alpha/100);
		bitBlt(this, 0, 0, &buffer);
	} else {
		QPixmap pixmap = getDisplay();
		bitBlt(this, 0, 0, &pixmap);
	}
}

void ImageDisplay::setSingleBG(QPixmap *singlebg)
{
	this->singlebg = singlebg;
}

void ImageDisplay::setDoubleBG(QPixmap *doublebg)
{
	this->doublebg = doublebg;
}

void ImageDisplay::setSingleSize(int width, int height)
{
	singlesize[0] = width;
	singlesize[1] = height;
}

void ImageDisplay::setDoubleSize(int width, int height)
{
	doublesize[0] = width;
	doublesize[1] = height;
}	

void ImageDisplay::setMargin(int left, int top, int right, int bottom)
{
	margin[0] = left;
	margin[1] = top;
	margin[2] = right;
	margin[3] = bottom;
	updateOffset();
}

void ImageDisplay::setPadding(int padding)
{
	this->padding = padding;
	updateOffset();
}

void ImageDisplay::setIconSize(int iconsize)
{
	this->iconsize = iconsize;
}

void ImageDisplay::updateOffset()
{
	offset[0] = margin[0] + padding;
	offset[1] = margin[1] + padding;
	offset[2] = margin[2] + padding;
	offset[3] = margin[3] + padding;
}

void ImageDisplay::readSettings(KConfigBase *config)
{
	fadeTime = config->readUnsignedNumEntry("FadeTime", 250);
	updateFadeStep();
//	fadeStep = fadeTime/FADEINTERVAL;
	QFont defaultFont = KGlobalSettings::generalFont();
	fontFace = config->readEntry("FontFace", defaultFont.family());
	minFontSize = config->readUnsignedNumEntry("MinFontSize", 7);
	maxFontSize = config->readUnsignedNumEntry("MaxFontSize", 14);
}

void ImageDisplay::writeSettings(KConfigBase *config)
{
	config->writeEntry("FadeTime", fadeTime);
	config->writeEntry("FontFace", fontFace);
	config->writeEntry("MinFontSize", minFontSize);
	config->writeEntry("MaxFontSize", maxFontSize);
}

QWidget * ImageDisplay::configure()
{
	ImageDisplaySettings *settings = new ImageDisplaySettings();
	
	settings->fadeTime->setValue(fadeTime);
	connect(settings->fadeTime, SIGNAL(valueChanged(int)), this, SLOT(setFadeOut(int)));
	
	settings->displayFont->setCurrentFont(fontFace);
	connect(settings->displayFont, SIGNAL(textChanged(const QString &)), this, SLOT(setFont(const QString &)));
	
	settings->minFontSize->setValue(minFontSize);
	connect(settings->minFontSize, SIGNAL(valueChanged(int)), this, SLOT(setMinFontSize(int)));
	
	settings->maxFontSize->setValue(maxFontSize);
	connect(settings->maxFontSize, SIGNAL(valueChanged(int)), this, SLOT(setMaxFontSize(int)));
	
	return settings;
}

void ImageDisplay::setFont(const QString &font)
{
	fontFace = font;
}

void ImageDisplay::setMinFontSize(int size)
{
	minFontSize = size;
}

void ImageDisplay::setMaxFontSize(int size)
{
	maxFontSize = size;
}

void ImageDisplay::setFadeOut(int fadeTime)
{
	this->fadeTime = fadeTime;
	updateFadeStep();
}

void ImageDisplay::updateFadeStep()
{
	if(fadeTime == 0)
		fadeTime = 1;
	fadeStep = FADECONST/fadeTime;
	if(fadeStep > 100)
		fadeStep = 100;
}
