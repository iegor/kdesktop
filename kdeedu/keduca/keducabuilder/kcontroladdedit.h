/***************************************************************************
                          kcontroladdedit.h  -  description
                             -------------------
    begin                : Sun May 27 2001
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

#ifndef KCONTROLADDEDIT_H
#define KCONTROLADDEDIT_H

#include "../libkeduca/fileread.h"
#include "kcontroladdeditbase.h"

// Qt forward declarations
class QListViewItem;

/**Add or edit questions
 * This is the Wizard that allows adding and editing of
 * Questions (and corresponding possible answers) to the
 * KEduca Document.
 * This will be called from the Editor (class KEducaBuilder)
 *@author Javier Campos
 * @author Klas Kalass (2003)
 */

class KControlAddEdit : public KControlAddEditBase  {
    Q_OBJECT

public:
    KControlAddEdit(QWidget *parent = 0, const char *name = 0, bool modal = true, WFlags f = 0);
    ~KControlAddEdit();
    /** Init keducaFile pointer and EditMode */
    void init(FileRead *keducaFile, bool editMode);
    /** show current Page */
    void showPage(QWidget *page);

private:

    // Private methods

    /** Init graphical interface */
//  void initGUI();
    /** Fill page of current record */
    void fillPage();
    /** Add question with form data */
    void addQuestion();
    /** Write config settings */
    void configWrite();
    /** Read config settings */
    void configRead();
    void setCurrentAnswerItem(QListViewItem *item);
    void resetAnswerValues();

    // Private attributes
    /** Pointer to keduca of kcontrol page */
    FileRead *_keducaFile;
    /** The currently selected answer item, important for editing of the answer*/
    QListViewItem *_currentAnswerItem;
    /** Is a edit modify? */
    bool _editMode;

private slots:
    /** Accept changes */
    void accept();
    /** An Answer has been selected */
    void slotAnswerSelected(QListViewItem *item);
    /** Remove answer */
    void slotRemoveAnswer();
    /** Button insert */
    void slotAddAnswer();
    /** Button Down */
    void slotMoveDownAnswer();
    /** Button Up */
    void slotMoveUpAnswer();
    /** Question or Answer data changed */
    void slotDataChanged();
    /** Preview image */
    void slotPreviewImage( const QString & );
    /** Change question type */
    void slotQuestionTypeChanged( int index );
};

#endif
