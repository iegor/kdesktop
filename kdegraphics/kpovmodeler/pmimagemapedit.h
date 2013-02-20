//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Passos Carvalho
    email                : lpassos@mail.telepac.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#ifndef PMIMAGEMAPEDIT_H
#define PMIMAGEMAPEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pmpalettevalueedit.h"
#include "pmdialogeditbase.h"

class PMImageMap;
class PMPaletteValue;
class PMVectorEdit;
class QComboBox;
class PMFloatEdit;
class PMIntEdit;
class QLabel;
class QCheckBox;
class QWidget;
class QLineEdit;
class QPushButton;

/**
 * Dialog edit class for @ref PMImageMap.
 */
class PMImageMapEdit : public PMDialogEditBase
{
   Q_OBJECT
   typedef PMDialogEditBase Base;
public:
   /**
    * Creates a PMImageMapEdit with parent and name
    */
   PMImageMapEdit( QWidget* parent, const char* name = 0 );

   /** */
   virtual void displayObject( PMObject* o );

   /** */
   virtual bool isDataValid( );
protected:
   /** */
   virtual void createTopWidgets( );
   /** */
   virtual void createBottomWidgets( );
   /** */
   virtual void saveContents( );
   /** */
   QValueList<PMPaletteValue> filters( );
   /** */
   QValueList<PMPaletteValue> transmits( );
   /** */
   void displayPaletteEntries( const QValueList<PMPaletteValue>& filters,
                               const QValueList<PMPaletteValue>& transmits );

private slots:
   /** */
   void slotImageFileTypeChanged( int a );
   /** */
   void slotMapTypeChanged( int a );
   /** */
   void slotInterpolateTypeChanged( int a );
   /** */
   void slotImageFileNameChanged( const QString& a );
   /** */
   void slotImageFileBrowseClicked( );
   /** */
   void slotFilterAllClicked( );
   /** */
   void slotTransmitAllClicked( );
   /** */
   void slotAddFilterEntry( );
   /** */
   void slotRemoveFilterEntry( );
   /** */
   void slotAddTransmitEntry( );
   /** */
   void slotRemoveTransmitEntry( );
private:
   PMImageMap*  m_pDisplayedObject;
   QComboBox*   m_pImageFileTypeEdit;
   QLineEdit*   m_pImageFileNameEdit;
   QPushButton* m_pImageFileNameBrowse;
   QCheckBox*   m_pOnceEdit;
   QComboBox*   m_pMapTypeEdit;
   QComboBox*   m_pInterpolateTypeEdit;
   QCheckBox*   m_pEnableFilterAllEdit;
   QCheckBox*   m_pEnableTransmitAllEdit;
   PMFloatEdit* m_pFilterAllEdit;
   PMFloatEdit* m_pTransmitAllEdit;
   QWidget*     m_pFiltersWidget;
   QWidget*     m_pTransmitsWidget;

   QPtrList<PMPaletteValueEdit> m_filterEntries;
   QPtrList<QPushButton>        m_filterAddButtons;
   QPtrList<QPushButton>        m_filterRemoveButtons;
   QPtrList<PMPaletteValueEdit> m_transmitEntries;
   QPtrList<QPushButton>        m_transmitAddButtons;
   QPtrList<QPushButton>        m_transmitRemoveButtons;
};

#endif
