//-----------------------------------------------------------------------------
//
// KDE xscreensaver launcher
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

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <qptrlist.h>
#include <qfile.h>
#include <qfileinfo.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kcmdlineargs.h>

#include "kxsitem.h"
#include "kxsxml.h"

#define MAX_ARGS  30

template class QPtrList<KXSConfigItem>;

//===========================================================================
static const char appName[] = "kxsrun";

static const char description[] = I18N_NOOP("KDE X Screen Saver Launcher");

static const char version[] = "3.0.0";

static const KCmdLineOptions options[] =
{
   {"+screensaver", I18N_NOOP("Filename of the screen saver to start"), 0},
   {"+-- [options]", I18N_NOOP("Extra options to pass to the screen saver"), 0},
   KCmdLineLastOption
};

int main(int argc, char *argv[])
{
    KLocale::setMainCatalogue("kxsconfig");
    KCmdLineArgs::init(argc, argv, appName, I18N_NOOP("KXSRun"), description, version);

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app( false, false );

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if ( !args->count() )
	exit( 1 );

    QString filename = args->arg(0);
    QString configFile(filename);

    // Get the config filename
    int slash = filename.findRev('/');
    if (slash >= 0)
	configFile = filename.mid(slash+1);

    QString exeName = configFile;
    configFile += "rc";

    // read configuration args
    KConfig config(configFile);

    QPtrList<KXSConfigItem> configItemList;

    QString xmlFile = "/doesntexist";
#ifdef XSCREENSAVER_CONFIG_DIR
    xmlFile = XSCREENSAVER_CONFIG_DIR;
#endif
    xmlFile += "/" + exeName + ".xml";
    if ( QFile::exists( xmlFile ) ) {
	// We can use the xscreensaver xml config files.
	KXSXml xmlParser(0);
	xmlParser.parse(xmlFile);
	configItemList = *xmlParser.items();
	QPtrListIterator<KXSConfigItem> it( configItemList );
	KXSConfigItem *item;
	while ( (item = it.current()) != 0 ) {
	    ++it;
	    item->read( config );
	}
    } else {
	// fall back to KDE's old config files.
	int idx = 0;
	while (true)
	{
	    QString group = QString("Arg%1").arg(idx);
	    if (config.hasGroup(group)) {
		config.setGroup(group);
		QString type = config.readEntry("Type");
		if (type == "Range") {
		    KXSRangeItem *rc = new KXSRangeItem(group, config);
		    configItemList.append(rc);
		} else if (type == "DoubleRange") {
		    KXSDoubleRangeItem *rc = new KXSDoubleRangeItem(group, config);
		    configItemList.append(rc);
		} else if (type == "Check") {
		    KXSBoolItem *cc = new KXSBoolItem(group, config);
		    configItemList.append(cc);
		} else if (type == "DropList") {
		    KXSSelectItem *si = new KXSSelectItem(group, config);
		    configItemList.append(si);
		}
	    } else {
		break;
	    }
	    idx++;
	}
    }

    // find the xscreensaver executable
    //work around a KStandarDirs::findExe() "feature" where it looks in $KDEDIR/bin first no matter what and sometimes finds the wrong executable
    QFileInfo checkExe;
    QString saverdir = QString("%1/%2").arg(XSCREENSAVER_HACKS_DIR).arg(filename);
    kdDebug() << "saverdir is" << saverdir << endl;
    QString exeFile;
    checkExe.setFile(saverdir);
    if (checkExe.exists() && checkExe.isExecutable() && checkExe.isFile())
    {
        exeFile = saverdir;
    }


    if (!exeFile.isEmpty()) {
	char *sargs[MAX_ARGS];
	sargs[0] = new char [strlen(filename.ascii())+1];
	strcpy(sargs[0], filename.ascii());

	// add the command line options
	QString cmd;
	unsigned int i;
	for (i = 1; i < (unsigned)args->count(); i++)
	    cmd += " " + QString(args->arg(i));

	// add the config options
	KXSConfigItem *item;
	for (item = configItemList.first(); item != 0; item = configItemList.next()) {
	    cmd += " " + item->command();
	}

	// put into char * array for execv
	QString word;
	int si = 1;
	i = 0;
	bool inQuotes = false;
	while (i < cmd.length() && si < MAX_ARGS-1) {
	    word = "";
	    while ( cmd[i].isSpace() && i < cmd.length() ) i++;
	    while ( (!cmd[i].isSpace() || inQuotes) && i < cmd.length() ) {
		if ( cmd[i] == '\"' ) {
		    inQuotes = !inQuotes;
		} else {
		    word += cmd[i];
		}
		i++;
	    }
	    if (!word.isEmpty()) {
		sargs[si] = new char [strlen(word.ascii())+1];
		strcpy(sargs[si], word.ascii());
		si++;
	    }
	}

	sargs[si] = 0;

	// here goes...
	execv(exeFile.ascii(), sargs);
    }
}


