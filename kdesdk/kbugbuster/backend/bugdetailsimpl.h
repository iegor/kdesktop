#ifndef __bugdetails_impl_h__
#define __bugdetails_impl_h__

#include <ksharedptr.h>

#include "bugdetailspart.h"

struct BugDetailsImpl : public KShared
{
public:
    BugDetailsImpl( const QString &_version, const QString &_source,
                    const QString &_compiler, const QString &_os,
                    const BugDetailsPart::List &_parts )
        : version( _version ), source( _source ), compiler( _compiler ),
          os( _os ), parts( _parts ) {}

    struct AttachmentDetails {
        AttachmentDetails() { }
        AttachmentDetails( const QString& descr, const QString& dt,
                           const QString& idf ) : description( descr ),
                                                  date( dt ),
                                                  id( idf ) { }
        QString description;
        QString date;
        QString id;
    };

    QString version;
    QString source;
    QString compiler;
    QString os;
    BugDetailsPart::List parts;
    QValueList<BugDetailsImpl::AttachmentDetails> attachments;
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
