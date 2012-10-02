
#include "bugdetailsjob.h"
#include "bug.h"
#include "bugdetails.h"
#include "bugdetailsimpl.h"
#include "packageimpl.h"
#include "bugserver.h"
#include "processor.h"

#include <kdebug.h>
#include <assert.h>

BugDetailsJob::BugDetailsJob( BugServer *server )
  : BugJob( server )
{
}

BugDetailsJob::~BugDetailsJob()
{
}

void BugDetailsJob::start( const Bug &bug )
{
    m_bug = bug;

    KURL bugUrl = server()->bugDetailsUrl( bug );

    kdDebug() << "BugDetailsJob::start(): " << bugUrl.url() << endl;
    BugJob::start( bugUrl );
}

void BugDetailsJob::process( const QByteArray &data )
{
    BugDetails bugDetails;

    KBB::Error err = server()->processor()->parseBugDetails( data, bugDetails );

    if ( err ) {
        emit error( i18n("Bug %1: %2").arg( m_bug.number() )
                                      .arg( err.message() ) );
    } else {
        emit bugDetailsAvailable( m_bug, bugDetails );
    }
}

#include "bugdetailsjob.moc"

/*
 * vim:sw=4:ts=4:et
 */
