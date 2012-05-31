/***************************************************************************
 *   Copyright (C) 2005 by Stefan Kebekus                                  *
 *   kebekus@kde.org                                                       *
 *                                                                         *
 *   Copyright (C) 2005 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#ifndef _DJVURENDERER_H_
#define _DJVURENDERER_H_

#include <kprogress.h>
#include <qpixmap.h>

#include "DjVuImage.h"
#include "DjVuDocEditor.h"
#include "DjVuText.h"
#include "DjVuToPS.h"
#include "ByteStream.h"

#include "documentRenderer.h"

class RenderedDocumentPage;

/*! \brief Well-documented minimal implementation of a documentRenderer
  
This class provides a well-documented reference implementation of a
documentRenderer, suitable as a starting point for a real-world
implementation. This class is responsible for document loading and
rendering. Apart from the constructor and the descructor, it
implements only the necessary methods setFile() and drawPage(). The
method setFile() ignores the file content and simulates a document
with 10 empty pages of A4 size and a few anchors and bookmarks.
*/

class DjVuRenderer : public DocumentRenderer
{
  Q_OBJECT

public:
   /** Default constructor

       This constructor simply prints a message and calls the default constructor.
   */
   DjVuRenderer(QWidget* parent);

   /** Destructor

       The destructor simply prints a message. It uses the mutex to
       ensure that this class is not destructed while another thread
       is currently using it.
   */
  ~DjVuRenderer();

  /** Opening a file

      This implementation does the necessary consistency checks and
      complains, e.g. if the file does not exist, but otherwise completely
      disregards the file content. It simulates a document of 10 empty pages of
      A4 size, with a few sample bookmarks and  anchors "p1", "p2", "p3"
      for page 1, 2 and 3, respectively.

      @param fname the name of the file that should be opened. 
  */
  virtual bool setFile(const QString& fname, const KURL &);

  /** Rendering a page
      @param res resolution at which drawing should take place
      @param page pointer to a page structur on which we should draw 
  */
  virtual void drawPage(double res, RenderedDocumentPage* page);

  /** Extract the hidden text layer

  This function decodes the hidden text layer without actually decoding the full page.
  It is therefore much faster then drawPage if you only need the text information.
  */
  virtual void getText(RenderedDocumentPage* page);

  virtual bool supportsTextSearch() const { return true; };

  /** DJVU to PostScript conversion
      
  This method uses the converter to convert the document to a PostScript.
  
  @param converter a DjVuToPS converter, whose options should already
  be set to
  
  @param filename name of the PostScript file to generate
  
  @param pageList list of pages that are to be converted, with the
  usual convention that "1" means "first page"
  
  @returns 'true' if the conversion was successful, 'false' if it
  wasn't. The conversion can fail, for example, when the user aborts
  the operation.
  */
  bool convertToPSFile( DjVuToPS &converter, QString filename, QValueList<int> &pageList );

  /** Deletes pages from the document */
  void deletePages(Q_UINT16 from, Q_UINT16 to);

  /** Saves the file */
  bool save(const QString &filename);

private:
  /* This method is called after a document is loaded with
     create_wait() or has been modified (e.g. inserting/deleting
     pages). It sets "numPages", fills the "pageSizes" array, and
     clear the anchorList. */
  bool initializeDocument();

  void getAnnotations(RenderedDocumentPage* page, GP<DjVuImage> djvuPage);

  bool getPageInfo(GP<DjVuFile> file, int& width, int& height, int& dpi);

  GP<DjVuTXT> getText(PageNumber pageNumber);

  void fillInText(RenderedDocumentPage* page, const GP<DjVuTXT>& text, DjVuTXT::Zone& zone, QSize& djvuPageSize);

  GP<DjVuDocEditor> document;

  /** Method used internally to report the progress of the DjVu to
      PostScript conversion */
  static void printerInfoCallBack(int page_num, int page_count, int tot_pages, DjVuToPS::Stage, void *);

  QPixmap pixmap;
  GP<ByteStream> PPMstream;
};

#endif
