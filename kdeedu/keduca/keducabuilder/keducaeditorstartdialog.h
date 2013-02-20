/***************************************************************************
                          keducaeditorstartdialog.h  -  description
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

#ifndef KEDUCAEDITORSTARTDIALOG_H
#define KEDUCAEDITORSTARTDIALOG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "keducaeditorstartdialogbase.h"

// forward declarations
class KEducaBuilder;

/**
 */
class KEducaEditorStartDialog : public KEducaEditorStartDialogBase{
    Q_OBJECT
public:
    enum {Rejected=0, OpenDoc=1, OpenRecentDoc=2, Gallery=3, NewDoc=4}StartChoice;
    /** Constructor.
     */
    KEducaEditorStartDialog(KEducaBuilder * parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    /** Destructor.
     */
    virtual ~KEducaEditorStartDialog();

    KURL getURL()const;
    int exec();

private:
    KURL _selectedURL;
    int _choice;
    unsigned int const _maxRecentDocumentItems;
    void buildRecentFilesList();

protected slots:
    void accept();

private slots:
    void slotChoiceChanged(int id);

};
#endif
