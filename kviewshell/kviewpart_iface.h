// -*- C++ -*-
#ifndef KVIEWPART_IFACE_H
#define KVIEWPART_IFACE_H

#include <kparts/part.h>

class QStringList;


class KViewPart_Iface : public KParts::ReadOnlyPart
{
  Q_OBJECT
public:
  KViewPart_Iface(QObject *parent, const char *name)
    : KParts::ReadOnlyPart(parent, name) {}

  virtual ~KViewPart_Iface() {}

  /* Returns a description of the current page size, for use in the
     statusbar of the kviewshell that embeds this KViewPart. */
  virtual QString pageSizeDescription() = 0;

  /* This method calls closeURL(), but asks first ("The document was
     modified. Do you really want to close?") if the document has been
     modified after it has been loaded.  */
  virtual bool closeURL_ask() = 0;

  virtual QStringList supportedMimeTypes() = 0;

public slots:
  virtual void slotSetFullPage(bool fullpage) = 0;
  virtual QStringList fileFormats() const = 0;
};

#endif
