/***************************************************************************
                          katefll_initplugin.cpp  -  description
                             -------------------
    begin                : FRE July 12th 2002
    copyright            : (C) 2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "katefll_initplugin.h"
#include "katefll_initplugin.moc"

#include <kate/pluginmanager.h>

#include <qfileinfo.h>
#include <kgenericfactory.h>
#include <kaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>
                                     
K_EXPORT_COMPONENT_FACTORY( katefll_initplugin, KGenericFactory<InitPluginKateFileListLoader>( "katefll_loader" ) )

InitPluginKateFileListLoader::InitPluginKateFileListLoader (QObject * parent, const char *name, const QStringList datalist)
	:InitPlugin((Kate::Application*)parent,name)
{
}


InitPluginKateFileListLoader:: ~InitPluginKateFileListLoader()
{
}


int InitPluginKateFileListLoader::actionsKateShouldNotPerformOnRealStartup()
{
	return 0x1;
}

int InitPluginKateFileListLoader::initKate()
{
	QString tmpFile;
	KURL tmpURL;
	if( KIO::NetAccess::download( configScript(), tmpFile ) )
	{
		QFile file(tmpFile);
		file.open(IO_ReadOnly);
		
		QTextStream t(&file);

		bool somethingOpened=false;
	
		application()->documentManager()->closeAllDocuments();
		while (!t.eof())
		{
			somethingOpened=true;
//			application()->activeMainWindow()->viewManager()->openURL(KURL(t.readLine()));
			application()->documentManager()->openURL(tmpURL=KURL(t.readLine()));
		}
		if (!somethingOpened) application()->documentManager()->openURL(KURL());
		else if (application()->activeMainWindow())
		application()->activeMainWindow()->viewManager()->openURL(tmpURL);

		file.close();

		KIO::NetAccess::removeTempFile( tmpFile );

	} else application()->documentManager()->openURL(KURL());

	Kate::Plugin *pl=application()->pluginManager()->plugin("katefll_plugin");
	if (pl) 
	{
			connect(this,SIGNAL(updateInit()),pl,SLOT(updateInit()));
			updateInit();
			disconnect(this,SIGNAL(updateInit()),pl,SLOT(updateInit()));
			
/*      		int id = pl->metaObject()->findSlot( SLOT(updateInit()) );
	        if ( id != -1 )
		{
        		kdDebug()<<"Action slot was found, it will be called now"<<endl;
        		QUObject o[ 1 ];
        		mod->module->qt_invoke( id, o );
		} */
	}
	return 0;
}

