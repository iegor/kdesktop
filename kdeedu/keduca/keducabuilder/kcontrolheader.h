/***************************************************************************
                          kcontrolheader.h  -  description
                             -------------------
    begin                : Tue May 29 2001
    copyright            : (C) 2001 by Javier Campos
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KCONTROLHEADER_H
#define KCONTROLHEADER_H

#include "../libkeduca/fileread.h"
#include "klangcombo.h"

#include <kdialogbase.h>
#include <kurlrequester.h>


/**Information header
 * Options Dialog for KEduca Document.
 * Will be called for each new KEduca Document and from the Editor (class KEducaBuilder)
 *@author Javier Campos
 */

class KControlHeader : public KDialogBase  {
    Q_OBJECT

public:
    KControlHeader(FileRead *keducaFile, QWidget *parent=0, const char *name=0);
    ~KControlHeader();

private:

    // Private attributes

    /** Keduca pointer */
    FileRead *_keducaFile;
    /** Title */
    QLineEdit *_headerTitle;
    /** Header type */
    KComboBox *_headerType;
    /** Header category */
    KComboBox *_headerCategory;
    /** Header level */
    KComboBox *_headerLevel;
    /** Header lang */
    KLanguageCombo *_headerLang;
    /** Header picture */
    KURLRequester *_headerPicture;
    /** Header name */
    QLineEdit *_headerName;
    /** Header email */
    QLineEdit *_headerEmail;
    /** Header www */
    QLineEdit *_headerWWW;
    /** Default categories */
    QStringList _defaultCategories;

    // Private methods

    /** Init graphical interface */
    void init();
    /** Insert header */
    void insertHeader();
    /** Load country list */
    void loadCountryList(KLanguageCombo *combo);
    /** Write settings */
    void configWrite();
    /** Read settings */
    void configRead();

private slots:
    /** Ok push */
    void slotOk();
};

#endif
