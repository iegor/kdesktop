 /***************************************************************************
                          ModelinePlugin.cpp  -  description
                             -------------------
    begin                : Mon Apr 1 2002
    copyright            : (C) 2002 by John Firebaugh
    email                : jfirebaugh@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qregexp.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kaction.h>
#include <kdebug.h>

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/viewmanager.h>
#include <kate/mainwindow.h>
#include <kate/document.h>
#include <kate/view.h>

#include "ModelinePlugin.h"
#include "ModelinePlugin.moc"
                                 
class PluginView : public KXMLGUIClient
{             
  friend class ModelinePlugin;

  public:
    Kate::MainWindow *win;
};

K_EXPORT_COMPONENT_FACTORY( katemodelineplugin, KGenericFactory<ModelinePlugin>( "katemodeline" ) )

ModelinePlugin::ModelinePlugin( QObject* parent, const char* name, const QStringList &args )
	: Kate::Plugin ( (Kate::Application *) parent, name )
{
}

ModelinePlugin::~ModelinePlugin()
{
}

void ModelinePlugin::addView(Kate::MainWindow *win)
{
    // TODO: doesn't this have to be deleted?
    PluginView *view = new PluginView ();
               
    new KAction( i18n("Apply Modeline"), 0,
		this, SLOT(applyModeline()),
		view->actionCollection(), "edit_apply_modeline" );   
       
    view->setInstance (new KInstance("kate"));
    view->setXMLFile( "plugins/katemodeline/ui.rc" );
    win->guiFactory()->addClient (view);
    view->win = win; 
    
   m_views.append (view);
}   

void ModelinePlugin::removeView(Kate::MainWindow *win)
{
  for (uint z=0; z < m_views.count(); z++)
    if (m_views.at(z)->win == win)
    {
      PluginView *view = m_views.at(z);
      m_views.remove (view);
      win->guiFactory()->removeClient (view);
      delete view;
    }  
}

void ModelinePlugin::applyModeline()
{
        if (!application()->activeMainWindow())
          return;

	Kate::Document* doc = application()->documentManager()->activeDocument();
	Kate::View* view = application()->activeMainWindow()->viewManager()->activeView();
	if( !doc || !view ) return;
	static QRegExp vim1( "\\b(?:vi:|vim:|ex:)[ \\t](.*)" );
	static QRegExp vim2( "\\b(?:vi:|vim:|ex:)[ \\t]set (.*):" );
	uint foundAtLine;
	uint foundAtCol;
	uint matchLen;
	QString options;
	if( doc->searchText( 0, 0, vim2, &foundAtLine, &foundAtCol, &matchLen ) ) {
		options = vim2.cap(1);
	} else if( doc->searchText( 0, 0, vim1, &foundAtLine, &foundAtCol, &matchLen ) ) {
		options = vim1.cap(1);
		options.replace( QRegExp( ":" ), " " );
	} 
	uint configFlags = doc->configFlags();
	kdDebug() << "Found modeline: " << options << endl;
	if( options.find( QRegExp( "\\bnoet\\b" ) ) >= 0 ) {
		kdDebug() << "Clearing replace tabs" << endl;
		configFlags &= ~Kate::Document::cfReplaceTabs;
	} else if( options.find( QRegExp( "\\bet\\b" ) ) >= 0 ) {
		kdDebug() << "Setting replace tabs" << endl;
		configFlags |= Kate::Document::cfReplaceTabs;
	}
	QRegExp ts( "ts=(\\d+)" );
	if( options.find( ts ) >= 0 ) {
		uint tabWidth = ts.cap(1).toUInt();
		kdDebug() << "Setting tab width " << tabWidth << endl;
		view->setTabWidth( tabWidth );
	}
	QRegExp tw( "tw=(\\d+)" );
	if( options.find( tw ) >= 0 ) {
		uint textWidth = tw.cap(1).toUInt();
		kdDebug() << "Setting text width " << textWidth << endl;
		doc->setWordWrap( true );
		doc->setWordWrapAt( textWidth );
	}
	doc->setConfigFlags( configFlags );
}
