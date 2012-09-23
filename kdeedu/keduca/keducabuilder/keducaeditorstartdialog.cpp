/***************************************************************************
                          keducaeditorstartdialog.cpp  -  description
                             -------------------
    begin                : Fri May 10 2002
    copyright            : (C) 2002 by Klas Kalass
    email                : klas@kde.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <kurlrequester.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kconfig.h>
#include <kcombobox.h>

#include "keducaeditorstartdialog.h"
#include "keducabuilder.h"

/**
 */
KEducaEditorStartDialog::KEducaEditorStartDialog(KEducaBuilder * parent, const char* name, bool modal, WFlags fl)
    :KEducaEditorStartDialogBase(parent,  name,  modal, fl),
     _choice(NewDoc),
     _maxRecentDocumentItems(20)
{
    newDocumentRB->setFocus();

    // read recent files
    buildRecentFilesList();

// setup connections
    urlRequester->setFilter( "application/x-edu");

    connect( startChoiceBtnGrp,  SIGNAL( clicked(int) ),
             this, SLOT( slotChoiceChanged(int) ) );
}

/** based on code in kaction.cpp
 */
void KEducaEditorStartDialog::buildRecentFilesList()
{
    QString     key;
    QString     value;
    QString     oldGroup;
    KConfig *config = KGlobal::config();

    oldGroup = config->group();

    config->setGroup( "RecentFiles" );

    // read file list
    for( unsigned int i = 1 ; i <= _maxRecentDocumentItems ; i++ )
    {
        key = QString( "File%1" ).arg( i );
        value = config->readEntry( key, QString::null );

        if (!value.isNull())
            recentDocumentCB->insertURL( KURL(value) );
    }

    config->setGroup( oldGroup );
}

KEducaEditorStartDialog::~KEducaEditorStartDialog() {

}

void KEducaEditorStartDialog::slotChoiceChanged(int id) {
    _choice = id+1;
}

KURL KEducaEditorStartDialog::getURL()const{
    switch(_choice) {
    case OpenDoc:
        return KURL(urlRequester->url());
        break;
    case OpenRecentDoc:
        return KURL(recentDocumentCB->currentText());
        break;
    default:
        break;
    }

    return KURL();
}

int KEducaEditorStartDialog::exec() {
    int ret = KEducaEditorStartDialogBase::exec();
    if ( ret == KEducaEditorStartDialogBase::Rejected )
        _choice = KEducaEditorStartDialog::Rejected;

    return _choice;
}

void KEducaEditorStartDialog::accept() {
    if (((_choice == OpenDoc || _choice == OpenRecentDoc)
          && getURL().isEmpty())) {
        KMessageBox::sorry(this, i18n("You need to specify the file to open!"));
    }else
        KEducaEditorStartDialogBase::accept();
}

#include "keducaeditorstartdialog.moc"
