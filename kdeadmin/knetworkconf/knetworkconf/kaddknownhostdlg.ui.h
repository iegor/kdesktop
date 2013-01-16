/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "kaddressvalidator.h"
#include "kadddnsserverdlg.h"

#include <kmessagebox.h>

bool _modifiedhost;

void KAddKnownHostDlg::init()
{
    _modifiedhost = false;
    makeButtonsResizeable();    
}    
void KAddKnownHostDlg::validateAddressSlot()
{
  if (!KAddressValidator::isValidIPAddress(kleIpAddress->text()))
  {
       KMessageBox::error(this,i18n("The format of the specified IP address is not valid."),i18n("Invalid IP Address"));
  }
  else if ((klbAliases->firstItem() == 0))     
  {
      KMessageBox::error(this,i18n("You must add at least one alias for the specified IP address."),i18n("Insufficient Aliases"));      
  }   
  else
  {
    _modifiedhost = true;
    close();
  }  

}


void KAddKnownHostDlg::makeButtonsResizeable()
{
    kpbAdd->setAutoResize(true);
    kpbCancel->setAutoResize(true);
    kpbAddHost->setAutoResize(true);
    kpbEditHost->setAutoResize(true);
    kpbRemoveHost->setAutoResize(true);
}


bool KAddKnownHostDlg::modified()
{
    return _modifiedhost;
}


void KAddKnownHostDlg::editHostSlot()
{
  KAddDNSServerDlg dlg(this, 0);
  
  if (klbAliases->currentItem() >= 0)
  {
    int currentPos = klbAliases->currentItem();
    dlg.setCaption(i18n("Edit Alias"));
    dlg.lIPAddress->setText(i18n("Alias:"));
    dlg.setAddingAlias(true);
    QListBoxItem *item = klbAliases->item(currentPos);
    QString currentText = item->text();
    dlg.kleNewServer->setText(currentText);
    dlg.kpbAddServer->setText(i18n("&OK"));
    dlg.exec();
  
    if(dlg.modified())
    {
      klbAliases->changeItem(dlg.kleNewServer->text(),currentPos);
//      enableApplyButtonSlot();
    }
  }    
}


void KAddKnownHostDlg::removeHostSlot()
{
  if (klbAliases->currentItem() >= 0)
    klbAliases->removeItem(klbAliases->currentItem());
}


void KAddKnownHostDlg::addHostSlot()
{
  KAddDNSServerDlg addDlg(this, 0);
  addDlg.setCaption(i18n("Add New Alias"));
  addDlg.lIPAddress->setText(i18n("Alias:"));
  addDlg.setAddingAlias(true);
  
  addDlg.exec();
  if(addDlg.modified())
  {
    klbAliases->insertItem(addDlg.kleNewServer->text());
//    enableApplyButtonSlot();
  }

}
