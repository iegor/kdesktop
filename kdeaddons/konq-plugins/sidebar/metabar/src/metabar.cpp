#include <kinstance.h>
#include <qstring.h>
#include <kimageio.h>
#include <klocale.h>

#include "metabarwidget.h"
#include "metabar.h"
#include "metabar.moc"

Metabar::Metabar(KInstance *inst,QObject *parent,QWidget *widgetParent, QString &desktopName, const char* name):
                   KonqSidebarPlugin(inst,parent,widgetParent,desktopName,name)
{
  KImageIO::registerFormats();
  KGlobal::locale()->insertCatalogue( "konqsidebar_metabar" );

  widget = new MetabarWidget(widgetParent, "metabarwidget");
}


Metabar::~Metabar()
{
}

void Metabar::handleURL(const KURL &url)
{
  const KFileItem *item = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, url, true);
  KFileItemList list;
  list.append(item);
  
  widget->setFileItems(list);
}

void Metabar::handlePreview(const KFileItemList &items)
{
  widget->setFileItems(items);
}


extern "C" {
  bool add_konqsidebar_metabar(QString* fn, QString* param, QMap<QString,QString> *map) {
    Q_UNUSED(param);

    map->insert("Type", "Link");
    map->insert("Icon", "metabar");
    map->insert("Name", "Metabar");
    map->insert("Open", "true");
    map->insert("X-KDE-KonqSidebarModule","konqsidebar_metabar");
    fn->setLatin1("metabar%1.desktop");
    return true;
  }
}

extern "C"
{
  void* create_konqsidebar_metabar(KInstance *instance,QObject *par,QWidget *widp,QString &desktopname,const char *name)
    {
      return new Metabar(instance,par,widp,desktopname,name);
    }
}
