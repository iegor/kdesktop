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

#ifndef __DJVUMULTIPAGE_H
#define __DJVUMULTIPAGE_H

#include <qstringlist.h>

#include "kmultipage.h"
#include "djvurenderer.h"

#include "DjVuToPS.h"

class KSelectAction;

/*! \mainpage DjVuMultiPage
  
\section intro_sec Introduction

kvsdemo is a minimal, but well-documented reference implementation of
a kviewshell plugin that can serve as a starting point for a
real-world implementation.

\section install_sec Usage

When kvsdemo is installed, the kviewshell program can open C++ source
files, i.e. files of mime type text/x-c++src. When such a file is
loaded, kviewshell shows 10 blank pages of A4 size.
 
\section content Content

Only the two classes that are absolutely necessary for a working
plugin are implemented. The only other file that is installed is a
desktop file, which tells kviewhshell to use the plugin.

- kvsdemo_multipage, an implementation of a kmultipage. In a real
application, this class would create and manage the GUI elements that
the plugin adds to the GUI of the kviewshell. This implementation adds
nothing, and does only the minimal initialization required..

- kvsdemo_renderer, an implementation of a documentRenderer. This
class is responsible for document loading and rendering.

- kvsdemo.desktop, the desktop entry file that tells KDE that kvsdemo
is a plugin for kviewshell that handles files of type
text/x-c++src. Without this file installed, the file dialog in
kviewshell would not show C++ source files, and the command line
"kvieshell test.cpp" would fail with an error dialog "No plugin for
text/x-c++src files installed".

*/



 
/*! \brief Well-documented minimal implementation of a KMultiPage
  
This class provides a well-documented reference implementation of a
KMultiPage, suitable as a starting point for a real-world
implementation. In a real application, this class would contain the
GUI elements that the plugin adds to the GUI of the kviewshell. Our
implementation adds nothing, and does only the minimal initialization
required.

*/

class DjVuMultiPage : public KMultiPage
{
  Q_OBJECT

public:
  /** Constructor
      
  The constructor needs to initialize several members of the
  kmultipage. Please have a look at the constructor's source code to
  see how to adjust this for your implementation.
  */
  DjVuMultiPage(QWidget *parentWidget, const char *widgetName, QObject *parent,
		   const char *name, const QStringList& args = QStringList());
  
  /** Destructor
		
  This destructor does nothing.
  */
  virtual ~DjVuMultiPage();

  virtual void setFile(bool r);

  /** List of file formats for file saving
  
  This method returns the list of supported file formats for saving
  the file.
  */
  virtual QStringList fileFormats() const;

  /** Author information
 
  This member returns a structure that contains information about the
  authors of the implementation
  */
  static KAboutData* createAboutData();

  /** Re-implementation of the print method */
  virtual void print();

  virtual bool isReadWrite() const;
  virtual bool isModified() const;

  virtual void slotSave();

 protected:
  virtual void enableActions(bool);

 private slots:
  void setRenderMode(int mode);

  /** Opens a dialog to delete pages */
  void slotDeletePages();

 private:
  /** This member holds the renderer which is used by the demo
      implementation */
  DjVuRenderer     djvuRenderer;

  KSelectAction* renderModeAction;
  KAction* deletePagesAction;
};

#endif
