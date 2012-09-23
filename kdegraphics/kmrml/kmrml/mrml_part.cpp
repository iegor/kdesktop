/* This file is part of the KDE project
   Copyright (C) 2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qcheckbox.h>
#include <qcursor.h>
#include <qdir.h>
#include <qfile.h>
#include <qgrid.h>
#include <qhgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kdatastream.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kprotocolinfo.h>
#include <kparts/genericfactory.h>
#include <ktempfile.h>

#include <mrml_utils.h>

#include "algorithmdialog.h"
#include "browser.h"
#include "collectioncombo.h"
#include "mrml_creator.h"
#include "mrml_elements.h"
#include "mrml_shared.h"
#include "mrml_view.h"
#include "mrml_part.h"
#include "version.h"

using namespace KMrml;

extern "C"
{
    void * init_libkmrmlpart() {
        return new KMrml::PartFactory();
    }
}

KInstance * PartFactory::s_instance = 0L;

PartFactory::PartFactory()
    : KParts::Factory()
{
    MrmlShared::ref();
}

PartFactory::~PartFactory()
{
    MrmlShared::deref();
    delete s_instance;
    s_instance = 0L;
}

KInstance * PartFactory::instance()
{
    if ( !s_instance ) {
        s_instance = new KInstance( "kmrml" );
        KGlobal::locale()->insertCatalogue( "kmrml" );
    }
    return s_instance;
}

KParts::Part * PartFactory::createPartObject( QWidget *parentWidget,
                                              const char *widgetName,
                                              QObject *parent,
                                              const char *name,
                                              const char *,
                                              const QStringList& args )
{
    return new MrmlPart( parentWidget, widgetName, parent, name, args );
}


// can't use this due to MrmlShared ref-counting
// typedef KParts::GenericFactory<KMrml::MrmlPart> PartFactory;
// K_EXPORT_COMPONENT_FACTORY( mrmlpart, PartFactory )

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


uint MrmlPart::s_sessionId = 0;

MrmlPart::MrmlPart( QWidget *parentWidget, const char * /* widgetName */,
                    QObject *parent, const char *name,
                    const QStringList& /* args */ )
    : KParts::ReadOnlyPart( parent, name ),
      m_job( 0L ),
      m_status( NeedCollection )
{
    m_sessionId = QString::number( s_sessionId++ ).prepend("kmrml_");

    setName( "MRML Part" );
    m_browser = new Browser( this, "mrml browserextension");
    setInstance( PartFactory::instance(), true ); // do load plugins :)
    KConfig *config = PartFactory::instance()->config();
    config->setGroup("MRML Settings");

    QVBox *box = new QVBox( parentWidget, "main mrml box" );
    m_view = new MrmlView( box, "MrmlView" );
    connect( m_view, SIGNAL( activated( const KURL&, ButtonState )),
             this, SLOT( slotActivated( const KURL&, ButtonState )));
    connect( m_view, SIGNAL( onItem( const KURL& )),
             this, SLOT( slotSetStatusBar( const KURL& )));

    m_panel = new QHGroupBox( box, "buttons box" );

    QGrid *comboGrid = new QGrid( 2, m_panel, "combo grid" );
    comboGrid->setSpacing( KDialog::spacingHint() );

    (void) new QLabel( i18n("Server to query:"), comboGrid );

    m_hostCombo = new KComboBox( false, comboGrid, "host combo" );
    initHostCombo();
    connect( m_hostCombo, SIGNAL( activated( const QString& ) ),
             SLOT( slotHostComboActivated( const QString& )));

    (void) new QLabel( i18n("Search in collection:"), comboGrid );
    m_collectionCombo = new CollectionCombo( comboGrid, "collection-combo" );
    // will be re-set in initCollections(), but we need to set it here to
    // prevent crashes when the connection to the server fails
    m_collectionCombo->setCollections( &m_collections );

    m_algoButton = new QPushButton( QString::null, m_panel );
    m_algoButton->setPixmap( SmallIcon("configure") );
    m_algoButton->setFixedSize( m_algoButton->sizeHint() );
    connect( m_algoButton, SIGNAL( clicked() ),
             SLOT( slotConfigureAlgorithm() ));
    QToolTip::add( m_algoButton, i18n("Configure algorithm") );

    QWidget *spacer = new QWidget( m_panel );
    spacer->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding,
                                        QSizePolicy::Minimum ) );

    int resultSize = config->readNumEntry( "Result-size", 20 );
    m_resultSizeInput = new KIntNumInput( resultSize, m_panel );
    m_resultSizeInput->setRange( 1, 100 );
    m_resultSizeInput->setLabel( i18n("Maximum result images:") );

    QVBox *tmp = new QVBox( m_panel );
    m_random = new QCheckBox( i18n("Random search"), tmp );

    m_startButton = new QPushButton( QString::null, tmp );
    connect( m_startButton, SIGNAL( clicked() ), SLOT( slotStartClicked() ));
    setStatus( NeedCollection );

    setWidget( box );

    // setXMLFile( "mrml_part.rc" );

    slotSetStatusBar( QString::null );

    enableServerDependentWidgets( false );
}

MrmlPart::~MrmlPart()
{
    closeURL();
}

void MrmlPart::enableServerDependentWidgets( bool enable )
{
    m_collectionCombo->setEnabled( enable );
    m_algoButton->setEnabled( enable && false ); // ### re-enable!!!
}

void MrmlPart::initCollections( const QDomElement& elem )
{
    m_collections.initFromDOM( elem );

    m_collectionCombo->setCollections( &m_collections );
    enableServerDependentWidgets( m_collectionCombo->count() > 0 );

    if ( m_collectionCombo->count() == 0 )
    {
        KMessageBox::information( widget(),
                               i18n("There is no image collection available\n"
                                    "at %1.\n"), i18n("No Image Collection"));
        setStatus( NeedCollection );
    }
    else
        m_collectionCombo->updateGeometry(); // adjust the entire grid
}

void MrmlPart::initAlgorithms( const QDomElement& elem )
{
    m_algorithms.initFromDOM( elem );
}

// this is where we start!
bool MrmlPart::openURL( const KURL& url )
{
    closeURL();

    if ( url.protocol() != "mrml" || !url.isValid() ) {
        qWarning("MrmlPart::openURL: cannot handle url: %s", url.prettyURL().latin1());
        return false; // what to do with that?
    }

    m_url = url;
    QString host = url.host().isEmpty() ?
                   QString::fromLatin1("localhost") : url.host();

    m_hostCombo->setCurrentItem( host );

    // urls we need to download before starting the query
    KURL::List downloadList;

    m_queryList.clear();
    QString param = url.queryItem( "relevant" );
    QStringList list = QStringList::split( ';', param );

    // we can only search by example on localhost
    if ( host != "localhost" )
    {
        if ( !list.isEmpty() )
            KMessageBox::sorry( m_view,
                                i18n("You can only search by example images "
                                     "on a local indexing server."),
                                i18n("Only Local Servers Possible") );
    }

    else // localhost query
    {
        for( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        {
            KURL u;
            if ( (*it).at(0) == '/' )
                u.setPath( *it );
            else
                u = *it;

            if ( u.isValid() )
            {
                if ( u.isLocalFile() )
                    m_queryList.append( u );
                else
                    downloadList.append( u );
            }
        }


        // ### we need a real solution for this!
        // gift refuses to start when no config file is available.
        if ( !QFile::exists( m_config.mrmldDataDir() + "/gift-config.mrml" ) )
        {
            if ( KMessageBox::questionYesNo(0L,
                             i18n("There are no indexable folders "
                                  "specified. Do you want to configure them "
                                  "now?"),
                                            i18n("Configuration Missing"),
                                            i18n("Configure"),
                                            i18n("Do Not Configure"),
                                            "kmrml_ask_configure_gift" )
                 == KMessageBox::Yes )
            {
                KApplication::kdeinitExec( "kcmshell",
                                           QString::fromLatin1("kcmkmrml"));
                setStatus( NeedCollection );
                return false;
            }
        }
    }


    if ( !downloadList.isEmpty() )
        downloadReferenceFiles( downloadList );
    else
        contactServer( m_url );

    return true;
}

void MrmlPart::contactServer( const KURL& url )
{
    m_job = transferJob( url );

    m_job->addMetaData( MrmlShared::kio_task(), MrmlShared::kio_initialize() );

    QString host = url.host().isEmpty() ?
                   QString::fromLatin1("localhost") : url.host();

    slotSetStatusBar( i18n("Connecting to indexing server at %1...").arg( host ));
}

//
// schedules a download all urls of downloadList (all remote and wellformed)
// No other downloads are running (closeURL() has been called before)
//
void MrmlPart::downloadReferenceFiles( const KURL::List& downloadList )
{
    assert( m_downloadJobs.isEmpty() );

    KURL::List::ConstIterator it = downloadList.begin();
    for ( ; it != downloadList.end(); it++ )
    {
        QString extension;
        int index = (*it).fileName().findRev( '.' );
        if ( index != -1 )
            extension = (*it).fileName().mid( index );

        KTempFile tmpFile( QString::null, extension );
        if ( tmpFile.status() != 0 )
        {
            kdWarning() << "Can't create temporary file, skipping: " << *it << endl;

            continue;
        }

        m_tempFiles.append( tmpFile.name() );
        KURL destURL;
        destURL.setPath( tmpFile.name() );

        KIO::FileCopyJob *job = KIO::file_copy( *it, destURL, -1,
                                                true /* overwrite tmpfile */ );
        connect( job, SIGNAL( result( KIO::Job * ) ),
                 SLOT( slotDownloadResult( KIO::Job * ) ));
        m_downloadJobs.append( job );
        // ### should this be only called for one job?
        emit started( job );
    }

    if ( !m_downloadJobs.isEmpty() )
        slotSetStatusBar( i18n("Downloading reference files...") );
    else // probably never happens
        contactServer( m_url );
}

bool MrmlPart::closeURL()
{
    m_view->stopDownloads();
    m_view->clear();

    QPtrListIterator<KIO::FileCopyJob> it( m_downloadJobs );
    for ( ; it.current(); ++it )
        it.current()->kill();
    m_downloadJobs.clear();

    QStringList::Iterator tit = m_tempFiles.begin();
    for ( ; tit != m_tempFiles.end(); ++tit )
        QFile::remove( *tit );
    m_tempFiles.clear();

    if ( m_job ) {
        m_job->kill();
        m_job = 0L;
    }

    setStatus( NeedCollection );

    return true;
}

KIO::TransferJob * MrmlPart::transferJob( const KURL& url )
{
    KIO::TransferJob *job = KIO::get( url, true, false ); // reload, no gui
    job->setAutoErrorHandlingEnabled( true, m_view );
    connect( job, SIGNAL( result( KIO::Job * )),
             SLOT( slotResult( KIO::Job * )));
    connect( job, SIGNAL( data( KIO::Job *, const QByteArray& )),
             SLOT( slotData( KIO::Job *, const QByteArray& )));

// ###
//     connect( job, SIGNAL( infoMessage( KIO::Job *, const QString& )),
//              SLOT( slotResult( KIO::Job *, const QString& )));

    job->setWindow( widget() );
    if ( !m_sessionId.isEmpty() )
        job->addMetaData( MrmlShared::sessionId(), m_sessionId );

    emit started( job );
    emit setWindowCaption( url.prettyURL() );
    setStatus( InProgress );

    return job;
}

void MrmlPart::slotResult( KIO::Job *job )
{
    if ( job == m_job )
        m_job = 0L;

    slotSetStatusBar( QString::null );

    if ( !job->error() )
        emit completed();
    else {
        emit canceled( job->errorString() );
//         qDebug("*** canceled: error: %s", job->errorString().latin1());
    }


    bool auto_random = m_view->isEmpty() && m_queryList.isEmpty();
    m_random->setChecked( auto_random );
    m_random->setEnabled( !auto_random );
    setStatus( job->error() ? NeedCollection : CanSearch );

    if ( !job->error() && !m_queryList.isEmpty() ) {
        // we have a connection and we got a list of relevant URLs to query for
        // (via the URL)

        createQuery( &m_queryList );
        m_queryList.clear();
    }
}

// ### when user cancels download, we crash :(
void MrmlPart::slotDownloadResult( KIO::Job *job )
{
    assert( job->inherits( "KIO::FileCopyJob" ) );
    KIO::FileCopyJob *copyJob = static_cast<KIO::FileCopyJob*>( job );

    if ( !copyJob->error() )
        m_queryList.append( copyJob->destURL() );

    m_downloadJobs.removeRef( copyJob );

    if ( m_downloadJobs.isEmpty() ) // finally, we can start the query!
    {
        if ( m_queryList.isEmpty() ) // rather unlikely, but could happen ;)
        {
            kdWarning() << "Couldn't download the reference files. Will start a random search now" << endl;
        }

        contactServer( m_url );
    }
}

// mrml-document in the bytearray
void MrmlPart::slotData( KIO::Job *, const QByteArray& data )
{
    if ( data.isEmpty() )
        return;

    QDomDocument doc;
    doc.setContent( data );

    if ( !doc.isNull() )
        parseMrml( doc );
}

void MrmlPart::parseMrml( QDomDocument& doc )
{
    QDomNode mrml = doc.documentElement(); // root element
    if ( !mrml.isNull() ) {
        QDomNode child = mrml.firstChild();
        for ( ; !child.isNull(); child = child.nextSibling() ) {
//             qDebug("**** HERE %s", child.nodeName().latin1());
            if ( child.isElement() ) {
                QDomElement elem = child.toElement();

                QString tagName = elem.tagName();

                if ( tagName == "acknowledge-session-op" )
                    m_sessionId = elem.attribute( MrmlShared::sessionId() );

                else if ( tagName == MrmlShared::algorithmList() ) {
                    initAlgorithms( elem );
                }

                else if ( tagName == MrmlShared::collectionList() ) {
                    initCollections( elem );
                }

                else if ( tagName == "error" ) {
                    KMessageBox::information( widget(),
                                       i18n("Server returned error:\n%1\n")
                                       .arg( elem.attribute( "message" )),
                                              i18n("Server Error") );
                }

                else if ( tagName == "query-result" ) {
                    m_view->clear();
                    parseQueryResult( elem );
                }


            } // child.isElement()
        }
    } // !mrml.isNull()
}

void MrmlPart::parseQueryResult( QDomElement& queryResult )
{
    QDomNode child = queryResult.firstChild();
    for ( ; !child.isNull(); child = child.nextSibling() ) {
        if ( child.isElement() ) {
            QDomElement elem = child.toElement();
            QString tagName = elem.tagName();

            if ( tagName == "query-result-element-list" ) {
                QValueList<QDomElement> list =
                    KMrml::directChildElements( elem, "query-result-element" );

                QValueListConstIterator<QDomElement> it = list.begin();
                for ( ; it != list.end(); ++it )
                {
                    QDomNamedNodeMap a = (*it).attributes();
                    m_view->addItem( KURL( (*it).attribute("image-location" ) ),
                                     KURL( (*it).attribute("thumbnail-location" ) ),
                                     (*it).attribute("calculated-similarity"));

                }
            }

            else if ( tagName == "query-result" )
                parseQueryResult( elem );
        }
    }
}

// creates/stops the query when the Start/Stop button was pressed
void MrmlPart::slotStartClicked()
{
    if ( m_status == InProgress )
    {
        closeURL();
        m_startButton->setText( i18n("&Search" ) );
        return;
    }

    // we need to reconnect, if the initial openURL() didn't work due to
    // the gift not being available.
    if ( m_status == NeedCollection )
    {
        openURL( m_url );
        return;
    }

    // cut off an eventual query and reference from the url, when the user
    // performs a real query (otherwise restoreState() would restore and
    // re-do the query from the URL
    m_url.setRef( QString::null );
    m_url.setQuery( QString::null );

    createQuery();
    m_browser->openURLNotify();
}

//
// relevantItems is 0L when called from slotStartClicked() and set to a
// non-empty list when called initially, from the commandline.
//
void MrmlPart::createQuery( const KURL::List * relevantItems )
{
    if ( relevantItems && relevantItems->isEmpty() )
        return;

    QDomDocument doc( "mrml" );
    QDomElement mrml = MrmlCreator::createMrml( doc,
                                                sessionId(),
                                                transactionId() );

    Collection coll = currentCollection();
//     qDebug("** collection: name: %s, id: %s, valid: %i", coll.name().latin1(), coll.id().latin1(), coll.isValid());
    Algorithm algo = firstAlgorithmForCollection( coll );
//     qDebug("** algorithm: name: %s, id: %s, valid: %i, collection-id: %s", algo.name().latin1(), algo.id().latin1(), algo.isValid(), algo.collectionId().latin1());

    if ( algo.isValid() )
    {
        MrmlCreator::configureSession( mrml, algo, sessionId() );
    }

    QDomElement query = MrmlCreator::addQuery( mrml,
                                               m_resultSizeInput->value() );
    if ( algo.isValid() )
        query.setAttribute( MrmlShared::algorithmId(), algo.id() );

    // ### result-cutoff, query-type?

    // start-up with/without urls on the commandline via mrmlsearch
    if ( relevantItems )
    {
        QDomElement elem = MrmlCreator::addRelevanceList( query );
        KURL::List::ConstIterator it = relevantItems->begin();
        for ( ; it != relevantItems->end(); ++it )
            MrmlCreator::createRelevanceElement( doc, elem, (*it).url(),
                                                 MrmlCreator::Relevant );
    }

    // get relevant items from the view? Only do this when relevantItems is 0L
    else if ( !m_random->isChecked() )
    {
        QDomElement relevants = MrmlCreator::addRelevanceList( query );
        m_view->addRelevanceToQuery( doc, relevants );
    }

    performQuery( doc );
}

Collection MrmlPart::currentCollection() const
{
    return m_collectionCombo->current();
}

Algorithm MrmlPart::firstAlgorithmForCollection( const Collection& coll ) const
{
    if ( !m_algorithms.isEmpty() )
    {
        AlgorithmList::ConstIterator it = m_algorithms.begin();
        for ( ; it != m_algorithms.end(); ++it )
        {
            Algorithm algo = *it;
            if ( algo.paradigms().matches( coll.paradigms() ) )
            {
                algo.setCollectionId( coll.id() );
                return algo;
            }
        }
    }

    qDebug("#################### -> ADEFAULT!");
    Algorithm algo = Algorithm::defaultAlgorithm();
    algo.setCollectionId( coll.id() );
    return algo;
}

// emits the given QDomDocument for eventual plugins, checks after that
// if there are any relevance elements. If there are none, random search is
// implied and performed.
// finally, the search is actually started
void MrmlPart::performQuery( QDomDocument& doc )
{
    QDomElement mrml = doc.documentElement();

    emit aboutToStartQuery( doc ); // let plugins play with it :)

    // no items available? All "neutral"? -> random search

    QDomElement queryStep = KMrml::firstChildElement( mrml, "query-step" );
    bool randomSearch = false;

    if ( !queryStep.isNull() )
    {
        QDomElement relevanceList =
            KMrml::firstChildElement(queryStep, "user-relevance-element-list");
        QValueList<QDomElement> relevanceElements =
            KMrml::directChildElements( relevanceList,
                                        "user-relevance-element" );

        randomSearch = relevanceElements.isEmpty();

        if ( randomSearch )
        {
            m_random->setChecked( true );
            m_random->setEnabled( false );
            queryStep.setAttribute("query-type", "at-random");

            // remove user-relevance-element-list element for random search
            relevanceList.parentNode().removeChild( relevanceList );
        }
    }
    else
    {
        KMessageBox::error( m_view, i18n("Error formulating the query. The "
                                         "\"query-step\" element is missing."),
                            i18n("Query Error") );
    }

    m_job = transferJob( url() );
    slotSetStatusBar( randomSearch ? i18n("Random search...") :
                                     i18n("Searching...") );
    m_job->addMetaData( MrmlShared::kio_task(), MrmlShared::kio_startQuery() );
    qDebug("\n\nSending XML:\n%s", doc.toString().latin1());
    m_job->addMetaData( MrmlShared::mrml_data(), doc.toString() );
}

void MrmlPart::slotSetStatusBar( const QString& text )
{
    if ( text.isEmpty() )
        emit setStatusBarText( i18n("Ready.") );
    else
        emit setStatusBarText( text );
}

void MrmlPart::slotActivated( const KURL& url, ButtonState button )
{
    if ( button == LeftButton )
        emit m_browser->openURLRequest( url );
    else if ( button == MidButton )
        emit m_browser->createNewWindow( url );
    else if ( button == RightButton ) {
        // enableExtensionActions( url, true ); // for now
        emit m_browser->popupMenu( QCursor::pos(), url, QString::null );
        // enableExtensionActions( url, false );
    }
}

void MrmlPart::enableExtensionActions( const KURL& url, bool enable )
{
    bool del = KProtocolInfo::supportsDeleting( url );
    emit m_browser->enableAction( "copy", enable );
    emit m_browser->enableAction( "trash", del );
    emit m_browser->enableAction( "del",   del );
    emit m_browser->enableAction( "shred", url.isLocalFile() );
    emit m_browser->enableAction( "properties", enable );
    // emit m_browser->enableAction( "print", enable ); // ### later
}


// only implemented because it's abstract in the baseclass
bool MrmlPart::openFile()
{
    return false;
}

void MrmlPart::slotConfigureAlgorithm()
{
    m_algoButton->setEnabled( false );

    m_algoConfig = new AlgorithmDialog( m_algorithms, m_collections,
                                        currentCollection(),
                                        m_view, "algorithm configuration" );
    connect( m_algoConfig, SIGNAL( applyClicked() ),
             SLOT( slotApplyAlgoConfig() ));
    connect( m_algoConfig, SIGNAL( finished() ),
             SLOT( slotAlgoConfigFinished() ));

    m_algoConfig->show();
}

void MrmlPart::slotApplyAlgoConfig()
{
    // ###
}

void MrmlPart::slotAlgoConfigFinished()
{
    if ( m_algoConfig->result() == QDialog::Accepted )
        slotApplyAlgoConfig();

    m_algoButton->setEnabled( true );
    m_algoConfig->deleteLater();
    m_algoConfig = 0L;
}

void MrmlPart::initHostCombo()
{
    m_hostCombo->clear();
    m_hostCombo->insertStringList( m_config.hosts() );
}

void MrmlPart::slotHostComboActivated( const QString& host )
{
    ServerSettings settings = m_config.settingsForHost( host );
    openURL( settings.getUrl() );
}

void MrmlPart::setStatus( Status status )
{
    switch ( status )
    {
        case NeedCollection:
            m_startButton->setText( i18n("&Connect") );
            break;
        case CanSearch:
            m_startButton->setText( i18n("&Search") );
            break;
        case InProgress:
            m_startButton->setText( i18n("Sto&p") );
            break;
    };

    m_status = status;
}


void MrmlPart::saveState( QDataStream& stream )
{
    stream << url();
    stream << m_sessionId;
    stream << m_queryList;
//     stream << m_algorithms;
//     stream << m_collections;

    stream << m_resultSizeInput->value();
    stream << *m_collectionCombo;

    m_view->saveState( stream );
}

void MrmlPart::restoreState( QDataStream& stream )
{
    KURL url;
    stream >> url;

    stream >> m_sessionId;
    stream >> m_queryList;
//     stream >> m_algorithms;
//     stream >> m_collections;

    int resultSize;
    stream >> resultSize;
    m_resultSizeInput->setValue( resultSize );
    stream >> *m_collectionCombo;

    m_view->restoreState( stream );

//     openURL( url );
    m_url = url;
}

KAboutData * MrmlPart::createAboutData()
{
    KAboutData *data = new KAboutData(
        "kmrml",
        I18N_NOOP("MRML Client for KDE"),
        KMRML_VERSION,
        I18N_NOOP("A tool to search for images by their content"),
        KAboutData::License_GPL,
        I18N_NOOP("(c) 2001-2002, Carsten Pfeiffer"),
        0,
        I18N_NOOP("http://devel-home.kde.org/~pfeiffer/kmrml/") );

    data->addAuthor( "Carsten Pfeiffer",
                     I18N_NOOP("Developer, Maintainer"),
                     "pfeiffer@kde.org" );
    data->addCredit( "Wolfgang Mller",
                     I18N_NOOP("Developer of the GIFT, Helping Hand") );

    return data;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

#include "mrml_part.moc"
