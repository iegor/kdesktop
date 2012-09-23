/*
  a GUI for a spectrum analyser
  Copyright (C) 1998  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef _GUISPECTRUMANALYSER_H
#define _GUISPECTRUMANALYSER_H

#include <math.h>

#include "winSkinVis.h"
#include "waWidget.h"
#include "waColor.h"


#define MAX_MODE 1

enum visualizationMode {MODE_DISABLED = 0, MODE_ANALYSER = 1};
enum analyserMode {MODE_NORMAL = 0, MODE_FIRE = 1, MODE_VERTICAL_LINES = 2};

class GuiSpectrumAnalyser : public WaWidget  {
    Q_OBJECT

  public:
    GuiSpectrumAnalyser();
    ~GuiSpectrumAnalyser();

    void mousePressEvent(QMouseEvent * mouseEvent);

  public slots:
    void pauseVisualization();
    void resumeVisualization();

  private slots:
    void pixmapChange();
    void updatePeaks();

    void setVisualizationMode(int);
    void setAnalyserMode(int);

  private:
    void paintEvent(QPaintEvent *);

    QPopupMenu *contextMenu;   
    QPopupMenu *visualizationMenu;
    QPopupMenu *analyserMenu;

    void freshenAnalyserCache();

    int visualization_mode;
    int analyser_mode;

    QPixmap *analyserCache;
    WinSkinVis *winSkinVis;
};
#endif

