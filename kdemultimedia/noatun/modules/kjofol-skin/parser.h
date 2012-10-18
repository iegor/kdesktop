#ifndef PARSER_H
#define PARSER_H

// system includes
#include <qstringlist.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qdict.h>

class Parser : public QDict<QStringList>
{
	class ImagePixmap
	{
	public:
		ImagePixmap() : mImage(0), mPixmap(0) {}
		~ImagePixmap() {}
		QImage mImage;
		QPixmap mPixmap;
	};

	public:
		Parser();

		void conserveMemory();
		void open(const QString &file);

		QString dir() const;
		QPixmap pixmap(const QString &pixmap) const
			{ return getPair(pixmap)->mPixmap; }
		QImage image(const QString &image) const
			{ return getPair(image)->mImage; }
		QString about() const { return mSkinAbout; };

		QString fileItem(const QString &file) const;

		bool exist(const QString &i) const;

	public:
		QStringList& operator[](const QString &l) { return *find(l);}

	private:
		ImagePixmap *getPair(const QString &i) const;

	private:
		mutable QDict<ImagePixmap> mImageCache;
		QString mDir;
		QString mSkinAbout;
};
#endif // PARSER_H
