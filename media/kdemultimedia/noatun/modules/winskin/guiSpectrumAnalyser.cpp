/*
  winamp visualisation plugin.
  Copyright (C) 2001  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <klocale.h>
#include <qcolor.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <kconfig.h>

#include "waColor.h"
#include "waSkinModel.h"

#include "guiSpectrumAnalyser.h"
#include "vis/winskinvis.h"

#define __BANDS     75

GuiSpectrumAnalyser::GuiSpectrumAnalyser()
  : WaWidget(_WA_MAPPING_ANALYSER)
{
    connect(WaSkinModel::instance(), SIGNAL(skinChanged()), this, SLOT(pixmapChange()));

    contextMenu = new QPopupMenu(this);
    visualizationMenu = new QPopupMenu();
    analyserMenu = new QPopupMenu();

    contextMenu->insertItem(i18n("Visualization Mode"), visualizationMenu);
    contextMenu->insertItem(i18n("Analyzer Mode"), analyserMenu);

    visualizationMenu->insertItem(i18n("Analyzer"), (int)MODE_ANALYSER);
    visualizationMenu->insertItem(i18n("Disabled"), (int)MODE_DISABLED);
    visualizationMenu->setCheckable(true);
    connect(visualizationMenu, SIGNAL(activated(int)), this, SLOT(setVisualizationMode(int)));

    analyserMenu->insertItem(i18n("Normal"), (int)MODE_NORMAL);
    analyserMenu->insertItem(i18n("Fire"), (int)MODE_FIRE);
    analyserMenu->insertItem(i18n("Vertical Lines"), (int)MODE_VERTICAL_LINES);
    analyserMenu->setCheckable(true);
    connect(analyserMenu, SIGNAL(activated(int)), this, SLOT(setAnalyserMode(int)));

    analyserCache = NULL;
    winSkinVis = NULL;

    KConfig *config = KGlobal::config();
    config->setGroup("Winskin");

    setVisualizationMode(config->readNumEntry("visualizationMode", MODE_ANALYSER));
    setAnalyserMode(config->readNumEntry("analyserMode", MODE_NORMAL));
}


GuiSpectrumAnalyser::~GuiSpectrumAnalyser()
{
    KConfig *config = KGlobal::config();
    config->setGroup("Winskin");

    config->writeEntry("visualizationMode", visualization_mode);
    config->writeEntry("analyserMode", analyser_mode);

    delete analyserCache;
}

void GuiSpectrumAnalyser::mousePressEvent ( QMouseEvent *e )
{
    if (e->button() == LeftButton) {
        if (visualization_mode == MODE_DISABLED)
            setVisualizationMode(MODE_ANALYSER);
        else
            setVisualizationMode(MODE_DISABLED);
    }
    else if (e->button() == RightButton) {
        contextMenu->popup(mapToGlobal(QPoint(e->x(), e->y())));
    }
}

void GuiSpectrumAnalyser::setAnalyserMode(int mode)
{
    analyser_mode = mode;

    analyserMenu->setItemChecked(MODE_NORMAL, (mode == MODE_NORMAL));
    analyserMenu->setItemChecked(MODE_FIRE, (mode == MODE_FIRE));
    analyserMenu->setItemChecked(MODE_VERTICAL_LINES, (mode == MODE_VERTICAL_LINES));

    delete analyserCache;
    analyserCache = NULL;
}

void GuiSpectrumAnalyser::pauseVisualization()
{
    hide();
}

void GuiSpectrumAnalyser::resumeVisualization()
{
    show();
}

void GuiSpectrumAnalyser::updatePeaks()
{
    if ((visualization_mode == MODE_DISABLED) || (!isVisible()))
        return;

    float* currentPeaks = winSkinVis->currentPeaks();

    if (!analyserCache)
        freshenAnalyserCache();

    for (int x = 0;x < __BANDS;x++) {
        int amp  = int(currentPeaks[x]);

        if (amp < 0)
            amp = 0;
        else if (amp > 16)
            amp = 16;

        bitBlt(this, x, 0, analyserCache, (amp * 2) + (x % 2), 0, 1, 16);
    }
}

void GuiSpectrumAnalyser::setVisualizationMode(int mode)
{
    visualization_mode = mode;

    visualizationMenu->setItemChecked(MODE_ANALYSER, (mode == MODE_ANALYSER));
    visualizationMenu->setItemChecked(MODE_DISABLED, (mode == MODE_DISABLED));

    if (mode == MODE_ANALYSER)
    {
        if (!winSkinVis)
        {
            winSkinVis=new WinSkinVis(this,"WinSkinVis");
            connect(winSkinVis,SIGNAL(doRepaint()),this,SLOT(updatePeaks()));
        }
    }
    else
    {
         delete winSkinVis;
         winSkinVis = NULL;
    }

    update();
}


void GuiSpectrumAnalyser::freshenAnalyserCache()
{
    // We need a color scheme
    if (!colorScheme)
         return;

    // The analyser cache is a 34x16 pixmap containing all the bits needed
    // to quickly draw the spectrum analyser
    analyserCache = new QPixmap(34, 16);
    QPainter p(analyserCache);

    for (unsigned int x = 0;x < 17;x++) {
        if (x != 16) {
            p.setPen(QPen(colorScheme->skinColors[INDEX_BACKGROUND_COLOR]));
            p.drawLine(x * 2, 0, x * 2, 16 - x - 1);
        }

        for (unsigned int y = 0; y < (16 - x);y++) {
            if (y % 2)
                p.setPen(QPen(colorScheme->skinColors[INDEX_GRID_COLOR]));
            else
                p.setPen(QPen(colorScheme->skinColors[INDEX_BACKGROUND_COLOR]));

            p.drawPoint((x * 2) + 1, y);
        }

        if (!x)
            continue;

        switch (analyser_mode) {
        case MODE_FIRE:
            for (unsigned int y = (16 - x); y < 16; y++) {
                p.setPen(QPen(colorScheme->skinColors[INDEX_SPEC_BASE + (y - (16 - x))]));
                p.drawPoint((x * 2), y);
                p.drawPoint((x * 2) + 1, y);
            }
            break;
        case MODE_VERTICAL_LINES:
            p.setPen(QPen(colorScheme->skinColors[INDEX_SPEC_BASE + (16 - x)]));
            p.drawLine((x * 2), (15 - x), (x * 2), 15);
            p.drawLine((x * 2) + 1, (15 - x), (x * 2) + 1, 15);
            break;
        case MODE_NORMAL:
        // Fall through
        default:
            for (unsigned int y = (16 - x); y < 16; y++) {
                p.setPen(QPen(colorScheme->skinColors[INDEX_SPEC_BASE + y]));
                p.drawPoint((x * 2), y);
                p.drawPoint((x * 2) + 1, y);
            }
            break;
        }
    }
}

void GuiSpectrumAnalyser::paintEvent (QPaintEvent *)
{
    if (visualization_mode == MODE_DISABLED) 
        paintBackground();
}

void GuiSpectrumAnalyser::pixmapChange()
{
    delete analyserCache;
    analyserCache = NULL;
}


#include "guiSpectrumAnalyser.moc"

