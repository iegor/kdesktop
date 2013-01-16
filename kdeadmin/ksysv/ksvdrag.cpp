// (c) 2000 Peter Putzer

#include <qstring.h>
#include <qcstring.h>

#include <kurl.h>

#include "Data.h"
#include "ksvdraglist.h"
#include "ksvdrag.h"

class KSVDrag::Private
{
public:
  QByteArray mNative;
  QString mText;
  KURL mURL;
};

KSVDrag::KSVDrag (const KSVData& item, QWidget* source, const char* name)
  : QDragObject (source, name),
    d (new Private())
{
  QDataStream ds (d->mNative, IO_ReadWrite);
  ds << item;

  d->mText = item.filenameAndPath ();
  d->mURL.setPath (item.path() + "/" + item.filename());
}

KSVDrag::KSVDrag (const KSVItem& item, QWidget* source, const char* name)
  : QDragObject (source, name),
    d (new Private())
{
  QDataStream ds (d->mNative, IO_ReadWrite);
  ds << *item.data();

  d->mText = item.toString();
  d->mURL.setPath (item.path() + "/" + item.filename());
}

KSVDrag::~KSVDrag ()
{
  delete d;
}

const char* KSVDrag::format (int i) const
{
  switch (i)
	{
	case Native:
	  return "application/x-ksysv";
	  break;
	  
	case Text:
	  return "text/plain";
	  break;
	  
	case URL:
	  return "text/uri-list";
	  break;
	  
	default:
	  return 0L;
	}
}

QByteArray KSVDrag::encodedData (const char* format) const
{
  QByteArray res;

  if (!strcmp (format, "application/x-ksysv"))
    {
      res = d->mNative;
    }
  else if (!strcmp (format, "text/plain"))
    {
      QDataStream ds (res, IO_ReadWrite);
	  ds << d->mText;
    }
  else if (!strcmp (format, "text/uri-list"))
    {
      res = QCString(d->mURL.url().latin1()).copy();
    }

  return res;
}

bool KSVDrag::decodeNative (const QMimeSource* mime, KSVData& data)
{
  if (mime && mime->provides ("application/x-ksysv"))
	{
	  QDataStream ds (mime->encodedData ("application/x-ksysv"), IO_ReadOnly);
	  ds >> data;

	  return true;
	}

  return false;
}

#include "ksvdrag.moc"
