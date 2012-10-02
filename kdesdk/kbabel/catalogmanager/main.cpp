/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky
			    <visnovsky@kde.org>

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
#include "kbprojectmanager.h"
#include "catalogmanager.h"
#include "catalogmanageriface.h"
#include "catalog.h"
#include "catalogmanagerapp.h"
#include "poinfo.h"

#include "version.h"
#include "resources.h"

#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcursor.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kwin.h>
#include <kmainwindow.h>

#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qtimer.h>

CatalogManager *CatalogManagerApp::_view = 0;

CatalogManagerApp::CatalogManagerApp()
    : KApplication()
{
    kbInterface = new CatalogManagerInterface;
    _view = 0;
    _preferredWindow = 0;
}

CatalogManagerApp::~CatalogManagerApp()
{
    delete kbInterface;
    KBabel::PoInfo::cacheWrite();
}

void CatalogManagerApp::setPreferredWindow(WId id)
{
    _preferredWindow = id;
    if( _view )
    {
	_view->raise();
	KWin::activateWindow(_view->winId());
    }
}

void CatalogManagerApp::updatedFile(QCString url)
{
    if( _view )
	_view->updateFile(url);
}

QCString CatalogManagerApp::findNextFile()
{
    QString reply = "";
    if( !CatalogManager::_foundFilesList.isEmpty() )
    {
	reply = CatalogManager::_foundFilesList.first();
	CatalogManager::_foundFilesList.pop_front();
	if( _view ) _view->decreaseNumberOfFound();
    } else
    {
	if( !CatalogManager::_toBeSearched.isEmpty() )
	    reply = QString(""); // nothing found yet
	else
	    return QCString(); // not found definitely
    }

    return reply.utf8();
}

int CatalogManagerApp::newInstance()
{
    if( isRestored() )
    {
	int n = 1;
	while (KMainWindow::canBeRestored(n)){
	    CatalogManager* cm = new CatalogManager();
	    cm->restore(n);
    	    n++;

	    // this view will be used as DCOP dispatcher
	    if( !_view )
		_view = cm;
	}
    }
    else
    {
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QString configfile = args->getOption("project");

	if( !configfile.isEmpty() )
	{
	    QFileInfo fi( configfile );
	    configfile = fi.absFilePath();
	}
	else
	{
	    configfile = KBabel::ProjectManager::defaultProjectName();
	}

	_view=new CatalogManager(configfile);

	_view->setPreferredWindow( _preferredWindow );
	_view->show();
	_view->raise();
	KWin::activateWindow(_view->winId());

	args->clear();
    }

    return 0;
}

CatalogManagerInterface::CatalogManagerInterface()
    : DCOPObject("CatalogManagerIFace")
{
}

void CatalogManagerInterface::setPreferredWindow( WId id )
{
    CatalogManagerApp::setPreferredWindow(id);
}

QCString CatalogManagerInterface::findNextFile()
{
    return CatalogManagerApp::findNextFile();
}

void CatalogManagerInterface::updatedFile( QCString url )
{
    CatalogManagerApp::updatedFile(url);
}

static KCmdLineOptions options[] =
{
   {"project <configfile>",I18N_NOOP("File to load configuration from"),0},
   KCmdLineLastOption
};


int main(int argc, char **argv)
{
    KLocale::setMainCatalogue("kbabel");
    KAboutData about("catalogmanager",I18N_NOOP("KBabel - Catalog Manager"),VERSION,
                     I18N_NOOP("An advanced catalog manager for KBabel"),KAboutData::License_GPL,
                     I18N_NOOP("(c) 1999,2000,2001,2002,2003,2004,2005,2006 The KBabel developers"),0,"http://kbabel.kde.org");

    about.addAuthor("Matthias Kiefer",I18N_NOOP("Original author"),"kiefer@kde.org");
    about.addAuthor("Stanislav Visnovsky",I18N_NOOP("Current maintainer, porting to KDE3/Qt3.")
                    ,"visnovsky@kde.org");
    about.addAuthor("Nicolas Goutte", I18N_NOOP("Current maintainer"), "goutte@kde.org");

    about.addCredit("Claudiu Costin",I18N_NOOP("Wrote documentation and sent "
                                               "many bug reports and suggestions for improvements.")
                    ,"claudiuc@kde.org");
    about.addCredit("Thomas Diehl",I18N_NOOP("Gave many suggestions for the GUI "
                                             "and the behavior of KBabel. He also contributed the beautiful splash screen.")
                    ,"thd@kde.org");
    about.addCredit("Wolfram Diestel"
                    ,I18N_NOOP("Wrote diff algorithm, fixed KSpell and gave a lot "
                               "of useful hints."),"wolfram@steloj.de");
    about.addCredit("Stephan Kulow",I18N_NOOP("Helped keep KBabel up to date "
                                              "with the KDE API and gave a lot of other help."),"coolo@kde.org");
    about.addCredit("Dwayne Bailey",I18N_NOOP("Various validation plugins.")
	 ,"dwayne@translate.org.za");
	about.addCredit("SuSE GmbH"
					,I18N_NOOP("Sponsored development of KBabel for a while.")
					,"suse@suse.de","http://www.suse.de");
    about.addCredit("Bram Schoenmakers",I18N_NOOP("Support for making diffs and some minor "
                                                  "improvements."),"bramschoenmakers@kde.nl");

    about.addCredit("Trolltech", I18N_NOOP("KBabel contains code from Qt"), 0, "http://www.trolltech.com");

    about.addCredit("GNU gettext", I18N_NOOP("KBabel contains code from GNU gettext"), 0, "http://www.gnu.org/software/gettext/");

    // Initialize command line args
    KCmdLineArgs::init(argc, argv, &about);

    // Tell which options are supported
    KCmdLineArgs::addCmdLineOptions( options );

    // Add options from other components
    KApplication::addCmdLineOptions();

    CatalogManagerApp app;

    app.newInstance();

    return app.exec();
}
