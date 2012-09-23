/***************************************************************************
	kjbackground.cpp
	--------------------------------------
	Just draws the main-pixmap of a KJöfol-Skin
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

#include "kjbackground.h"

KJBackground::KJBackground(KJLoader *parent)
	: KJWidget(parent)
{
	QImage ibackground;

	mBackground = parent->pixmap(parser()["backgroundimage"][1]);
	ibackground = parent->image(parser()["backgroundimage"][1]);

	parent->setMask( getMask(ibackground) );
	parent->setFixedSize ( QSize(mBackground.width(), mBackground.height()) );

	setRect(0,0,parent->width(),parent->height());
}

void KJBackground::paint(QPainter *painter, const QRect &rect)
{
	bitBlt(painter->device(), rect.topLeft(), &mBackground, rect, Qt::CopyROP);
}
