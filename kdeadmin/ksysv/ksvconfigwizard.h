/*
** Copyright (C) 2000 Peter Putzer <putzer@kde.org>
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
#ifndef KSVCONFIGWIZARD_H
#define KSVCONFIGWIZARD_H
#include "configwizard.h"

#include <qstring.h>

class KSVConfigWizard : public ConfigWizard
{ 
  Q_OBJECT
  
public:
  KSVConfigWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~KSVConfigWizard();

public:
  QString runlevelPath();
  QString servicesPath();

protected slots:
  void browseRunlevels();
  void chooseDistribution(int);
  void browseServices();

private slots:
  void selectedPage (const QString&);

private:
  typedef enum {
    Debian=0,
    RedHat=1,
    SuSE=2,
    Mandrake=3,
    Corel=4,
	 Conectiva=5,
    Other=6
  } Distribution;

  Distribution mChosenDistribution;
};

#endif // KSVCONFIGWIZARD_H
