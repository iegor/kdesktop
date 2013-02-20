/***************************************************************************
                          plugin_katetextfilter.cpp  -  description
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

#include "plugin_katetextfilter.h"
#include "plugin_katetextfilter.moc"

#include <kaction.h>
#include <kinstance.h>
#include <klineeditdlg.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <cassert>
#include <kdebug.h>
#include <qstring.h>
#include <ktexteditor/editinterfaceext.h>
#include <kapplication.h>
#include <kcompletion.h>
#include <klineedit.h>
#define POP_(x) kdDebug(13000) << #x " = " << flush << x << endl

#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( katetextfilterplugin, KGenericFactory<PluginKateTextFilter>( "katetextfilter" ) )

class PluginView : public KXMLGUIClient
{
  friend class PluginKateTextFilter;

  public:
    Kate::MainWindow *win;
};

PluginKateTextFilter::PluginKateTextFilter( QObject* parent, const char* name, const QStringList& )
  : Kate::Plugin ( (Kate::Application *)parent, name ),
    Kate::Command(),
    m_pFilterShellProcess (NULL)
{
  Kate::Document::registerCommand( this );
}

PluginKateTextFilter::~PluginKateTextFilter()
{
  delete m_pFilterShellProcess;
  Kate::Document::unregisterCommand( this );
}

void PluginKateTextFilter::addView(Kate::MainWindow *win)
{
    // TODO: doesn't this have to be deleted?
    PluginView *view = new PluginView ();

    (void)  new KAction ( i18n("Filter Te&xt..."), /*"edit_filter",*/ CTRL + Key_Backslash, this,
  SLOT( slotEditFilter() ), view->actionCollection(), "edit_filter" );

    view->setInstance (new KInstance("kate"));
    view->setXMLFile( "plugins/katetextfilter/ui.rc" );
    win->guiFactory()->addClient (view);
    view->win = win;

   m_views.append (view);
}

void PluginKateTextFilter::removeView(Kate::MainWindow *win)
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

        void
splitString (QString q, char c, QStringList &list)  //  PCP
{

// screw the OnceAndOnlyOnce Principle!

  int pos;
  QString item;

  while ( (pos = q.find(c)) >= 0)
    {
      item = q.left(pos);
      list.append(item);
      q.remove(0,pos+1);
    }
  list.append(q);
}


        static void  //  PCP
slipInNewText (Kate::View & view, QString pre, QString marked, QString post, bool reselect)
{

  uint preDeleteLine = 0, preDeleteCol = 0;
  view.cursorPosition (&preDeleteLine, &preDeleteCol);

  if (marked.length() > 0)
    view.keyDelete ();
  uint line = 0, col = 0;
  view.cursorPosition (&line, &col);
  view.insertText (pre + marked + post);

  //  all this muck to leave the cursor exactly where the user
  //  put it...

  //  Someday we will can all this (unless if it already
  //  is canned and I didn't find it...)

  //  The second part of the if disrespects the display bugs
  //  when we try to reselect. TODO: fix those bugs, and we can
  //  un-break this if...

  //  TODO: fix OnceAndOnlyOnce between this module and plugin_katehtmltools.cpp

  if (reselect && preDeleteLine == line && -1 == marked.find ('\n'))
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


        static QString  //  PCP
KatePrompt
        (
        const QString & strTitle,
        const QString & strPrompt,
        QWidget * that,
	QStringList *completionList
        )
{
  //  TODO: Make this a "memory edit" field with a combo box
  //  containing prior entries

  KLineEditDlg dlg(strPrompt, QString::null, that);
  dlg.setCaption(strTitle);
  KCompletion *comple=dlg.lineEdit()->completionObject();
  comple->setItems(*completionList);
  if (dlg.exec()) {
    if (!dlg.text().isEmpty())	{
	comple->addItem(dlg.text());
	(*completionList)=comple->items();
    }
    return dlg.text();
  }
  else
    return "";
}


	void
PluginKateTextFilter::slotFilterReceivedStdout (KProcess * pProcess, char * got, int len)
{

	assert (pProcess == m_pFilterShellProcess);

	if (got && len)
		{

  //  TODO: got a better idea?

//   		while (len--)  m_strFilterOutput += *got++;
    m_strFilterOutput += QString::fromLocal8Bit( got, len );
//		POP_(m_strFilterOutput);
		}

}


	void
PluginKateTextFilter::slotFilterReceivedStderr (KProcess * pProcess, char * got, int len)
	{
	slotFilterReceivedStdout (pProcess, got, len);
	}


	void
PluginKateTextFilter::slotFilterProcessExited (KProcess * pProcess)
{

	assert (pProcess == m_pFilterShellProcess);
	Kate::View * kv (application()->activeMainWindow()->viewManager()->activeView());
	if (!kv) return;
	KTextEditor::EditInterfaceExt *ext=KTextEditor::editInterfaceExt(kv->getDoc());
	if (ext) ext->editBegin();
	QString marked = kv->getDoc()->selection ();
	if (marked.length() > 0)
          kv -> keyDelete ();
	kv -> insertText (m_strFilterOutput);
	if (ext) ext->editEnd();
//	slipInNewText (*kv, "", m_strFilterOutput, "", false);
	m_strFilterOutput = "";

}


        static void  //  PCP
slipInFilter (KShellProcess & shell, Kate::View & view, QString command)
{
  QString marked = view.getDoc()->selection ();
  if( marked.isEmpty())
      return;
//  POP_(command.latin1 ());
  shell.clearArguments ();
  shell << command;

  shell.start (KProcess::NotifyOnExit, KProcess::All);
  shell.writeStdin (marked.local8Bit (), marked.length ());
  //  TODO: Put up a modal dialog to defend the text from further
  //  keystrokes while the command is out. With a cancel button...

}


	void
PluginKateTextFilter::slotFilterCloseStdin (KProcess * pProcess)
	{
	assert (pProcess == m_pFilterShellProcess);
	pProcess -> closeStdin ();
	}


                 void
PluginKateTextFilter::slotEditFilter ()  //  PCP
{
  if (!kapp->authorize("shell_access")) {
      KMessageBox::sorry(0,i18n(
          "You are not allowed to execute arbitrary external applications. If "
          "you want to be able to do this, contact your system administrator."),
          i18n("Access Restrictions"));
      return;
  }
  if (!application()->activeMainWindow())
    return;

  Kate::View * kv (application()->activeMainWindow()->viewManager()->activeView());
  if (!kv) return;

  QString text ( KatePrompt ( i18n("Filter"),
                        i18n("Enter command to pipe selected text through:"),
                        (QWidget*)  kv,
			&completionList
                        ) );

  if ( !text.isEmpty () )
    runFilter( kv, text );
}

void PluginKateTextFilter::runFilter( Kate::View *kv, const QString &filter )
{
  m_strFilterOutput = "";

  if (!m_pFilterShellProcess)
  {
    m_pFilterShellProcess = new KShellProcess;

    connect ( m_pFilterShellProcess, SIGNAL(wroteStdin(KProcess *)),
              this, SLOT(slotFilterCloseStdin (KProcess *)));

    connect ( m_pFilterShellProcess, SIGNAL(receivedStdout(KProcess*,char*,int)),
              this, SLOT(slotFilterReceivedStdout(KProcess*,char*,int)) );

    connect ( m_pFilterShellProcess, SIGNAL(receivedStderr(KProcess*,char*,int)),
              this, SLOT(slotFilterReceivedStderr(KProcess*,char*,int)) );

    connect ( m_pFilterShellProcess, SIGNAL(processExited(KProcess*)),
              this, SLOT(slotFilterProcessExited(KProcess*) ) ) ;
  }

  slipInFilter (*m_pFilterShellProcess, *kv, filter);
}

//BEGIN Kate::Command methods
QStringList PluginKateTextFilter::cmds()
{
  return QStringList("textfilter");
}

bool PluginKateTextFilter::help( Kate::View *, const QString&, QString &msg )
{
  msg = i18n(
      "<qt><p>Usage: <code>textfilter COMMAND</code></p>"
      "<p>Replace the selection with the output of the specified shell command.</p></qt>");
  return true;
}

bool PluginKateTextFilter::exec( Kate::View *v, const QString &cmd, QString &msg )
{
  if (! v->getDoc()->hasSelection() )
  {
    msg = i18n("You need to have a selection to use textfilter");
    return false;
  }

  QString filter = cmd.section( " ", 1 ).stripWhiteSpace();

  if ( filter.isEmpty() )
  {
    msg = i18n("Usage: textfilter COMMAND");
    return false;
  }

  runFilter( v, filter );
  return true;
}
//END
// kate: space-indent on; indent-width 2; replace-tabs on; mixed-indent off;
