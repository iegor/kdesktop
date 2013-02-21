// (c) 2000 Peter Putzer

#ifndef KSVDRAG_H
#define KSVDRAG_H

#include <qdragobject.h>

class KSVData;
class KSVItem;
class QWidget;

class KSVDrag : public QDragObject
{
  Q_OBJECT

public:
  KSVDrag (const KSVData& item, QWidget* dragSource = 0L, const char* name = 0L);
  KSVDrag (const KSVItem& item, QWidget* dragSource = 0L, const char* name = 0L);
  virtual ~KSVDrag();

  virtual const char* format (int i) const;
  QByteArray encodedData (const char*) const;
  
  static bool decodeNative (const QMimeSource*, KSVData&);
  
private:
  enum
  {
	Native, Text, URL
  };

  class Private;
  Private* d;
};

#endif // KSVDRAG_H
