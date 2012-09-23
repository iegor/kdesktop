#ifndef KJFONT_H
#define KJFONT_H

#include <qstring.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qfont.h>

class KJLoader;

class KJFont
{
public:
	KJFont(const QString &prefix, KJLoader *parent);
	// draw the string str to dev at position pos, within rectangle limit in relation to pos
	QPixmap draw(const QCString &str, int wide, const QPoint &pt=QPoint(0,0)) const;
	QPixmap draw(const QString &str, int wide, const QPoint &pt=QPoint(0,0)) const
		{ return draw(QCString(str.latin1()), wide, pt); }

	int fontHeight() const {return mHeight;}
	int fontWidth() const {return mWidth;}
	int fontSpacing() const {return mSpacing;}
	bool isTransparent() const {return mTransparent;}

	// !!! Call if you changed the systemfont !!!
	void recalcSysFont(void);

protected:
	QPixmap drawSysFont(const QCString &s, int wide, const QPoint &pos=QPoint(0,0)) const;
	QPixmap drawPixmapFont(const QCString &, int, const QPoint &pos=QPoint(0,0)) const;

	void drawCharacter(QPixmap *dev, QBitmap *devMask, const QPoint &to, char c) const;
	QPoint charSource(char c) const;

private:
	QPixmap mText;
	QBitmap mTextMask;
	QRgb mTransparentRGB; // this color will be transparent
	int mSpacing;
	int mWidth, mHeight;
	bool mTransparent; // indicates wether there's transparency
	const char *mString[3];
	char mNullChar;
	QFontMetrics *sysFontMetrics;
	QFont sysFont;
	QColor sysFontColor;
	bool mUseSysFont;
};

#endif
