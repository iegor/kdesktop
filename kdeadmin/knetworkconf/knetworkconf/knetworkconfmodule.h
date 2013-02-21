/***************************************************************************
                          knetworkconfmodule.h  -  description
                             -------------------
    begin                : Tue Apr 1 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : juancho@linuxmail.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KNETWORKCONFMODULE_H
#define KNETWORKCONFMODULE_H

#include "knetworkconf.h"
#include <kaboutdata.h>
#include <kcmodule.h>

class KNetworkConfModule : public KCModule
{
  Q_OBJECT

public:
  KNetworkConfModule(QWidget* parent, const char *name/*, QStringList list*/);
  ~KNetworkConfModule();

  void load();
  void save();
  int buttons();

  bool useRootOnlyMsg() const;
  QString rootOnlyMsg() const;
  KAboutData* aboutData() const;
  QString quickHelp() const;

private slots:
  void configChanged(bool);

private:
  KNetworkConf* conf;
};

#endif
