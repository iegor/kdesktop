/** 
 * Copyright (C) 1997-2003 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __KGV_MINIWIDGET_H
#define __KGV_MINIWIDGET_H

#include <qprinter.h>

#include "dscparse_adapter.h"
#include "kgv.h"
#include "kgvdocument.h"
#include "displayoptions.h"

#include "thumbnailservice.h"

#include <qobject.h>

class InfoDialog;
class KGVPart;
class KPSWidget;
class MarkList;

class KGVMiniWidget : public QObject
{
    Q_OBJECT

public:
    KGVMiniWidget( KGVPart* part, const char* name = 0 );

    void setDocument( KGVDocument* document );

    static QString pageSizeToString( QPrinter::PageSize );

    void setPSWidget( KPSWidget* psWidget );

    void setDisplayOptions( const DisplayOptions& newOptions );
    const DisplayOptions& displayOptions() const { return _options; }

    void restoreOverrideOrientation();
    void setOverrideOrientation( CDSC_ORIENTATION_ENUM );

    void restoreOverridePageMedia();
    void setOverridePageMedia( const QString& mediaName );

    ThumbnailService* getThumbnailService() { return _thumbnailService; }
    
    /**
     * Enable/disable fancy, document-supplied page labels.
     **/
    void enablePageLabels( bool e = true );
    bool arePageLabelsEnabled () { return _usePageLabels; }

    /**
     * Return true if the current page is the first page, false otherwise.
     */
    bool atFirstPage() const;
    /**
     * Return true if the current page is the last page, false otherwise.
     */
    bool atLastPage()  const;

    /**
     * Return true if we're zoomed in fully, false otherwise.
     */
    bool atMaxZoom() const;
    /**
     * Return true if we're zoomed out fully, false otherwise.
     */
    bool atMinZoom()  const;

    int currentPage() const { return _options.page(); }

public slots:
    bool prevPage();
    bool nextPage();
    void firstPage();
    void lastPage();
    void goToPage();
    void goToPage( int page );

    void zoomIn();
    void zoomOut();

    void fitWidth( unsigned int );
    void fitHeight( unsigned int );
    void fitWidthHeight( unsigned int, unsigned int );

    void info();

    /**
     * Redisplay the page if the file has changed on disk.
     **/
    void redisplay();

signals:
    /**
     * Page changed.
     */
    void newPageShown( int pageNumber );  // Should this one be under DOCUMENT?

    void newPageImage( QPixmap image );

    void setStatusBarText( const QString& );

protected:
    void showPage( int pageNumber );
    void buildTOC();

protected slots:
    void sendPage();
    void updateStatusBarText( int pageNumber );

protected:
    void reset();

private:

    CDSC_ORIENTATION_ENUM orientation() const;
    CDSC_ORIENTATION_ENUM orientation( int pageNo ) const;

    QString pageMedia() const;
    QString pageMedia( int pageNo ) const;

    KDSCBBOX boundingBox() const;
    KDSCBBOX boundingBox( int pageNo ) const;

    void setMagnification( double );

    KDSC* const dsc() const
    { return _document ? _document->dsc() : 0; }
    FILE* psFile() 
    { return document()->psFile(); }
    KGVDocument* const document() const 
    { return _document; }

private slots:
    void slotDocumentOpened();

private:

    friend class ThumbnailService;
    KGVDocument* _document;

    KGVPart*   _part;
    KPSWidget* _psWidget;
    ThumbnailService* _thumbnailService;

    bool _usePageLabels;

    int _visiblePage;

    DisplayOptions _options;
    QString        _fallBackPageMedia;
};

#endif

// vim:sw=4:sts=4:ts=8:sta:tw=78:noet
