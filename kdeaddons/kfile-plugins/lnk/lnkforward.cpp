/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This helper app runs the associated action for a linked file inside a *
 *   M$-Windoze .lnk file, which is useful if you work in a mixed          *
 *   Linux/Windoze environment.                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include "read_lnk.cpp"
#include "lnkforward.h"

#include <stdlib.h>

#include <klocale.h>
#include <krun.h>
#include <kdebug.h>

//--------------------------------------------------------------------------------

static const char appName[] = "lnkforward";
static const char programName[] = I18N_NOOP("lnkforward");

static const char description[] = I18N_NOOP("KDE tool for opening URLs given in a Windows .lnk file");

static const char version[] = "1.0";

static const KCmdLineOptions options[] =
{
   { "map <drive=path>", I18N_NOOP("Map a Windows drive to a local mountpoint, e.g. \"C=/mnt/windows\""
                                   "\nThis option can be given multiple times"), 0},
   { "+URL", I18N_NOOP("Windows .lnk file to open"), 0},
   KCmdLineLastOption
};

//--------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  KCmdLineArgs::init(argc, argv, appName, programName, description, version, false);

  KCmdLineArgs::addCmdLineOptions(options);

  LNKForwarder app;

  return app.run(KCmdLineArgs::parsedArgs());
}

//--------------------------------------------------------------------------------

int LNKForwarder::run(KCmdLineArgs *args)
{
  if ( args->count() == 0 ) return 1;

  LNKInfo info;

  bool ret = readLNK(args->arg(0), info);
  if ( ! ret ) return 1;

  info.path.replace(QChar('\\'), QChar('/'));

  QString path;

  if ( info.isNetworkPath )
  {
    path = "smb:" + info.path;
  }
  else
  {
    if ( info.driveName.isNull() ) return 1;  // can this ever happen ?

    QCStringList map = args->getOptionList("map");
    bool found = false;

    for (unsigned int i = 0; i < map.count(); i++)
    {
      if ( map[i].lower()[0] == info.driveName.lower()[0] )
      {
        if ( map[i].length() < 3 )
        {
          return 1;
        }
        path = map[i].mid(2);  // skip e.g. "C="
        found = true;
        break;
      }
    }

    if ( !found ) return 1;  // no mapped drive found

    path += '/';  // make sure that the dir ends with /
    path += info.path;
  }

  kdDebug(7034) << "running:" << path << endl;
  KRun * run = new KRun(path);
  QObject::connect(run, SIGNAL(finished()), this, SLOT(delayedQuit()));
  QObject::connect(run, SIGNAL(error()), this, SLOT(delayedQuit()));

  return exec();
}

//--------------------------------------------------------------------------------
// copied from kfmclient.cc

void LNKForwarder::delayedQuit()
{
  // Quit in 2 seconds. This leaves time for KRun to pop up
  // "app not found" in KProcessRunner, if that was the case.
  QTimer::singleShot(2000, this, SLOT(deref()));
}

//--------------------------------------------------------------------------------

#include "lnkforward.moc"
