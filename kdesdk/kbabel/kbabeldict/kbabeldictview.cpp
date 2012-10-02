/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than 
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */


#include "kbabeldictview.h"
#include "kbabeldictbox.h"
#include "searchengine.h"


#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qwidgetstack.h>
#include <qsplitter.h>
#include <qvbox.h>

#include <kcombobox.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kglobal.h>
#include <klineedit.h>
#include <klocale.h>
#include <kprogress.h>
#include <kseparator.h>
#include <kdebug.h>
#include <kiconloader.h>

/* 
 *  Constructs a KBabelDictView which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
KBabelDictView::KBabelDictView( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QVBoxLayout    *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->setMargin(KDialog::marginHint());

    splitter = new QSplitter(this);
    mainLayout->addWidget(splitter);

    QWidget *w = new QWidget(splitter);
    QVBoxLayout *wLayout= new QVBoxLayout(w);
    wLayout->setSpacing(KDialog::spacingHint());
    wLayout->setMargin(KDialog::marginHint());
    
    QHBoxLayout *hbox = new QHBoxLayout(wLayout);
    QLabel *label = new QLabel(i18n("Search in module:"), w);
    hbox->addWidget(label);
    moduleCombo = new KComboBox(w);
    hbox->addWidget(moduleCombo);

    QWidget *temp = new QWidget(w);
    hbox->addWidget(temp);
    hbox->setStretchFactor(temp,2);
    editButton = new QPushButton(i18n("&Edit"),w);
    editButton->setEnabled(false);
    hbox->addWidget(editButton);

    // added a button "clear search" here
    hbox = new QHBoxLayout(wLayout);
    QPushButton* clearButton = new QPushButton(w);
    clearButton->setFlat(true);
    clearButton->setPixmap(SmallIcon("locationbar_erase"));
    hbox->addWidget(clearButton);
    textEdit = new KLineEdit(w,"textedit");
    textEdit->setFocus();
    hbox->addWidget(textEdit);

    hbox = new QHBoxLayout(wLayout);
    startButton = new QPushButton(i18n("&Start Search"),w);
    hbox->addWidget(startButton);
    inTransButton = new QCheckBox(i18n("Sea&rch in translations"),w);
    hbox->addWidget(inTransButton);
    hbox->addStretch(1);
    stopButton = new QPushButton(i18n("S&top"),w);
    stopButton->setEnabled(false);
    hbox->addWidget(stopButton);

    KSeparator *sep = new KSeparator(w);
    wLayout->addWidget(sep);
    dictBox = new KBabelDictBox(w,"kbabeldictbox");
    wLayout->addWidget(dictBox);

    prefWidget = new QWidget(splitter);
    QVBoxLayout *tempLayout= new QVBoxLayout(prefWidget);
    tempLayout->setSpacing(KDialog::spacingHint());
    tempLayout->setMargin(KDialog::marginHint());

    label = new QLabel(i18n("Settings:"),prefWidget);
    tempLayout->addWidget(label);
    
    prefStack = new QWidgetStack(prefWidget);
    tempLayout->addWidget(prefStack);
    tempLayout->addStretch(1);

    KConfig *config = KGlobal::config();
    dictBox->readSettings(config);
    dictBox->setAutoUpdateOptions(true);
           
    QStringList modules = dictBox->moduleNames();
    moduleCombo->insertStringList(modules);

    QPtrList<PrefWidget> prefs = dictBox->modPrefWidgets(prefStack);
    prefs.setAutoDelete(false);

    PrefWidget *p;
    int i=0;
    for(p = prefs.first(); p != 0; p=prefs.next())
    {
        prefStack->addWidget(p,i);
        i++;
    }

    int active=dictBox->activeModule();
    prefStack->raiseWidget(active);
    moduleCombo->setCurrentItem(active);


    QHBox *h = new QHBox(this);
    h->setSpacing(KDialog::spacingHint());
    mainLayout->addWidget(h);
    progressLabel = new QLabel(h);
    progressBar = new KProgress(h);

    connect(textEdit,SIGNAL(returnPressed()),startButton,SLOT(animateClick()));
    connect(startButton,SIGNAL(clicked()),this, SLOT(startSearch()));
    connect(stopButton, SIGNAL(clicked()), dictBox,SLOT(slotStopSearch()));
    connect(editButton, SIGNAL(clicked()), dictBox, SLOT(edit()));
    connect(dictBox, SIGNAL(searchStarted()), this, SLOT(searchStarted()));
    connect(dictBox, SIGNAL(searchStopped()), this, SLOT(searchStopped()));
    connect(dictBox, SIGNAL(progressed(int)), progressBar, SLOT(setProgress(int)));
    connect(dictBox, SIGNAL(activeModuleChanged(bool))
            , editButton, SLOT(setEnabled(bool)));
    
    connect(dictBox, SIGNAL(progressStarts(const QString&))
            , this, SLOT(progressStarted(const QString&)));
    connect(dictBox, SIGNAL(progressEnds()), this, SLOT(progressStopped()));
    
    connect(moduleCombo, SIGNAL(activated(int)), 
                    dictBox, SLOT(setActiveModule(int)));
    connect(dictBox, SIGNAL(activeModuleChanged(int))
                    , this, SLOT(switchModule(int)));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(slotClearSearch()));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KBabelDictView::~KBabelDictView()
{
    // no need to delete child widgets, Qt does it all for us
    KConfig *config = KGlobal::config();
    dictBox->saveSettings(config);
}

/* 
 * public slot
 */
void KBabelDictView::startSearch()
{
    QString text = textEdit->text();
    
    if(!text.isEmpty())
    {
        if(inTransButton->isChecked())
        {
            dictBox->startTranslationSearch(text);
        }
        else
        {
            dictBox->startSearch(text);
        }
    }


}

void KBabelDictView::progressStopped()
{
    progressBar->setProgress(0);
    progressLabel->setText("");
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}

void KBabelDictView::progressStarted(const QString& msg)
{
    progressLabel->setText(msg);
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void KBabelDictView::searchStopped()
{
    progressLabel->setText("");
    progressBar->setProgress(0);
    startButton->setEnabled(true);
    stopButton->setEnabled(false);
}

void KBabelDictView::searchStarted()
{
    progressLabel->setText(i18n("Searching"));
    startButton->setEnabled(false);
    stopButton->setEnabled(true);
}

void KBabelDictView::switchModule(int m)
{
    moduleCombo->blockSignals(true);
    moduleCombo->setCurrentItem(m);
    moduleCombo->blockSignals(false);

    prefStack->raiseWidget(m);
}


void KBabelDictView::about()
{
    dictBox->about();
}


void KBabelDictView::aboutModule()
{
    dictBox->aboutActiveModule();
}

int KBabelDictView::togglePref()
{
    int prefWidth=0;
    if(prefWidget->isVisibleTo(this))
    {
        prefWidget->hide();
        prefWidth=prefWidget->width();
    }
    else
    {
        prefWidget->show();
        prefWidth=prefWidget->width();
    }

    return prefWidth;
}

bool KBabelDictView::prefVisible() 
{
    return prefWidget->isVisibleTo(this);
}

void KBabelDictView::slotClearSearch()
{
  textEdit->clear();
  textEdit->setFocus();
}

#include "kbabeldictview.moc"

