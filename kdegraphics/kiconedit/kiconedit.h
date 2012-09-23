/*
    kiconedit - a small graphics drawing program for creating KDE icons
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/  

#ifndef __KICONEDIT_H__
#define __KICONEDIT_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qptrlist.h>

#include <kmainwindow.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <ktoolbar.h>
#include <kaccel.h>
#include <kurl.h>
#include <kaction.h>

#include "knew.h"
#include "kicon.h"
#include "kiconconfig.h"
#include "kicongrid.h"
#include "kresize.h"
#include "properties.h"

class KIconEdit;
class KCommandHistory;
typedef QPtrList<KIconEdit> WindowList;

class QWhatsThis;
class QToolButton;
class QLabel;
class PaletteToolBar;

/**
* KIconEdit
* @short KIconEdit
* @author Thomas Tanghus <tanghus@kde.org>
* @version 0.4
*/
class KIconEdit : public KMainWindow
{
    Q_OBJECT
public:
  KIconEdit( KURL url = KURL(), const char *name = "kiconedit");
  KIconEdit( const QImage image, const char *name = "kiconedit");
  ~KIconEdit();

  virtual QSize sizeHint() const;
  static WindowList windowList;

signals:
  void newname(const QString &);

public slots:
  virtual void saveProperties(KConfig*);
  virtual void readProperties(KConfig*);
  void updateProperties();

protected slots:
  void slotNewWin(const QString & url = 0);
  void slotNew();
  void slotOpen();
  void slotClose();
  void slotSave();
  void slotSaveAs();
  void slotPrint();
  void slotZoomIn();
  void slotZoomOut();
  void slotZoom1();
  void slotZoom2();
  void slotZoom5();
  void slotZoom10();
  void slotCopy();
  void slotCut();
  void slotPaste();
  void slotClear();
  void slotSaved();
  void slotSelectAll();
  void slotOpenRecent(const KURL&);
  void slotToolPointer();
  void slotToolFreehand();
  void slotToolRectangle();
  void slotToolFilledRectangle();
  void slotToolCircle();
  void slotToolFilledCircle();
  void slotToolEllipse();
  void slotToolFilledEllipse();
  void slotToolSpray();
  void slotToolFlood();
  void slotToolLine();
  void slotToolEraser();
  void slotToolSelectRect();
  void slotToolSelectCircle();
  void slotConfigureSettings();
  void slotConfigureKeys();
  void slotShowGrid();
  void slotUpdateZoom( int );
  void slotUpdateStatusColors(uint);
  void slotUpdateStatusColors(uint, uint*);
  void slotUpdateStatusPos(int, int);
  void slotUpdateStatusSize(int, int);
  void slotUpdateStatusMessage(const QString &);
  void slotUpdateStatusName(const QString &);
  void slotUpdateStatusModified(bool);
  void slotUpdateStatusScaling(int);
  void slotUpdatePaste(bool);
  void slotUpdateCopy(bool);
  void slotOpenBlank(const QSize);
  void addRecent(const QString &);

  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dropEvent(QDropEvent *e);

protected:
  void init();
  void setupActions();
  bool setupStatusBar();
  void writeConfig();
  void updateAccels();

  virtual bool queryClose();
  virtual QWidget *createContainer( QWidget*, int, const QDomElement&, int& );

  KCommandHistory* history;
  PaletteToolBar *m_paletteToolBar;
  KStatusBar *statusbar;
  KIconEditGrid *grid;
  KGridView *gridview;
  KIconEditIcon *icon;
  QImage img;
  QString m_name;

  KAction *m_actCopy, *m_actPaste, *m_actCut, *m_actPasteNew;
  KAction *m_actZoomIn, *m_actZoomOut;
  KRecentFilesAction *m_actRecent;
};

#endif //__KICONEDIT_H__
