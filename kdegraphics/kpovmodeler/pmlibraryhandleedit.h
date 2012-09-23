//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Luis Carvalho
    email                : lpassos@oninetspeed.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#ifndef PMLIBRARYHANDLEEDIT_H
#define PMLIBRARYHANDLEEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>

class QLineEdit;
class QTextEdit;
class QListBox;
class QCheckBox;
class PMLibraryHandle;

/** 
 * This class provides a dialog to edit the definitions of a library.
 */
class PMLibraryHandleEdit: public KDialogBase
{
   Q_OBJECT
public:
   /**
    * Construct a dialog to edit the properties of lib. The library will be
    * modified only if Ok is pressed.
    */
   PMLibraryHandleEdit( PMLibraryHandle* lib, QWidget *parent = NULL, const char* name = NULL );

   static void saveConfig( KConfig* cfg );
   static void restoreConfig( KConfig* cfg );
   static QSize s_size;

private slots:
   void slotOk( );
   void slotEditsChanged( const QString& );
   void slotDescriptionChanged( );
   void slotReadOnlyChanged( );
   void resizeEvent( QResizeEvent *ev );

private:
   PMLibraryHandle* m_pLibrary;
   QLineEdit*       m_pNameEdit;
   QLineEdit*       m_pAuthorEdit;
   QTextEdit*       m_pDescriptionEdit;
   QCheckBox*     m_pReadOnlyEdit;
};

#endif
