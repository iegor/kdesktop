/***************************************************************************
                          kgallerydialog.h  -  description
                             -------------------
    begin                : mar abr 1 2003
    copyright            : (C) 2003 by javi
    email                : javi@recibos
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGALLERYDIALOG_H
#define KGALLERYDIALOG_H

#include "kgallerydialogbase.h"

#include <kurl.h>
// #include <kio/global.h>
// #include <kio/job.h>

/**Gallery
  *@author Javier Campos
  */

// forward declarations
class KTempFile;

namespace KIO { class Job; }

class KGalleryDialog : public KGalleryDialogBase  {
   Q_OBJECT
public: 
	KGalleryDialog(QWidget *parent=0, const char *name=0);
	~KGalleryDialog();
  /** Open keduca file. This function can open a remote or local url. */
  bool openFile( const KURL &url );
  /** Get keduca test url */
  KURL getURL();
  /** Add url */
  void putURL(const KURL &urlFile);
protected slots: // Private slots
  /** No descriptions */
  void slotButtonAdd();
  /** Open selected document */
  void accept();
  /** Select Server */
  void slotServerSelected( QListViewItem *item );
protected:
    /** Open keduca file. This function does the actual work and expects a local filename . */
    bool loadFile( const QString &filename );
private: // Private attributes
  /** Current url */
  KURL _currentURL;
  /** The temporary file to which the document is saved, NULL if no temporary file is needed */
  KTempFile *_tmpfile;
private: // Private methods
  /** Read servers */
  void configRead();
  /** Write servers lists */
  void configWrite();
  /** Init gui settings */
};

#endif
