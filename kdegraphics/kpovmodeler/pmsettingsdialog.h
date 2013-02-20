//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/

#ifndef PMSETTINGSDIALOG_H
#define PMSETTINGSDIALOG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

class QFrame;
class QCheckBox;
class QLineEdit;
class QListBox;
class QListView;
class QButtonGroup;
class QComboBox;
class KColorButton;
class KConfig;
class PMIntEdit;
class PMFloatEdit;
class PMPart;
class PMViewLayout;
class PMViewLayoutEntry;

/**
 * Base class for configuration dialog pages.
 *
 * All base classes have to implement the pure virtual
 * methods @ref displaySettings, @ref displayDefaults, @ref validateData
 * and @ref applySettings
 */
class PMSettingsDialogPage : public QWidget
{
   Q_OBJECT
public:
   /**
    * Constructor
    */
   PMSettingsDialogPage( QWidget* parent, const char* name = 0 );
   /**
    * Display the settings here.
    *
    * Base classes have to implement this method.
    */
   virtual void displaySettings( ) = 0;
   /**
    * Validate the changed data here and return true
    * if the data is valid. Display an error message
    * and return false otherwise.
    *
    * Base classes have to implement this method.
    */
   virtual bool validateData( ) = 0;
   /**
    * Make the changes permanent here.
    *
    * Base classes have to implement this method.
    */
   virtual void applySettings( ) = 0;
   /**
    * Display the default values.
    *
    * Base classes have to implement this method.
    */
   virtual void displayDefaults( ) = 0;
signals:
   /**
    * Emit this signal if a parameter was changed
    * that influences the wire frame rendering.
    */
   void repaintViews( );
   /**
    * Tells the settings dialog to show this page.
    */
   void showMe( );
};


/**
 * Helper class, used internally by @ref PMSettingsDialog
 */
class PMRegisteredSettingsPage
{
public:
   PMRegisteredSettingsPage( )
   {
      topPage = 0;
      page = 0;
      index = 0;
   }
   PMRegisteredSettingsPage( QWidget* top, PMSettingsDialogPage* p,
                             int i )
   {
      topPage = top;
      page = p;
      index = i;
   }
   QWidget* topPage;
   PMSettingsDialogPage* page;
   int index;
};

/**
 * Configuration dialog
 */
class PMSettingsDialog : public KDialogBase
{
   Q_OBJECT
public:
   /**
    * Standard constructor
    */
   PMSettingsDialog( PMPart* part, QWidget* parent = 0, const char* name = 0 );
   /**
    * Registers a new settings page.
    *
    * @param topPage The page created with addVBoxPage
    * @param page The internal settings page
    */
   void registerPage( QWidget* topPage, PMSettingsDialogPage* page );

   static void saveConfig( KConfig* cfg );
   static void restoreConfig( KConfig* cfg );

protected:
   virtual void resizeEvent( QResizeEvent* ev );

protected slots:
   /**
    * Validates the data and makes the changes permanent.
    */
   virtual void slotApply( );
   /**
    * Validates the data, makes the changes permanent and closes the dialog.
    */
   virtual void slotOk( );
   /**
    * Displays the default values.
    */
   virtual void slotDefault( );
   /**
    * Closes the dialog without saving the data.
    */
   virtual void slotCancel( );

   /**
    * Repaints the opengl views
    */
   void slotRepaint( );
   /**
    * Shows the sender page
    */
   void slotShowPage( );
   
private:
   void displaySettings( );
   bool validateData( );
   void saveSettings( );
   int findPage( const PMSettingsDialogPage* page );
   bool m_repaint;
   QValueList<PMRegisteredSettingsPage> m_pages;
   PMPart* m_pPart;
   
   static QSize s_size;
};

#endif
