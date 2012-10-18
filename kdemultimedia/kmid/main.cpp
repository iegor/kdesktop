/**************************************************************************

    main.cpp  - The main function for KMid
    Copyright (C) 1997,98  Antonio Larrosa Jimenez

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    Send comments and bug fixes to larrosa@kde.org
    or to Antonio Larrosa, Rio Arnoya, 10 5B, 29006 Malaga, Spain

***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <qwidget.h>
#include <qtextcodec.h>

#include <kapplication.h>
#include <klocale.h>
#include <kio/job.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kmidframe.h"
#include "kmid_part.h"
#include "version.h"

int main(int argc, char **argv)
{
    printf("%s Copyright (C) 1997,98,99,2000,01 Antonio Larrosa Jimenez. Malaga (Spain)\n",VERSION_TXT);
    printf("KMid comes with ABSOLUTELY NO WARRANTY; for details view file COPYING\n");
    printf("This is free software, and you are welcome to redistribute it\n");
    printf("under certain conditions\n\n");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
/*
    struct sigaction act;
    act.sa_handler = SIG_DFL;
    sigemptyset(&(act.sa_mask));
    act.sa_flags=0;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
*/

    KCmdLineArgs::init(argc, argv, KMidFactory::aboutData() );

    static KCmdLineOptions options[] =
    {
       { "+file", I18N_NOOP("File to open"), 0 },
       KCmdLineLastOption
    };
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    kmidFrame *kmidframe=new kmidFrame("KMid");

    app.setMainWidget ( kmidframe );

    QObject::connect(&app,SIGNAL(shutDown()),kmidframe,SLOT(shuttingDown()));

    if (app.isRestored())
        {
        if (kmidframe->canBeRestored(1)) kmidframe->restore(1);
        }

    kmidframe->show();

    //CT    KIOJob::initStatic();

/*
    if (app->isRestored())
        {
        RESTORE(kmidFrame);
        }
       else
        {
        kmidFrame *kmidframe=new kmidFrame("KMid");
        kmidframe->show();
        };
*/



    return app.exec();
}

