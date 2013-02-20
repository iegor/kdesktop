/*
    $Id: options.h 465369 2005-09-29 14:33:08Z mueller $


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


#ifndef _OPTIONS_DIALOG_H_
#define _OPTIONS_DIALOG_H_

#include <qapplication.h>
#include <qfiledialog.h>
#include <kdialogbase.h>
#include <qstring.h>

class QGroupBox;
class QLabel;
class QButtonGroup;
class QRadioButton;
class QCheckBox;
class QRadioButton;


struct optionsinfo {
  int geomauto;
  int resauto;
  int width ;
  int height;
  int fine;
  int landscape;
  int flip;
  int invert;
  int lsbfirst;
  int raw;
};

class KIntNumInput;

class OptionsDialog : public KDialogBase {

    Q_OBJECT

public:
    OptionsDialog( QWidget *parent = 0, const char *name = 0);

    struct optionsinfo* getInfo();
    void setWidgets(struct optionsinfo *oi);

signals:

public slots:
      void slotHelp();
      void geomtoggled();
      void g32toggled();
      void g4toggled();
      void g3toggled();

private:

    QGroupBox	 *bg;
    QLabel 	*reslabel;
    QButtonGroup *resgroup;
    QRadioButton *fine;
    QRadioButton *resauto;
    QRadioButton *normal;
    QLabel	*displaylabel;
    QButtonGroup *displaygroup;
    QCheckBox *landscape;
    QCheckBox *geomauto;
    QCheckBox *flip;
    QCheckBox *invert;

    QButtonGroup  *lsbgroup;
    QLabel        *lsblabel;
    QCheckBox *lsb;
    QButtonGroup *rawgroup;
    QRadioButton *g3;
    QRadioButton *g32d;
    QRadioButton *g4;

    QLabel 	*rawlabel;

    QLabel 	*widthlabel;
    QLabel 	*heightlabel;
    KIntNumInput *widthedit;
    KIntNumInput *heightedit;

    struct optionsinfo oi;

};


#endif
