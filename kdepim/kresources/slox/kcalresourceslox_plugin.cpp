/*
    This file is part of kdepim.

    Copyright (c) 2004 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "kcalresourcesloxconfig.h"
#include "kcalresourceslox.h"

#include <kglobal.h>
#include <klocale.h>

using namespace KCal;

typedef KRES::PluginFactory<KCalResourceSlox,KCalResourceSloxConfig> SLOXFactory;
// FIXME: Use K_EXPORT_COMPONENT_FACTORY( kcal_slox, SLOXFactory ); here
// Problem: How do I insert the catalogue???
extern "C"
{
  void *init_kcal_slox()
  {
    KGlobal::locale()->insertCatalogue( "libkcal" );
    KGlobal::locale()->insertCatalogue( "kabc_slox" );
    return new SLOXFactory;
  }
}
