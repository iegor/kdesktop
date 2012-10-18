/*
 * systemtray.h
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ksystemtray.h>

#include <qwidget.h>

/**
  *@author Jürgen  Kofler
  */

  
class TitleLabel;
class QTimer;  

class SystemTray : public KSystemTray  {
   Q_OBJECT
public: 
  SystemTray(QWidget *parent = 0, const char *name = 0);
  virtual ~SystemTray();
  
  void showTitle(const QString&, uint secs = 5);
  
protected:
   virtual void wheelEvent(QWheelEvent *e);
   void mousePressEvent(QMouseEvent *e);

signals:
   void signalPlay();
   void signalNext();
   void signalPrevious();
   void signalStop();
   void signalMute();

private slots:
  void slotHideOSD();   
   
private:
  TitleLabel* m_osd;
  QTimer m_hideTimer;
};

#endif /* SYSTEMTRAY_H */
