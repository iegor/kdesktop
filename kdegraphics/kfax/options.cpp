/*
    $Id: options.cpp 465369 2005-09-29 14:33:08Z mueller $

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1996 Bernd Johannes Wuebben
                       wuebben@math.cornell.edu

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



*/

#include <stdio.h>

#include <qlayout.h>
#include <klocale.h>
#include <knuminput.h>
#include <qdir.h>
#include <qprinter.h>
#include <qframe.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qscrollbar.h>
#include <qtooltip.h>

#include "kfax.h"
#include "options.h"

OptionsDialog::OptionsDialog( QWidget *parent, const char *name)
    : KDialogBase( parent, name, true, i18n("Configure"), Ok|Cancel)
{
  QWidget *mainWidget = new QWidget(this);
  setMainWidget(mainWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget, 0, spacingHint());

  bg = new QGroupBox(mainWidget,"bg");
  bg->setColumnLayout( 0, Qt::Horizontal );
  mainLayout->addWidget( bg );

  QVBoxLayout *vbl = new QVBoxLayout(bg->layout());

  QHBoxLayout *hbl1 = new QHBoxLayout();

  vbl->addLayout( hbl1 );

  displaylabel = new QLabel(i18n("Display options:"), bg,"displaylabel");
  displaylabel->setFixedSize( displaylabel->sizeHint() );
  hbl1->addWidget( displaylabel );

  landscape = new QCheckBox(i18n("Landscape"), bg,"Landscape");
  landscape->setFixedSize( landscape->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( landscape );

  flip = new QCheckBox(i18n("Upside down"), bg,"upsidedown");
  flip->setFixedSize( flip->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( flip );

  invert = new QCheckBox(i18n("Invert"), bg,"invert");
  invert->setFixedSize( invert->sizeHint() );
  hbl1->addSpacing( 10 );
  hbl1->addWidget( invert );

  vbl->addSpacing( 20 );

  QHBoxLayout *hbl8 = new QHBoxLayout();
  vbl->addLayout( hbl8 );
  hbl8->addSpacing( 10 );

  resgroup = new QButtonGroup(bg,"resgroup");
  resgroup->setFrameStyle(QFrame::NoFrame);
  hbl8->addWidget( resgroup );

  QHBoxLayout *hbl2 = new QHBoxLayout(resgroup);

  reslabel = new QLabel(i18n("Raw fax resolution:"),resgroup,"relabel");
  hbl2->addWidget( reslabel );

  resauto = new QRadioButton(i18n("Auto"),resgroup,"resauto");
  hbl2->addSpacing( 20 );
  hbl2->addWidget( resauto );

  fine = new QRadioButton(i18n("Fine"),resgroup,"fine");
  hbl2->addSpacing( 30 );
  hbl2->addWidget( fine );

  normal = new QRadioButton(i18n("Normal"),resgroup,"normal");
  hbl2->addSpacing( 30 );
  hbl2->addWidget( normal );

  vbl->addSpacing( 20 );

  QHBoxLayout *hbl3 = new QHBoxLayout();
  vbl->addLayout( hbl3 );

  lsblabel = new QLabel(i18n("Raw fax data are:"), bg,"lsblabel");
  hbl3->addSpacing( 10 );
  hbl3->addWidget( lsblabel );

  lsb = new QCheckBox(i18n("LS-Bit first"), bg,"lsbitfirst");
  hbl3->addSpacing( 10 );
  hbl3->addWidget( lsb );

  vbl->addSpacing( 15 );

  QHBoxLayout *hbl9 = new QHBoxLayout();
  vbl->addLayout( hbl9 );
  hbl9->addSpacing( 10 );

  rawgroup = new QButtonGroup(bg,"rawgroup");
  hbl9->addWidget( rawgroup );

  QHBoxLayout *hbl4 = new QHBoxLayout( rawgroup );

  rawgroup->setFrameStyle(QFrame::NoFrame);

  rawlabel = new QLabel(i18n("Raw fax format:"),rawgroup,"rawlabel");
  rawlabel->setFixedSize( rawlabel->sizeHint() );
  hbl4->addWidget( rawlabel );

  g3 = new QRadioButton("G3",rawgroup,"g3");
  connect(g3,SIGNAL(clicked()), this, SLOT(g3toggled()));
  hbl4->addSpacing( 20 );
  hbl4->addWidget( g3 );

  g32d = new QRadioButton("G32d",rawgroup,"g32d");
  connect(g32d,SIGNAL(clicked()), this,SLOT(g32toggled()));
  hbl4->addSpacing( 30 );
  hbl4->addWidget( g32d );

  g4 = new QRadioButton("G4",rawgroup,"g4");
  connect(g4,SIGNAL(clicked()), this, SLOT(g4toggled()));
  hbl4->addSpacing( 30 );
  hbl4->addWidget( g4 );

  vbl->addSpacing( 20 );

  QHBoxLayout *hbl5 = new QHBoxLayout();
  vbl->addLayout( hbl5 );

  widthlabel = new QLabel(i18n("Raw fax width:"),bg,"widthlabel");
  hbl5->addSpacing( 10 );
  hbl5->addWidget( widthlabel );

  widthedit = new KIntNumInput(1, bg);
  widthedit->setRange(1, 10000, 1, false);
  hbl5->addWidget( widthedit );

  heightlabel = new QLabel(i18n("Height:"),bg,"heightlabel");
  hbl5->addSpacing( 10 );
  hbl5->addWidget( heightlabel );

  heightedit = new KIntNumInput(1, bg);
  heightedit->setRange(0, 100000, 1, false);
  hbl5->addWidget( heightedit );

  geomauto = new QCheckBox(i18n("Auto"),bg,"geomauto");
  connect(geomauto,SIGNAL(clicked()),this,SLOT(geomtoggled()));
  hbl5->addSpacing( 10 );
  hbl5->addWidget( geomauto );
}


struct optionsinfo *  OptionsDialog::getInfo(){

  if(resauto->isChecked())
    oi.resauto = 1;

  if(fine->isChecked())
    oi.fine = 1;
  else
    oi.fine = 0;

  if(landscape->isChecked())
    oi.landscape = 1;
  else
    oi.landscape = 0;

  if(flip->isChecked())
    oi.flip = 1;
  else
    oi.flip = 0;

  if(invert->isChecked())
    oi.invert = 1;
  else
    oi.invert = 0;

  if(lsb->isChecked())
    oi.lsbfirst = 1;
  else
    oi.lsbfirst = 0;

  if(geomauto->isChecked())
    oi.geomauto = 1;


  if(g3->isChecked()){
    oi.raw = 3;
  }

  if(g32d->isChecked()){
    oi.raw = 2;
    oi.geomauto = 0;

  }

  if(g4->isChecked()){
    oi.raw = 4;
    oi.geomauto = 0;
  }

  oi.height = heightedit->value();
  oi.width = widthedit->value();



  return &oi;

}

void OptionsDialog::setWidgets(struct optionsinfo* newoi ){

  if(!newoi)
    return;

  if(newoi->resauto == 1){
    resauto->setChecked(newoi->resauto);
    fine->setChecked(!newoi->resauto);
    normal->setChecked(!newoi->resauto);
  }
  else{
    if(newoi->fine == 1){
      resauto->setChecked(FALSE);
      fine->setChecked(TRUE);
      normal->setChecked(FALSE);
    }
    else{
      resauto->setChecked(FALSE);
      fine->setChecked(FALSE);
      normal->setChecked(TRUE);
    }
  }

  if(newoi->landscape == 1)
    landscape->setChecked(TRUE);
  else
    landscape->setChecked(FALSE);

  if(newoi->flip == 1)
    flip->setChecked(TRUE);
  else
    flip->setChecked(FALSE);

  if(newoi->invert == 1)
    invert->setChecked(TRUE);
  else
    invert->setChecked(FALSE);

  if(newoi->lsbfirst == 1)
    lsb->setChecked(TRUE);
  else
    lsb->setChecked(FALSE);

  if(newoi->raw == 3){
    geomauto->setEnabled(TRUE);
    g3->setChecked(TRUE);
  }

  if(newoi->raw == 2){
    geomauto->setEnabled(FALSE);
    g32d->setChecked(TRUE);
  }

  if(newoi->raw == 4){
    geomauto->setEnabled(FALSE);
    g4->setChecked(TRUE);
  }
  widthedit->setValue(newoi->width);
  heightedit->setValue(newoi->height);

  // auto height and width can only work with g3 faxes
  if(newoi->geomauto == 1 && newoi->raw != 4 && newoi->raw != 2){
    geomauto->setChecked(TRUE);
    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);
  }
  else{
    geomauto->setChecked(FALSE);
    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}


void OptionsDialog::g32toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);

}

void OptionsDialog::g4toggled(){

  geomauto->setChecked(FALSE);
  geomauto->setEnabled(FALSE);
  widthedit->setEnabled(TRUE);
  heightedit->setEnabled(TRUE);


}


void OptionsDialog::g3toggled(){

  geomauto->setEnabled(TRUE);
  geomauto->setChecked(TRUE);
  widthedit->setEnabled(FALSE);
  heightedit->setEnabled(FALSE);


}

void OptionsDialog::geomtoggled(){

  if(geomauto->isChecked()){

    widthedit->setEnabled(FALSE);
    heightedit->setEnabled(FALSE);

  }
  else{

    widthedit->setEnabled(TRUE);
    heightedit->setEnabled(TRUE);

  }

}

void OptionsDialog::slotHelp(){
  kapp->invokeHelp();
}


#include "options.moc"
