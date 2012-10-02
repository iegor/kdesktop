/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002-2005 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <goutte@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#include "kbabel.h"
#include "kbabeliface.h"
#include "kbprojectmanager.h"
#include "catalog.h"
#include "kbabelsplash.h"
#include "findoptions.h"

#include "version.h"

#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcursor.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kuniqueapplication.h>
#include <kwin.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>

class KBabelInterface : public KBabelIface
{
public:
    KBabelInterface();

    virtual void openURL(QCString url, QCString package, WId window, int newWindow);
    virtual void openURL(QCString url, QCString package, WId window, int newWindow, QCString projectFile);
    virtual void openTemplate(QCString openFilename, QCString saveFilename, QCString package, int newWindow );
    virtual void openTemplate(QCString openFilename, QCString saveFilename, QCString package, int newWindow, QCString projectFile );
    virtual void gotoFileEntry(QCString url, QCString msgid);
    virtual void gotoFileEntry(QCString url, QCString package, int msgid);
    virtual void gotoFileEntry(QCString url, QCString package, int msgid, QCString projectFile);
    virtual bool findInFile(QCString fileSource, QCString url,
	QString findStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave);
    virtual bool replaceInFile(QCString fileSource, QCString url,
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int ask, int askForNextFile, int askForSave);
    virtual bool findInFile(QCString fileSource, QCString url,
	QString findStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave, QCString project );
    virtual bool replaceInFile(QCString fileSource, QCString url,
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int ask, int askForNextFile, int askForSave, QCString project );
    virtual void spellcheck(QStringList fileList);
private:
    KBabelMW* findInstance( const KURL& url, const QString& project, const QString& package) const;
};


class KBabelApp : public KUniqueApplication
{
public:
    KBabelApp();
    virtual ~KBabelApp();

    virtual int newInstance();

private:
    KBabelInterface *kbInterface;
};

KBabelApp::KBabelApp()
    : KUniqueApplication()
{
    kbInterface = new KBabelInterface;
}

KBabelApp::~KBabelApp()
{
    delete kbInterface;
}

int KBabelApp::newInstance()
{
    bool first=true;
    if(KBabelMW::memberList && !KBabelMW::memberList->isEmpty())
    {
        first=false;
    }

    // see if we are starting with session management
#if KDE_IS_VERSION(3,3,0)
    if (!restoringSession())
#else
    if (!isRestored() || !first)
#endif
    {
	kdDebug () << "Suspending DCOP" << endl;
	kapp->dcopClient()->suspend();

        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        QTimer timer;
        QWidget *splash=0;
        bool showSplash=args->isSet("splash");

        if( showSplash && first )
        {
            setOverrideCursor(KCursor::waitCursor());
            splash = new KBabelSplash();
	    splash->show();
            timer.start(4000,true);
        }

        QString projectFile=args->getOption("project");
	if( !projectFile.isEmpty() )
	{
	    QFileInfo fi(projectFile);
	    projectFile = fi.absFilePath();
	} 
	else
	{
	    projectFile = KBabel::ProjectManager::defaultProjectName();
	    kdDebug(KBABEL) << "Using the default project Project: " << projectFile << endl;
	}

	kdDebug() << "Project: " << projectFile << endl;
	QCString msgid=args->getOption("gotomsgid");
        if(!msgid.isEmpty() && args->count() > 0)
        {
            kdDebug(KBABEL) << "gotomsgid" << endl;
            QString m = QString::fromLocal8Bit(msgid);

	    kdDebug () << "Resuming DCOP" << endl;
	    kapp->dcopClient()->resume();
            kbInterface->gotoFileEntry(args->url(0).url().local8Bit(),m.utf8());
        }
        else
        {
            // no session.. just start up normally
    	    KBabelMW *widget=0;
            if(args->count() > 0)
            {
                KURL u = args->url(0);
                widget=KBabelMW::winForURL(u,projectFile);
            }

            if(!widget)
                widget=KBabelMW::emptyWin(projectFile);

            if(!widget)
                widget=new KBabelMW(projectFile);

            while(timer.isActive()) // let the user admire the splash screen ;-)
                processEvents();

            widget->show();
            for (int i=0; i < args->count(); i++)
            {
               widget->open( args->url(i) , QString::null, i != 0 );
            }

	    kdDebug () << "Resuming DCOP" << endl;
	    kapp->dcopClient()->resume();
        }


        args->clear();

        if(splash)
        {
	    delete KBabelSplash::instance;
        }
        if(showSplash)
        {
            KApplication::restoreOverrideCursor();
            /*
            KMessageBox::information(0,
                    "This is a development version of KBabel!\n"
                    "Please double check the files you edit "
                    "and save with this version for correctness.\n"
                    "Please report any bug you find to kiefer@kde.org.\n"
                    "Thanks.", "Warning");
            */
        }
    }

    return 0;
}

KBabelInterface::KBabelInterface()
    : DCOPObject("KBabelIFace")
{
}

void KBabelInterface::openURL(QCString url, QCString package, WId window, int newWindow)
{
    openURL(url,package,window,newWindow,  KBabel::ProjectManager::defaultProjectName().local8Bit());
}

void KBabelInterface::openURL(QCString url, QCString package, WId window, int newWindow, QCString projectFile)
{
    const QString project( QString::fromLocal8Bit( projectFile ) );

    kdDebug() << "openURL " << url << endl;

    KURL u(QString::fromLocal8Bit(url));

    kdDebug () << "Suspending DCOP" << endl;
    kapp->dcopClient()->suspend();

    KBabelMW *kb = KBabelMW::winForURL(u,project);
    if(kb)
    {
        KWin::activateWindow(kb->topLevelWidget()->winId());
    }
    else
    {
        KMainWindow *mw = 0;
        if(KMainWindow::memberList && !KMainWindow::memberList->isEmpty())
            mw=KMainWindow::memberList->first();

	// first, try to lookup correct winid
        while( mw ) {
	    if( mw->inherits("KBabelMW") && mw->winId() == window)
    	    {
        	kb = static_cast<KBabelMW*>(mw);
        	KWin::activateWindow(kb->topLevelWidget()->winId());
        	kb->open(u, QString::fromUtf8(package),newWindow);

		kdDebug () << "Resuming DCOP" << endl;
		kapp->dcopClient()->resume();

		return ;
	    }
	    mw = KMainWindow::memberList->next();
        }

	// now, the empty window
	kb = KBabelMW::emptyWin(projectFile);
	if (kb)
	{
	    // here, we don't care about "open in new window", because
	    // it's empty
    	    KWin::setActiveWindow(kb->topLevelWidget()->winId());
	    kb->projectOpen(projectFile);
    	    kb->open(u,QString::fromUtf8(package),false);

	    kdDebug () << "Resuming DCOP" << endl;
	    kapp->dcopClient()->resume();

	    return;
	}

        if(KMainWindow::memberList && !KMainWindow::memberList->isEmpty())
            mw=KMainWindow::memberList->first();

        while( mw ) {
	    if( mw->inherits("KBabelMW") && static_cast<KBabelMW*>(mw)->project()==project)
    	    {
        	kb = static_cast<KBabelMW*>(mw);
        	KWin::activateWindow(kb->topLevelWidget()->winId());
        	kb->open(u, QString::fromUtf8(package),newWindow);

		kdDebug () << "Resuming DCOP" << endl;
		kapp->dcopClient()->resume();

		return ;
	    }
	    mw = KMainWindow::memberList->next();
        }

        if( !mw )
	{
	    kb = new KBabelMW(project);
    	    kb->show();
	} else kb = static_cast<KBabelMW*>(mw);
        KWin::activateWindow(kb->topLevelWidget()->winId());
        kb->open(u,QString::fromUtf8(package),newWindow);
    }
    
    kdDebug () << "Resuming DCOP" << endl;
    kapp->dcopClient()->resume();
}

void KBabelInterface::openTemplate(QCString openFilename, QCString saveFilename, QCString package, int newWindow)
{
    openTemplate(openFilename, saveFilename, package, newWindow, KBabel::ProjectManager::defaultProjectName().local8Bit());
}

void KBabelInterface::openTemplate(QCString openFilename, QCString saveFilename, QCString package, int newWindow, QCString projectFile)
{
    const QString project( QString::fromLocal8Bit( projectFile ) );

    const KURL u( QString::fromLocal8Bit( saveFilename ) );
    const KURL t( QString::fromLocal8Bit( openFilename ) );

    kdDebug () << "Suspending DCOP" << endl;
    kapp->dcopClient()->suspend();

    KBabelMW *kb = KBabelMW::winForURL(u, project);
    if(kb)
    {
        KWin::activateWindow(kb->topLevelWidget()->winId());
    }
    else
    {
        KMainWindow *mw = 0;
        if(KMainWindow::memberList && !KMainWindow::memberList->isEmpty())
            mw=KMainWindow::memberList->first();

        if(mw && mw->inherits("KBabelMW") && static_cast<KBabelMW*>(mw)->project()==project)
        {
            kb = static_cast<KBabelMW*>(mw);
            KWin::activateWindow(kb->topLevelWidget()->winId());
	    kb->projectOpen(projectFile);
            kb->openTemplate(t,u,QString::fromUtf8(package),newWindow);
        }
        else
        {
            kb = new KBabelMW(project);
            kb->show();
            KWin::activateWindow(kb->topLevelWidget()->winId());
            kb->openTemplate(t,u,QString::fromUtf8(package));
        }
    }

    kdDebug () << "Resuming DCOP" << endl;
    kapp->dcopClient()->resume();
}

void KBabelInterface::gotoFileEntry(QCString url, QCString m)
{
    const KURL u( QString::fromLocal8Bit( url ) );
    KBabelMW *kb = findInstance( u, KBabel::ProjectManager::defaultProjectName(), QString() );

    if(!kb) return;

    QString msgid = QString::fromUtf8(m);
    int index = kb->m_view->catalog()->indexForMsgid(msgid);
    if(index >= 0)
    {
	KBabel::DocPosition pos;
	pos.item=index;
	pos.form=0;
        kb->m_view->gotoEntry(pos);
    }
}

void KBabelInterface::gotoFileEntry(QCString url, QCString package, int m)
{
    gotoFileEntry(url, package, m, KBabel::ProjectManager::defaultProjectName().local8Bit() );
}

void KBabelInterface::gotoFileEntry(QCString url, QCString package, int m, QCString projectFile)
{
    const KURL u ( QString::fromLocal8Bit( url ) );
    const QString p ( QString::fromUtf8( package ) ); // ### VERIFY encoding!
    KBabelMW *kb = findInstance( u, projectFile, p );

    if(!kb) return;

    KBabel::DocPosition pos;
    pos.item=m;
    pos.form=0;
    kb->m_view->gotoEntry(pos);
}

bool KBabelInterface::findInFile(QCString fileSource, QCString url,
	QString findStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave)
{
	// no project given, open with default project
	return findInFile ( fileSource, url, findStr, caseSensitive, 
                             wholeWords, isRegExp, inMsgid, inMsgstr, inComment, ignoreAccelMarker, ignoreContextInfo,
                             askForNextFile, askForSave,
			     KBabel::ProjectManager::defaultProjectName().utf8() );
}

bool KBabelInterface::findInFile(QCString fileSource, QCString url,
	QString findStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int askForNextFile, int askForSave, QCString project )
{
    kdDebug(KBABEL) << "findInFile (" <<fileSource<< "): " << url << " for " << findStr << endl;

    const KURL u( QString::fromLocal8Bit( url ) );
    KBabelMW *kb = findInstance( u, QString::fromLocal8Bit(project), QString() );

    if(!kb) return false;

    KBabel::FindOptions options;
    options.findStr = findStr;
    options.caseSensitive = (caseSensitive>0);
    options.fromCursor = false;
    options.backwards = false;
    options.wholeWords = (wholeWords>0);
    options.isRegExp = (isRegExp>0);
    options.inMsgid = (inMsgid>0);
    options.inMsgstr = (inMsgstr>0);
    options.inComment = (inComment>0);
    options.ignoreAccelMarker = (ignoreAccelMarker>0);
    options.ignoreContextInfo = (ignoreContextInfo>0);
    options.askForNextFile = (askForNextFile>0);
    options.askForSave = (askForSave>0);
    kb->m_view->findInFile(fileSource, options);

    return true;
}

bool KBabelInterface::replaceInFile(QCString fileSource, QCString url,
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int ask, int askForNextFile, int askForSave)
{
	return replaceInFile( fileSource, url,
                               findStr, replaceStr, caseSensitive, wholeWords, isRegExp,
                               inMsgid, inMsgstr, inComment, ignoreAccelMarker, ignoreContextInfo,
                               ask, askForNextFile, askForSave,
			       KBabel::ProjectManager::defaultProjectName().utf8() );	
}

bool KBabelInterface::replaceInFile(QCString fileSource, QCString url,
	QString findStr, QString replaceStr, int caseSensitive, int wholeWords, int isRegExp,
	int inMsgid, int inMsgstr, int inComment,
	int ignoreAccelMarker, int ignoreContextInfo, int ask, int askForNextFile, int askForSave,
        QCString project )
{
    kdDebug(KBABEL) << "replaceInFile (" <<fileSource<< "): " << url << " for " << findStr << endl;

    const KURL u ( QString::fromLocal8Bit( url ) );
    KBabelMW *kb = findInstance( u, QString::fromLocal8Bit(project), QString() );

    if( !kb ) return false;

    KBabel::ReplaceOptions options;
    options.findStr = findStr;
    options.replaceStr = replaceStr;
    options.caseSensitive = (caseSensitive>0);
    options.fromCursor = false;
    options.backwards = false;
    options.wholeWords = (wholeWords>0);
    options.isRegExp = (isRegExp>0);
    options.inMsgid = (inMsgid>0);
    options.inMsgstr = (inMsgstr>0);
    options.inComment = (inComment>0);
    options.ignoreAccelMarker = (ignoreAccelMarker>0);
    options.ignoreContextInfo = (ignoreContextInfo>0);
    options.ask = (ask>0);
    options.askForNextFile = (askForNextFile>0);
    options.askForSave = (askForSave>0);
    kb->m_view->replaceInFile(fileSource, options);

    return true;
}

void KBabelInterface::spellcheck(QStringList fileList)
{
    // ### FIXME: the default project might use the wrong language!
    KBabelMW *kb = findInstance( KURL(), KBabel::ProjectManager::defaultProjectName(), QString() );
    kb->show();
    kb->spellcheckMoreFiles( fileList );
}

KBabelMW* KBabelInterface::findInstance( const KURL& url, const QString& project, const QString& package) const
{
    kdDebug () << "Suspending DCOP" << endl;
    kapp->dcopClient()->suspend();

    KBabelMW *kb = 0;
    if( !url.isEmpty() )
    {
	kb = KBabelMW::winForURL( url, project );

	if(kb)
	{
    	    KWin::activateWindow(kb->topLevelWidget()->winId());
	}
    }

    if( !kb )
    {
	kb = KBabelMW::emptyWin(project);
	if( !kb ) 
	{
	    kb = new KBabelMW(project);
	}
	else
	{
	    kb->projectOpen(project);
	}

        kb->show();
        if ( !url.isEmpty() )
            kb->open( url, package, false );
    }

    kdDebug () << "Resuming DCOP" << endl;
    kapp->dcopClient()->resume();

    return kb;
}

static KCmdLineOptions options[] =
{
   {"gotomsgid <msgid>",I18N_NOOP("Go to entry with msgid <msgid>"),0},
   {"nosplash",I18N_NOOP("Disable splashscreen at startup"),0},
   {"project <configfile>",I18N_NOOP("File to load configuration from"),0},
   {"+[file]",I18N_NOOP("Files to open"),0},
   KCmdLineLastOption
};


int main(int argc, char **argv)
{
    KAboutData about("kbabel",I18N_NOOP("KBabel"),VERSION,
       I18N_NOOP("An advanced PO file editor"),KAboutData::License_GPL,
       I18N_NOOP("(c) 1999,2000,2001,2002,2003,2004,2005,2006 The KBabel developers"),0,"http://kbabel.kde.org");

    about.addAuthor("Matthias Kiefer",I18N_NOOP("Original author"),"kiefer@kde.org");
    about.addAuthor("Wolfram Diestel"
         ,I18N_NOOP("Wrote diff algorithm, fixed KSpell and gave a lot "
         "of useful hints."),"wolfram@steloj.de");
    about.addAuthor("Andrea Rizzi",I18N_NOOP("Wrote the dictionary plugin "
		"for searching in a database and some other code.")
            ,"rizzi@kde.org");
    about.addAuthor("Stanislav Visnovsky",I18N_NOOP("Current maintainer, porting to KDE3/Qt3.")
	,"visnovsky@kde.org");
    about.addAuthor("Marco Wegner",I18N_NOOP("Bug fixes, KFilePlugin for PO files, CVS support, mailing files")
         ,"dubbleu@web.de");
    about.addAuthor("Asgeir Frimannsson",I18N_NOOP("Translation List View")
         ,"asgeirf@redhat.com");
    about.addAuthor("Nicolas Goutte", I18N_NOOP("Current maintainer"), "goutte@kde.org");

    about.addCredit("Claudiu Costin",I18N_NOOP("Wrote documentation and sent "
		"many bug reports and suggestions for improvements.")
         ,"claudiuc@geocities.com");
    about.addCredit("Thomas Diehl",I18N_NOOP("Gave many suggestions for the GUI "
         "and the behavior of KBabel. He also contributed the beautiful splash screen.")
            ,"thd@kde.org");
    about.addCredit("Stephan Kulow",I18N_NOOP("Helped keep KBabel up to date "
		"with the KDE API and gave a lot of other help."),"coolo@kde.org");
    about.addCredit("Stefan Asserhall",I18N_NOOP("Implemented XML validation/highlighting "
	 "plus other small fixes.") ,"stefan.asserhall@telia.com");
    about.addCredit("Dwayne Bailey",I18N_NOOP("Various validation plugins.")
	 ,"dwayne@translate.org.za");
	about.addCredit("SuSE GmbH"
					,I18N_NOOP("Sponsored development of KBabel for a while.")
					,"suse@suse.de","http://www.suse.de");
    about.addCredit("Trolltech", I18N_NOOP("KBabel contains code from Qt"), 0, "http://www.trolltech.com");

    about.addCredit("Eva Brucherseifer", I18N_NOOP("String distance algorithm implementation"), "eva@kde.org");

    about.addCredit("Albert Cervera Areny", I18N_NOOP("Error list for current entry, regexp data tool"), "albertca@hotpop.com");

    about.addCredit("Nick Shaforostoff", I18N_NOOP("Word-by-word string difference algorithm implementation"), "shafff@ukr.net");

    // Initialize command line args
    KCmdLineArgs::init(argc, argv, &about);

    // Tell which options are supported
    KCmdLineArgs::addCmdLineOptions( options );

    // Add options from other components
    KUniqueApplication::addCmdLineOptions();


    if(!KUniqueApplication::start())
    {
        return 0;
    }

    KBabelApp app;

    if( app.isRestored() )
    {
	RESTORE(KBabelMW)
    }

    return app.exec();
}
