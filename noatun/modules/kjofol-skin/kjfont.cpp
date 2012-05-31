/***************************************************************************
	kjfont.cpp
	--------------------------------------
	Font-Handling of KJöfol
	Creates pixmaps of strings using the supplied font-pixmap
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

#include "kjfont.h"
#include "kjloader.h"
#include "kjwidget.h"
#include "kjprefs.h"

#include <kdebug.h>
#include <kglobalsettings.h>

#include <qimage.h>
#include <qpainter.h>

/*******************************************
 * KJFont
 *******************************************/

KJFont::KJFont(const QString &prefix, KJLoader *parent) : mTextMask(0), mTransparentRGB(0)
{
//	kdDebug(66666) << "KJFont::KJFont(const QString &prefix, KJLoader *parent)" << prefix.latin1() << endl;

	if (prefix=="timefont")
	{
		mString[0]="0123456789: ";
		mString[1]=mString[2]="";
		mNullChar=' ';
	}
	else if ( (prefix=="volumefont") || (prefix=="pitchfont") )
	{
		mString[0]="0123456789% ";
		mString[1]=mString[2]="";
		mNullChar=' ';
	}
	else
	{
		mString[0]="abcdefghijklmnopqrstuvwxyz\"@";
		mString[1]="0123456789;_:()-'!_+\\/[]*&%.=$#";
		mString[2]="åöä?*,                          ";
		mNullChar=' ';
	}

	mText = parent->pixmap(parent->item(prefix+"image")[1]);

	if ( parent->exist(prefix+"size") )
	{
		mWidth = parent->item(prefix+"size")[1].toInt();
		mHeight = parent->item(prefix+"size")[2].toInt();
	}
	else // try to load the font even we are missing important settings
	{	// this still can cause crashes!
		kdDebug(66666) << "font height/width missing, this skin might crash noatun!" << endl;

		mWidth = mText.width() / strlen(mString[0]);

		if ( (prefix=="timefont") || (prefix=="volumefont") || (prefix=="pitchfont")  )
			mHeight = mText.height();
		else
			mHeight = mText.height()/3;
	}

//	kdDebug(66666) << prefix << " h: " << mHeight << " w: " << mWidth << endl;

	// fix for wrong numbers in rc (a skin named steelforged needs that)
	if ( mHeight > mText.height() )
		mHeight = mText.height();

	// Stupid Skin authors tend to forget keys :/
	if ( parent->exist(prefix+"spacing") )
		mSpacing = parent->item(prefix+"spacing")[1].toInt();
	else
		mSpacing = 0; // FIXME: What's default for this in kjöfol???

	if ( parent->exist(prefix+"transparent") )
		mTransparent = (bool)parent->item(prefix+"transparent")[1].toInt();
	else
		mTransparent = true; // transparency seems to be default in kjöfol

	// define color in font that will be transparent later on
	if ( mTransparent )
	{
		QImage ibackground = mText.convertToImage();
		mTransparentRGB = ibackground.pixel( ibackground.width()-1, ibackground.height()-1 );
//		kdDebug(66666) << "color (" << qRed(mTransparentRGB) << "," << qGreen(mTransparentRGB) << "," << qBlue(mTransparentRGB) << ") will be transparent for " << prefix.latin1() << endl;
		mTextMask = KJWidget::getMask(ibackground,mTransparentRGB);
	}

	mUseSysFont = KJLoader::kjofol->prefs()->useSysFont();
	sysFontMetrics = 0L;
	if (mUseSysFont)
		recalcSysFont();
}

void KJFont::recalcSysFont(void)
{
//	kdDebug(66666) << k_funcinfo << "called." << endl;

	mUseSysFont = KJLoader::kjofol->prefs()->useSysFont();
	if (!mUseSysFont)
		return;
	sysFont = QFont(KJLoader::kjofol->prefs()->sysFont());
	sysFont.setStyleStrategy( QFont::NoAntialias );
	if ( sysFontMetrics )
		delete sysFontMetrics;
	sysFontColor = KJLoader::kjofol->prefs()->sysFontColor();

//	kdDebug(66666) << "mHeight=" << mHeight << endl;

	int fSize;
	for(fSize = mHeight; fSize>=4; fSize--)
	{
		sysFont.setPixelSize ( fSize );
		sysFontMetrics = new QFontMetrics(sysFont);
//		kdDebug(66666) << "wanted fSize=" << fSize << ", metric h=" << sysFontMetrics->height() << endl;
		// either found a small enough font or found no proper font at all
		if ( sysFontMetrics->height() <= mHeight || fSize==4 )
		{
//			kdDebug(66666) << "stopping @ fSize=" << fSize << ", metric h=" << sysFontMetrics->height() << endl;
			return;
		}
		delete sysFontMetrics;
	}
}

QPixmap KJFont::draw(const QCString &str, int wide, const QPoint &pos) const
{
	if ( mUseSysFont )
		return drawSysFont(str,wide,pos);
	else
		return drawPixmapFont(str,wide,pos);
}

QPixmap KJFont::drawSysFont(const QCString &s, int wide, const QPoint &pos) const
{
//	kdDebug(66666) << k_funcinfo << "BEGIN, s='" << s << "'" << endl;
	QPoint to(pos);
	QString string(s);

	int stringWidth = sysFontMetrics->width( string );
//	kdDebug(66666) << "final metrics; w=" << stringWidth << ", h=" << sysFontMetrics->height() << endl;

	QPixmap region(
		(stringWidth > wide ? stringWidth : wide),
		mHeight);
	QPainter rp(&region); // region painter

	QBitmap regionMask(
		(stringWidth > wide ? stringWidth : wide),
		mHeight, true); // fully transparent mask
	QPainter mp(&regionMask); // mask painter

//	kdDebug(66666) << "region; w=" << region.width() << ", h=" << region.height() << endl;

	int freeSpace=0;
	// center string into pixmap if its chars won't fill the whole pixmap
	if ( stringWidth < wide )
	{
		freeSpace = wide - stringWidth;
		mp.fillRect ( to.x(), 0, (freeSpace/2), mHeight, Qt::color0 );
		to += QPoint ( (freeSpace/2), 0 );
//		kdDebug(66666) << "centering text, freeSpace=" << freeSpace << endl;
	}

	rp.setFont(sysFont);
	rp.setPen(sysFontColor);
	rp.drawText(to.x(), to.y(), region.width()-freeSpace, mHeight, Qt::AlignLeft|Qt::AlignTop, string);

	mp.setFont(sysFont);
	mp.setPen(Qt::color1);
	mp.drawText(to.x(), to.y(), region.width()-freeSpace, mHeight, Qt::AlignLeft|Qt::AlignTop, string);

	to += QPoint(region.width()-freeSpace,0);
//	kdDebug(66666) << "text width=" << region.width()-freeSpace << endl;

	if (freeSpace > 0)
	{
		mp.fillRect ( to.x(), 0, (freeSpace/2), mHeight, Qt::color0 );
		to += QPoint ( (freeSpace/2), 0 );
//		kdDebug(66666) << "centering text, freeSpace=" << freeSpace << endl;
	}

	region.setMask( regionMask );
//	kdDebug(66666) << "width: " << wide << "|end after drawing: " << to.x() << endl;
//	kdDebug(66666) << k_funcinfo << "END" << endl << endl;
	return region;
}

QPixmap KJFont::drawPixmapFont(const QCString &str, int wide, const QPoint &pos) const
{
//	kdDebug(66666) << k_funcinfo << "BEGIN" << endl;
	QPoint to(pos);

	QCString string = str.lower();
	QPixmap region(
		(string.length()*mWidth+string.length()*mSpacing > (unsigned int)wide
			? string.length()*mWidth+string.length()*mSpacing : wide),
		mHeight);

	QBitmap regionMask(
			 (string.length()*mWidth+string.length()*mSpacing > (unsigned int)wide
			? string.length()*mWidth+string.length()*mSpacing : wide),
			 mHeight, true); // fully transparent mask
	QPainter mask( &regionMask );

//	kdDebug(66666) << "draw: {" << str << "}" << endl;

	int freeSpace=0;
	// center string into pixmap if its chars won't fill the whole pixmap
	if ( string.length()*mWidth+string.length()*mSpacing < (unsigned int)wide )
	{
		freeSpace = wide - string.length()*mWidth+string.length()*mSpacing;
		mask.fillRect ( to.x(), 0, (freeSpace/2), mHeight, Qt::color0 );
		to += QPoint ( (freeSpace/2), 0 );
	}

//	kdDebug(66666) << k_funcinfo << "pixmap width=" << region.width() << endl;

	// draw every char and add spacing in between these chars if defined
	unsigned int stringLength(string.length());
	for ( unsigned int charPos=0; charPos < stringLength; charPos++ )
	{
		char c = string[charPos]; // the character to be drawn next

		drawCharacter(&region, &regionMask, to, c);
		to += QPoint(mWidth, 0);

		// draw according to "spacing"
		if ( (charPos < string.length()-1) && mSpacing > 0 )
		{	// make the spacing-area transparent
			mask.fillRect ( to.x(), 0, mSpacing, mHeight, Qt::color0 );
			to += QPoint ( mSpacing, 0 );
		}
	}

	if (freeSpace > 0)
	{
		mask.fillRect ( to.x(), 0, (freeSpace/2), mHeight, Qt::color0 );
		to += QPoint ( (freeSpace/2), 0 );
	}

	region.setMask( regionMask );
//	kdDebug(66666) << "width: " << wide << "|end after drawing: " << to.x() << endl;
	return region;
}

void KJFont::drawCharacter(QPixmap *dev, QBitmap *devMask, const QPoint &to, char c) const
{
	QPoint src=charSource(c);
	int x=src.x();
	int y=src.y();
	int xs=mWidth;
	int ys=mHeight;

	bitBlt(dev, to, &mText, QRect(x,y,xs,ys), Qt::CopyROP);

	// bitBlt mask for transparency
	if ( mTransparent )
	{
		bitBlt(devMask, to, &mTextMask, QRect(x,y,xs,ys), Qt::OrROP);
	}
	else // fill mask
	{
		QPainter tempPainter (devMask);
		tempPainter.fillRect ( to.x(), 0, xs,ys, Qt::color1 );
	}
}

// needed for strchr
#include <string.h>

// searches for top/left coordinate of a given character inside the font-pixmap
QPoint KJFont::charSource(char c) const
{
	for (int i=0; i<3; i++)
	{
		const char *pos = strchr(mString[i], c);

		if (!pos) continue;
		return QPoint(mWidth*((int)(pos-mString[i])), mHeight*i);
	}

	return charSource(mNullChar);
}
