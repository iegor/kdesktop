/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include "kaddressvalidator.h"
#include <kmessagebox.h>

bool _modified2;
bool addingAlias;

void KAddDNSServerDlg::setAddingAlias(bool add)
{
  addingAlias = add;
}

bool KAddDNSServerDlg::modified()
{
   return _modified2;   
}    
void KAddDNSServerDlg::init()
{
   _modified2 = false; 
   addingAlias = false;
   makeButtonsResizeable();    
}    

void KAddDNSServerDlg::validateAddressSlot()
{
  if (!addingAlias)
  {
    if (KAddressValidator::isValidIPAddress(kleNewServer->text()))
    {   
       _modified2 = true; 
      close();
    }
    else
    {
      KMessageBox::error(this,i18n("The format of the specified IP address is not valid."),i18n("Invalid IP Address"));
    }  
  }
  else 
  {
    if (kleNewServer->text() !="")
    {
       _modified2 = true; 
       close();  
    }
    else
    {
       KMessageBox::error(this,i18n("You have to type an alias first."),i18n("Invalid Text"));
    }
  }  
}


void KAddDNSServerDlg::makeButtonsResizeable()
{
  kpbAddServer->setAutoResize(true);
  kpbCancel->setAutoResize(true);
}
