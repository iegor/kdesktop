/***************************************************************************
	helpers.cpp
	Just a few functions needed in several Kjofol-classes
	---------------------------------------------
	Maintainer: Charles Samuels <charles@kde.org>

 ***************************************************************************/

#ifndef KJHELPERS_CPP
#define KJHELPERS_CPP

static int grayRgb(QRgb r)
{
	return qGray(qRed(r), qGreen(r), qBlue(r));
}

static int isGray(QRgb r)
{
// this is more tolerant than the old version
// i.e. RGB 162 163 162 is treated as gray too
// too many broken skins around having such colors

//	cerr << "r("<<qRed(r)<<","<<qGreen(r)<<","<<qBlue(r)<<")"<<endl;

	if ( (qRed(r)==qGreen(r)) || (qRed(r)+1==qGreen(r)) || (qRed(r)-1==qGreen(r)))
	{
		if ( (qRed(r)==qBlue(r)) || (qRed(r)+1==qBlue(r)) || (qRed(r)-1==qBlue(r)))
		{
			// looks a bit like gray, so return true
			return (1);
		}
	}
	// well, it's not gray
	return(0);

/*
	// mETz: wrong braces in the code below ??
	return (qRed(r)==qGreen(r)) && (qRed(r) == qBlue(r));
*/
}

// only works little endian
// UPDATE: should work on both little and big endian now (haven't tested that!)
// this code is taken from the QT-docu and I hope that this example
// is one of the working ones ;)
inline void setPixel1BPP(QImage &image, int x, int y, bool value)
{
	if ( image.bitOrder() == QImage::LittleEndian )
	{
		if (value)
			*(image.scanLine(y) + (x >> 3)) |= 1 << (x & 7);
		else
			*(image.scanLine(y) + (x >> 3)) &= ~(1 << (x & 7));
	}
	else
	{
		if (value)
			*(image.scanLine(y) + (x >> 3)) |= 1 << (7-(x & 7));
		else
			*(image.scanLine(y) + (x >> 3)) &= ~(1 << (7-(x & 7)));
	}
}

#endif
