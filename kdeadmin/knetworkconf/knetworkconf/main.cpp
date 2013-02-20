/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jan 12 00:54:19 UTC 2003
    copyright            : (C) 2003 by Juan Luis Baptiste
    email                : jbaptiste@merlinux.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <unistd.h>
#include <sys/types.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kuniqueapplication.h>
#include <dcopclient.h>
#include <kglobal.h>
#include <kgenericfactory.h>

//#include "knetworkconf.h"
#include "knetworkconfmodule.h"
#include "version.h"
	
	
static KCmdLineOptions options[] =
{
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};
/*typedef KGenericFactory<KNetworkConfModule> KDEDFactory; 
K_EXPORT_COMPONENT_FACTORY( knetworkconfmodule, KDEDFactory( "kcm_knetworkconfmodule" ) ); */

extern "C"
{
  KDE_EXPORT KCModule *create_knetworkconfmodule(QWidget *parent, const char *name)
  {
    KGlobal::locale()->insertCatalogue("knetworkconf");
    return new KNetworkConfModule(parent, name);
  }
}

