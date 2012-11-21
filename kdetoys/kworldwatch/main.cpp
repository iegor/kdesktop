/*
**
** Copyright (C) 1998-2001 by Matthias Hölzer-Klüpfel <hoelzer@kde.org>
**	Maintainence has ceased - send questions to kde-devel@kde.org.
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#include <stdlib.h>


#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>


#include <kapplication.h>
#include <kconfig.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>


#include "main.moc"
#include "mapwidget.h"
#include "zoneclock.h"


WorldWideWatch::WorldWideWatch(bool restore, QWidget *parent, const char *name)
  : KMainWindow(parent, name)
{
  KGlobal::locale()->insertCatalogue("timezones"); // For time zone translation

  QWidget *w = new QWidget(this);
  setCentralWidget(w);

  setPlainCaption(i18n("KDE World Clock"));
  
  QVBoxLayout *vbox = new QVBoxLayout(w, 0,0);
  
  _map = new MapWidget(false, restore, w);
  vbox->addWidget(_map,1);

  _clocks = new ZoneClockPanel(w);
  vbox->addWidget(_clocks);

  connect(_map, SIGNAL(addClockClicked(const QString &)), _clocks, SLOT(addClock(const QString &)));
  connect(_map, SIGNAL(saveSettings()), this, SLOT(doSave()));

  if (restore)
    load(kapp->config());
}


void WorldWideWatch::load(KConfig *config)
{
  _map->load(config);
  _clocks->load(config);

  resize(config->readNumEntry("Width", 320),
  config->readNumEntry("Height", 200));
}


void WorldWideWatch::save(KConfig *config)
{
  _map->save(config);
  _clocks->save(config);
  
  config->writeEntry("Width", width());
  config->writeEntry("Height", height());
}


void WorldWideWatch::doSave()
{
  save(kapp->config());
}


void WatchApplication::dumpMap()
{
  // guess some default parameters
  QSize mapSize(kapp->desktop()->size());
 
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
 
  QCString themeName = args->getOption("theme");
  QCString outName = args->getOption("o");
 
  QCString ssize = args->getOption("size");
  if (!ssize.isEmpty())
    {
      int w,h;
      if (sscanf(ssize.data(), "%dx%d", &w, &h) == 2)
        mapSize = QSize(w,h);
    }
 
  kdDebug() << "theme=" << themeName << " out=" << outName << " size=" << mapSize.width() << "x" << mapSize.height() << endl;

  MapWidget *w = new MapWidget(false, true, 0);
  w->setTheme(themeName);
  w->setSize(mapSize.width(), mapSize.height());

  QPixmap pm = w->calculatePixmap();
  QPainter p(&pm);
  w->paintContents(&p);
  pm.save(outName, "PPM");

  delete w;
}


int WatchApplication::newInstance()
{
  // dump mode, used for background drawing
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->isSet("dump"))
    {
      dumpMap();
      return 0;
    }

  if (!restoringSession())
  {
    WorldWideWatch *www = new WorldWideWatch(true);
    www->show();
  }

  return 0;
}

static void listThemes()
{
  QPtrList<MapTheme> _themes = MapLoader::themes();
  QPtrListIterator<MapTheme> it(_themes);
  for ( ; it.current(); ++it)
  {
     printf("%s\n", it.current()->tag().local8Bit().data());
  }
}

static KCmdLineOptions options[] =
{
  { "dump", I18N_NOOP("Write out a file containing the actual map"), 0 },
  { "theme <file>", I18N_NOOP("The name of the theme to use"), "depths"},
  { "themes", I18N_NOOP("List available themes"), 0 },
  { "o <file>", I18N_NOOP("The name of the file to write to"), "dump.ppm" },
  { "size <WxH>", I18N_NOOP("The size of the map to dump"), 0 },
  KCmdLineLastOption
};


int main(int argc, char *argv[])
{
  KAboutData about("kworldclock", I18N_NOOP("KDE World Clock"), "1.5");
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KUniqueApplication::addCmdLineOptions();

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->isSet("themes"))
  {
    KInstance app(&about);
    listThemes();
    return 0;
  }

  if (!KUniqueApplication::start())
    return 0;

  WatchApplication app;
  if (args->isSet("dump"))
      app.disableSessionManagement();

  if (app.isRestored())
    RESTORE(WorldWideWatch)

  return app.exec();
}
