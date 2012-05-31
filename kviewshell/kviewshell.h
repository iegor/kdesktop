// -*- C++ -*-
#ifndef KVIEWSHELL_H
#define KVIEWSHELL_H

#include <qstring.h>

#include <kparts/mainwindow.h>

class KRecentFilesAction;
class KURL;
class KViewPart_Iface;


class QLabel;

class KViewShell : public KParts::MainWindow
{
  Q_OBJECT

public:
  KStatusBar *statusbar;
  KStatusBar *action;

  KViewShell(const QString& defaultMimeType = QString::null);
  virtual ~KViewShell();

public slots:
  void openURL(const KURL&);
  void addRecentFile();


protected slots:
  void slotFullScreen();
  void slotQuit();
  void slotConfigureKeys();
  void slotEditToolbar();
  void slotFileClose();
  void slotNewToolbarConfig();

  void slotChangeZoomText(const QString &);
  void slotChangePageText(const QString &);
  void slotChangeSizeText(const QString &);

signals:
  void restoreDocument(const KURL &url, int page);
  void saveDocumentRestoreInfo(KConfig* config);

protected:
  void readSettings();
  void writeSettings();

  /**
   * This method is called when it is time for the app to save its
   * properties for session management purposes.
   */
  void saveProperties(KConfig*);

  /**
   * This method is called when this app is restored.  The KConfig
   * object points to the session management config file that was saved
   * with @ref saveProperties
   */
  void readProperties(KConfig*);

  void checkActions();

  void dragEnterEvent(QDragEnterEvent *event);
  void dropEvent(QDropEvent *event);
  void keyPressEvent(QKeyEvent * e);

private:
  KViewPart_Iface *view;

  KRecentFilesAction *recent;
  QString cwd;

  KAction *closeAction, *reloadAction;
  KToggleAction *fullScreenAction;

  // In the attribute, the status of the statusbar (shown of hidden)
  // is saved when the kviewshell switches to fullscreen mode. The
  // statusbar can then be restored when the application returns to normal mode.
  bool isStatusBarShownInNormalMode;
  // ditto, for the toolbar
  bool isToolBarShownInNormalMode;
};


#endif
