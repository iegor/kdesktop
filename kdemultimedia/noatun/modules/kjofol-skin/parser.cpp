/***************************************************************************
	parser.cpp  -  Reads *.rc files in kjöfol-config-format into a QDict
	--------------------------------------
	Maintainer: Stefan Gehn <sgehn@gmx.net>

 ***************************************************************************/

// local includes
#include "parser.h"
#include "kjprefs.h"

// system includes
#include <qtextstream.h>
#include <qimage.h>
#include <qfile.h>
#include <kdebug.h>
#include <kmimemagic.h>
#include <kurl.h>

Parser::Parser() : QDict<QStringList>(17,false)
{
	mSkinAbout="";
	mImageCache.setAutoDelete(true);
	setAutoDelete(true);
}

void Parser::conserveMemory()
{
	mImageCache.clear();
}

void Parser::open(const QString &file)
{
	clear();
	mImageCache.clear();
	mSkinAbout="";
	mDir=KURL(file).directory();
	QFile f(file);
	if ( !f.exists() )
		return;
	f.open(IO_ReadOnly);

	f.at(0);
	QTextStream stream(&f);
	while (!stream.eof())
	{
		QString line=stream.readLine();
		line=line.simplifyWhiteSpace();
		if ((!line.length()) || line[0]=='#')
			continue;
		QStringList *l=new QStringList(QStringList::split(" ", (line.lower())));
		QString first=l->first();

 		// special handling for about-texts as the key "about" can appear multiple
		// times and thus does not fit into qdict.
 		if (first=="about")
		{
			if (!mSkinAbout.isEmpty())
				mSkinAbout+="\n";

			mSkinAbout += line.mid(6);
//			kdDebug(66666) << "found About-line, mSkinAbout is now '" << mSkinAbout << "'" << endl;
			delete l; // don't need the stringlist anymore
		}
		else
			insert(first, l);
	}
}

QString Parser::fileItem(const QString &i) const
{
	return dir()+'/'+i;
}

QString Parser::dir() const
{
	return mDir;
}

Parser::ImagePixmap* Parser::getPair(const QString &filenameOld) const
{
	// is it in there?
	ImagePixmap *pair;
	{
		pair=mImageCache.find(filenameOld);
		if (pair)
			return pair;
	}

	QString filename=fileItem(filenameOld);

	QImage image;

	// Determine file-format trough mimetype (no stupid .ext test)
	KMimeMagicResult * result = KMimeMagic::self()->findFileType( filename );

	if ( result->mimeType() == "image/png" )
	{
//		image = NoatunApp::readPNG(filenameNoCase(filename));
		QImageIO iio;
		iio.setFileName( filenameNoCase(filename) );
		// forget about gamma-value, fix for broken PNGs
		iio.setGamma( 0.00000001 );
		if ( iio.read() )
		{
			image = iio.image();
			image.setAlphaBuffer(false); // we don't want/support alpha-channels
		}
		else
		{
			kdDebug(66666) << "Could not load file: " << filename.latin1() << endl;
		}
	}
	else
	{
		image = QImage(filenameNoCase(filename));
	}

	//add to the cache
	QPixmap pixmap;
	pixmap.convertFromImage(image, QPixmap::AutoColor|QPixmap::ThresholdDither|QPixmap::AvoidDither);
	pair = new Parser::ImagePixmap;
	pair->mImage = image;
	pair->mPixmap = pixmap;
	mImageCache.insert(filenameOld, pair);
	return pair;
}

bool Parser::exist(const QString &i) const
{
	return (bool)find(i);
}
