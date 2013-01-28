/* $Id: plugin_domtreeviewer.h 541824 2006-05-17 12:17:30Z dfaure $ */

#ifndef __plugin_domtreeviewer_h
#define __plugin_domtreeviewer_h

#include <kparts/plugin.h>
// #include <klibloader.h>

class DOMTreeWindow;
class KURL;

class PluginDomtreeviewer : public KParts::Plugin
{
  Q_OBJECT
public:
  PluginDomtreeviewer( QObject* parent, const char* name,
	               const QStringList & );
  virtual ~PluginDomtreeviewer();

public slots:
  void slotShowDOMTree();
  void slotDestroyed();
private:
  DOMTreeWindow* m_dialog;
};

#endif
