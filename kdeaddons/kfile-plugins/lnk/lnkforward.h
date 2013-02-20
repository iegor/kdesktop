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

#include <kapplication.h>
#include <kcmdlineargs.h>

#ifndef _LNKFORWARDER_H_
#define _LNKFORWARDER_H_

class LNKForwarder : public KApplication
{
  Q_OBJECT

  public:
    int run(KCmdLineArgs *args);

  protected slots:
    void delayedQuit();
};

#endif
