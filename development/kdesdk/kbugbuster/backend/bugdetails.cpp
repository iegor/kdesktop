
#include "bugdetails.h"

#include "bugdetailsimpl.h"
#include <qstringlist.h>
#include <kdebug.h>
#include <kmdcodec.h>
#include <kmessagebox.h>
#include <qregexp.h>

BugDetails::BugDetails()
{
}

BugDetails::BugDetails( BugDetailsImpl *impl ) :
    m_impl( impl )
{
}

BugDetails::BugDetails( const BugDetails &other )
{
    (*this) = other;
}

BugDetails &BugDetails::operator=( const BugDetails &rhs )
{
    m_impl = rhs.m_impl;
    return *this;
}

BugDetails::~BugDetails()
{
}

QString BugDetails::version() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->version;
}

QString BugDetails::source() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->source;
}

QString BugDetails::compiler() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->compiler;
}

QString BugDetails::os() const
{
    if ( !m_impl )
        return QString::null;

    return m_impl->os;
}

QDateTime BugDetails::submissionDate() const
{
    if ( !m_impl ) return QDateTime();

    if ( m_impl->parts.count() > 0 ) {
        return m_impl->parts.last().date;
    }
    return QDateTime();
}

int BugDetails::age() const
{
   if ( !m_impl )
       return 0;

   return submissionDate().daysTo( QDateTime::currentDateTime() );
}

BugDetailsPart::List BugDetails::parts() const
{
    if ( !m_impl )
        return BugDetailsPart::List();

    return m_impl->parts;
}

QValueList<BugDetailsImpl::AttachmentDetails> BugDetails::attachmentDetails() const
{
    if ( m_impl )
        return m_impl->attachments;
    else
        return QValueList<BugDetailsImpl::AttachmentDetails>();
}

void BugDetails::addAttachmentDetails( const QValueList<BugDetailsImpl::AttachmentDetails>& attch )
{
    if ( m_impl )
        m_impl->attachments =  attch;
}

QValueList<BugDetails::Attachment> BugDetails::extractAttachments() const
{
    QValueList<BugDetails::Attachment> lst;
    if ( !m_impl )
        return lst;
    BugDetailsPart::List parts = m_impl->parts;
    for ( BugDetailsPart::List::ConstIterator it = parts.begin(); it != parts.end(); ++it ) {
        lst += extractAttachments( (*it).text );
    }
    return lst;
}

//#define DEBUG_EXTRACT

QValueList<BugDetails::Attachment> BugDetails::extractAttachments( const QString& text )
{
    QValueList<BugDetails::Attachment> lst;
    QStringList lines = QStringList::split( '\n', text );
#ifdef DEBUG_EXTRACT
    kdDebug() << k_funcinfo << lines.count() << " lines." << endl;
#endif
    QString boundary;
    for ( QStringList::Iterator it = lines.begin() ; it != lines.end() ; ++it )
    {
#ifdef DEBUG_EXTRACT
        kdDebug() << "Line: " << *it << endl;
#endif
        if ( (*it).startsWith( " Content-Type" ) ) // ## leading space comes from khtml
        {
#ifdef DEBUG_EXTRACT
            //kdDebug() << "BugDetails::extractAttachments going back, looking for empty or boundary=" << boundary << endl;
#endif
            // Rewind until last empty line
            QStringList::Iterator rit = it;
            for ( ; rit != lines.begin() ; --rit ) {
                QString line = *rit;
                if ( line.endsWith( "<br />" ) )
                    line = line.left( line.length() - 6 );
                while ( !line.isEmpty() && line[0] == ' ' )
                    line.remove( 0, 1 );
#ifdef DEBUG_EXTRACT
                kdDebug() << "Back: '" << line << "'" << endl;
#endif
                if ( line.isEmpty() ) {
                    ++rit; // boundary is next line
                    boundary = *rit;
                    if ( boundary.endsWith( "<br />" ) )
                        boundary = boundary.left( boundary.length() - 6 );
                    if ( boundary[0] == ' ' )
                        boundary.remove( 0, 1 );
                    kdDebug() << "BugDetails::extractAttachments boundary=" << boundary << endl;
                    break;
                }
                if ( line == boundary )
                    break;
            }
            // Forward until next empty line (end of headers) - and parse filename
            QString filename;
            QString encoding;
            rit = it;
            for ( ; rit != lines.end() ; ++rit ) {
                QString header = *rit;
                if ( header.endsWith( "<br />" ) )
                    header = header.left( header.length() - 6 );
                if ( header[0] == ' ' )
                    header.remove( 0, 1 );
#ifdef DEBUG_EXTRACT
                kdDebug() << "Header: '" << header << "'" << endl;
#endif
                if ( header.isEmpty() )
                    break;
                if ( header.startsWith( "Content-Disposition:" ) )
                {
#ifdef DEBUG_EXTRACT
                    kdDebug() << "BugDetails::extractAttachments found header " << *rit << endl;
#endif
                    // Taken from libkdenetwork/kmime_headers.cpp
                    int pos=header.find("filename=", 0, false);
                    QString fn;
                    if(pos>-1) {
                        pos+=9;
                        fn=header.mid(pos, header.length()-pos);
                        if ( fn.startsWith( "\"" ) && fn.endsWith( "\"" ) )
                            fn = fn.mid( 1, fn.length() - 2 );
                        //filename=decodeRFC2047String(fn, &e_ncCS, defaultCS(), forceCS());
                        filename = fn; // hack
                        kdDebug() << "BugDetails::extractAttachments filename=" << filename << endl;
                    }

                }
                else if ( header.startsWith( "Content-Transfer-Encoding:" ) )
                {
                    encoding = header.mid( 26 ).lower();
                    while ( encoding[0] == ' ' )
                        encoding.remove( 0, 1 );
                    kdDebug() << "BugDetails::extractAttachments encoding=" << encoding << endl;
                }
            } //for
            if ( rit == lines.end() )
                break;

            it = rit;
            if ( rit != lines.end() && !filename.isEmpty() )
            {
                ++it;
                if ( it == lines.end() )
                    break;
                // Read encoded contents
                QString contents;
                for ( ; it != lines.end() ; ++it )
                {
                    QString line = *it;
                    if ( line.endsWith( "</tt>" ) )
                        line = line.left( line.length() - 5 );
                    if ( line.endsWith( "<br />" ) ) // necessary for the boundary check
                        line = line.left( line.length() - 6 );
                    while ( !line.isEmpty() && line[0] == ' ' )
                        line.remove( 0, 1 );
                    if ( line.isEmpty() || line == boundary ) // end of attachment
                        break;
                    if ( line == boundary+"--" ) // end of last attachment
                    {
                        boundary = QString::null;
                        break;
                    }
                    contents += line; // no newline, because of linebreaking between <br and />
                }
                contents = contents.replace( QRegExp("<br */>"), QString::null );
#ifdef DEBUG_EXTRACT
                kdDebug() << "BugDetails::extractAttachments contents=***\n" << contents << "\n***" << endl;
#endif
                kdDebug() << "BugDetails::extractAttachments creating attachment " << filename << endl;
                Attachment a;
                if ( encoding == "base64" )
                    KCodecs::base64Decode( contents.local8Bit(), a.contents /*out*/ );
                else
                    //KCodecs::uudecode( contents.local8Bit(), a.contents /*out*/ );
                    KMessageBox::information( 0, i18n("Attachment %1 could not be decoded.\nEncoding: %2").arg(filename).arg(encoding) );
#ifdef DEBUG_EXTRACT
                kdDebug() << "Result: ***\n" << QCString( a.contents.data(), a.contents.size()+1 ) << "\n*+*" << endl;
#endif
                a.filename = filename;
                lst.append(a);
                if ( it == lines.end() )
                    break;
            }
        }
    }
#ifdef DEBUG_EXTRACT
    kdDebug() << "BugDetails::extractAttachments returning " << lst.count() << " attachments for this part." << endl;
#endif
    return lst;
}

bool BugDetails::operator==( const BugDetails &rhs )
{
    return m_impl == rhs.m_impl;
}

/**
 * vim:ts=4:sw=4:et
 */
