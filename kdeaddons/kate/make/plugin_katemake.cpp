/* plugin_katemake.h                    Kate Plugin
**
** Copyright (C) 2003 by Adriaan de Groot
**
** This is the hader for the Make plugin.
**
** This code was mostly copied from the GPL'ed xmlcheck plugin
** by Daniel Naber.
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

#include "plugin_katemake.moc"

#include <cassert>

#include <config.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qinputdialog.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qpalette.h>
#include <qvbox.h>
#include <qlabel.h>

#include <kaction.h>
#include <kcursor.h>
#include <kdebug.h>
#include <kdockwidget.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kpassivepopup.h>
#include <klineedit.h>
#include <kdialogbase.h>
#include <kconfig.h>

#include <kate/toolviewmanager.h>

#include <kgenericfactory.h>

K_EXPORT_COMPONENT_FACTORY( katemakeplugin, KGenericFactory<PluginKateMake>( "katemake" ) )

// #define FUNCTIONSETUP kdDebug() << k_funcinfo << endl;
#define FUNCTIONSETUP

PluginKateMake::PluginKateMake( QObject* parent, const char* name, const QStringList& )
	: Kate::Plugin ( (Kate::Application *)parent, name )
{
	FUNCTIONSETUP;
}


PluginKateMake::~PluginKateMake()
{
	FUNCTIONSETUP;
}


void PluginKateMake::addView(Kate::MainWindow *win)
{
	FUNCTIONSETUP;

	Kate::ToolViewManager *viewmanager = win->toolViewManager();
	QWidget *w = viewmanager->createToolView("kate_plugin_make",
			Kate::ToolViewManager::Bottom,
			SmallIcon(QString::fromLatin1("misc")),
			i18n("Make Output"));
	PluginKateMakeView *view = new PluginKateMakeView (w,win,
		"katemakeview");
	if( ! view ) {
		kdDebug() << "Error: no plugin view" << endl;
		return;
	}

	if( ! win ) {
		kdDebug() << "Error: no main win" << endl;
		return;
	}

	win->guiFactory()->addClient(view);
	view->win = win;
	m_views.append(view);
}


void PluginKateMake::removeView(Kate::MainWindow *win)
{
	FUNCTIONSETUP;

	for (unsigned int z=0; z < m_views.count(); z++) {
		if (m_views.at(z)->win == win) {
			PluginKateMakeView *view = m_views.at(z);
			m_views.remove (view);
			win->guiFactory()->removeClient (view);
		}
	}
}

#define COL_LINE	(1)
#define COL_FILE	(0)
#define COL_MSG		(2)


class ErrorMessage : public QListViewItem
{
public:
	ErrorMessage(QListView *parent,
		const QString &filename,
		int lineno,
		const QString &message) :
		QListViewItem(parent,
			filename,
			(lineno > 0 ? QString::number(lineno) : QString::null),
			message)
	{
		m_isError = !message.contains(QString::fromLatin1("warning"));
		m_lineno = lineno;
		m_serial = s_serial++;
	} 
	ErrorMessage(QListView *parent,const QString &message) :
		QListViewItem(parent,QString::null,QString::null,QString::null)
	{
		QString m(message);
		m.remove('\n');
		m.stripWhiteSpace();
		setText(COL_MSG,m);

		m_isError=false;
		m_lineno=-1;
		m_serial = s_serial++;
		setSelectable(false);
	} 
	ErrorMessage(QListView *parent, bool start) :
		QListViewItem(parent,QString::null)
	{
		m_isError=false;
		m_lineno=-1;
		m_serial=-1;
		setSelectable(false);
		if (start) setText(COL_MSG,i18n("Running make..."));
		else setText(COL_MSG,i18n("No Errors."));
	}
	virtual ~ErrorMessage() ;

	bool isError() const { return m_isError; } 
	QString message() const { return text(COL_MSG); } 
	QString fancyMessage() const;
	QString caption() const;
	QString filename() const { return text(COL_FILE); } 
	int line() const { return m_lineno; } 
	int serial() const { return m_serial; } 

	virtual int compare(QListViewItem *,int,bool) const;

	static void resetSerial() { s_serial=10; } 

protected:
	virtual void paintCell(QPainter *,const QColorGroup &,
		int,int,int);

	bool m_isError;
	int m_lineno;
	int m_serial;

	static int s_serial;
} ;

/* static */ int ErrorMessage::s_serial = 0;

/* virtual */ ErrorMessage::~ErrorMessage()
{
}

QString ErrorMessage::caption() const
{
	return QString::fromLatin1("%1:%2").arg(text(COL_FILE)).arg(line());
}

QString ErrorMessage::fancyMessage() const
{
	QString msg = QString::fromLatin1("<qt>");
	if (isError())
	{
		msg.append(QString::fromLatin1("<font color=\"red\">"));
	}
	msg.append(message());
	if (isError())
	{
		msg.append(QString::fromLatin1("</font>"));
	}
	msg.append(QString::fromLatin1("<qt>"));
	return msg;
}

/* virtual */ void ErrorMessage::paintCell(QPainter *p,
	const QColorGroup &cg,
	int column,
	int width,
	int align)
{
	if ((column!=COL_LINE) || (serial()<0))
	{
		QListViewItem::paintCell(p,cg,column,width,align);
	}
	else
	{
		QColorGroup myCG(cg);
#if 0
			red, 	//darkRed,
			green, 	//darkGreen,
			blue, 	//darkBlue,
			cyan, 	// darkCyan,
			magenta, // darkMagenta,
			yellow, //darkYellow,
			gray);
#endif
		myCG.setColor(QColorGroup::Light,red);

		if (!isSelected())
		{
			myCG.setColor(QColorGroup::Base,gray);
			myCG.setColor(QColorGroup::Text,
				m_isError ? red : yellow);
		}


		QListViewItem::paintCell(p,myCG,column,width,align);
	}
}

/* virtual */ int ErrorMessage::compare(QListViewItem *i,
	int /* column */ , bool /* ascending */) const
{
	kdDebug() << "In compare " << serial() << endl;
	ErrorMessage *e = dynamic_cast<ErrorMessage*>(i);
	if (!e) return 1;
	if (e->serial() < serial()) return 1;
	else if (e->serial() == serial()) return 0;
	else return -1;
}

class LinePopup : public KPassivePopup
{
protected:
	LinePopup( QWidget *parent=0, const char *name=0, WFlags f=0 );
	virtual ~LinePopup();

public:
	static LinePopup *message(QWidget *,
		const QPoint &p,ErrorMessage *e);

protected:
	virtual void positionSelf();

	QPoint fLoc;

	// There should be only one
	static LinePopup *one;
} ;

/* static */ LinePopup *LinePopup::one = 0L;

LinePopup::LinePopup(QWidget *p,const char *n,WFlags f) :
	KPassivePopup(p,n,f),
	fLoc(-1,-1)
{
	ASSERT(!one);
	one=this;
}

LinePopup::~LinePopup()
{
	one=0L;
}

/* static */ LinePopup *LinePopup::message(QWidget *parent,
	const QPoint &p,
	ErrorMessage *e)
{
	if (one) delete one;
	LinePopup *pop = new LinePopup( parent );
	pop->setAutoDelete( true );


	pop->setView( e->caption(), e->fancyMessage(), QPixmap() );
	// pop->hideDelay = timeout;
	pop->fLoc=p;
	pop->show();

	return pop;
}

/* virtual */ void LinePopup::positionSelf()
{
	if (fLoc.x()==-1) KPassivePopup::positionSelf();
	else
	{
		// Move above or below the intended line
		if (fLoc.y()>320) fLoc.setY(fLoc.y()-80);
		else fLoc.setY(fLoc.y()+80);
		moveNear(QRect(fLoc.x(),fLoc.y(),40,30));
	}
}

PluginKateMakeView::PluginKateMakeView(QWidget *parent,
	Kate::MainWindow *mainwin,
	const char* name) :
	QListView(parent,name),
	KXMLGUIClient(),
	win(mainwin),
	filenameDetector(0L),
	running_indicator(0L)
{
	FUNCTIONSETUP;

	m_proc=0;
	(void) new KAction ( i18n("Next Error"), KShortcut(ALT+CTRL+Key_Right),
		this, SLOT( slotNext() ),
		actionCollection(), "make_right" );

	(void) new KAction ( i18n("Previous Error"), KShortcut(ALT+CTRL+Key_Left),
		this, SLOT( slotPrev() ),
		actionCollection(), "make_left" );

	(void) new KAction ( i18n("Make"), KShortcut(ALT+Key_R),
		this, SLOT( slotValidate() ),
		actionCollection(), "make_check" );

	(void) new KAction ( i18n("Configure..."), KShortcut(),
		this, SLOT( slotConfigure() ),
		actionCollection(), "make_settings" );

	setInstance(new KInstance("kate"));
	setXMLFile(QString::fromLatin1("plugins/katemake/ui.rc"));


	setFocusPolicy(QWidget::NoFocus);
	setSorting(COL_LINE);

	addColumn(i18n("File"), -1);
	addColumn(i18n("Line"), -1);
	setColumnAlignment(COL_LINE, AlignRight);
	addColumn(i18n("Message"), -1);
	setAllColumnsShowFocus(true);
	setResizeMode(QListView::LastColumn);
	connect(this, SIGNAL(clicked(QListViewItem *)), SLOT(slotClicked(QListViewItem *)));

	m_proc = new KProcess();

	connect(m_proc, SIGNAL(processExited(KProcess*)), this, SLOT(slotProcExited(KProcess*)));
	connect(m_proc, SIGNAL(receivedStderr(KProcess*,char*,int)),
		this, SLOT(slotReceivedProcStderr(KProcess*, char*, int)));


	KConfig c("katemakepluginrc");
	c.setGroup("Prefixes");
	source_prefix = c.readEntry("Source",QString::null);
	build_prefix = c.readEntry("Build",QString::null);

//	if (source_prefix.isEmpty())
	{
		filenameDetector = new QRegExp(
			QString::fromLatin1("[a-zA-Z0-9_\\.\\-]*\\.[chp]*:[0-9]*:"));
	}
//	else
	{
//		filenameDetector = 0L;
	}
}


PluginKateMakeView::~PluginKateMakeView()
{
	FUNCTIONSETUP;

	delete m_proc;
	delete filenameDetector;
	delete running_indicator;
}


void PluginKateMakeView::processLine(const QString &l)
{
	kdDebug() << "Got line " << l ;


	if (!filenameDetector && l.find(source_prefix)!=0)
	{
		/* ErrorMessage *e = */ (void) new ErrorMessage(this,l);
		return;
	}

	if (filenameDetector && l.find(*filenameDetector)<0)
	{
		ErrorMessage *e = new ErrorMessage(this,l);
		kdDebug() << "Got message(1) #" << e->serial() << endl;
		return;
	}

	int ofs1 = l.find(':');
	int ofs2 = l.find(':',ofs1+1);
	//
	QString m = l.mid(ofs2+1);
	m.remove('\n');
	m.stripWhiteSpace();
	QString filename = l.left(ofs1);
	int line = l.mid(ofs1+1,ofs2-ofs1-1).toInt();
	ErrorMessage *e = new ErrorMessage(this,
		filename,line,m);
	kdDebug() << "Got message(2) #" << e->serial() << endl;

	// Should cache files being found and check for
	// existence.
	 kdDebug() << ": Looking at " << document_dir+filename << endl;
	if (!QFile::exists(document_dir+filename))
	{
		e->setSelectable(false);
	}
	if (filename.startsWith(source_prefix) && !source_prefix.isEmpty())
	{
		e->setSelectable(true);
	}
	found_error=true;
}


void PluginKateMakeView::slotReceivedProcStderr(KProcess *, char *result, int len)
{
	FUNCTIONSETUP;

	QString l = QString::fromLocal8Bit( QCString(result, len+1) );

	output_line += l;

	int nl_p = -1;
	while ((nl_p = output_line.find('\n')) > 1)
	{
		processLine(output_line.left(nl_p+1));
		output_line.remove(0,nl_p+1);
	}
}


void PluginKateMakeView::slotProcExited(KProcess *p)
{
	FUNCTIONSETUP;

	delete running_indicator;
	running_indicator=0L;

	if (!output_line.isEmpty())
	{
		processLine(output_line);
	}

#if 0
	// FIXME: doesn't work correct the first time:
	if( m_dockwidget->isDockBackPossible() ) {
		m_dockwidget->dockBack();
	}
#endif

	kdDebug() << "slotProcExited()" << endl;
	QApplication::restoreOverrideCursor();

	sort();

	if ( found_error || !p->normalExit() || p->exitStatus() )
	{
		QListViewItem *i = firstChild();
		while (i && !i->isSelectable())
		{
			i = i->nextSibling();
		}
		if (i)
		{
			setSelected(i,true);
			slotClicked(i);
		}
	}
	else
	{
		KPassivePopup::message(i18n("Make Results"),
			i18n("No errors."),
			this);
		clear();
#if 0
		QListViewItem *i = new QListViewItem(this,QString::null,
			QString::null,
			i18n("No Errors."));
		i->setSelectable(false);
#else
		(void) new ErrorMessage(this,false);
#endif
	}
}


void PluginKateMakeView::slotClicked(QListViewItem *item)
{
	FUNCTIONSETUP;
	if (!item)
	{
		kdDebug() << ": No item clicked." << endl;
		return;
	}
	if (!item->isSelectable()) return;

	ErrorMessage *e = dynamic_cast<ErrorMessage *>(item);
	if (!e) return;

	ensureItemVisible(e);

	QString filename = document_dir + e->filename();
	int lineno = e->line();

	if (!build_prefix.isEmpty())
	{
		filename = e->filename();
	}


	kdDebug() << ": Looking at " << filename
		<< ":" << lineno << endl;

	if (QFile::exists(filename))
	{
		KURL u;
		u.setPath(filename);
		win->viewManager()->openURL(u);
		Kate::View *kv = win->viewManager()->activeView();

		kv->setCursorPositionReal(lineno-1,1);


		QPoint globalPos = kv->mapToGlobal(kv->cursorCoordinates());
		kdDebug() << ": Want to map at "
			<< globalPos.x() << "," << globalPos.y() << endl;
#if 0
		KPassivePopup::message(
			QString::fromLatin1("%1:%2").arg(filename).arg(lineno),
			msg,
			this);
#else
              if ( ! isVisible() )
		LinePopup::message(this,globalPos,e);
#endif
	}
}

void PluginKateMakeView::slotNext()
{
	FUNCTIONSETUP;

	QListViewItem *i = selectedItem();
	if (!i) return;

	QListViewItem *n = i;
	while ((n=n->nextSibling()))
	{
		if (n->isSelectable())
		{
			if (n==i) return;
			setSelected(n,true);
			ensureItemVisible(n);
			slotClicked(n);
			return;
		}
	}
}

void PluginKateMakeView::slotPrev()
{
	FUNCTIONSETUP;

	QListViewItem *i = selectedItem();
	if (!i) return;

	QListViewItem *n = i;
	while ((n=n->itemAbove()))
	{
		if (n->isSelectable())
		{
			if (n==i) return;
			setSelected(n,true);
			ensureItemVisible(n);
			slotClicked(n);
			return;
		}
	}
}

bool PluginKateMakeView::slotValidate()
{
	FUNCTIONSETUP;

	clear();
	win->toolViewManager()->showToolView (this);

	m_proc->clearArguments();

	Kate::View *kv = win->viewManager()->activeView();
	if( ! kv ) {
		kdDebug() << "Error (slotValidate()): no Kate::View" << endl;
		return false;
	}
	if( ! kv->getDoc() ) {
		kdDebug() << "Error (slotValidate()): no kv->getDoc()" << endl;
		return false;
	}

	Kate::Document *doc = (Kate::Document*)kv->document();
	doc->save();
	KURL url(doc->url());

	output_line = QString::null;
	ErrorMessage::resetSerial();
	found_error=false;

	kdDebug() << ": Document " << url.protocol() << " : " <<
		url.path() << endl;

	if (!url.isLocalFile())
	{
		KMessageBox::sorry(0,
			i18n("The file <i>%1</i> is not a local file. "
				"Non-local files cannot be compiled.")
				.arg(url.path()));
		return false;
	}

	document_dir = QFileInfo(url.path()).dirPath(true) +
		QString::fromLatin1("/");

	if (document_dir.startsWith(source_prefix))
	{
		document_dir = build_prefix + document_dir.mid(source_prefix.length());
	}

	m_proc->setWorkingDirectory(document_dir);
	QString make = KStandardDirs::findExe( "gmake" );
	if (make.isEmpty())
		make = KStandardDirs::findExe("make");
	*m_proc << make;
	if( make.isEmpty() || ! m_proc->start(KProcess::NotifyOnExit, KProcess::AllOutput) ) {
		KMessageBox::error(0, i18n("<b>Error:</b> Failed to run %1.").arg(make.isEmpty() ?
			"make" : make));
		return false;
	}
	QApplication::setOverrideCursor(KCursor::waitCursor());
	running_indicator = new ErrorMessage(this,true);
	return true;
}

class Settings : public KDialogBase
{
public:
	Settings( QWidget *parent,
		const QString &src, const QString &bld);

	KLineEdit *edit_src,*edit_bld;
} ;


Settings::Settings(QWidget *p,
	const QString &s, const QString &b) :
	KDialogBase(p,"settings",true,
	i18n("Directories"),
	KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true )
{
	QVBox *page = makeVBoxMainWidget();
	QHBox *h = new QHBox(page);
	(void) new QLabel(i18n("Source prefix:"),h);
	edit_src = new KLineEdit(h);
	edit_src->setText(s);

	h = new QHBox(page);
	(void) new QLabel(i18n("Build prefix:"),h);
	edit_bld = new KLineEdit(h);
	edit_bld->setText(b);
}

void PluginKateMakeView::slotConfigure()
{
	Kate::View *kv = win->viewManager()->activeView();


	Settings s(kv,source_prefix,build_prefix);

	if (!s.exec()) return;


	source_prefix = s.edit_src->text();
	build_prefix = s.edit_bld->text();

	//if (source_prefix.isEmpty())
	{
		if (!filenameDetector)
		{
			filenameDetector = new QRegExp(
				QString::fromLatin1("[a-zA-Z0-9_\\.\\-]*\\.[chp]*:[0-9]*:"));
		}
	}
	//else
	{
// 		if (filenameDetector)
// 		{
// 			delete filenameDetector;
// 			filenameDetector = 0L;
// 		}
	}

	KConfig c("katemakepluginrc");
	c.setGroup("Prefixes");
	c.writeEntry("Source",source_prefix);
	c.writeEntry("Build",build_prefix);
}
