/* This file is part of the KDE project
   Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef MRML_VIEW_H
#define MRML_VIEW_H

#include <qevent.h>
#include <qframe.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qscrollview.h>

class QDomDocument;
class QDomElement;
class QTimer;

class KComboBox;

namespace KMrml
{

class MrmlViewItem;


class MrmlViewItemList : public QPtrList<MrmlViewItem>
{
protected:
    // sort by similarity
    virtual int compareItems( QPtrCollection::Item, QPtrCollection::Item );

};


class MrmlView : public QScrollView
{
    friend class MrmlViewItem;

    Q_OBJECT

public:
    MrmlView( QWidget *parent = 0L, const char *name = 0L );
    ~MrmlView();

    MrmlViewItem * addItem( const KURL& url, const KURL& thumbURL,
                            const QString& similarity );
    MrmlViewItem * addItem( const KURL& url, const KURL& thumbURL,
                            double similarity );


    void addRelevanceToQuery( QDomDocument&, QDomElement& parent );

    void clear();

    bool isEmpty() const { return m_items.isEmpty(); }

    void stopDownloads();

    void saveState( QDataStream& stream );
    void restoreState( QDataStream& stream );

signals:
    void activated( const KURL& url, ButtonState button );
    void onItem( const KURL& url );

protected:
    virtual void resizeEvent( QResizeEvent * );

private slots:
    void slotLayout();
    void slotDownloadFinished( const KURL&, const QByteArray& );

private:
    /**
     * @returns a _temporary_ pointer to a pixmap. Copy it!
     */
    QPixmap * getPixmap( const KURL& url );

    MrmlViewItemList m_items;
    QTimer *m_timer;
    QPixmapCache m_pixmapCache;
    QPixmap m_unavailablePixmap;


};


class MrmlViewItem : public QFrame
{
    Q_OBJECT

public:
    enum Relevance
    {
        Relevant   = 0,
        Neutral    = 1,
        Irrelevant = 2
    };

    MrmlViewItem( const KURL& url, const KURL& thumbURL, double similarity,
                  MrmlView *view, const char *name=0L );
    virtual ~MrmlViewItem();

    void setPixmap( const QPixmap& pixmap );

    void createRelevanceElement( QDomDocument& document, QDomElement& parent );

    double similarity() const { return m_similarity; }

    void setSimilarity( double value );

    virtual QSize sizeHint() const;

    const KURL& url() const { return m_url; }
    const KURL& thumbURL() const { return m_thumbURL; }

    bool hasRemotePixmap() const { return !m_thumbURL.isLocalFile() && m_hasRemotePixmap; }

    Relevance relevance() const;
    void setRelevance( Relevance relevance );

protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void resizeEvent( QResizeEvent * );

    virtual void mousePressEvent( QMouseEvent * );
    virtual void mouseMoveEvent( QMouseEvent * );
    virtual void mouseReleaseEvent( QMouseEvent * );

private:
    bool hitsPixmap( const QPoint& ) const;
    MrmlView * view() const { return m_view; }

    inline int pixmapX() const {
        return QMAX( margin, (width() - m_pixmap.width()) / 2);
    }
    inline int pixmapY() const {
        return m_combo->y() - similarityHeight - m_pixmap.height() - margin;
    }

    KComboBox *m_combo; // for relevance
    MrmlView *m_view;

    KURL m_url;
    KURL m_thumbURL;

    QPixmap m_pixmap;

    double m_similarity;
    const int similarityFullWidth;
    bool m_hasRemotePixmap;

    QPoint pressedPos;

    static const int spacing = 3;
    static const int margin = 5;
    static const int similarityHeight = 4;

};

QDataStream& operator <<( QDataStream& stream, const KMrml::MrmlViewItem& );

}

#endif // MRML_VIEW_H
