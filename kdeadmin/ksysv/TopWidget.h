/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-99 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KSV_TOPWIDGET_H
#define KSV_TOPWIDGET_H

#include <kmainwindow.h>

class QCheckBox;

class KAction;
class KToggleAction;
class KActionMenu;
class KMenuBar;
class KStatusBar;
class KToolBar;
class KActionCollection;

class ActionList;
class KSVAction;
class ServiceDlg;
class KSVConfig;
class KSVItem;
class KSVContent;
class KSVPreferences;
class RunlevelAuthIcon;

class KSVTopLevel : public KMainWindow
{
  Q_OBJECT

public:
  KSVTopLevel();
  virtual ~KSVTopLevel();

public slots:
  void setCaption (bool changed);

protected:
  virtual bool queryExit();
  virtual void closeEvent( QCloseEvent* e );
  void initTools();
  void initActions ();
  void initStatusBar();

  KActionCollection* filteredActions ();

private slots:
  void slotClearChanges();
  void slotAcceptChanges();
  void pushUndoAction (KSVAction*);
  void slotShowConfig();
  void slotReadConfig();
  void pasteAppend ();
  void toggleLog();
  void saveOptions();
  void slotUpdateRunning (const QString&);
  void editCut();
  void editCopy();
  void editPaste();
  void editUndo();
  void editRedo();
  void editService ();
  void saveAs ();
  void load ();
  void print();
  void printLog();
  void setChanged( bool val = true );
  void setPaste( bool val = false );
  void properties();
  void scriptProperties();
  void setLog( bool val );
  void catchCannotGenerateNumber();

  void configureKeys ();
  void configureToolbars ();

  void enableUndo();
  void disableUndo();
  void enableRedo();
  void disableRedo();

  void dispatchEditService (bool);
  void dispatchStartService (bool);
  void dispatchStopService (bool);
  void dispatchRestartService (bool);

  void dispatchEdit ();

  /**
   * initialize the view (plus some other things)
   */
  void initView();

  void slotSaveLog();
  void enableLogActions();

  void toggleRunlevel (int index);

  void updateColors ();
  void updateServicesPath ();
  void updateRunlevelsPath ();

  void writingEnabled (bool);
  void slotNewToolbarConfig();

private:
  KSVConfig* mConfig;
  KSVContent* mView;

  KSVPreferences* mPreferences;

  KAction *mEditUndo, *mEditRedo, *mEditCut, *mEditCopy, *mEditPaste,
    *mPasteAppend, *mEditProperties;

  KAction *mFileRevert, *mFileLoad, *mFilePrint, *mFilePrintLog,
    *mFileSave, *mFileSaveAs, *mFileSaveLog, *mFileQuit;

  KToggleAction *mToolsStartService, *mToolsStopService,
    *mToolsRestartService, *mToolsEditService;

  KToggleAction *mOptionsToggleLog;

  KActionMenu* mOpenWith;
  KAction* mOpenDefault;

  bool mChanged;

  ActionList* mUndoList;
  ActionList* mRedoList;

  ServiceDlg* mStartDlg;
  ServiceDlg* mStopDlg;
  ServiceDlg* mRestartDlg;
  ServiceDlg* mEditDlg;

  RunlevelAuthIcon* mAuth;
  QCheckBox** mVisible;
};

#endif // KSV_TOPWIDGET_H

