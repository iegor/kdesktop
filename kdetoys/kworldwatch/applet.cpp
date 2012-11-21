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
#include <qlayout.h>


#include <kapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <krun.h>
#include <kglobalsettings.h>


#include "mapwidget.h"
#include "applet.moc"


extern "C"
{
  KDE_EXPORT KPanelApplet *init(QWidget *parent, const QString& configFile)
  {
    KGlobal::locale()->insertCatalogue("kworldclock");
    KGlobal::locale()->insertCatalogue("timezones"); // For time zone translation
    return new KWWApplet(configFile, KPanelApplet::Normal,
			 0,
			 parent, "kwwapplet");
  }
}


KWWApplet::KWWApplet(const QString& configFile, Type type, int actions,
		     QWidget *parent, const char *name)
  : KPanelApplet(configFile, type, actions, parent, name)
{
  // make use of the icons installed for ksaferppp
  KGlobal::iconLoader()->addAppDir("kworldwatch");

  QVBoxLayout *vbox = new QVBoxLayout(this, 0,0);

  map = new MapWidget(true, true, this);
  map->load(config());
  vbox->addWidget(map);
  setCustomMenu(map->contextMenu());

  map->installEventFilter(this);
}


KWWApplet::~KWWApplet()
{
  map->save(config());
}


int KWWApplet::widthForHeight(int height) const
{
  return height*2;
}


int KWWApplet::heightForWidth(int width) const
{
  return width/2;
}


// catch the mouse clicks of our child widgets
bool KWWApplet::eventFilter( QObject *o, QEvent *e )
{
  if ((e->type() == QEvent::MouseButtonPress) || (e->type() == QEvent::MouseButtonDblClick)) 
  {
    mousePressEvent(static_cast<QMouseEvent*>(e));
    return true;
  }

  return KPanelApplet::eventFilter(o, e);
}


void KWWApplet::mousePressEvent(QMouseEvent *e)
{
  bool clicked = false;

  if (KGlobalSettings::singleClick())
  {
    clicked = e->type() == QMouseEvent::MouseButtonPress;
  }
  else
  {
    clicked = e->type() == QMouseEvent::MouseButtonDblClick;
  }

  if (clicked && e->button() == QMouseEvent::LeftButton) 
  {
    KRun::run("kworldclock", KURL::List());
  }
}
