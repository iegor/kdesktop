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

#ifndef KSV_SERVICE_DIALOG_H
#define KSV_SERVICE_DIALOG_H

#include <kdialogbase.h>
#include <qmap.h>

class KSVDragList;
class KSVItem;
class QString;
class QComboBox;

class ServiceDlg : public KDialogBase
{
  Q_OBJECT

public:
  ServiceDlg (const QString& action, const QString& label,
	      QWidget* parent = 0, const char* name = 0);
  virtual ~ServiceDlg();

  int count() const;

  void resetChooser (KSVDragList* data, bool edit);

public slots:
  virtual void show ();
  virtual void hide ();
  void toggle ();

private:
  QComboBox* mServices;
  QMap<QString,const KSVItem*> mMapServices;

protected slots:
  virtual void slotApply();

signals:
  void doAction (const QString& on);
  void display (bool);
};

#endif
