/* This file is part of the KDE project
   Copyright (C) 2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <qdom.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qtimer.h>
#include <qtooltip.h>

#include <kcursor.h>
#include <kdatastream.h>
#include <klocale.h>
#include <kcombobox.h>
#include <kmimetype.h>
#include <kurl.h>
#include <kurldrag.h>

#include "loader.h"
#include "mrml_creator.h"
#include "mrml_view.h"

using namespace KMrml;

MrmlView::MrmlView( QWidget *parent, const char *name )
    : QScrollView( parent, name )
{
    setStaticBackground( true );
    setResizePolicy( Manual );
    setHScrollBarMode( AlwaysOff );
    enableClipper( true ); // ### test this

    m_items.setAutoDelete( true );

    connect( Loader::self(), SIGNAL( finished(const KURL&, const QByteArray&)),
             SLOT( slotDownloadFinished( const KURL&, const QByteArray& )));

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), SLOT( slotLayout() ));

    // we need a pixmap to be shown when no thumbnail is available for a
    // query result image
    QLabel l( i18n( "No thumbnail available" ), 0L );
    l.setFixedSize( 80, 80 );
    l.setAlignment( WordBreak | AlignCenter );
//     l.setFrameStyle( QLabel::Box | QLabel::Plain );
//     l.setLineWidth( 1 );
    l.setPaletteBackgroundColor( Qt::white );
    l.setPaletteForegroundColor( Qt::black );
    m_unavailablePixmap = QPixmap::grabWidget( &l );
}

MrmlView::~MrmlView()
{
}

MrmlViewItem * MrmlView::addItem( const KURL& url, const KURL& thumbURL,
                                  const QString& similarity )
{
    bool ok;
    double value = similarity.toDouble( &ok );
    if ( !ok || value < 0.05 )
        return 0L;

    return addItem( url, thumbURL, value );
}

MrmlViewItem * MrmlView::addItem( const KURL& url, const KURL& thumbURL,
                                  double similarity )
{
    if ( !url.isValid() ) {
        qWarning( "MrmlPart: received malformed URL from query: %s",
                  url.prettyURL().isNull() ? "(null)" : url.prettyURL().latin1() );
        return 0L;
    }

//     qDebug("** url: %s", thumbURL.url().latin1());

    MrmlViewItem *item = new MrmlViewItem( url, thumbURL, similarity, this );
    QPixmap *pixmap = getPixmap( thumbURL );
    if ( pixmap )
        item->setPixmap( *pixmap );

    m_items.append( item );

    m_timer->start( 0, true );
    return item;
}

void MrmlView::addRelevanceToQuery( QDomDocument& document,
                                    QDomElement& parent )
{
    QPtrListIterator<MrmlViewItem> it( m_items );
    for( ; it.current(); ++it ) {
        it.current()->createRelevanceElement( document, parent );
    }
}

void MrmlView::clear()
{
    m_items.clear(); // items are deleted and removed from scrollview
    setContentsPos( 0, 0 );
}

QPixmap * MrmlView::getPixmap( const KURL& url )
{
    QString u = url.url();
    QPixmap *pix = m_pixmapCache.find( u );
    if ( pix )
        return pix;

    if ( url.isLocalFile() ) {
        QPixmap p;
        if ( !p.load( url.path() ) )
             p = m_unavailablePixmap;

        m_pixmapCache.insert( u, p );
        return m_pixmapCache.find( u );
    }
    else { // remote url, download with KIO
        Loader::self()->requestDownload( url );
    }

    return 0L;
}

void MrmlView::slotDownloadFinished( const KURL& url, const QByteArray& data )
{
    QPtrListIterator<MrmlViewItem> it( m_items );
    for( ; it.current(); ++it ) {
        MrmlViewItem *item = it.current();
        if ( item->thumbURL() == url )
        {
            QPixmap p;
            if ( data.isEmpty() || !p.loadFromData( data ) )
                p = m_unavailablePixmap;

            m_pixmapCache.insert( url.url(), p );
            item->setPixmap( p );

            slotLayout();
            return;
        }
    }
}

void MrmlView::stopDownloads()
{
    Loader *loader = Loader::self();
    QPtrListIterator<MrmlViewItem> it( m_items );
    for( ; it.current(); ++it ) {
        MrmlViewItem *item = it.current();
        if ( !item->hasRemotePixmap() )
            loader->removeDownload( item->url() );
    }
}

void MrmlView::slotLayout()
{
    int itemWidth = 0;
    QPtrListIterator<MrmlViewItem> it( m_items );

    for ( ; it.current(); ++it ) {
        itemWidth = QMAX( itemWidth, it.current()->sizeHint().width() );
    }

    if ( itemWidth == 0 )
        return;


    uint itemsPerRow = visibleWidth() / itemWidth;
    int margin = (visibleWidth() - (itemsPerRow * itemWidth)) / 2;
    int rowHeight = 0;
    uint item = 0;
    uint y = 5;

    // pointing to the first item of a row
    QPtrListIterator<MrmlViewItem> rowIt( m_items );

    for ( it.toFirst(); it.current(); ++it ) {
        if ( item >= itemsPerRow ) {
            item = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        if ( item == 0 )
            rowIt = it;

        rowHeight = QMAX( rowHeight, it.current()->sizeHint().height() );
        addChild( it.current(), margin + item * itemWidth, y );
        it.current()->show();

        item++;

        // resize all items of the current row so they all have the same size
        if ( item >= itemsPerRow || it.atLast() )
        {
            for ( uint i = 0; (i < itemsPerRow && rowIt.current()); i++ )
            {
                rowIt.current()->resize( itemWidth, rowHeight );
                ++rowIt;
            }
        }
    }

    resizeContents( visibleWidth(), y + rowHeight );
}

void MrmlView::resizeEvent( QResizeEvent *e )
{
    int oldW = visibleWidth();
    QScrollView::resizeEvent( e );

    if ( visibleWidth() != oldW )
        slotLayout();
}

void MrmlView::saveState( QDataStream& stream )
{
    stream << m_items.count();
    QPtrListIterator<MrmlViewItem> it( m_items );
    for( ; it.current(); ++it ) {
        stream << *it.current();
    }

}

void MrmlView::restoreState( QDataStream& stream )
{
    stopDownloads();
    clear();

    int count;
    stream >> count;

    KURL url, thumbURL;
    double similarity;
    Q_UINT32 relevance;
    MrmlViewItem *item;


    for ( int i = 0; i < count; i++ )
    {
        stream >> url;
        stream >> thumbURL;
        stream >> similarity;
        stream >> relevance;

        item = addItem( url, thumbURL, similarity );
        if ( item )
            item->setRelevance( (MrmlViewItem::Relevance) relevance );
    }
}

QDataStream& KMrml::operator<<( QDataStream& stream,
                                const KMrml::MrmlViewItem& item )
{
    return stream << item.url()
                  << item.thumbURL()
                  << item.similarity()
                  << static_cast<Q_UINT32>( item.relevance() );
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


MrmlViewItem::MrmlViewItem( const KURL& url, const KURL& thumbURL,
                            double similarity, MrmlView *view,
                            const char *name )
    : QFrame( view->viewport() , name ),
      m_view( view ),
      m_url( url ),
      m_thumbURL( thumbURL ),
      similarityFullWidth( 120 ), // ###
      m_hasRemotePixmap( false )
{
    if ( m_similarity != -1 )
        m_similarity = QMAX( 0.0, QMIN( 1.0, similarity ));
    setFrameStyle( Panel | Sunken );
    setMouseTracking( true );

    m_combo = new KComboBox( this );
    QToolTip::add( m_combo, i18n("You can refine queries by giving feedback about the current result and pressing the Search button again.") );
    m_combo->insertItem( i18n("Relevant"),   Relevant );
    m_combo->insertItem( i18n("Neutral"),    Neutral );
    m_combo->insertItem( i18n("Irrelevant"), Irrelevant );
    m_combo->adjustSize();
    m_combo->setCurrentItem( Neutral );

    /*
    if ( similarity > -1 )
        QToolTip::add( this, QString::fromLatin1("<qt>%1<br>%1</qt>")
                       .arg( url )
                       .arg(i18n("Similarity: %1").arg( QString::number(similarity))));
    else
        QToolTip::add( this, QString::fromLatin1("<qt>%1</qt>").arg( url ) );
    */

    setMinimumSize( 130, 130 ); // ###
}

MrmlViewItem::~MrmlViewItem()
{
}

void MrmlViewItem::setPixmap( const QPixmap& pix )
{
    if ( !m_url.isLocalFile() )
        m_hasRemotePixmap = true;

    m_pixmap = pix;
    adjustSize();
    update();
}

void MrmlViewItem::paintEvent( QPaintEvent *e )
{
    QFrame::paintEvent( e );

    if ( !m_pixmap.isNull() ) {
        bitBlt( this, pixmapX(), pixmapY(),
                &m_pixmap, 0, 0, m_pixmap.width(), m_pixmap.height(),
                CopyROP );
    }

    if ( m_similarity >= 0 ) {
        QPainter p( this );
        QPen pen( colorGroup().highlight(), 1, QPen::SolidLine );
        p.setPen( pen );
        int x = margin;
        int y = m_combo->y() - similarityHeight - 2;
        int w = (int) (similarityFullWidth * m_similarity);
        int h = similarityHeight;
        p.drawRect( x, y, similarityFullWidth, h );
        p.fillRect( x, y, w, h, colorGroup().highlight() );
    }
}

void MrmlViewItem::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );

    int y = height() - m_combo->height() - margin;
    m_combo->move( width()/2 - m_combo->width()/2, y );
}

QSize MrmlViewItem::sizeHint() const
{
    int w = QMAX( QMAX(minimumHeight(), m_combo->width()), m_pixmap.width() );
    w += 2 * margin;

    int h = m_pixmap.isNull() ? margin : margin + spacing + m_pixmap.height();
    h += (m_similarity > -1) ? similarityHeight + spacing : 0;
    h += m_combo->height() + margin;

    return QSize( w, h );
}

void MrmlViewItem::mousePressEvent( QMouseEvent *e )
{
    QFrame::mousePressEvent( e );
    pressedPos.setX( 0 );
    pressedPos.setY( 0 );


    if ( e->button() == LeftButton || e->button() == MidButton ) {
        if ( hitsPixmap( e->pos() ) )
            pressedPos = e->pos();
    }
    else if ( e->button() == RightButton && hitsPixmap( e->pos() ) )
        emit view()->activated( m_url, e->button() );
}

void MrmlViewItem::mouseMoveEvent( QMouseEvent *e )
{
    if ( hitsPixmap( e->pos() ) ) {
        if ( !ownCursor() ) { // nice hacklet :)
            setCursor( KCursor::handCursor() );
            emit view()->onItem( m_url );
        }
    }
    else {
        if ( ownCursor() ) {
            unsetCursor();
            emit view()->onItem( KURL() );
        }
    }

    if ( (e->state() & LeftButton) && !pressedPos.isNull() ) {
        QPoint dist = e->pos() - pressedPos;
        if ( dist.manhattanLength() > KGlobalSettings::dndEventDelay() ) {
            // start drag here
            KURL::List urls;
            // ### support multiple files?
            urls.append( m_url );
            KURLDrag *drag = new KURLDrag( urls, this );
            drag->setPixmap( KMimeType::pixmapForURL( m_url ) );
            drag->drag();
        }
    }
}

void MrmlViewItem::mouseReleaseEvent( QMouseEvent *e )
{
    if ( hitsPixmap( e->pos() )) {
        QPoint dist = e->pos() - pressedPos;
        if ( dist.manhattanLength() < KGlobalSettings::dndEventDelay() ) {
            emit view()->activated( m_url, e->button() );
        }
    }
}

bool MrmlViewItem::hitsPixmap( const QPoint& pos ) const
{
    if ( m_pixmap.isNull() )
        return false;

    if ( pos.x() > pixmapX() && pos.x() < pixmapX() + m_pixmap.width() &&
         pos.y() > pixmapY() && pos.y() < pixmapY() + m_pixmap.height() )
        return true;
    return false;
}

void MrmlViewItem::createRelevanceElement( QDomDocument& document,
                                           QDomElement& parent )
{
    int rel = m_combo->currentItem();
    if ( rel == Neutral )
        return;

    MrmlCreator::createRelevanceElement( document, parent, m_url.url(),
        (rel == Relevant) ? MrmlCreator::Relevant : MrmlCreator::Irrelevant );
}

MrmlViewItem::Relevance MrmlViewItem::relevance() const
{
    return (Relevance) m_combo->currentItem();
}

void MrmlViewItem::setRelevance( Relevance relevance )
{
    m_combo->setCurrentItem( relevance );
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int MrmlViewItemList::compareItems( QPtrCollection::Item item1,
                                    QPtrCollection::Item item2 )
{
    double s1 = (static_cast<MrmlViewItem*>( item1 ))->similarity();
    double s2 = (static_cast<MrmlViewItem*>( item2 ))->similarity();

    if ( s1 < s2 )
        return 1;
    else if ( s1 > s2 )
        return -1;
    else
        return 0;
}

#include "mrml_view.moc"
