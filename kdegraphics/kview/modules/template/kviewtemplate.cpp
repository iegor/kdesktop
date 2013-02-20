/* This file is in the public domain */

// $Id: kviewtemplate.cpp 285970 2004-02-07 23:36:38Z goossens $

#include "kviewtemplate.h"

#include <qobjectlist.h>

#include <kaction.h>
/*#include <klocale.h>*/
#include <kgenericfactory.h>
#include <kdebug.h>
#include <kimageviewer/viewer.h>

typedef KGenericFactory<KViewTemplate> KViewTemplateFactory;
K_EXPORT_COMPONENT_FACTORY( kview_templateplugin, KViewTemplateFactory( "kviewtemplateplugin" ) )

KViewTemplate::KViewTemplate( QObject* parent, const char* name, const QStringList & )
	: Plugin( parent, name )
{
	QObjectList * viewerList = parent->queryList( 0, "KImageViewer Part", false, false );
	m_pViewer = static_cast<KImageViewer::Viewer *>( viewerList->getFirst() );
	delete viewerList;
	if( m_pViewer )
	{
		(void) new KAction( /*i18n(*/ "&Do Something" /*)*/, 0, 0,
							this, SLOT( yourSlot() ),
							actionCollection(), "plugin_template" );
	}
	else
		kdWarning( 4630 ) << "no KImageViewer interface found - the template plugin won't work" << endl;
}

KViewTemplate::~KViewTemplate()
{
}

void KViewTemplate::yourSlot()
{
}

// vim:sw=4:ts=4:cindent
#include "kviewtemplate.moc"
