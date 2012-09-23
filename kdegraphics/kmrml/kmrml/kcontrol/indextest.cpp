#include "indexer.h"
#include <kmrml_config.h>
#include "indextest.moc"

#include <kapplication.h>
#include <kconfig.h>
#include <kglobal.h>

using namespace KMrmlConfig;

IndexTest::IndexTest()
{
    KMrml::Config *config = new KMrml::Config( KGlobal::config() );
    Indexer *indexer = new Indexer( *config, this );
    connect( indexer, SIGNAL( finished( bool )), SLOT( slotFinished( bool )));
    connect( indexer, SIGNAL( progress( int, const QString& )),
             SLOT( slotProgress( int, const QString& )));

    indexer->startIndexing( "/home/gis/testcoll" );
}

IndexTest::~IndexTest()
{

}

void IndexTest::slotFinished( bool success )
{
    qDebug("##### FINISHED: %i", success );
}

void IndexTest::slotProgress( int percent, const QString& message )
{
    qDebug("--- progress: %i: %s", percent, message.latin1());
}

int main( int argc, char **argv )
{
    KApplication app( argc, argv, "indextest" );
    IndexTest *test = new IndexTest();

    return app.exec();
}
