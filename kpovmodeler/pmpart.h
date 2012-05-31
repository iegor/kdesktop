//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2002 by Andreas Zehender
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


#ifndef PMPART_H
#define PMPART_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <kaccel.h>
#include <kurl.h>
#include <kparts/part.h>
#include <kparts/browserextension.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qvaluelist.h>
#include <kglobal.h>
#include <klocale.h>

#include "pmobject.h"
#include "pmcommandmanager.h"
#include "pmpartiface.h"

class PMView;
class PMShell;
class PMObjectDrag;
class PMScene;
class PMCamera;
class PMPovrayWidget;
class PMParser;

class PMBrowserExtension;
class PMSymbolTable;
class PMDeclare;

class PMPrototypeManager;
class PMInsertRuleSystem;
class PMIOManager;

class QMimeSource;
class KAction;
class KSelectAction;
class PMComboAction;
class PMSpinBoxAction;
class PMLabelAction;
class KProcess;

/**
 * The part for kpovmodeler (povray document)
 */
class PMPart : public KParts::ReadWritePart, virtual public PMPartIface
{
   Q_OBJECT
public:
   /**
    * construtor of PMPart, calls all init functions to create the
    * application.
    */
   PMPart( QWidget* parentWidget, const char* widgetName,
           QObject* parent, const char* name, bool readWrite,
           PMShell* shell = 0 );

   /**
    * construtor of PMPart, calls all init functions to create the
    * application. It does not create the main widget.
    */
   PMPart( QWidget* parentWidget, const char* widgetName,
           QObject* parent, const char* name, bool readWrite,
           bool onlyCutPaste, PMShell* shell = 0 );

   /**
    * Destructor
    */
   ~PMPart( );
   /**
    * deletes the document's contents
    */
   void deleteContents( );
   /**
    * initializes the document generally
    */
   bool newDocument( );
   /**
    * closes the actual document
    */
   void closeDocument( );
   /**
    * Exports the scene as povray code
    */
   bool exportPovray( const KURL& url );

   /**
    * returns the toplevel object
    */
   PMScene* scene( ) const { return m_pScene; }
   /**
    * Returns a pointer to the shell if there is one
    */
   PMShell* shell( ) const { return m_pShell; }
   /**
    * saves settings
    */
   void saveConfig( KConfig* cfg );
   /**
    * loads settings
    */
   void restoreConfig( KConfig* cfg );

   /**
    * Updates the sorted list of selected objects if necessary and
    * returns a reference to the list.
    */
   const PMObjectList& selectedObjects( );

   /**
    * Executes the command cmd.
    *
    * All changes to the document have to be made
    * with @ref PMCommand objects for undo/redo. Each slot that changes the
    * document creates a PMCommand object and calls this function.
    *
    * If this function returns false, the command was not executed
    * and the command object was deleted.
    */
   bool executeCommand( PMCommand* cmd );
   /**
    * Checks, where the objects in list can be inserted, as child
    * or as sibling of obj and asks the user if there is more than one
    * possibility
    *
    * Returns PMIFirstChild, PMILastChild, PMISibling or 0, if the objects
    * should not be inserted.*/
   int whereToInsert( PMObject* obj, const PMObjectList& list );
   /**
    * Checks, where the objects in list can be inserted, as child
    * or as sibling of obj and asks the user if there is more than one
    * possibility
    *
    * Returns PMIFirstChild, PMILastChild, PMISibling or 0, if the objects
    * should not be inserted.*/
   int whereToInsert( PMObject* obj, const QStringList& list );
   /**
    * Asks the user, where to insert new objects.
    *
    * Returns PMIFirstChild, PMILastChild, PMISibling or 0, if the objects
    * should not be inserted.*/
   int whereToInsert( PMObject* obj );

   /**
    * Returns the symbol table of this document
    */
   PMSymbolTable* symbolTable( ) const { return m_pSymbolTable; }
   /**
    * Returns the first camera
    */
   PMCamera* firstCamera( );
   /**
    * Returns an iterator to the list of cameras
    */
   QPtrListIterator<PMCamera> cameras( );
   /**
    * The active object
    */
   PMObject* activeObject( ) const { return m_pActiveObject; }
   /**
    * The active object's name
    */
   virtual QString activeObjectName( );
   /**
    * Set the active object
    */
   virtual bool setActiveObject( const QString& name );
   /**
    * Get the valid properties of the currently active object
    */
   virtual QStringList getProperties( );
   /**
    * set a property on the active object
    */
   virtual bool setProperty( const QString& name, const PMVariant& value );
   /**
    * set a property on the active object
    */
   virtual bool setProperty( const QString& name, const QString& value );
   /**
    * get a property on the active object
    */
   virtual const PMVariant getProperty( const QString& name );
   /**
    * get a property on the active object
    */
   virtual const QString getPropertyStr( const QString& name );
   /**
    * List of control points of the active object
    */
   PMControlPointList activeControlPoints( ) const { return m_controlPoints; }

   /**
    * Called when the user moves some objects in the tree view.
    * If obj is 0, the user moved the objects to another document (remove)
    *
    * Returns true if successful.
    */
   bool dragMoveSelectionTo( PMObject* obj );
   /**
    * Called when the user drags some objects onto the tree view
    *
    * Returns true if successful.
    */
   bool drop( PMObject* obj, QMimeSource* e );
   /**
    * Tries to parse the data and insert the parsed objects
    * as child or siblings of object obj
    *
    * Type is the actions description for the undo/redo menu.
    */
   bool insertFromParser( const QString& type, PMParser* parser, PMObject* obj );

   /**
    * Returns a pointer to the prototype manager
    */
   PMPrototypeManager* prototypeManager( ) const { return m_pPrototypeManager; }
   /**
    * Returns a pointer to the insert rules system
    */
   PMInsertRuleSystem* insertRuleSystem( ) const { return m_pInsertRuleSystem; }
   /**
    * Returns a pointer to the IO formats manager for this part
    */
   PMIOManager* ioManager( ) const { return m_pIOManager; }

public slots:
   /**
    * Opens the import file dialog
    */
   void slotFileImport( );
   /**
    * Opens the export file dialog
    */
   void slotFileExport( );

   /**
    * puts the marked text/object into the clipboard and removes the objects
    */
   void slotEditCut( );
   /**
    * removes the selected object
    */
   void slotEditDelete( );
   /**
    * puts the marked text/object into the clipboard
    */
   void slotEditCopy( );
   /**
    * paste the clipboard into the document
    */
   void slotEditPaste( );
   /**
    * undoes the last change
    */
   void slotEditUndo( );
   /**
    * redoes the last undone command
    */
   void slotEditRedo( );

   /**
    * Called when an object is changed.
    * Mode is a bit combination of @ref PMChange constants
    */
   void slotObjectChanged( PMObject* obj, const int mode, QObject* sender );
   /**
    * Called when an id is changed
    */
   void slotIDChanged( PMObject* obj, const QString& oldID );

   /**
    * Inserts a new PMObject of type type
    */
   void slotNewObject( const QString& type );
   /**
    * Inserts a new PMObject of type type at position pos.
    * pos can be one of:
    *     FirstChild
    *     LastChild
    *     Sibling
    */
   void slotNewObject( const QString& type, const QString& pos );
   /**
    * Inserts the new PMObject. The object will be deleted if it can't be
    * inserted or the user aborts the action
    */
   void slotNewObject( PMObject* newObject, int insertAs = 0 );

   /**
    * List the known object types
    */
   virtual QStringList getObjectTypes( );

   /**
    * Adds transformations to the object and calls slotNewObject
    * for it
    */
   void slotNewTransformedObject( PMObject* o );

   /**
    * Inserts a new PMGlobalSettings
    */
   void slotNewGlobalSettings( );
   /**
    * Inserts a new PMSkySphere
    */
   void slotNewSkySphere( );
   /**
    * Inserts a new PMRainbow
    */
   void slotNewRainbow( );
   /**
    * Inserts a new PMFog
    */
   void slotNewFog( );
   /**
    * Inserts a new PMInterior
    */
   void slotNewInterior( );
   /**
    * Inserts a new PMMedia
    */
   void slotNewMedia( );
   /**
    * Inserts a new PMDensity
    */
   void slotNewDensity( );
   /**
    * Inserts a new PMMaterial
    */
   void slotNewMaterial( );
   /**
    * Inserts a new PMBox
    */
   void slotNewBox( );
   /**
    * Inserts a new PMSphere
    */
   void slotNewSphere( );
   /**
    * Inserts a new PMCylinder
    */
   void slotNewCylinder( );
   /**
    * Inserts a new PMCone
    */
   void slotNewCone( );
   /**
    * Inserts a new PMTorus
    */
   void slotNewTorus( );
   /**
    * Inserts a new PMLathe
    */
   void slotNewLathe( );
   /**
    * Inserts a new PMPrism
    */
   void slotNewPrism( );
   /**
    * Inserts a new PMSurfaceOfRevolution
    */
   void slotNewSurfaceOfRevolution( );
   /**
    * Inserts a new PMSuperquadricEllipsoid
    */
   void slotNewSuperquadricEllipsoid( );
   /**
    * Inserts a new PMJuliaFractal
    */
   void slotNewJuliaFractal( );
   /**
    * Inserts a new PMHeightField
    */
   void slotNewHeightField( );
   /**
    * Inserts a new text object
    */
   void slotNewText( );

   /**
    * Inserts a new PMBlob
    */
   void slotNewBlob( );
   /**
    * Inserts a new PMBlobSphere
    */
   void slotNewBlobSphere( );
   /**
    * Inserts a new PMBlobCylinder
    */
   void slotNewBlobCylinder( );

   /**
    * Inserts a new PMPlane
    */
   void slotNewPlane( );
   /**
    * Inserts a new PMPolynom
    */
   void slotNewPolynom( );
   /**
    * Inserts a new PMDeclare
    */
   void slotNewDeclare( );
   /**
    * Inserts a new PMObjectLink
    */
   void slotNewObjectLink( );

   /**
    * Inserts a new PMCSG ( union )
    */
   void slotNewUnion( );
   /**
    * Inserts a new PMCSG ( intersection )
    */
   void slotNewIntersection( );
   /**
    * Inserts a new PMCSG ( difference )
    */
   void slotNewDifference( );
   /**
    * Inserts a new PMCSG ( merge )
    */
   void slotNewMerge( );

   /**
    * Inserts a new PMDisc
    */
   void slotNewDisc( );
   /**
    * Inserts a new PMBicubicPatch
    */
   void slotNewBicubicPatch( );
   /**
    * Inserts a new PMTriangle
    */
   void slotNewTriangle( );

   /**
    * Inserts a new PMBoundedBy
    */
   void slotNewBoundedBy( );
   /**
    * Inserts a new PMClippedBy
    */
   void slotNewClippedBy( );

   /**
    * Inserts a new PMLight object
    */
   void slotNewLight( );
   /**
    * Inserts a new PMLooksLike object
    */
   void slotNewLooksLike( );
   /**
    * Inserts a new PMProjectedThrough object
    */
   void slotNewProjectedThrough( );

   /**
    * Inserts a new PMCamera
    */
   void slotNewCamera( );

   /**
    * Inserts a new PMTexture
    */
   void slotNewTexture( );
   /**
    * Inserts a new PMPigment
    */
   void slotNewPigment( );
   /**
    * Inserts a new PMNormal
    */
   void slotNewNormal( );
   /**
    * Inserts a new PMSolidColor
    */
   void slotNewSolidColor( );
   /**
    * Inserts a new PMTextureList
    */
   void slotNewTextureList( );
   /**
    * Inserts a new PMColorList
    */
   void slotNewColorList( );
   /**
    * Inserts a new PMPigmentList
    */
   void slotNewPigmentList( );
   /**
    * Inserts a new PMNormalList
    */
   void slotNewNormalList( );
   /**
    * Inserts a new PMDensityList
    */
   void slotNewDensityList( );
   /**
    * Inserts a new PMFinish
    */
   void slotNewFinish( );
   /**
    * Inserts a new PMPattern
    */
   void slotNewPattern( );
   /**
    * Inserts a new PMBlendMapModifiers
    */
   void slotNewBlendMapModifiers( );
   /**
    * Inserts a new PMTextureMap
    */
   void slotNewTextureMap( );
   /**
    * Inserts a new PMMaterialMap
    */
   void slotNewMaterialMap( );
   /**
    * Inserts a new PMColorMap
    */
   void slotNewColorMap( );
   /**
    * Inserts a new PMPigmentMap
    */
   void slotNewPigmentMap( );
   /**
    * Inserts a new PMNormalMap
    */
   void slotNewNormalMap( );
   /**
    * Inserts a new PMBumpMap
    */
   void slotNewBumpMap( );
   /**
    * Inserts a new PMSlopeMap
    */
   void slotNewSlopeMap( );
   /**
    * Inserts a new PMDensityMap
    */
   void slotNewDensityMap( );
   /**
    * Inserts a new PMSlope
    */
   void slotNewSlope( );
   /**
    * Inserts a new PMWarp
    */
   void slotNewWarp( );
   /**
    * Inserts a new PMImageMap
    */
   void slotNewImageMap( );
   /**
    * Inserts a new PMQuickColor
    */
   void slotNewQuickColor( );

   /**
    * Inserts a new PMTranslate
    */
   void slotNewTranslate( );
   /**
    * Inserts a new PMRotate
    */
   void slotNewRotate( );
   /**
    * Inserts a new PMScale
    */
   void slotNewScale( );
   /**
    * Inserts a new PMPovrayMatrix
    */
   void slotNewMatrix( );
   /**
    * Inserts a new PMComment
    */
   void slotNewComment( );
   /**
    * Inserts a new PMRaw
    */
   void slotNewRaw( );

   // POV-Ray 3.5 objects
   /**
    * Inserts a new PMIsoSurfate
    */
   void slotNewIsoSurface( );
   /**
    * Inserts a new PMRadiosity
    */
   void slotNewRadiosity( );
   /**
    * Inserts a new PMGlobalPhotons
    */
   void slotNewGlobalPhotons( );
   /**
    * Inserts a new PMPhotons
    */
   void slotNewPhotons( );
   /**
    * Inserts a new PMLightGroup
    */
   void slotNewLightGroup( );
   /**
    * Inserts a new PMInteriorTexture
    */
   void slotNewInteriorTexture( );
   /**
    * Inserts a new PMSphereSweep
    */
   void slotNewSphereSweep( );
   /**
    * Inserts a new PMMesh
    */
   void slotNewMesh( );


   /**
    * Called when the clipboard contents changed
    */
   void slotClipboardDataChanged( );
   /**
    * updates the undo/redo menu items
    */
   void slotUpdateUndoRedo( const QString& undo, const QString& redo );

   /**
    * Starts rendering with povray
    */
   virtual void slotRender( );
   /**
    * Opens the render settings
    */
   void slotRenderSettings( );
   /**
    * Called when a render mode is selected
    */
   void slotRenderMode( int index );
   /**
    * Called when the render modes combo action is plugged into the toolbar
    */
   void slotRenderModeActionPlugged( );
   /**
    * Shows the render window
    */
   void slotViewRenderWindow( );
   /**
    * Called when the visibility level has changed
    */
   void slotVisibilityLevelChanged( int );
   /**
    * Called when the visibility level action is plugged into the toolbar
    */
   void slotVisibilityActionPlugged( );
   /**
    * Called when the global detail level has changed
    */
   void slotGlobalDetailLevelChanged( int level );

   /**
    * Opens the search library object dialog
    */
   void slotSearchLibraryObject( );

   /** */
   virtual void setModified( );
   /** */
   virtual void setModified( bool modified );

   /** Set the scene object. Must be used with extreme care. */
   void setScene( PMScene* scene );

	/**
	 * Emits the aboutToSave signal
	 */
	void slotAboutToSave( );

signals:
   /**
    * Signal that is emitted when an object is changed.
    * Mode is a bit combination of @ref PMChange constants.
    */
   void objectChanged( PMObject* obj, const int mode, QObject* sender );
   /**
    * Signal that is emitted when the views have to be refreshed.
    * Usually on newDocument or openDocument
    */
   void refresh( );
   /**
    * Emitted when all views should delete all data
    */
   void clear( );
   /**
    * Emitted, when the modified flag changes
    */
   void modified( );
   /**
    * Emitted when the mouse is over a control point
    */
   void controlPointMessage( const QString& msg );
   /**
    * Emitted when the active render mode has changed
    */
   void activeRenderModeChanged( );
   /**
    * Emitted before the scene is rendered.
    *
    * Views should ask the user to save pending changes.
    */
   void aboutToRender( );
   /**
    * Emitted before the scene is saved or exported
    *
    * Views should ask the user to save pending changes.
    */
   void aboutToSave( );

protected:
   /**
    * reimplemented from @ref KParts::ReadOnlyPart
    */
   virtual bool openFile( );
   /**
    * reimplemented from @ref KParts::ReadOnlyPart
    */
   virtual bool saveFile( );
   /**
    * Inits all actions
    */
   void initActions( );
   /**
    * Inits only actions related to copy&paste.
    * Required by the library browser.
    */
   void initCopyPasteActions( );

   /**
    * creates the widget of the part instance and sets
    * it as the view
    */
   void initView( QWidget* parent, const char* name );
   /**
    * initializes the documents contents
    */
   void initDocument( );
   /**
    * clears the selection
    */
   void clearSelection( );

private:
   /**
    * Disables all actions, that modify the part
    */
   void disableReadWriteActions( );
   /**
    * Updates all "new object" actions
    */
   void updateNewObjectActions( );
   /**
    * Finds a free id of the format <prefix><number>.
    *
    * Adds the object to the symbol table.
    *
    * Returns the number.
    */
   unsigned int findNewID( const QString& prefix, unsigned int firstNumber,
                           PMDeclare* obj );
   /**
    * Updates the list of cameras
    */
   void updateCameraList( );

   /**
    * Generic drop/paste function
    */
   bool pasteOrDrop( const QString& type, QMimeSource* mime, PMObject* obj );
   /**
    * Generic cut/delete/remove function
    */
   bool removeSelection( const QString& type );
   /**
    * Updates the render mode combo action
    */
   void updateRenderModes( );
   /**
    * Updates the visibility level action
    */
   void updateVisibilityLevel( );
   /**
    * Updates the control point list
    */
   void updateControlPoints( PMObject* oldActive );

   PMView* m_pView;
   PMShell* m_pShell;
   PMBrowserExtension* m_pExtension;

   /**
    * the selected objects, unsorted!
    */
   PMObjectList m_selectedObjects;
   /**
    * the selected objects, sorted. This list is only created if necessary.
    */
   PMObjectList m_sortedSelectedObjects;
   /**
    * true if the list m_sortedSelectedObjects is up to date
    */
   bool m_sortedListUpToDate;
   /**
    * the active object
    */
   PMObject* m_pActiveObject;
   /**
    * the new selection after a command was executed
    */
   PMObject* m_pNewSelection;
   /**
    * List of all cameras
    */
   QPtrList<PMCamera> m_cameras;
   /**
    * true if the m_cameras list is up to date
    */
   bool m_bCameraListUpToDate;
   /**
    * true if the clipboard data can be decoded
    */
   bool m_canDecode;
   /**
    * Commands stack for undo and redo
    */
   PMCommandManager m_commandManager;
   /**
    * The povray scene, top level object
    */
   PMScene* m_pScene;
   /**
    * Number of added objects during the last executed command
    */
   unsigned int m_numAddedObjects;
   /**
    * Number of insert errors during the last executed command
    */
   unsigned int m_numInsertErrors;
   /**
    * Details of insert errors
    */
   QStringList m_insertErrorDetails;
   /**
    * The symbol table for this document
    */
   PMSymbolTable* m_pSymbolTable;
   /**
    * The povray render window
    */
   PMPovrayWidget* m_pPovrayWidget;
   /**
    * true if the new object actions have to be updated
    */
   bool m_updateNewObjectActions;
   /**
    * Control points of the active object
    */
   PMControlPointList m_controlPoints;
   /**
    * true if only copy'n'paste actions are available
    */
   bool m_onlyCopyPaste;

   // the actions
   KAction* m_pImportAction;
   KAction* m_pExportAction;

   KAction* m_pCutAction;
   KAction* m_pCopyAction;
   KAction* m_pPasteAction;
   KAction* m_pUndoAction;
   KAction* m_pRedoAction;
   KAction* m_pDeleteAction;

   KAction* m_pNewGlobalSettingsAction;
   KAction* m_pNewSkySphereAction;
   KAction* m_pNewRainbowAction;
   KAction* m_pNewFogAction;
   KAction* m_pNewInteriorAction;
   KAction* m_pNewMediaAction;
   KAction* m_pNewDensityAction;
   KAction* m_pNewMaterialAction;
   KAction* m_pNewBoxAction;
   KAction* m_pNewSphereAction;
   KAction* m_pNewCylinderAction;
   KAction* m_pNewConeAction;
   KAction* m_pNewTorusAction;
   KAction* m_pNewLatheAction;
   KAction* m_pNewPrismAction;
   KAction* m_pNewSurfaceOfRevolutionAction;
   KAction* m_pNewSuperquadricEllipsoidAction;
   KAction* m_pNewJuliaFractalAction;
   KAction* m_pNewHeightFieldAction;
   KAction* m_pNewTextAction;

   KAction* m_pNewBlobAction;
   KAction* m_pNewBlobSphereAction;
   KAction* m_pNewBlobCylinderAction;

   KAction* m_pNewPlaneAction;
   KAction* m_pNewPolynomAction;

   KAction* m_pNewDeclareAction;
   KAction* m_pNewObjectLinkAction;

   KAction* m_pNewUnionAction;
   KAction* m_pNewDifferenceAction;
   KAction* m_pNewIntersectionAction;
   KAction* m_pNewMergeAction;

   KAction* m_pNewBoundedByAction;
   KAction* m_pNewClippedByAction;

   KAction* m_pNewBicubicPatchAction;
   KAction* m_pNewDiscAction;
   KAction* m_pNewTriangleAction;

   KAction* m_pNewLightAction;
   KAction* m_pNewLooksLikeAction;
   KAction* m_pNewProjectedThroughAction;

   KAction* m_pNewCameraAction;

   KAction* m_pNewTextureAction;
   KAction* m_pNewPigmentAction;
   KAction* m_pNewNormalAction;
   KAction* m_pNewSolidColorAction;
   KAction* m_pNewTextureListAction;
   KAction* m_pNewColorListAction;
   KAction* m_pNewPigmentListAction;
   KAction* m_pNewNormalListAction;
   KAction* m_pNewDensityListAction;
   KAction* m_pNewFinishAction;
   KAction* m_pNewPatternAction;
   KAction* m_pNewBlendMapModifiersAction;
   KAction* m_pNewTextureMapAction;
   KAction* m_pNewMaterialMapAction;
   KAction* m_pNewPigmentMapAction;
   KAction* m_pNewColorMapAction;
   KAction* m_pNewNormalMapAction;
   KAction* m_pNewBumpMapAction;
   KAction* m_pNewSlopeMapAction;
   KAction* m_pNewDensityMapAction;
   KAction* m_pNewSlopeAction;
   KAction* m_pNewWarpAction;
   KAction* m_pNewImageMapAction;
   KAction* m_pNewQuickColorAction;

   KAction* m_pNewTranslateAction;
   KAction* m_pNewScaleAction;
   KAction* m_pNewRotateAction;
   KAction* m_pNewMatrixAction;

   KAction* m_pNewCommentAction;
   KAction* m_pNewRawAction;

   // POV-Ray 3.5 objects
   KAction* m_pNewIsoSurfaceAction;
   KAction* m_pNewRadiosityAction;
   KAction* m_pNewGlobalPhotonsAction;
   KAction* m_pNewPhotonsAction;
   KAction* m_pNewLightGroupAction;
   KAction* m_pNewInteriorTextureAction;
   KAction* m_pNewSphereSweepAction;
   KAction* m_pNewMeshAction;

   KAction* m_pSearchLibraryObjectAction;

   PMComboAction* m_pRenderComboAction;
   KAction* m_pRenderSettingsAction;
   KAction* m_pViewRenderWindowAction;
   KAction* m_pRenderAction;
   PMSpinBoxAction* m_pVisibilityLevelAction;
   PMLabelAction* m_pVisibilityLabelAction;
   KSelectAction* m_pGlobalDetailAction;
   PMLabelAction* m_pGlobalDetailLabelAction;

   QPtrList<KAction> m_readWriteActions;

   PMPrototypeManager* m_pPrototypeManager;
   PMInsertRuleSystem* m_pInsertRuleSystem;
   PMIOManager* m_pIOManager;
};

class PMBrowserExtension : public KParts::BrowserExtension
{
   Q_OBJECT
   friend class PMPart;
public:
   PMBrowserExtension( PMPart* parent )
      : KParts::BrowserExtension( parent, "PMBrowserExtension" )
   {
       KGlobal::locale()->insertCatalogue("kpovmodeler");
   }

   virtual ~PMBrowserExtension( )
   {
   }
};

#endif
