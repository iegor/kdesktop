#include "test.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "v0.1";

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Image to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kimageviewertest", I18N_NOOP("KImageViewerTest"), version, description, KAboutData::License_GPL, "(C) 2001 Matthias Kretz", 0, 0, "kretz@kde.org");
    about.addAuthor( "Matthias Kretz", 0, "kretz@kde.org" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;

    if (app.isRestored())
        RESTORE(KImageViewerTest)
    else
    {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        if ( args->count() == 0 )
        {
        KImageViewerTest *widget = new KImageViewerTest;
        widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++ )
            {
                KImageViewerTest *widget = new KImageViewerTest;
                widget->show();
                widget->load( args->url( i ) );
            }
        }
        args->clear();
    }

    return app.exec();
}
