/***************************************************************************
                          plugin_katehtmltools.cpp  -  description
                             -------------------
    begin                : FRE Feb 23 2001
    copyright            : (C) 2001 by Joseph Wenninger
    email                : jowenn@bigfoot.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "plugin_katehtmltools.h"
#include "plugin_katehtmltools.moc"

#include <klineeditdlg.h>
#include <kaction.h>
#include <kinstance.h>
#include <klocale.h>
#include <cassert>
#include <kdebug.h>
#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( katehtmltoolsplugin, KGenericFactory<PluginKateHtmlTools>( "katehtmltools" ) )

class PluginView : public KXMLGUIClient
{
  friend class PluginKateHtmlTools;

  public:
    Kate::MainWindow *win;
};

PluginKateHtmlTools::PluginKateHtmlTools( QObject* parent, const char* name, const QStringList& )
    : Kate::Plugin ( (Kate::Application *)parent, name )
{
}

PluginKateHtmlTools::~PluginKateHtmlTools()
{
}

void PluginKateHtmlTools::addView(Kate::MainWindow *win)
{
    // TODO: doesn't this have to be deleted?
    PluginView *view = new PluginView ();

     (void)  new KAction ( i18n("HT&ML Tag..."), /*"edit_HTML_tag",*/ ALT + Key_Minus, this,
                                SLOT( slotEditHTMLtag() ), view->actionCollection(), "edit_HTML_tag" );

    view->setInstance (new KInstance("kate"));
    view->setXMLFile( "plugins/katehtmltools/ui.rc" );
    win->guiFactory()->addClient (view);
    view->win = win;

   m_views.append (view);
}

void PluginKateHtmlTools::removeView(Kate::MainWindow *win)
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

void PluginKateHtmlTools::slotEditHTMLtag()
//  PCP
{
  if (!application()->activeMainWindow())
    return;

  Kate::View *kv=application()->activeMainWindow()->viewManager()->activeView();
  if (!kv) return;

  QString text ( KatePrompt ( i18n("HTML Tag"),
                        i18n("Enter HTML tag contents (the <, >, and closing tag will be supplied):"),
                        (QWidget *)kv)
                         );

  if ( !text.isEmpty () )
      slipInHTMLtag (*kv, text); // user entered something and pressed ok

}


QString PluginKateHtmlTools::KatePrompt
        (
        const QString & strTitle,
        const QString & strPrompt,
        QWidget * that
        )
{
  //  TODO: Make this a "memory edit" field with a combo box
  //  containing prior entries

  KLineEditDlg dlg(strPrompt, QString::null, that);
  dlg.setCaption(strTitle);

  if (dlg.exec())
    return dlg.text();
  else
    return "";
}


void PluginKateHtmlTools::slipInHTMLtag (Kate::View & view, QString text)  //  PCP
{

  //  We must add a heavy elaborate HTML markup system. Not!

  QStringList list = QStringList::split (' ', text);
  QString marked = view.getDoc()->selection ();
  uint preDeleteLine = 0, preDeleteCol = 0;
  view.cursorPosition (&preDeleteLine, &preDeleteCol);

  if (marked.length() > 0)
    view.keyDelete ();
  uint line = 0, col = 0;
  view.cursorPosition (&line, &col);
  QString pre ("<" + text + ">");
  QString post;
  if (list.count () > 0)  post = "</" + list[0] + ">";
  view.insertText (pre + marked + post);

  //  all this muck to leave the cursor exactly where the user
  //  put it...

  //  Someday we will can all this (unless if it already
  //  is canned and I didn't find it...)

  //  The second part of the if disrespects the display bugs
  //  when we try to reselect. TODO: fix those bugs, and we can
  //  un-break this if...

  if (preDeleteLine == line && -1 == marked.find ('\n'))
    if (preDeleteLine == line && preDeleteCol == col)
        {
        view.setCursorPosition (line, col + pre.length () + marked.length () - 1);

        for (int x (marked.length());  x--;)
                view.shiftCursorLeft ();
        }
    else
        {
        view.setCursorPosition (line, col += pre.length ());

        for (int x (marked.length());  x--;)
                view.shiftCursorRight ();
        }

}
