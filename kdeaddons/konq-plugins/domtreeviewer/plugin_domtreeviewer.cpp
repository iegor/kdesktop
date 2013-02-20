#include "plugin_domtreeviewer.h"
#include "domtreewindow.h"
#include "domtreeview.h"

#include <kaction.h>
#include <kinstance.h>
#include <klibloader.h>

#include <kdebug.h>
#include <klocale.h>
#include <kgenericfactory.h>

#include <khtml_part.h>

typedef KGenericFactory<PluginDomtreeviewer> DomtreeviewerFactory;
K_EXPORT_COMPONENT_FACTORY( libdomtreeviewerplugin, DomtreeviewerFactory( "domtreeviewer" ) )

PluginDomtreeviewer::PluginDomtreeviewer( QObject* parent, const char* name,
	                                  const QStringList & )
  : Plugin( parent, name ), m_dialog( 0 )
{
  (void) new KAction( i18n("Show &DOM Tree"),
                      "domtreeviewer", 0,
                      this, SLOT(slotShowDOMTree()),
                      actionCollection(), "viewdomtree" );
}

PluginDomtreeviewer::~PluginDomtreeviewer()
{
  kdDebug(90180) << k_funcinfo << endl;
  delete m_dialog;
}

void PluginDomtreeviewer::slotShowDOMTree()
{
  if ( m_dialog )
  {
    delete m_dialog;
    Q_ASSERT((DOMTreeWindow *)m_dialog == (DOMTreeWindow *)0);
  }
  if (KHTMLPart *part = ::qt_cast<KHTMLPart *>(parent()))
  {
    m_dialog = new DOMTreeWindow(this);
    connect( m_dialog, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
    m_dialog->view()->setHtmlPart(part);
    m_dialog->show();
  }
}

void PluginDomtreeviewer::slotDestroyed()
{
  kdDebug(90180) << k_funcinfo << endl;
  m_dialog = 0;
}

#include <plugin_domtreeviewer.moc>
