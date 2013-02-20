#ifndef _PLUGIN_KATE_HELLOWORLD_H_
#define _PLUGIN_KATE_HELLOWORLD_H_

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>

class KatePluginHelloWorld : public Kate::Plugin, Kate::PluginViewInterface
{
  Q_OBJECT

  public:
    KatePluginHelloWorld( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
    virtual ~KatePluginHelloWorld();

    void addView (Kate::MainWindow *win);
    void removeView (Kate::MainWindow *win);
    
  public slots:
    void slotInsertHello();  
    
  private:
    QPtrList<class PluginView> m_views; 
};

#endif
