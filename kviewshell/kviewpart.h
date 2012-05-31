// -*- C++ -*-
#ifndef KVIEWPART_H
#define KVIEWPART_H

#include "kviewpart_iface.h"
#include "pageSize.h"
#include "zoom.h"

#include <kparts/browserextension.h>
#include <qtimer.h>

class KAboutData;
class KAboutDialog;
class KAccel;
class KAction;
class KConfig;
class KDirWatch;
class KInstance;
class KMultiPage;
class KRadioAction;
class KSelectAction;
class KTempFile;
class KToggleAction;
class KURL;
class KViewPartExtension;
class QHBoxLayout;
class pageSizeDialog;
class QSize;


class KViewPart : public KViewPart_Iface
{
  Q_OBJECT

public:
  KViewPart(QWidget *parentWidget, const char *widgetName, QObject *parent,
            const char *name, const QStringList& args);
  virtual ~KViewPart();

  static KAboutData* createAboutData();

  bool isValid() { return multiPage; }

  /* This method calls closeURL(), but asks first ("The document was
     modified. Do you really want to close?") if the document has been
     modified after it has been loaded.  */
  virtual bool closeURL_ask();

  /* Returns a description of the current page size, for use in the
     statusbar of the kviewshell that embeds this KViewPart. */
  QString pageSizeDescription();

  /** Returns a list of mimetypes supported by the installed KViewShell plugins. */
  virtual QStringList supportedMimeTypes();

signals:
  void zoomChanged(const QString &);
  void pageChanged(const QString &);
  void sizeChanged(const QString &);
  void scrollbarStatusChanged(bool);
  void fileOpened();

  void pluginChanged(KParts::Part*);

public slots:
  void slotSetFullPage(bool fullpage);

  virtual void slotFileOpen();
  virtual bool closeURL();
  virtual QStringList fileFormats() const;
  void setStatusBarTextFromMultiPage(const QString &);

  /** Calling this slot will cause the kmultipage to reload the file */
  void reload();

  void restoreDocument(const KURL &url, int page);
  void saveDocumentRestoreInfo(KConfig* config);

protected slots:
  void slotShowSidebar();
  void slotMedia (int);

  void goToPage();

  void zoomIn();
  void zoomOut();

  void disableZoomFit();
  void updateZoomLevel();

  void enableFitToPage(bool);
  void enableFitToHeight(bool);
  void enableFitToWidth(bool);

  void fitToPage();
  void fitToHeight();
  void fitToWidth();

  void slotPrint();

  void fileChanged(const QString&);

  // Connected to the QLineEdit in the toolbar.
  void setZoomValue(const QString &);


protected:
  KToggleAction *showSidebar, *scrollbarHandling;
  KSelectAction *orientation, *media, *zoom_action;

  virtual bool openFile();

  void connectNotify ( const char * );

  void partActivateEvent( KParts::PartActivateEvent *ev );
  void guiActivateEvent( KParts::GUIActivateEvent *ev );

private slots:
  void pageInfo(int numpages, int currentpage);
  void checkActions();

  void slotStartFitTimer();

  void doSettings();
  void preferencesChanged();

  void aboutKViewShell();

  void slotEnableMoveTool(bool enable);

  // Relay signals to the multipage. We cannot connect to the
  // slots of the multipage directly because than we would have
  // to recreate the whole interface whenever a new multipage is loaded.
  void mp_prevPage();
  void mp_nextPage();
  void mp_firstPage();
  void mp_lastPage();

  void mp_readUp();
  void mp_readDown();

  void mp_scrollUp();
  void mp_scrollDown();
  void mp_scrollLeft();
  void mp_scrollRight();
  void mp_scrollUpPage();
  void mp_scrollDownPage();
  void mp_scrollLeftPage();
  void mp_scrollRightPage();

  void mp_slotSave();
  void mp_slotSave_defaultFilename();

  void mp_doGoBack();
  void mp_doGoForward();

  void mp_showFindTextDialog();
  void mp_findNextText();
  void mp_findPrevText();

  void mp_doSelectAll();
  void mp_clearSelection();

  void mp_copyText();

  void mp_exportText();

private:
  void initializeMultiPage();

  // This method reads the configuration file. It should only be
  // called when no document is loaded.
  void readSettings();
  void writeSettings();

  // The method openFile of this kviewpart can be called even if
  // m_file points to a compressed file. In that case, the temporary
  // file tmpUnzipped will be created and a decompressed copy of the
  // file stored there.
  KTempFile *tmpUnzipped;

  KDirWatch *watch;
  KAccel *accel;
  KAction *zoomInAct, *zoomOutAct, *backAct, *forwardAct,
    *startAct, *endAct, *gotoAct,
    *saveAction, *saveAsAction, *printAction, *readUpAct, *readDownAct;
  KAction *backAction;
  KAction *forwardAction;
  KAction *settingsAction;
  KAction* aboutAction;
  KToggleAction *watchAct,*useDocumentSpecifiedSize,
    *fitPageAct, *fitHeightAct, *fitWidthAct;
  KSelectAction* viewModeAction;

  KRadioAction* moveModeAction;
  KRadioAction* selectionModeAction;

  KAction* exportTextAction;

  KAction* copyTextAction;
  KAction* selectAllAction;
  KAction* deselectAction;
  KAction* findTextAction;
  KAction* findNextTextAction;
  KAction* findPrevAction;
  KAction* findNextAction;

  KParts::PartManager* partManager;

  QGuardedPtr<KMultiPage> multiPage;
  // Name of the library of the currently loaded multiPage.
  // Is used to check if it is really necessary to load a new MultiPage.
  QString multiPageLibrary;

  KViewPartExtension *m_extension;

  bool pageChangeIsConnected;

  QWidget *mainWidget;
  QHBoxLayout* mainLayout;

  /** This entry stores the paper size that the user has requested in
      the preferences dialog. If that paper size is actually used or
      not, depends on if the document specifies a paper size of its
      own and if the user has chosen the option "use document
      specified paper size if available" */
  pageSize  userRequestedPaperSize;

  /** stores the current zoom value */
  Zoom _zoomVal;
  pageSizeDialog *_pageSizeDialog;

  QTimer fitTimer;

  KAboutDialog* aboutDialog;
};


class KViewPartExtension : public KParts::BrowserExtension
{
  Q_OBJECT
  friend class KViewPart;

public:

  KViewPartExtension(KViewPart *parent);
  virtual ~KViewPartExtension() {}

};


#endif
