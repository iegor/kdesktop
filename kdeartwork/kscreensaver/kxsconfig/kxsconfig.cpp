//-----------------------------------------------------------------------------
//
// KDE xscreensaver configuration dialog
//
// Copyright (c)  Martin R. Jones <mjones@kde.org> 1999
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation;
// version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

// This file contains code copied from xscreensaver.  The xscreensaver
// copyright notice follows.

/*
 * xscreensaver, Copyright (c) 1993-2002 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 */

#include <config.h>

#include <stdlib.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdialogbase.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>

#include "kxsconfig.h"
#include "kxscontrol.h"
#include "kxsxml.h"

template class QPtrList<KXSConfigItem>;

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>

int ignoreXError(Display *, XErrorEvent *);

//===========================================================================

const uint widgetEventMask =                 // X event mask
(uint)(
       ExposureMask |
       PropertyChangeMask |
       StructureNotifyMask
      );

KXSConfigDialog::KXSConfigDialog(const QString &filename, const QString &name)
  : KDialogBase(Plain, name, Ok| Cancel, Ok, 0, 0, false),
    mFilename(filename), mPreviewProc(0), mKilled(true)
{
    int slash = filename.findRev('/');
    if (slash >= 0)
	mConfigFile = filename.mid(slash+1);
    else
	mConfigFile = filename;

    mExeName = mConfigFile;
    mConfigFile += "rc";
}

bool KXSConfigDialog::create()
{
    QVBoxLayout *topLayout = new QVBoxLayout(plainPage(), spacingHint());
    QHBoxLayout *layout = new QHBoxLayout(topLayout, spacingHint());
    QVBox *controlLayout = new QVBox(plainPage());
    controlLayout->setSpacing(spacingHint());
    layout->addWidget(controlLayout);
    ((QBoxLayout*)controlLayout->layout())->addStrut(120);

    KConfig config(mConfigFile);

    QString xmlFile = "/doesntexist";
#ifdef XSCREENSAVER_CONFIG_DIR
    xmlFile = XSCREENSAVER_CONFIG_DIR;
#endif

    xmlFile += "/" + mExeName + ".xml";
    if ( QFile::exists( xmlFile ) ) {
	// We can use the xscreensaver xml config files.
	KXSXml xmlParser(controlLayout);
	xmlParser.parse( xmlFile );
	mConfigItemList = *xmlParser.items();
	QWidget *spacer = new QWidget(controlLayout);
	controlLayout->setStretchFactor(spacer, 1 );
	QString descr = xmlParser.description();
	if ( !descr.isEmpty() ) {
	    descr.replace('\n',' ');
	    descr = descr.simplifyWhiteSpace();
	    QLabel *l = new QLabel( i18n( descr.utf8() ), plainPage() );
	    l->setAlignment ( WordBreak );
 	    topLayout->addWidget( l );
 	}
    } else {
        // fall back to KDE's old config files.
	int idx = 0;
	while (true) {
	    QString group = QString("Arg%1").arg(idx);
	    if (config.hasGroup(group)) {
		config.setGroup(group);
		QString type = config.readEntry("Type");
		if (type == "Range") {
		    KXSRangeControl *rc = new KXSRangeControl(controlLayout, group, config);
		    mConfigItemList.append(rc);
		} else if (type == "DoubleRange") {
		    KXSDoubleRangeControl *rc =
			new KXSDoubleRangeControl(controlLayout, group, config);
		    mConfigItemList.append(rc);
		} else if (type == "Check") {
		    KXSCheckBoxControl *cc = new KXSCheckBoxControl(controlLayout, group,
			    config);
		    mConfigItemList.append(cc);
		} else if (type == "DropList") {
		    KXSDropListControl *dl = new KXSDropListControl(controlLayout, group,
			    config);
		    mConfigItemList.append(dl);
		}
	    } else {
		break;
	    }
	    idx++;
	}
	if ( idx == 0 )
	    return false;
    }

    QPtrListIterator<KXSConfigItem> it( mConfigItemList );
    KXSConfigItem *item;
    while ( (item = it.current()) != 0 ) {
	++it;
	item->read( config );
        QWidget *i = dynamic_cast<QWidget*>(item);
        if (i) {
            connect( i, SIGNAL(changed()), SLOT(slotChanged()) );
        }
    }

    mPreviewProc = new KProcess;
    connect(mPreviewProc, SIGNAL(processExited(KProcess *)),
	    SLOT(slotPreviewExited(KProcess *)));

    mPreviewTimer = new QTimer(this);
    connect(mPreviewTimer, SIGNAL(timeout()), SLOT(slotNewPreview()));

    mPreview = new QWidget(plainPage());
    mPreview->setFixedSize(250, 200);
    //  mPreview->setBackgroundMode(QWidget::NoBackground);
    mPreview->setBackgroundColor(Qt::black);

    layout->add(mPreview);
    show();

    // So that hacks can XSelectInput ButtonPressMask
    XSelectInput(qt_xdisplay(), mPreview->winId(), widgetEventMask );

    slotPreviewExited(0);
    return true;
}

//---------------------------------------------------------------------------
KXSConfigDialog::~KXSConfigDialog()
{
  if (mPreviewProc && mPreviewProc->isRunning()) {
    int pid = mPreviewProc->pid();
    mPreviewProc->kill();
    waitpid(pid, (int *)0, 0);
    delete mPreviewProc;
  }
}

//---------------------------------------------------------------------------
QString KXSConfigDialog::command()
{
  QString cmd;
  KXSConfigItem *item;

  for (item = mConfigItemList.first(); item != 0; item = mConfigItemList.next())
  {
    cmd += " " + item->command();
  }

  return cmd;
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotPreviewExited(KProcess *)
{
    if ( mKilled ) {
	mKilled = false;
	mPreviewProc->clearArguments();
	QString saver;
	saver.sprintf( "%s -window-id 0x%lX", mFilename.latin1(), long(mPreview->winId()) );
	saver += command();
	kdDebug() << "Command: " <<  saver << endl;

	unsigned int i = 0;
	QString word;
	saver = saver.stripWhiteSpace();
	while ( !saver[i].isSpace() ) word += saver[i++];
	//work around a KStandarDirs::findExe() "feature" where it looks in $KDEDIR/bin first no matter what and sometimes finds the wrong executable
	QFileInfo checkExe;
	QString saverdir = QString("%1/%2").arg(XSCREENSAVER_HACKS_DIR).arg(word);
	QString path;
	checkExe.setFile(saverdir);
	if (checkExe.exists() && checkExe.isExecutable() && checkExe.isFile())
	{
		path = saverdir;
	}
	if (!path.isEmpty()) {
	    (*mPreviewProc) << path;

	    bool inQuotes = false;
	    while ( i < saver.length() ) {
		word = "";
		while ( saver[i].isSpace() && i < saver.length() ) i++;
		while ( (!saver[i].isSpace() || inQuotes) && i < saver.length() ) {
		    if ( saver[i] == '\"' ) {
			inQuotes = !inQuotes;
		    } else {
			word += saver[i];
		    }
		    i++;
		}
		if (!word.isEmpty()) {
		    (*mPreviewProc) << word;
		}
	    }

	    mPreviewProc->start();
	}
    } else {
	// stops us from spawning the hack really fast, but still not the best
	QString path = KStandardDirs::findExe(mFilename, XSCREENSAVER_HACKS_DIR);
	if ( QFile::exists(path) ) {
	    mKilled = true;
	    slotChanged();
	}
    }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotNewPreview()
{
  if (mPreviewProc->isRunning()) {
    mKilled = true;
    mPreviewProc->kill(); // restarted in slotPreviewExited()
  } else {
    slotPreviewExited(0);
  }
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotChanged()
{
    if (mPreviewTimer->isActive())
	mPreviewTimer->changeInterval(1000);
    else
	mPreviewTimer->start(1000, true);
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotOk()
{
  KXSConfigItem *item;
  KConfig config(mConfigFile);

  for (item = mConfigItemList.first(); item != 0; item = mConfigItemList.next())
  {
    item->save(config);
  }

  kapp->quit();
}

//---------------------------------------------------------------------------
void KXSConfigDialog::slotCancel()
{
  kapp->quit();
}


//===========================================================================

static const char appName[] = "kxsconfig";

static const char description[] = I18N_NOOP("KDE X Screen Saver Configuration tool");

static const char version[] = "3.0.0";

static const KCmdLineOptions options[] =
{
   {"+screensaver", I18N_NOOP("Filename of the screen saver to configure"), 0},
   {"+[savername]", I18N_NOOP("Optional screen saver name used in messages"), 0},
   KCmdLineLastOption
};

static const char *defaults[] = {
#include "XScreenSaver_ad.h"
 0
};

const char *progname = 0;
const char *progclass = "XScreenSaver";
XrmDatabase db;

int main(int argc, char *argv[])
{
  KCmdLineArgs::init(argc, argv, appName, I18N_NOOP("KXSConfig"), description, version);

  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if(args->count()==0)
    exit(1);

  /* We must read exactly the same resources as xscreensaver.
     That means we must have both the same progclass *and* progname,
     at least as far as the resource database is concerned.  So,
     put "xscreensaver" in argv[0] while initializing Xt.
   */
  const char *dummyargs[] = { "xscreensaver" };
  int dummyargc = 1;
  progname = dummyargs[0];

  // Teach Xt to use the Display that Qt has already opened.
  XtToolkitInitialize ();
  XtAppContext xtApp = XtCreateApplicationContext ();
  Display *dpy = qt_xdisplay();
  XtAppSetFallbackResources (xtApp, const_cast<char**>(defaults));
  XtDisplayInitialize (xtApp, dpy, progname, progclass, 0, 0,
                       &dummyargc,
                       const_cast<char**>(dummyargs));
  Widget toplevel_shell = XtAppCreateShell (progname, progclass,
	  applicationShellWidgetClass,
	  dpy, 0, 0);
  dpy = XtDisplay (toplevel_shell);
  db = XtDatabase (dpy);
  XtGetApplicationNameAndClass (dpy, const_cast<char**>(&progname),
                                const_cast<char**>(&progclass));

  QString name = QString::fromLocal8Bit(args->arg(args->count() - 1));
  KXSConfigDialog *dialog=new KXSConfigDialog(args->arg(0), name);
  if ( dialog->create() ) {
      dialog->show();
      app.setMainWidget(dialog);
      app.exec();
  } else {
      KMessageBox::sorry(0,
	      i18n("No configuration available for %1").arg(name),
	      name );
  }

  delete dialog;
}

#include "kxsconfig.moc"
