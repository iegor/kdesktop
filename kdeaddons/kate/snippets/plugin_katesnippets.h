/*
 * Copyright (C) 2004 Stephan Möres <Erdling@gmx.net>
 */

#ifndef _PLUGIN_KATESNIPPETS_H_
#define _PLUGIN_KATESNIPPETS_H_

#include <kate/application.h>
#include <kate/documentmanager.h>
#include <kate/document.h>
#include <kate/mainwindow.h>
#include <kate/plugin.h>
#include <kate/view.h>
#include <kate/viewmanager.h>
#include <klibloader.h>
#include <klocale.h>

#include <kate/toolviewmanager.h>

#include <qlistview.h>
#include <qregexp.h>
#include <kconfig.h>
#include <qptrlist.h>
#include <qtoolbutton.h>
#include <qtextedit.h>
#include <kiconloader.h>

#include "csnippet.h"
#include "cwidgetsnippets.h"

class KatePluginSnippetsView : public CWidgetSnippets, public KXMLGUIClient {

  Q_OBJECT

  friend class KatePluginSnippets;

public:
  KatePluginSnippetsView (Kate::MainWindow *w, QWidget *dock);
  virtual ~KatePluginSnippetsView ();
  CSnippet* findSnippetByListViewItem(QListViewItem *item);

public slots:
  void slot_lvSnippetsSelectionChanged(QListViewItem  * item);
  void slot_lvSnippetsClicked (QListViewItem * item);
  void slot_lvSnippetsItemRenamed(QListViewItem *lvi,int col, const QString& text);
  void slot_btnNewClicked();
  void slot_btnSaveClicked();
  void slot_btnDeleteClicked();

protected:
  void readConfig();
  void writeConfig();

private:
  KConfig                                 *config;
  QPtrList<CSnippet>                      lSnippets;

public:
  Kate::MainWindow *win;
  QWidget *dock;
};

class KatePluginSnippets : public Kate::Plugin, Kate::PluginViewInterface {
  Q_OBJECT

public:
  KatePluginSnippets( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
  virtual ~KatePluginSnippets();

  void addView (Kate::MainWindow *win);
  void removeView (Kate::MainWindow *win);

private:
  QPtrList<class KatePluginSnippetsView>  m_views;

};

#endif // _PLUGIN_KATESNIPPETS_H_
