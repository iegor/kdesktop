/*****************************************************
 * FSView, a simple TreeMap application
 *
 * (C) 2002, Josef Weidendorfer
 */

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>

#include "fsview.h"


static KCmdLineOptions options[] =
{
  { "+[folder]", I18N_NOOP("View filesystem starting from this folder"), 0 },
  KCmdLineLastOption // End of options.
};

int main(int argc, char* argv[])
{
  // KDE compliant startup
  KAboutData aboutData("fsview", I18N_NOOP("FSView"), "0.1",
                       I18N_NOOP("Filesystem Viewer"),
                       KAboutData::License_GPL,
                       I18N_NOOP("(c) 2002, Josef Weidendorfer"));
  KCmdLineArgs::init(argc, argv, &aboutData);
  KCmdLineArgs::addCmdLineOptions(options);
  KApplication a;

  KConfigGroup gconfig(KGlobal::config(), QCString("General"));
  QString path = gconfig.readPathEntry("Path", ".");

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->count()>0) path = args->arg(0);

  // TreeMap Widget as toplevel window
  FSView w(new Inode());

  QObject::connect(&w,SIGNAL(clicked(TreeMapItem*)),
                   &w,SLOT(selected(TreeMapItem*)));
  QObject::connect(&w,SIGNAL(returnPressed(TreeMapItem*)),
                   &w,SLOT(selected(TreeMapItem*)));
  QObject::connect(&w,
                   SIGNAL(contextMenuRequested(TreeMapItem*,const QPoint&)),
                   &w,SLOT(contextMenu(TreeMapItem*, const QPoint&)));

  w.setPath(path);
  w.show();

  a.connect( &a, SIGNAL( lastWindowClosed() ), &w, SLOT( quit() ) );
  return a.exec();
}
