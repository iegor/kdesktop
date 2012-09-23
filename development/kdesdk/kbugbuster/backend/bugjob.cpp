
#include "bugjob.h"

#include "kbbprefs.h"

#include <kio/job.h>

#include <string.h>
#include <klocale.h>
#include <kdebug.h>

BugJob::BugJob( BugServer *server )
    : Job( false ), mServer( server )
{
}

BugJob::~BugJob()
{
}

void BugJob::start( const KURL &url )
{
    kdDebug() << "BugJob::start(): " << url.url() << endl;

    if ( KBBPrefs::instance()->mDebugMode ) {
        BugSystem::saveQuery( url );
    }

    // ### obey post, if necessary

    KIO::Job *job = KIO::get( url, true /*always 'reload=true', we have our own cache*/, false );

    connect( job, SIGNAL( result( KIO::Job * ) ),
             this, SLOT( ioResult( KIO::Job * ) ) );
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray & ) ),
             this, SLOT( ioData( KIO::Job *, const QByteArray & ) ) );
    connect( job, SIGNAL( infoMessage( KIO::Job *, const QString & ) ),
             this, SLOT( ioInfoMessage( KIO::Job *, const QString & ) ) );
    connect( job, SIGNAL( percent( KIO::Job *, unsigned long ) ),
             this, SLOT( ioInfoPercent( KIO::Job *, unsigned long ) ) );
}

void BugJob::ioResult( KIO::Job *job )
{
    m_error = job->error();
    m_errorText = job->errorText();

    if ( job->error() )
    {
        emit error( m_errorText );
        BugSystem::self()->unregisterJob(this);
        this->kill();
        return;
    }

    infoMessage( i18n( "Parsing..." ) );

#if 0
    kdDebug() << "--START:" << m_data << ":END--" << endl;
#endif

    if ( KBBPrefs::instance()->mDebugMode ) {
        BugSystem::saveResponse( m_data );
    }

    process( m_data );
    infoMessage( i18n( "Ready." ) );

    emit jobEnded( this );

    delete this;
}

void BugJob::ioData( KIO::Job *, const QByteArray &data )
{
    unsigned int start = m_data.size();

    m_data.resize( m_data.size() + data.size() );
    memcpy( m_data.data() + start, data.data(), data.size() );
}

void BugJob::ioInfoMessage( KIO::Job *, const QString &_text )
{
    QString text = _text;
    emit infoMessage( text );
}

void BugJob::ioInfoPercent( KIO::Job *, unsigned long percent )
{
    emit infoPercent( percent );
}

#include "bugjob.moc"

/*
 * vim:sw=4:ts=4:et
 */
