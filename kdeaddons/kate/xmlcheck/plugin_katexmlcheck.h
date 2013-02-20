 /***************************************************************************
                           plugin_katexmlcheck.h
                           -------------------
	begin                : 2002-07-06
	copyright            : (C) 2002 by Daniel Naber
	email                : daniel.naber@t-online.de
 ***************************************************************************/

/***************************************************************************
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 ***************************************************************************/

#ifndef _PLUGIN_KANT_XMLCHECK_H
#define _PLUGIN_KANT_XMLCHECK_H

#include <qlistview.h>
#include <qstring.h>

#include <kate/plugin.h>
#include <kate/application.h>
#include <kate/view.h>
#include <kate/document.h>
#include <kate/documentmanager.h>
#include <kate/mainwindow.h>
#include <kate/viewmanager.h>

#include <kdockwidget.h>
#include <kiconloader.h>
#include <kprocess.h>
#include <ktempfile.h>

class PluginKateXMLCheckView : public QListView, public KXMLGUIClient
{
  Q_OBJECT

  public:
	PluginKateXMLCheckView(QWidget *parent,Kate::MainWindow *mainwin,const char* name);
	virtual ~PluginKateXMLCheckView();

	Kate::MainWindow *win;
	QWidget *dock;

  public slots:
	bool slotValidate();
	void slotClicked(QListViewItem *item);
	void slotProcExited(KProcess*);
	void slotReceivedProcStderr(KProcess*, char*, int);
	void slotUpdate();

  private:
	KTempFile *m_tmp_file;
	KParts::ReadOnlyPart *part;
	bool m_validating;
	KProcess *m_proc;
	QString m_proc_stderr;
	QString m_dtdname;
};


class PluginKateXMLCheck : public Kate::Plugin, Kate::PluginViewInterface
{
  Q_OBJECT

  public:
	PluginKateXMLCheck( QObject* parent = 0, const char* name = 0, const QStringList& = QStringList() );
	virtual ~PluginKateXMLCheck();

	void addView (Kate::MainWindow *win);
	void removeView (Kate::MainWindow *win);

  private:
	QPtrList<PluginKateXMLCheckView> m_views;
};

#endif // _PLUGIN_KANT_XMLCHECK_H
