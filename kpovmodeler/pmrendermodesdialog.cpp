/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001 by Andreas Zehender
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


#include "pmrendermodesdialog.h"

#include <qlistbox.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

#include "pmlineedits.h"

QSize PMRenderModesDialog::s_size = QSize( 300, 200 );

PMRenderModesDialog::PMRenderModesDialog( PMRenderModeList* modes, QWidget* parent, const char* name )
      : KDialogBase( parent, name, true, i18n( "Render Modes" ),
                     Ok | Cancel, Ok )
{
   m_selectionIndex = modes->at( );
   m_pOriginalModes = modes;
   PMRenderModeListIterator it( *modes );
   for( ; it.current( ); ++it )
      m_workingModes.append( new PMRenderMode( *( it.current( ) ) ) );
   m_workingModes.setAutoDelete( true );

   QVBox* mainPage = makeVBoxMainWidget( );
   m_pListBox = new QListBox( mainPage );
   connect( m_pListBox, SIGNAL( highlighted( int ) ), SLOT( slotModeSelected( int ) ) );

   QHBox* buttons = new QHBox( mainPage );
   buttons->setSpacing( KDialog::spacingHint( ) );
   m_pAddButton = new QPushButton( i18n( "Add" ), buttons );
   connect( m_pAddButton, SIGNAL( clicked( ) ), SLOT( slotAdd( ) ) );
   m_pRemoveButton = new QPushButton( i18n( "Remove" ), buttons );
   connect( m_pRemoveButton, SIGNAL( clicked( ) ), SLOT( slotRemove( ) ) );
   m_pEditButton = new QPushButton( i18n( "Edit..." ), buttons );
   connect( m_pEditButton, SIGNAL( clicked( ) ), SLOT( slotEdit( ) ) );
   m_pUpButton = new QPushButton( i18n( "Up" ), buttons );
   connect( m_pUpButton, SIGNAL( clicked( ) ), SLOT( slotUp( ) ) );
   m_pDownButton = new QPushButton( i18n( "Down" ), buttons );
   connect( m_pDownButton, SIGNAL( clicked( ) ), SLOT( slotDown( ) ) );

   m_pRemoveButton->setEnabled( false );
   m_pUpButton->setEnabled( false );
   m_pDownButton->setEnabled( false );

   enableButtonOK( false );

   resize( s_size );
   displayList( );
   connect( m_pListBox,  SIGNAL( doubleClicked ( QListBoxItem *) ), this, SLOT(slotEdit( ) ) );
}

void PMRenderModesDialog::slotChanged( )
{
   enableButtonOK( true );
}

void PMRenderModesDialog::slotModeSelected( int index )
{
   m_selectionIndex = index;
   checkButtons( );
   slotChanged( );
}

void PMRenderModesDialog::displayList( )
{
   PMRenderModeListIterator it( m_workingModes );
   bool b = m_pListBox->signalsBlocked( );
   m_pListBox->blockSignals( true );

   m_pListBox->clear( );
   for( ; it.current( ); ++it )
      m_pListBox->insertItem( it.current( )->description( ) );
   m_pListBox->setSelected( m_selectionIndex, true );

   m_pListBox->blockSignals( b );

   checkButtons( );
}

void PMRenderModesDialog::checkButtons( )
{
   if( m_selectionIndex < 0 )
   {
      m_pRemoveButton->setEnabled( false );
      m_pEditButton->setEnabled( false );
      m_pUpButton->setEnabled( false );
      m_pDownButton->setEnabled( false );
   }
   else
   {
      int num = m_workingModes.count( );

      m_pRemoveButton->setEnabled( true );
      m_pEditButton->setEnabled( true );
      m_pUpButton->setEnabled( m_selectionIndex != 0 );
      m_pDownButton->setEnabled( m_selectionIndex != ( num - 1 ) );
   }
}

void PMRenderModesDialog::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );
   cfg->writeEntry( "RenderModesDialogSize", s_size );
}

void PMRenderModesDialog::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   QSize defaultSize( 300, 200 );
   s_size = cfg->readSizeEntry( "RenderModesDialogSize", &defaultSize );
}

void PMRenderModesDialog::resizeEvent( QResizeEvent* ev )
{
   s_size = ev->size( );
}

void PMRenderModesDialog::slotAdd( )
{
   m_selectionIndex++;
   if( m_selectionIndex < 0 )
      m_selectionIndex = 0;
   m_workingModes.insert( ( uint ) m_selectionIndex, new PMRenderMode( ) );

   displayList( );
   slotChanged( );
}

void PMRenderModesDialog::slotRemove( )
{
   m_workingModes.remove( ( uint ) m_selectionIndex );

   int num = m_workingModes.count( );
   if( m_selectionIndex >= num )
      m_selectionIndex = num - 1;

   displayList( );
   slotChanged( );
}

void PMRenderModesDialog::slotUp( )
{
   PMRenderMode* mode = m_workingModes.take( ( uint ) m_selectionIndex );
   m_selectionIndex--;
   if( m_selectionIndex < 0 )
      m_selectionIndex = 0;
   m_workingModes.insert( m_selectionIndex, mode );

   displayList( );
   slotChanged( );
}

void PMRenderModesDialog::slotDown( )
{
   PMRenderMode* mode = m_workingModes.take( ( uint ) m_selectionIndex );
   m_selectionIndex++;

   int num = m_workingModes.count( );
   if( m_selectionIndex > num )
      m_selectionIndex = num;
   m_workingModes.insert( m_selectionIndex, mode );

   displayList( );
   slotChanged( );
}

void PMRenderModesDialog::slotEdit( )
{
    if (  m_selectionIndex==-1 )
        return;
   PMRenderModeDialog dlg( m_workingModes.at( m_selectionIndex ) );
   bool changed = ( dlg.exec( ) == QDialog::Accepted );
   if( changed )
   {
      slotChanged( );
      displayList( );
   }
}

void PMRenderModesDialog::slotOk( )
{
   m_pOriginalModes->setAutoDelete( true );
   m_pOriginalModes->clear( );
   m_pOriginalModes->setAutoDelete( false );
   *m_pOriginalModes = m_workingModes;
   m_pOriginalModes->at( m_selectionIndex );

   m_workingModes.setAutoDelete( false );
   m_workingModes.clear( );

   accept( );
}


QSize PMRenderModeDialog::s_size = QSize( 300, 200 );

const int numQuality = 9;
const char* qualityString[numQuality] =
{
   I18N_NOOP( "0, 1: Quick colors, full ambient lighting only" ),
   I18N_NOOP( "2, 3: Show specified diffuse and ambient light" ),
   I18N_NOOP( "4: Render shadows, but no extended lights" ),
   I18N_NOOP( "5: Render shadows, including extended lights" ),
   I18N_NOOP( "6, 7: Compute texture patterns" ),
   I18N_NOOP( "8: Compute reflected, refracted, and transmitted rays" ),
   I18N_NOOP( "9: Compute media" ),
   I18N_NOOP( "10: Compute radiosity but no media" ),
   I18N_NOOP( "11: Compute radiosity and media" )
};

const int c_qualityToIndex[12] = { 0, 0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8 };
const int c_indexToQuality[numQuality] = { 0, 2, 4, 5, 6, 8, 9, 10, 11 };

PMRenderModeDialog::PMRenderModeDialog( PMRenderMode* mode, QWidget* parent, const char* name )
      : KDialogBase( parent, name, true, i18n( "Render Modes" ),
                     Ok | Cancel, Ok )
{
   m_pMode = mode;
   int i;

   // main page
   QWidget* page = new QWidget( this );
   setMainWidget( page );
   QVBoxLayout* topLayout = new QVBoxLayout( page, 0, spacingHint( ) );

   QHBoxLayout* descrLayout = new QHBoxLayout( topLayout );
   QLabel* descrLabel = new QLabel( i18n( "Description:" ), page );
   descrLayout->addWidget( descrLabel );

   m_pDescriptionEdit = new QLineEdit( page );
   descrLayout->addWidget( m_pDescriptionEdit );

   m_pTabWidget = new QTabWidget( page );
   topLayout->addWidget( m_pTabWidget );

   QWidget* tab;
   QVBoxLayout* tabLayout;

   // size tab
   tab = new QWidget( );
   m_pTabWidget->addTab( tab, i18n( "Size" ) );
   tabLayout = new QVBoxLayout( tab, marginHint( ), spacingHint( ) );

   QHBoxLayout* sizeHelpLayout = new QHBoxLayout( tabLayout );
   QGridLayout* sizeLayout = new QGridLayout( sizeHelpLayout, 2, 2 );
   sizeLayout->addWidget( new QLabel( i18n( "Width:" ), tab ), 0, 0 );
   m_pWidthEdit = new PMIntEdit( tab );
   m_pWidthEdit->setValidation( true, 1, false, 0 );
   sizeLayout->addWidget( m_pWidthEdit, 0, 1 );
   sizeLayout->addWidget( new QLabel( i18n( "Height:" ), tab ), 1, 0 );
   m_pHeightEdit = new PMIntEdit( tab );
   m_pHeightEdit->setValidation( true, 1, false, 0 );
   sizeLayout->addWidget( m_pHeightEdit, 1, 1 );
   sizeHelpLayout->addStretch( 1 );

   m_pSubsectionBox = new QCheckBox( i18n( "Subsection" ), tab );
   tabLayout->addWidget( m_pSubsectionBox );

   QHBoxLayout* ssHelpLayout = new QHBoxLayout( tabLayout );
   QGridLayout* ssLayout = new QGridLayout( ssHelpLayout, 4, 2 );
   ssLayout->addWidget( new QLabel( i18n( "Start column:" ), tab ), 0, 0 );
   m_pStartColumnEdit = new PMFloatEdit( tab );
   m_pStartColumnEdit->setValidation( true, 0.0, false, 0.0 );
   ssLayout->addWidget( m_pStartColumnEdit, 0, 1 );
   ssLayout->addWidget( new QLabel( i18n( "End column:" ), tab ), 1, 0 );
   m_pEndColumnEdit = new PMFloatEdit( tab );
   m_pEndColumnEdit->setValidation( true, 0.0, false, 0.0 );
   ssLayout->addWidget( m_pEndColumnEdit, 1, 1 );
   ssLayout->addWidget( new QLabel( i18n( "Start row:" ), tab ), 2, 0 );
   m_pStartRowEdit = new PMFloatEdit( tab );
   m_pStartRowEdit->setValidation( true, 0.0, false, 0.0 );
   ssLayout->addWidget( m_pStartRowEdit, 2, 1 );
   ssLayout->addWidget( new QLabel( i18n( "End row:" ), tab ), 3, 0 );
   m_pEndRowEdit = new PMFloatEdit( tab );
   m_pEndRowEdit->setValidation( true, 0.0, false, 0.0 );
   ssLayout->addWidget( m_pEndRowEdit, 3, 1 );
   ssHelpLayout->addStretch( 1 );

   tabLayout->addStretch( 1 );

   // quality tab
   tab = new QWidget( );
   m_pTabWidget->addTab( tab, i18n( "Quality" ) );
   tabLayout = new QVBoxLayout( tab, marginHint( ), spacingHint( ) );

   QHBoxLayout* quHelpLayout = new QHBoxLayout( tabLayout );
   quHelpLayout->addWidget( new QLabel( i18n( "Quality:" ), tab ) );
   m_pQualityCombo = new QComboBox( tab );
   quHelpLayout->addWidget( m_pQualityCombo );
   for( i = 0; i < numQuality; i++ )
      m_pQualityCombo->insertItem( i18n( qualityString[i] ) );

   m_pAntialiasingBox = new QCheckBox( i18n( "Antialiasing" ), tab );
   tabLayout->addWidget( m_pAntialiasingBox );

   QHBoxLayout* aaHelpLayout = new QHBoxLayout( tabLayout );
   QGridLayout* aaGridLayout = new QGridLayout( aaHelpLayout, 5, 2 );
   aaGridLayout->addWidget( new QLabel( i18n( "Method:" ), tab ), 0, 0 );
   m_pSamplingCombo = new QComboBox( tab );
   aaGridLayout->addWidget( m_pSamplingCombo, 0, 1 );
   m_pSamplingCombo->insertItem( i18n( "Non Recursive" ) );
   m_pSamplingCombo->insertItem( i18n( "Recursive" ) );

   aaGridLayout->addWidget( new QLabel( i18n( "Threshold:" ), tab ), 1, 0 );
   m_pThresholdEdit = new PMFloatEdit( tab );
   aaGridLayout->addWidget( m_pThresholdEdit, 1, 1 );

   aaGridLayout->addWidget( new QLabel( i18n( "Depth:" ), tab ), 2, 0 );
   m_pAntialiasDepthEdit = new PMIntEdit( tab );
   m_pAntialiasDepthEdit->setValidation( true, 1, true, 9 );
   aaGridLayout->addWidget( m_pAntialiasDepthEdit, 2, 1 );

   m_pJitterBox = new QCheckBox( i18n( "Jitter" ), tab );
   aaGridLayout->addMultiCellWidget( m_pJitterBox, 3, 3, 0, 1 );

   aaGridLayout->addWidget( new QLabel( i18n( "Amount:" ), tab ), 4, 0 );
   m_pJitterAmountEdit = new PMFloatEdit( tab );
   aaGridLayout->addWidget( m_pJitterAmountEdit, 4, 1 );

   aaHelpLayout->addStretch( 1 );

   m_pRadiosityBox = new QCheckBox( i18n( "Radiosity" ), tab );
   tabLayout->addWidget( m_pRadiosityBox );

   tabLayout->addStretch( 1 );

   // output options tab
   tab = new QWidget( );
   m_pTabWidget->addTab( tab, i18n( "Output" ) );
   tabLayout = new QVBoxLayout( tab, marginHint( ), spacingHint( ) );

   m_pAlphaBox = new QCheckBox( i18n( "Alpha" ), tab );
   tabLayout->addWidget( m_pAlphaBox );

   tabLayout->addStretch( 1 );


   resize( s_size );

   // display the mode BEFORE the signals are connected!!!
   displayMode( );

   enableButtonOK( false );

   // connect signals
   connect( m_pDescriptionEdit, SIGNAL( textChanged( const QString& ) ), SLOT( slotTextChanged( const QString& ) ) );
   connect( m_pHeightEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pWidthEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pSubsectionBox, SIGNAL( toggled( bool ) ), SLOT( slotSubsectionToggled( bool ) ) );
   connect( m_pStartRowEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pEndRowEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pStartColumnEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pEndColumnEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pQualityCombo, SIGNAL( activated( int ) ), SLOT( slotActivated( int ) ) );
   connect( m_pRadiosityBox, SIGNAL( clicked( ) ), SLOT( slotChanged( ) ) );
   connect( m_pAntialiasingBox, SIGNAL( toggled( bool ) ), SLOT( slotAntialiasingToggled( bool ) ) );
   connect( m_pSamplingCombo, SIGNAL( activated( int ) ), SLOT( slotActivated( int ) ) );
   connect( m_pThresholdEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pJitterBox, SIGNAL( toggled( bool ) ), SLOT( slotJitterToggled( bool ) ) );
   connect( m_pJitterAmountEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pAntialiasDepthEdit, SIGNAL( dataChanged( ) ), SLOT( slotChanged( ) ) );
   connect( m_pAlphaBox, SIGNAL( toggled( bool ) ), SLOT( slotToggled( bool ) ) );
}

void PMRenderModeDialog::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );
   cfg->writeEntry( "RenderModeDialogSize", s_size );
}

void PMRenderModeDialog::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   QSize defaultSize( 400, 400 );
   s_size = cfg->readSizeEntry( "RenderModeDialogSize", &defaultSize );
}

void PMRenderModeDialog::resizeEvent( QResizeEvent* ev )
{
   s_size = ev->size( );
}

bool PMRenderModeDialog::saveChanges( )
{
   if( validate( ) )
   {
      m_pMode->setDescription( m_pDescriptionEdit->text( ) );
      m_pMode->setWidth( m_pWidthEdit->value( ) );
      m_pMode->setHeight( m_pHeightEdit->value( ) );
      m_pMode->setSubSection( m_pSubsectionBox->isChecked( ) );
      if( m_pSubsectionBox->isChecked( ) )
      {
         m_pMode->setStartRow( m_pStartRowEdit->value( ) );
         m_pMode->setEndRow( m_pEndRowEdit->value( ) );
         m_pMode->setStartColumn( m_pStartColumnEdit->value( ) );
         m_pMode->setEndColumn( m_pEndColumnEdit->value( ) );
      }
      m_pMode->setQuality( indexToQuality( m_pQualityCombo->currentItem( ) ) );
      m_pMode->setRadiosity( m_pRadiosityBox->isChecked( ) );
      m_pMode->setAntialiasing( m_pAntialiasingBox->isChecked( ) );
      if( m_pAntialiasingBox->isChecked( ) )
      {
         m_pMode->setSamplingMethod( m_pSamplingCombo->currentItem( ) );
         m_pMode->setAntialiasingThreshold( m_pThresholdEdit->value( ) );
         m_pMode->setAntialiasingJitter( m_pJitterBox->isChecked( ) );
         if( m_pJitterBox->isChecked( ) )
            m_pMode->setAntialiasingJitterAmount( m_pJitterAmountEdit->value( ) );
         m_pMode->setAntialiasingDepth( m_pAntialiasDepthEdit->value( ) );
      }
      m_pMode->setAlpha( m_pAlphaBox->isChecked( ) );
      return true;
   }
   return false;
}

bool PMRenderModeDialog::validate( )
{
   if( m_pDescriptionEdit->text( ).isEmpty( ) )
   {
      KMessageBox::error( this, i18n( "Please enter a description for the "
                                      "render mode." ), i18n( "Error" ) );
      m_pDescriptionEdit->selectAll( );
      return false;
   }

   // tab 0
   bool error = true;

   if( m_pHeightEdit->isDataValid( ) )
      if( m_pWidthEdit->isDataValid( ) )
         error = false;
   if( !error && m_pSubsectionBox->isChecked( ) )
   {
      error = true;
      if( m_pStartColumnEdit->isDataValid( ) )
         if( m_pEndColumnEdit->isDataValid( ) )
            if( m_pStartRowEdit->isDataValid( ) )
               if( m_pEndRowEdit->isDataValid( ) )
                  error = false;
   }

   if( error )
   {
      m_pTabWidget->setCurrentPage( 0 );
      return false;
   }

   // tab 1
   if( m_pAntialiasingBox->isChecked( ) )
   {
      error = false;
      if( m_pThresholdEdit->isDataValid( ) )
         if( m_pAntialiasDepthEdit->isDataValid( ) )
            error = false;

      if( m_pJitterBox->isChecked( ) && !error )
         error = !m_pJitterAmountEdit->isDataValid( );

      if( error )
      {
         m_pTabWidget->setCurrentPage( 1 );
         return false;
      }
   }

   // tab 2

   return true;
}

void PMRenderModeDialog::displayMode( )
{
   m_pDescriptionEdit->setText( m_pMode->description( ) );
   m_pHeightEdit->setValue( m_pMode->height( ) );
   m_pWidthEdit->setValue( m_pMode->width( ) );
   m_pSubsectionBox->setChecked( m_pMode->subSection( ) );
   enableSubsection( m_pMode->subSection( ) );
   m_pStartRowEdit->setValue( m_pMode->startRow( ) );
   m_pEndRowEdit->setValue( m_pMode->endRow( ) );
   m_pStartColumnEdit->setValue( m_pMode->startColumn( ) );
   m_pEndColumnEdit->setValue( m_pMode->endColumn( ) );
   m_pQualityCombo->setCurrentItem( qualityToIndex( m_pMode->quality( ) ) );
   m_pRadiosityBox->setChecked( m_pMode->radiosity( ) );
   m_pAntialiasingBox->setChecked( m_pMode->antialiasing( ) );
   enableAntialiasing( m_pMode->antialiasing( ) );
   m_pSamplingCombo->setCurrentItem( m_pMode->samplingMethod( ) );
   m_pThresholdEdit->setValue( m_pMode->antialiasingThreshold( ) );
   m_pJitterBox->setChecked( m_pMode->antialiasingJitter( ) );
   enableJitter( m_pMode->antialiasingJitter( ) && m_pMode->antialiasing( ) );
   m_pJitterAmountEdit->setValue( m_pMode->antialiasingJitterAmount( ) );
   m_pAntialiasDepthEdit->setValue( m_pMode->antialiasingDepth( ) );
   m_pAlphaBox->setChecked( m_pMode->alpha( ) );
}

void PMRenderModeDialog::enableSubsection( bool yes )
{
   m_pStartRowEdit->setEnabled( yes );
   m_pEndRowEdit->setEnabled( yes );
   m_pStartColumnEdit->setEnabled( yes );
   m_pEndColumnEdit->setEnabled( yes );
}

void PMRenderModeDialog::enableAntialiasing( bool yes )
{
   m_pSamplingCombo->setEnabled( yes );
   m_pThresholdEdit->setEnabled( yes );
   m_pAntialiasDepthEdit->setEnabled( yes );
   m_pJitterBox->setEnabled( yes );
   enableJitter( m_pJitterBox->isChecked( ) );
}

void PMRenderModeDialog::enableJitter( bool yes )
{
   m_pJitterAmountEdit->setEnabled( yes );
}

int PMRenderModeDialog::qualityToIndex( int quality )
{
   if( quality < 0 )
      quality = 0;
   if( quality > 11 )
      quality = 11;

   return c_qualityToIndex[quality];
}

int PMRenderModeDialog::indexToQuality( int index )
{
   if( index < 0 )
      index = 0;
   if( index >= numQuality )
      index = numQuality - 1;

   return c_indexToQuality[index];
}

void PMRenderModeDialog::slotOk( )
{
   if( saveChanges( ) )
      accept( );
}

void PMRenderModeDialog::slotChanged( )
{
   enableButtonOK( true );
}

void PMRenderModeDialog::slotTextChanged( const QString& )
{
   slotChanged( );
}

void PMRenderModeDialog::slotActivated( int )
{
   slotChanged( );
}

void PMRenderModeDialog::slotSubsectionToggled( bool on )
{
   slotChanged( );
   enableSubsection( on );
}

void PMRenderModeDialog::slotAntialiasingToggled( bool on )
{
   slotChanged( );
   enableAntialiasing( on );
}

void PMRenderModeDialog::slotJitterToggled( bool on )
{
   slotChanged( );
   enableJitter( on );
}

void PMRenderModeDialog::slotToggled( bool )
{
   slotChanged( );
}

#include "pmrendermodesdialog.moc"

