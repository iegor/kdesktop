// -*-c++-*-
//
// C++ Interface for module: DubApp
//
// Description: KDE application entry point for dub playlist
// Stripped down main window code from kapptemplate
//
// Author: Eray (exa) Ozkural, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef DubApp_Interface
#define DubApp_Interface

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// include files for KDE
#include <kapplication.h>
#include <kmainwindow.h>
#include <kaccel.h>
#include <kaction.h>

// forward declaration of the Dub classes
class DubView;

// Application stuff
class DubApp : public KMainWindow
{
  Q_OBJECT

  friend class DubView;

public:
  /** construtor of DubApp, calls all init functions to create the application.
   */
  DubApp(QWidget* parent, const char* name="Dub Playlist");
  ~DubApp();

  void initActions();
  /** sets up the statusbar for the main window by initialzing a statuslabel.
   */
  void initStatusBar();
  /** initializes the document object of the main window that is connected to the view in initView().
   * @see initView();
   */
  void initView();

  DubView *view;
  DubView* getView() {
    return view;
  }

protected:
  virtual void closeEvent(QCloseEvent*e);

private:
  KConfig *config;
  /** view is the main widget which represents your working area. The View
   * class should handle all events of the view widget.  It is kept empty so
   * you can create your view according to your application's needs by
   * changing the view class.
   */

  // KAction pointers to enable/disable actions
  //KAction* fileQuit;
  KAction* fileClose;

  // signals and slots

public slots:
  /** changes the statusbar contents for the standard label permanently, used to indicate current actions.
   * @param text the text that is displayed in the statusbar
   */
  void slotStatusMsg(const QString &text);
  /** get a pointer to view object */

};

#endif
