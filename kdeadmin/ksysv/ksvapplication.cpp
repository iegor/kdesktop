// (c) 2000 Peter Putzer

#include <unistd.h>

#include <kwin.h>

#include "ksvapplication.h"

int KSVApplication::newInstance ()
{
  QWidget* main = mainWidget();

  if (main)
    KWin::activateWindow (main->winId());

  return getpid();
}

#include "ksvapplication.moc"

