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
#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <qevent.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qimage.h>

#include "katapultdisplay.h"

class Katapult;
class QFont;

/**
@author Joe Ferris, Bastian Holst
*/
class ImageDisplay : public KatapultDisplay
{
	Q_OBJECT
public:
	ImageDisplay(QObject *, const char *, const QStringList&);
	virtual ~ImageDisplay();
	
public slots:
	void continueFade();
	virtual void hide();

	virtual void readSettings(KConfigBase *);
	virtual void writeSettings(KConfigBase *);
	virtual QWidget * configure();
	
	void setFont(const QString &);
	void setMinFontSize(int);
	void setMaxFontSize(int);
	void setFadeOut(int);
	void updateFadeStep();
	
protected:
	virtual void showEvent(QShowEvent *);
	virtual void paintEvent(QPaintEvent *);

	void setSingleBG(QPixmap *);
	void setDoubleBG(QPixmap *);
	void setSingleSize(int width, int height);
	void setDoubleSize(int width, int heught);
	void setMargin(int left, int top, int right, int bottom);
	void setPadding(int);
	void setIconSize(int);
	
	QPixmap getDisplay();
	void drawText(QPixmap &, int, int, QString, int) const;
	void placeWindow(int);
    
private:
	void updateOffset();
	
	QPixmap *bgSngl, *bgDbl, *singlebg, *doublebg;
	QRect desktopSize;
	QImage *fadeImg;
	QTimer *fadeTimer;
	QString fontFace;
	int minFontSize, maxFontSize;
	int alpha;
	bool fadeOut;
	int fadeTime, fadeStep;
	int displaySize;
	// 0: left, 1: top, 2: right, 3: bottom
	int margin[4];
	int offset[4];
	// 0: width, 1: height
	int singlesize[2];
	int doublesize[2];
	int padding;
	int iconsize;
};

#endif
