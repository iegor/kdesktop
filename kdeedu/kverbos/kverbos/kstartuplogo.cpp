/***************************************************************************
                          kstartuplogo.cpp  -  description
                             -------------------
    begin                : Fri Dec 14 2001
    copyright            : (C) 2001 by Arnold Kraschinski
    email                : arnold.k67@gmx.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kstartuplogo.h"
#include <kapplication.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>

KStartupLogo::KStartupLogo(QWidget * parent, const char *name)
: QWidget(parent,name,WStyle_Customize | WStyle_NoBorder  )
{ 	
  QPixmap pm;
  pm.load(locate("appdata", "pics/startuplogo.png"));
  setBackgroundPixmap(pm);
  QRect desk = KGlobalSettings::splashScreenDesktopGeometry();
  setGeometry(desk.center().x()-pm.width()/2, desk.center().y()-pm.height()/2,
              pm.width(),pm.height());
}

KStartupLogo::~KStartupLogo(){
}

bool KStartupLogo::event(QEvent* ev){
  raise();
  return QWidget::event(ev);
}

#include "kstartuplogo.moc"
