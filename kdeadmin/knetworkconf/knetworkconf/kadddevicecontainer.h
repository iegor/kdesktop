/***************************************************************************
                          kadddevicecontainer.h  -  description
                             -------------------
    begin                : Wed Jun 15 00:40:33 UTC 2005
    copyright            : (C) 2005 by Juan Luis Baptiste
    email                : juan.baptiste@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef KADDDEVICECONTAINER_H
#define KADDDEVICECONTAINER_H

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <kcombobox.h>
#include <kdialog.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpassdlg.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>

#include "kadddevicedlg.h"
#include "kadddevicedlgextension.h"
#include "kadddevicewifiext.h"
#include "kaddressvalidator.h"

/**
Network interface configuration dialog. This dialog contains the KAddDeviceDlg and KAddDeviceWifiExt widgets.

@author Juan Luis Baptiste
*/
class KAddDeviceContainer : public KDialog
{
  Q_OBJECT
  public:
    KAddDeviceContainer(QWidget *parent = 0, const char *name = 0);

    ~KAddDeviceContainer();
    KPushButton* kpbAdvanced;
    KPushButton* kpbApply;
    KPushButton* kpbCancel;
    void addButtons();
    KAddDeviceDlg *addDlg;
    KAddDeviceWifiExt *extDlg;
    void addWirelessWidget();
    bool modified();
    bool advanced();

  private:
    void makeButtonsResizeable();
  
  protected:
    QVBoxLayout* mainLayout;    
    QHBoxLayout* buttonsLayout;
    QSpacerItem* buttonsSpacer;
    QSpacerItem* widgetHSpacer;    
    bool _modified;
    bool _advanced;    
  
  protected slots:
    void toggleApplyButtonSlot( const QString & );
    void toggleApplyButtonSlot( int );
    void toggleAdvancedOptionsSlot(bool enabled );
    void verifyDeviceInfoSlot();
    void advancedOptionsSlot();
    void cancelSlot();    
};

#endif
