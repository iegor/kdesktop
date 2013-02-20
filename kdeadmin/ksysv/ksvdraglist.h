/*
** Copyright (C) 2000 Peter Putzer <putzer@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#ifndef KSVDRAGLIST_H
#define KSVDRAGLIST_H

#include <qstring.h>
#include <qcolor.h>
#include <qpixmap.h>

#include <klistview.h>

#include "Data.h"

class QPen;
class KSVAction;
class SimpleAction;
class KSVDragList;
class KPopupMenu;
class KSVConfig;
class KDLToolTip;

class KSVItem : public QListViewItem
{
public:
  typedef enum {
	SortNumber = 0,
	Icon,
	ServiceName,
	FileName,

	// insert additional columns here

	Last
  } Columns;

  KSVItem (KListView*);
  KSVItem (const KSVItem&);
  explicit KSVItem (KSVDragList*, const KSVData&);
  KSVItem (KListView* view, QString file, QString path, QString label, Q_INT8 nr );
  virtual ~KSVItem ();

  virtual QString key (int col, bool ascending) const;

  inline bool isChanged() const { return mData->changed(); }
  
  inline bool isNumChanged() const { return mData->numberChanged(); }
  inline bool isLabelChanged() const { return mData->labelChanged(); }
  inline bool isNew() const { return mData->newEntry(); }

  inline const QString& oldFilename() const { return mData->oldFilename(); }
  inline const QString& filename() const { return mData->filename(); }
  inline const QString& oldLabel() const { return mData->oldLabel(); }
  inline const QString& label() const { return mData->label(); }
  inline const QString& runlevel() const { return mData->runlevel(); }
  
  inline QString filenameAndPath() const { return mData->filenameAndPath (); }

  inline const QString& path() const { return mData->path(); }

  inline Q_INT8 number() const { return mData->number(); }
  inline Q_INT8 oldNumber() const { return mData->oldNumber(); }

  void copy (const KSVData&);

  KSVData* data () { return mData; }
  const KSVData* data () const { return mData; }

  QString toString () const;

  inline bool operator== (const KSVItem & rhs) const { return mData == rhs.mData; }

  inline const QColor& newNormalColor () const
  {
    return mNewNormalColor;
  }

  inline const QColor& changedNormalColor () const
  {
    return mChangedNormalColor;
  }

  inline QString originalRunlevel() const { return mData->originalRunlevel(); }  
  
  void setIcon (const QPixmap& icon);
  void setLabel (const QString& label);
  void setFilename (const QString& file);
  void setRunlevel (const QString& runlevel);
  void setNumber (Q_INT8 nr);
  void setPath (const QString& path);
  void setChanged (bool);
  void setNewNormalColor (const QColor&);
  void setChangedNormalColor (const QColor&);
  void setNewSelectedColor (const QColor&);
  void setChangedSelectedColor (const QColor&);
  void setNew (bool);
  void setOriginalRunlevel (const QString&);

  inline void setNumberChanged (bool val) { mData->setNumberChanged (val); }

  QString tooltip () const;
  virtual void paintCell (QPainter* p, const QColorGroup& cg, int column, int width, int align);

protected:
  friend class KSVDragList;
  friend class KServiceDragList;

  QPixmap paintDragIcon (const QFont& f, const QColorGroup& g) const;

private:
  friend class KSVDrag;

  KSVData* mData;
  KSVConfig* mConfig;

  QColor mNewNormalColor;
  QColor mNewSelectedColor;
  QColor mChangedNormalColor;
  QColor mChangedSelectedColor;
};

class KSVDragList : public KListView
{
  Q_OBJECT

public:
  KSVDragList ( QWidget* parent = 0, const char* name = 0 );
  virtual ~KSVDragList();

  virtual void clear();

  inline bool displayToolTips () const { return mDisplayToolTips; }
  inline bool commonToolTips () const { return mCommonToolTips; }

  inline const QString& tooltip () const { return mToolTip; }
  inline const QString& horizontalScrollBarTip () const { return mHorizontalTip; }
  inline const QString& verticalScrollBarTip () const { return mVerticalTip; }

  virtual void initItem (QString file, QString path, QString name, Q_INT8 nr);
  QPtrList<KSVData>& getDeletedItems() { return mRMList; }
  
  /**
   * @return whether this is the currently "selected" KSVDragList.
   */
  inline bool isOrigin() const { return mOrigin; }

  inline KSVItem* currentItem() { return static_cast<KSVItem*> (KListView::currentItem()); }

  inline KSVItem* lastItem () { return static_cast<KSVItem*> (KListView::lastItem()); }

  /**
   * @return true if the insertion was successful.
   */
  bool insert (const KSVData& data, const KSVData* above = 0L, const KSVData* below = 0L);

  bool insert (const KSVData& data, const KSVItem* where, KSVAction*& action);

  inline const QPixmap& defaultIcon() const { return mIcon; }

  bool removeFromRMList (const KSVData&);
  bool addToRMList (const KSVData&);
  void clearRMList();

  KSVItem* match (const KSVData&);

  inline void setDisplayToolTips (bool val) { mDisplayToolTips = val; }
  inline void setCommonToolTips (bool val) { mCommonToolTips = val; }
  inline void setHorizontalScrollBarTip (const QString& t) { mHorizontalTip = t; }
  inline void setVerticalScrollBarTip (const QString& t) { mVerticalTip = t; }
  inline void setToolTip (const QString& t) { mToolTip = t; }

public slots:
  void setNewNormalColor (const QColor &);
  void setChangedNormalColor (const QColor &);
  void setNewSelectedColor (const QColor &);
  void setChangedSelectedColor (const QColor &);

  void setOrigin (bool);
  inline void slotNewOrigin() { setOrigin (false); }

  void setDefaultIcon (const QPixmap& icon);

  void drop (QDropEvent*, QListViewItem*);

  virtual void setEnabled (bool);

protected:
  virtual void focusInEvent (QFocusEvent*);
  virtual bool acceptDrag (QDropEvent*) const;
  virtual QDragObject* dragObject ();
//  virtual KSVDrag* dragObject ();

  virtual void startDrag ();

  /**
   * Generates a sorting number for an item at
   * the given index by taking the average of the item
   * above and the item below.
   */
  Q_INT8 generateNumber (Q_INT8 high, Q_INT8 low) const;

  /**
   * A convenience function that also takes the label into account
   */
  Q_INT8 generateNumber (const QString& label, const KSVData* itemAbove, const KSVData* itemBelow) const;
  
  KSVItem* mItemToDrag;

private:
  QPixmap mIcon;
  
  bool mOrigin;
  QPtrList<KSVData> mRMList;

  QColor mNewNormalColor;
  QColor mNewSelectedColor;
  QColor mChangedNormalColor;
  QColor mChangedSelectedColor;

  KPopupMenu* mDragMenu;
  KPopupMenu* mDragCopyMenu;
  KPopupMenu* mDragMoveMenu;
  KDLToolTip* toolTip;
  typedef enum { Copy, Move } DragAction;

  QString mToolTip, mHorizontalTip, mVerticalTip;
  bool mDisplayToolTips, mCommonToolTips;

signals:
  void newOrigin();
  void newOrigin (KSVDragList*);

  void cannotGenerateNumber();
  void undoAction(KSVAction*);
};

class KServiceDragList : public KSVDragList
{
public:
  KServiceDragList (QWidget* parent = 0L, const char* name = 0L);
  virtual ~KServiceDragList ();

protected:
  virtual void startDrag ();
};

#endif
