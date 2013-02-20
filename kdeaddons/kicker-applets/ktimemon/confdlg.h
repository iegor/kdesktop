/* -*- C++ -*- */

/**********************************************************************/
/*   TimeMon (c)  1994  Helmut Maierhofer			                  */
/*   KDE-ified M. Maierhofer 1998                                     */
/*                                                                    */
/*   Ported to KDE 2.0 and other stuff:                               */
/*     Copyright (c) Dirk A. Mueller <dmuell@gmx.net>                 */
/*                                                                    */
/**********************************************************************/

/*
 * confdlg.h
 *
 * Definitions for the timemon configuration dialog.
 */

#ifndef CONFDLG_H
#define CONFDLG_H

#include <qcolor.h>
#include <qcheckbox.h>
#include <qtabdialog.h>

#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kdialogbase.h>
#include <kurlrequester.h>

#include "timemon.h"

// -- forward declaration ------------------------------------------------

class KTimeMon;
class QLineEdit;
class QSlider;
class KColorButton;

// -- KConfDialog declaration --------------------------------------------

/*
 * KConfDialog
 */

class KConfDialog : public KDialogBase
{

    Q_OBJECT

public:
    KConfDialog(KTimeMon *timemon);
    ~KConfDialog() {}

    void update();		// get values from timemon

  unsigned getInterval() const { return intervalEdit->value(); }
  QColor getKernelColour() const { return kernelCB->color(); }
  QColor getUserColour() const { return userCB->color(); }
  QColor getNiceColour() const { return niceCB->color(); }
  QColor getIOWaitColour() const { return iowaitCB->color(); }
  QColor getCachedColour() const { return cachedCB->color(); }
  QColor getUsedColour() const { return usedCB->color(); }
  QColor getBuffersColour() const { return buffersCB->color(); }
  QColor getMKernelColour() const { return mkernelCB->color(); }
  QColor getSwapColour() const { return swapCB->color(); }
  QColor getBgColour() const { return bgCB->color(); }

  bool getAutoScale() const { return autoScaleBox->isChecked(); }
  unsigned getPageScale() const { return pageScaleEdit->value(); }
  unsigned getSwapScale() const { return swapScaleEdit->value(); }
  unsigned getCtxScale() const { return ctxScaleEdit->value(); }

  unsigned getMouseAction(int i) const;
  QString getMouseActionCommand(int i) const { return mouseLE[i]->lineEdit()->text(); }

private slots:
  void updateSampleWidget(const QColor &); // update colours in configuration
  void toggle(bool state);	// enable/disable scales
  void mouseCommandEnable();

private:

  KTimeMon *timemon;
  KIntNumInput *intervalEdit, *swapScaleEdit, *pageScaleEdit, *ctxScaleEdit;
  QLineEdit *procFileEdit;
  QCheckBox *autoScaleBox;
  KColorButton *kernelCB, *userCB, *niceCB, *iowaitCB;
  KColorButton *buffersCB, *usedCB, *cachedCB, *mkernelCB;
  KColorButton *swapCB, *bgCB;
  KURLRequester *mouseLE[MAX_MOUSE_ACTIONS];
  KComboBox *mouseC[MAX_MOUSE_ACTIONS];
  bool haveWarned;

  friend class KTimeMon;
};

#endif // CONFDLG_H
