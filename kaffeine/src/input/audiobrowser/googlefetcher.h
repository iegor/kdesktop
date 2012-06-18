/*
 * googlefetcher.h
 *
 * Copyright (C) 2004 Nathan Toone <nathan@toonetown.com>
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef GOOGLEFETCHER_H
#define GOOGLEFETCHER_H

#include <kdialogbase.h>

#include <qpixmap.h>
#include <qstringlist.h>
#include <qregexp.h>

namespace DOM {
    class HTMLDocument;
}

class KURL;

class GoogleImage
{
public:
    GoogleImage(QString thumbURL = QString::null, QString size = QString::null);

    QString imageURL() const { return m_imageURL; }
    QString thumbURL() const { return m_thumbURL; }
    QString size() const { return m_size; }

private:
    QString m_imageURL;
    QString m_thumbURL;
    QString m_size;
};

typedef QValueList<GoogleImage> GoogleImageList;

class GoogleFetcher : public QObject
{
    Q_OBJECT

public:
    enum ImageSize { All, Icon, Small, Medium, Large, XLarge };

    GoogleFetcher( QString artist, QString album, bool gallery=false );
    QPixmap pixmap( bool forceFetch = false );
    QImage galleryNext( bool &end );

signals:
    void signalNewSearch(GoogleImageList &images);

private:
    void displayWaitMessage();
    bool requestNewSearchTerms(bool noResults = false);

    // Returns true if there are results in the search, otherwise returns false.
    bool hasImageResults( QString &doc );

private slots:
    void slotLoadImageURLs(GoogleFetcher::ImageSize size = All);

private:
    QString m_artist, m_album;
    QString m_searchString;
    QString m_loadedQuery;
    ImageSize m_loadedSize;
    GoogleImageList m_imageList;
    uint m_selectedIndex;
    int encoding;
    int galleryImage;
    int galleryPage;
    bool galleryMode;
};

#endif /* GOOGLEFETCHER_H */
