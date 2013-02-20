#ifndef KBB_BUGJOB_H
#define KBB_BUGJOB_H

#include <kio/jobclasses.h>

#include "bugserver.h"

class BugJob : public KIO::Job
{
    Q_OBJECT
  public:
    BugJob( BugServer * );
    virtual ~BugJob();

    BugServer *server() const { return mServer; }

  signals:
    void infoMessage( const QString &text );
    void infoPercent( unsigned long percent );
    void error( const QString &text );
    void jobEnded( BugJob * );

  protected:
    void start( const KURL &url /*, const KParts::URLArgs &args = KParts::URLArgs()*/ );

    virtual void process( const QByteArray &data ) = 0;

  private slots:
    void ioResult( KIO::Job *job );

    void ioData( KIO::Job *job, const QByteArray &data );

    void ioInfoMessage( KIO::Job *job, const QString &text );

    void ioInfoPercent( KIO::Job *job, unsigned long percent );

  private:
    QByteArray m_data;
    BugServer *mServer;
};

#endif
/*
 * vim:sw=4:ts=4:et
 */
