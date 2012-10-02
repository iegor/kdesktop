/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include <kmessagebox.h>
#include "toolselectionwidget.h"

void EditorPreferences::ledWarning( bool show )
{
if(show)
      KMessageBox::information( this,i18n("This option takes no effect until KBabel is restarted.") );
}


void EditorPreferences::toggleOther( bool other )
{
    radioButton2->setChecked(! other);
}
