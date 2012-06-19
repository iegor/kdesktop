#ifndef __bugdetails_h__
#define __bugdetails_h__

#include "person.h"
#include "bugdetailspart.h"
#include "bugdetailsimpl.h"

#include <qvaluelist.h>

#include <ksharedptr.h>

class BugDetailsImpl;

class BugDetails
{
public:
    typedef QValueList<BugDetails> List;
    struct Attachment {
        QByteArray contents;
        QString filename;
    };

    BugDetails();
    BugDetails( BugDetailsImpl *impl );
    BugDetails( const BugDetails &other );
    BugDetails &operator=( const BugDetails &rhs );
    ~BugDetails();

    QString version() const;
    QString source() const;
    QString compiler() const;
    QString os() const;
    BugDetailsPart::List parts() const;
    void addAttachmentDetails( const QValueList<BugDetailsImpl::AttachmentDetails>& attch );
    QValueList<BugDetailsImpl::AttachmentDetails> attachmentDetails() const;
    QValueList<BugDetails::Attachment> extractAttachments() const;
    static QValueList<BugDetails::Attachment> extractAttachments( const QString& text );

    QDateTime submissionDate() const;
    int age() const;

    bool operator==( const BugDetails &rhs );

    bool isNull() const { return m_impl == 0; }

private:
    BugDetailsImpl *impl() const { return m_impl; }

    KSharedPtr<BugDetailsImpl> m_impl;
};

#endif

/* vim: set sw=4 ts=4 et softtabstop=4: */

