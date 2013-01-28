
#include "plugin_katehelloworld.h"
#include "plugin_katehelloworld.moc"

#include <kaction.h>
#include <klocale.h>
#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( katehelloworldplugin, KGenericFactory<KatePluginHelloWorld>( "katehelloworld" ) )
                       
class PluginView : public KXMLGUIClient
{             
  friend class KatePluginHelloWorld;

  public:
    Kate::MainWindow *win;
};

KatePluginHelloWorld::KatePluginHelloWorld( QObject* parent, const char* name, const QStringList& )
    : Kate::Plugin ( (Kate::Application*)parent, name )
{
}

KatePluginHelloWorld::~KatePluginHelloWorld()
{
}

void KatePluginHelloWorld::addView(Kate::MainWindow *win)
{
    // TODO: doesn't this have to be deleted?
    PluginView *view = new PluginView ();
             
     (void) new KAction ( i18n("Insert Hello World"), 0, this,
                      SLOT( slotInsertHello() ), view->actionCollection(),
                      "edit_insert_helloworld" );
    
    view->setInstance (new KInstance("kate"));
    view->setXMLFile("plugins/katehelloworld/ui.rc");
    win->guiFactory()->addClient (view);
    view->win = win; 
    
   m_views.append (view);
}   

void KatePluginHelloWorld::removeView(Kate::MainWindow *win)
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

void KatePluginHelloWorld::slotInsertHello()
{
  if (!application()->activeMainWindow())
    return;

  Kate::View *kv = application()->activeMainWindow()->viewManager()->activeView();

  if (kv)
    kv->insertText ("Hello World");
}
