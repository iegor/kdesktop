/**********************************************************************/
/*   TimeMon (c)  1994  Helmut Maierhofer			      */
/*   KDE-ified M. Maierhofer 1998                                     */
/**********************************************************************/

/*
 * confdlg.h
 *
 * Definitions for the timemon configuration dialog.
 */

#include <config.h>
#include <stdio.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qslider.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvgroupbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#include <kcolorbutton.h>
#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurlrequester.h>

#include "confdlg.h"
#include "timemon.h"

// -- KConfDialog definition ---------------------------------------------

KConfDialog::KConfDialog(KTimeMon *t)
    : KDialogBase( Tabbed, i18n("Configuration" ),
          Ok|Cancel|Apply, Ok, t, 0, false ),
      timemon(t)
{
    QFrame *page;
    QBoxLayout *bl;
    QGridLayout *gl;
    QLabel *l;
    QGroupBox *b;
    KColorButton *cb;
    unsigned i, j;

    setIcon( SmallIcon( "ktimemon" ) );

    // first tab: general
    page = addPage( i18n( "&General" ) );

    bl = new QVBoxLayout(page, 0, spacingHint());

    b = new QVGroupBox(i18n("Sample &Rate"), page);
    bl->addWidget(b);

    intervalEdit = new KIntNumInput(250, b);
    intervalEdit->setRange(20, 1000, 10);
    intervalEdit->setSuffix(i18n(" msec"));

    // scaling group box
    b = new QVGroupBox(i18n("Scaling"), page);
    bl->addWidget(b);

    bl->addStretch();

    autoScaleBox = new QCheckBox(i18n("&Automatic"), b);
    connect(autoScaleBox, SIGNAL(toggled(bool)), this, SLOT(toggle(bool)));

    pageScaleEdit = new KIntNumInput(intervalEdit, 1000, b);
    pageScaleEdit->setRange(10, 10000, 10);
    pageScaleEdit->setLabel(i18n("&Paging:"), AlignVCenter | AlignLeft);

    swapScaleEdit = new KIntNumInput(pageScaleEdit, 1000, b);
    swapScaleEdit->setRange(1, 10000, 5);
    swapScaleEdit->setLabel(i18n("&Swapping:"), AlignVCenter | AlignLeft);

    ctxScaleEdit  = new KIntNumInput(swapScaleEdit, 10000, b);
    ctxScaleEdit->setLabel(i18n("&Context switch:"), AlignVCenter | AlignLeft);
    ctxScaleEdit->setRange(1, 10000, 30);

    bl->addStretch(1);

    // second tab: colours
    page = addPage( i18n( "C&olors" ) );

    gl = new QGridLayout(page, 12, 10, 0, spacingHint());

    gl->setColStretch(3, 1); gl->setColStretch(6, 1); // eat up horizontal space
    gl->setRowStretch(11, 1);	// eat up vertical space

    gl->addRowSpacing(0, 20); gl->addRowSpacing(4, 20); gl->addRowSpacing(8, 20);
    gl->addRowSpacing(2, 8); gl->addRowSpacing(6, 8); gl->addRowSpacing(10, 8);

    gl->addColSpacing(0, 10); gl->addColSpacing(9, 25);

    QString cpuColourLabels[4];
    cpuColourLabels[0] = i18n("Kernel:");
    cpuColourLabels[1] = i18n("User:");
    cpuColourLabels[2] = i18n("Nice:");
    cpuColourLabels[3] = i18n("IOWait:");

    QString memColourLabels[4];
    memColourLabels[0] = i18n("Kernel:");
    memColourLabels[1] = i18n("Used:");
    memColourLabels[2] = i18n("Buffers:");
    memColourLabels[3] = i18n("Cached:");

    KColorButton **cpuColourButtons[4] = { &kernelCB, &userCB, &niceCB, &iowaitCB };
    b = new QGroupBox(i18n("CPU"), page);
    gl->addMultiCellWidget(b, 0, 2, 0, 13);
    
    for (j = 0; j < 4; j++) {
      l = new QLabel(cpuColourLabels[j], page);
      gl->addWidget(l, 1, 3*j+1, AlignVCenter | AlignRight);

      cb = *cpuColourButtons[j] = new KColorButton(white, page);
      gl->addWidget(cb, 1, 3*j+2, AlignCenter);
      
      connect(cb, SIGNAL(changed(const QColor &)),
              this, SLOT(updateSampleWidget(const QColor &)));
    }

    KColorButton **memColourButtons[4] = { &mkernelCB, &usedCB, &buffersCB, &cachedCB };
    b = new QGroupBox(i18n("Memory"), page);
    gl->addMultiCellWidget(b, 4, 6, 0, 13);

    for (j = 0; j < 4; j++) {
      l = new QLabel(memColourLabels[j], page);
      gl->addWidget(l, 5, 3*j+1, AlignVCenter | AlignRight);

      cb = *memColourButtons[j] = new KColorButton(white, page);
      gl->addWidget(cb, 5, 3*j+2, AlignCenter);
      
      connect(cb, SIGNAL(changed(const QColor &)),
              this, SLOT(updateSampleWidget(const QColor &)));
    }

    b = new QGroupBox(i18n("Swap"), page);
    gl->addMultiCellWidget(b, 8, 10, 0, 6);

    l = new QLabel(i18n("Swap:"), page);
    gl->addWidget(l, 9, 1, AlignVCenter | AlignRight);

    cb = swapCB = new KColorButton(red, page);
    gl->addWidget(cb, 9, 2);

    connect(cb, SIGNAL(changed(const QColor &)),
            this, SLOT(updateSampleWidget(const QColor &)));

    l = new QLabel(i18n("Backgd:"), page);
    gl->addWidget(l, 9, 4, AlignVCenter | AlignRight);

    cb = bgCB = new KColorButton(blue, page);
    gl->addWidget(cb, 9, 5);

  connect(cb, SIGNAL(changed(const QColor &)),
	  this, SLOT(updateSampleWidget(const QColor &)));

  //b = new QGroupBox(i18n("Sample"), page);
  //gl->addMultiCellWidget(b, 8, 10, 7, 9);

  // third tab: interaction
  page = addPage( i18n( "&Interaction" ) );

  bl = new QVBoxLayout(page, 0, spacingHint());

  b = new QGroupBox(i18n("Mouse Events"), page);
  b->setColumnLayout( 0, Qt::Vertical );
  bl->addWidget(b);
  bl->addStretch();

  QVBoxLayout *vbox = new QVBoxLayout( b->layout() );

  gl = new QGridLayout(b, MAX_MOUSE_ACTIONS + 1, 3, 0, 6 );

  vbox->addLayout( gl );

  for (i = 1; i < MAX_MOUSE_ACTIONS + 1; i++)
    gl->setRowStretch(i, 1);
  gl->setColStretch(2, 1);

  QString buttonText[MAX_MOUSE_ACTIONS] = { i18n("Left button:"),
						i18n("Middle button:"),
						i18n("Right button:") };

  for (i = 0; i < (int) MAX_MOUSE_ACTIONS; i++) {

    l = new QLabel(buttonText[i], b);
    gl->addWidget(l, i+1, 0);

    mouseC[i] = new KComboBox(false, b);
    mouseC[i]->insertItem(i18n("Is Ignored"), KTimeMon::NOTHING);
//  SWITCH doesn't DO anything. remove it from config dialog for now
//    mouseC[i]->insertItem(i18n("Switches Mode"), KTimeMon::SWITCH);
    mouseC[i]->insertItem(i18n("Pops Up Menu"), KTimeMon::MENU - 1);
    mouseC[i]->insertItem(i18n("Starts"), KTimeMon::COMMAND - 1);
    gl->addWidget(mouseC[i], i+1, 1);

    connect( mouseC[ i ], SIGNAL( activated( int ) ), this,
           SLOT( mouseCommandEnable() ) );

    mouseLE[i] = new KURLRequester(b);
    mouseLE[i]->lineEdit()->setText(t->mouseActionCommand[i]);
    gl->addWidget(mouseLE[i], i+1, 2);
  }

  gl->activate();

  resize(380, 300);

  connect(this, SIGNAL(applyClicked()), timemon, SLOT(apply()));
  connect(this, SIGNAL(okClicked()), timemon, SLOT(apply()));
}

// Adjust the colours of the sample widget in the configuration dialog.
void KConfDialog::updateSampleWidget(const QColor &)
{
#if 0
  sample->kernelColour = kernelCB->color();
  sample->userColour = userCB->color();
  sample->niceColour = niceCB->color();
  sample->iowaitColour = iowaitCB->color();
  sample->kernelColour = kernelCB->color();
  sample->cachedColour = cachedCB->color();
  sample->usedColour = usedCB->color();
  sample->buffersColour = buffersCB->color();
  sample->swapColour = swapCB->color();
  sample->bgColour = bgCB->color();
  sample->update();
#endif
}

// -----------------------------------------------------------------------------

// enable/disable the scale widgets
void KConfDialog::toggle(bool state)
{
    swapScaleEdit->setEnabled(!state);
    pageScaleEdit->setEnabled(!state);
    ctxScaleEdit->setEnabled(!state);
}

void KConfDialog::mouseCommandEnable()
{
  for ( int i = 0; i < MAX_MOUSE_ACTIONS; i++ ) {
    unsigned action = mouseC[ i ]->currentItem();

    // the - 1 is for compat with the no longer shown Switch option
    mouseLE[ i ]->setEnabled( action == KTimeMon::COMMAND - 1);
  }
}

// update the dialog fields
void KConfDialog::update()
{
  intervalEdit->setValue(timemon->interval);
  kernelCB->setColor(timemon->kernelColour);
  userCB->setColor(timemon->userColour);
  niceCB->setColor(timemon->niceColour);
  iowaitCB->setColor(timemon->iowaitColour);
  buffersCB->setColor(timemon->buffersColour);
  mkernelCB->setColor(timemon->mkernelColour);
  usedCB->setColor(timemon->usedColour);
  cachedCB->setColor(timemon->cachedColour);
  swapCB->setColor(timemon->swapColour);
  bgCB->setColor(timemon->bgColour);
  pageScaleEdit->setValue(timemon->pageScale);
  swapScaleEdit->setValue(timemon->swapScale);
  ctxScaleEdit->setValue(timemon->ctxScale);
  autoScaleBox->setChecked(timemon->autoScale);

  for ( int i = 0; i < MAX_MOUSE_ACTIONS; i++ )
  {
    int action = timemon->mouseAction[i];
    if (action > 0)
        --action; // compat for the no longer shown Switch action
    mouseC[i]->setCurrentItem(action);
  }
  mouseCommandEnable();

  updateSampleWidget(white);	// fake colour
}

unsigned int KConfDialog::getMouseAction(int i) const
{
    int action = mouseC[i]->currentItem();

    if (action > 0)
        ++action; // compat for the no longer shown Switch action

    return action;
}

#include "confdlg.moc"
