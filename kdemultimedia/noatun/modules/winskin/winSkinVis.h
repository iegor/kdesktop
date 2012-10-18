/*
  noatun visualisation interface for winskin
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __WINSKINVIS_H
#define __WINSKINVIS_H

#include <qobject.h>

// These are needed for the Visualisation class 
#include <soundserver.h>
#include "noatunarts/noatunarts.h"
#include "noatun/plugin.h"

#include "vis/winskinvis.h"

class WinSkinVis : public QObject, public Visualization {
  Q_OBJECT

 public:
  WinSkinVis(QObject* parent,const char* name);
  ~WinSkinVis();

  /**
  * reimplemented from class Visualization, you
  * should never need to reimplement this yourself
  **/
  void timeout();
  float* currentPeaks();

 signals:
  void doRepaint();

 private:
  bool initServerObject();
  void scopeEvent(float* spectrum, unsigned int size);

  Noatun::WinSkinFFT* m_winSkinFFT;
  long m_id;
  float* m_currentPeaks;


};

#endif
