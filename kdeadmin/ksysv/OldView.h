/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1999 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KSV_VIEW_H
#define KSV_VIEW_H

#include <qsplitter.h>
#include <qvaluelist.h>
#include <qsize.h>

#include <ktrader.h>

class QPopupMenu;
class QFileInfo;
class QLabel;
class QListViewItem;
class QTextEdit;
class QFrame;
class QVBox;
class QPixmap;
class QLayout;

class KScroller;
class KProcess;
class KListView;
class QSplitter;
class KPopupMenu;
class KSVTrash;
class KSVDragList;
class KSVConfig;
class KSVData;
class KSVItem;
class KSVAction;
class KSVTopLevel;

class KSVContent : public QSplitter
{
  Q_OBJECT

public:
  KSVContent (KPopupMenu* openWithMenu, KSVTopLevel* parent = 0, const char* name = 0);
  ~KSVContent();

  KSVDragList* getOrigin();

  const QString& log() const;
  const QString& xmlLog() const;

  void setColors (const QColor& newNormal,
                  const QColor& newSelected,
                  const QColor& changedNormal,
                  const QColor& changedSelected);

  void mergeLoadedPackage (QValueList<KSVData>* start,
                           QValueList<KSVData>* stop);

public slots:
  void slotWriteSysV();

  void infoOnData(KSVItem* data);
  void setDisplayScriptOutput(bool val);
  void slotScriptProperties (QListViewItem*);
  void multiplexEnabled (bool);

  void hideRunlevel (int index);
  void showRunlevel (int index);

protected:
  virtual void resizeEvent (QResizeEvent* e);
  virtual void moveEvent (QMoveEvent* e);
  virtual void showEvent (QShowEvent*);

private slots:
  void calcMinSize ();
  void fwdOrigin (KSVDragList*);
  void startService();
  void startService (const QString& path);
  void stopService();
  void stopService (const QString& path);
  void restartService();
  void restartService (const QString& path);
  void editService();
  void editService (const QString& path);
  void slotOutputOrError( KProcess* _p, char* _buffer, int _buflen );
  void slotExitedProcess(KProcess* proc);
  void slotScriptsNotRemovable();
  void slotDoubleClick (QListViewItem*);
  void slotExitDuringRestart(KProcess* proc);
  void appendLog(const QString& rich, const QString& plain);
  void appendLog(const QCString& _buffer);
  void fwdCannotGenerateNumber();
  void fwdOrigin();
  void reSortRL();
  void pasteAppend();
  void fwdUndoAction(KSVAction*);
  void updatePanningFactor();

  void popupRunlevelMenu (KListView*, QListViewItem*, const QPoint&);
  void popupServicesMenu (KListView*, QListViewItem*, const QPoint&);

  void updateServicesAfterChange (const QString&);
  void updateRunlevelsAfterChange ();

  void repaintRunlevels ();

  void openWith ();
  void openWith (int index);

signals:
  void sigUpdateParent();
  void sigRun (const QString&);
  void sigStop();
  void sigNotRemovable();
  void cannotGenerateNumber();
  void selected (KSVItem*);
  void selectedScripts (KSVItem*);
  void sizeChanged();

  void undoAction (KSVAction*);
  void logChanged();

  void newOrigin();

private:
  static int splitterToPanningFactor (const QValueList<int>&);
  static const QValueList<int>& panningFactorToSplitter (int);

  void merge (QValueList<KSVData>& list, KSVDragList* widget);
  void initLList();
  void initScripts();
  void initRunlevels();
  void info2Widget (QFileInfo* info, int index);
  void writeToDisk (const KSVData& _w, int _rl, bool _start);
  void clearRL( int _rl );

  friend class KSVTopLevel;

  QFrame* mContent;
  KScroller* mScroller;

  KSVDragList** startRL;
  KSVDragList** stopRL;
  KSVDragList* scripts;

  KPopupMenu* mItemMenu;
  KPopupMenu* mContextMenu;
  KPopupMenu* mScriptMenu;
  KSVTrash* trash;
  QTextEdit* textDisplay;
  KSVConfig* conf;

  QVBox* mScriptBox;
  QVBox** mRunlevels;
  QWidget* mBuffer;

  KSVDragList* mOrigin;

  QString mLogText, mXMLLogText;

  QSize mMinSize;

  KPopupMenu* mOpenWithMenu;
  KTrader::OfferList mOpenWithOffers;

  QCString m_buffer;
};

#endif
