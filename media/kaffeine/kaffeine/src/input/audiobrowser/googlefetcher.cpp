/*
 * googlefetcher.cpp
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

#include <dom/html_document.h>
#include <dom/html_misc.h>
#include <dom/html_table.h>
#include <dom/dom_exception.h>
#include <dom/dom2_traversal.h>

#include <khtml_part.h>

#include <klocale.h>
#include <kinputdialog.h>
#include <kurl.h>
#include <kio/netaccess.h>

#include "googlefetcher.h"
#include "googlefetcherdialog.h"

GoogleImage::GoogleImage(QString thumbURL, QString size)
{
    // thumbURL is in the following format - and we can regex the imageURL
    // images?q=tbn:hKSEWNB8aNcJ:www.styxnet.com/deyoung/styx/stygians/cp_portrait.jpg
    m_thumbURL = thumbURL.replace("\"","");
    m_imageURL = thumbURL.remove(QRegExp("^.*q=tbn:[^:]*:")).replace("\"","");
    m_size = size.replace("pixels - ", "\n(") + ")";
}


GoogleFetcher::GoogleFetcher( QString artist, QString album, bool gallery )
{
  m_artist = artist;
  m_album = album;
  m_searchString = artist+" "+album;
  encoding = 0;
  galleryImage=galleryPage=0;
  galleryMode = gallery;
}

void GoogleFetcher::slotLoadImageURLs(GoogleFetcher::ImageSize size)
{
    if(m_loadedQuery == m_searchString && m_loadedSize == size)
        return;

    m_imageList.clear();

    KURL url("http://images.google.com/images");
    if ( encoding )
    	url.addQueryItem("q", m_searchString,encoding);
    else
    	url.addQueryItem("q", m_searchString);

    switch (size) {
        case XLarge:
            url.addQueryItem("imgsz", "xlarge|xxlarge");
            break;
        case Large:
            url.addQueryItem("imgsz", "large");
            break;
        case Medium:
            url.addQueryItem("imgsz", "medium");
            break;
        case Small:
            url.addQueryItem("imgsz", "small");
            break;
        case Icon:
            url.addQueryItem("imgsz", "icon");
            break;
        default:
            break;
    }

    m_loadedQuery = m_searchString;
    m_loadedSize = size;

    // We don't normally like exceptions but missing DOMException will kill
    // JuK rather abruptly whether we like it or not so we don't really have a
    // choice if we're going to screen-scrape Google.
    try {

    KHTMLPart part;

    // Create empty document.

    part.begin();
    part.end();

    DOM::Document search = part.document();
    search.setAsync(false); // Grab the document before proceeding.

    kdDebug() << "Performing Google Search: " << url << endl;

    search.load(url.url()+QString("&start=%1&sa=N").arg(galleryPage*20) );

    QString text = search.toString().string();

    if ( !hasImageResults(text) ) {
	kdDebug() << "Search returned no results.\n";
        emit signalNewSearch(m_imageList);
        return;
    }

    // Go through each of the top (table) nodes

    int pos = text.find("/imgres?imgurl");
    int tpos;
    QString s, c;
    while ( pos>-1 ) {
        text = text.right( text.length()-pos-14 );
        tpos = text.find("&h=");
        text = text.right( text.length()-tpos-3 );
        tpos = text.find("&w=");
        c = "pixels - " + text.left( tpos );
        text = text.right( text.length()-tpos-3 );
        tpos = text.find("&sz=");
        c = c + "x" + text.left( tpos );
        tpos = text.find("src=");
        text = text.right( text.length()-tpos-4 );
        if ( (tpos=text.find("width="))==-1 )
        	break;
        s = text.left( tpos-1 );
        text = text.right( text.length()-tpos );
        if ( (tpos=text.find("></a>"))==-1 )
        	break;
	m_imageList.append(GoogleImage(s, c));
	pos = text.find("/imgres?imgurl");
    }
    } // try
    catch (DOM::DOMException &e)
    {
	kdError() << "Caught DOM Exception: " << e.code << endl;
    }
    catch (...)
    {
	kdError() << "Caught unknown exception.\n";
    }

    emit signalNewSearch(m_imageList);
}

QImage GoogleFetcher::galleryNext( bool &end )
{
    QString file;
    QImage image;
    m_loadedSize = All;
    int tries = 0;
    end = false;

    while ( tries<3 ) {
    	if( m_imageList.isEmpty() ) {
    		if ( tries==0 ) {
    			encoding = 0;
    			m_loadedQuery = "";
    			++tries;
    			displayWaitMessage();
    		}
    		else if ( tries==1 ) {
    			encoding = 4; // try iso8859-1
    		    	m_loadedQuery = "";
    		    	++tries;
    		    	kdDebug() << "Trying iso8859-1" << endl;
        		displayWaitMessage();
       		 }
     		else {
        		end = true;
        		return QImage();
        	}
    	}
    	else {
		if(KIO::NetAccess::download( m_imageList[galleryImage].imageURL(), file, 0))
        		image = QImage(file);
        	else
        		image = QImage();
        	++galleryImage;
        	if ( galleryImage==(int)m_imageList.count() ) {
        		m_imageList.clear();
        		galleryImage = 0;
        		++galleryPage;
        	}
        	KIO::NetAccess::removeTempFile(file);
        	return image;
    	}
    }
    return QImage();
}

QPixmap GoogleFetcher::pixmap( bool forceFetch )
{
    bool chosen = false;
    m_loadedSize = All;
    int tries = 0;

    encoding = 0;
    displayWaitMessage();

    QPixmap pixmap;

    while(!chosen) {

        if(m_imageList.isEmpty())
            switch(tries) {
              case 0:
              	encoding = 4; // try iso8859-1
              	m_loadedQuery = "";
              	++tries;
              	kdDebug() << "Trying iso8859-1" << endl;
              	displayWaitMessage();
              	break;
              default :
		if ( forceFetch == true )
	              	chosen = !requestNewSearchTerms(true);
		else
			chosen = true;
            }
        else {
            GoogleFetcherDialog dialog("google", m_imageList, m_artist, m_album, 0);
            connect(&dialog, SIGNAL(sizeChanged(GoogleFetcher::ImageSize)),
                    this, SLOT(slotLoadImageURLs(GoogleFetcher::ImageSize)));
            connect(this, SIGNAL(signalNewSearch(GoogleImageList &)),
                    &dialog, SLOT(refreshScreen(GoogleImageList &)));
            dialog.exec();
            pixmap = dialog.result();
            chosen = dialog.takeIt();

            if(dialog.newSearch())
                requestNewSearchTerms();
        }
    }
    return pixmap;
}

void GoogleFetcher::displayWaitMessage()
{
    //KStatusBar *statusBar = static_cast<KMainWindow *>(kapp->mainWidget())->statusBar();
    //statusBar->message(i18n("Searching for Images. Please Wait..."));
    slotLoadImageURLs();
    //statusBar->clear();*/
}

bool GoogleFetcher::requestNewSearchTerms(bool noResults)
{
    bool ok;
    m_searchString = KInputDialog::getText(i18n("Cover Downloader"),
                                           noResults ?
                                             i18n("No matching images found, please enter new search terms:") :
                                             i18n("Enter new search terms:"),
                                           m_searchString, &ok);
    if(ok && !m_searchString.isEmpty())
        displayWaitMessage();
    else
        m_searchString = m_loadedQuery;

    return ok;
}

bool GoogleFetcher::hasImageResults( QString &doc )
{
    if ( !doc.contains( "/imgres?imgurl" ) )
    	return false;

    return true;
}

#include "googlefetcher.moc"
