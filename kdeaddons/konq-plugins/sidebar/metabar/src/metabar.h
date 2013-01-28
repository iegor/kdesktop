#ifndef METABAR_H
#define METABAR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <konqsidebarplugin.h>
#include <qstring.h>
#include <kconfig.h>


class Metabar : public KonqSidebarPlugin
{
    Q_OBJECT
    
  public:
    Metabar(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name=0);
    ~Metabar();
      
    virtual QWidget *getWidget(){ return widget; }
    virtual void *provides(const QString &) { return 0; }

  protected:
    MetabarWidget *widget;
    
    void handleURL(const KURL &url);
    void handlePreview(const KFileItemList &items);

};

#endif
