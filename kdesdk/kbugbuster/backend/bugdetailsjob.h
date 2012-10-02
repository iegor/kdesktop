#ifndef __bugdetailsjob_h__
#define __bugdetailsjob_h__

#include "bugjob.h"
#include "bug.h"
#include "bugdetails.h"
#include "bugdetailspart.h"

class BugDetailsJob : public BugJob
{
    Q_OBJECT
  public:
    BugDetailsJob( BugServer * );
    virtual ~BugDetailsJob();

    void start( const Bug &bug );

  signals:
    void bugDetailsAvailable( const Bug &bug, const BugDetails &details );

  protected:
    virtual void process( const QByteArray &data );

  private:
    Bug m_bug;
};

#endif

/*
 * vim:ts=4:sw=4:et
 */
