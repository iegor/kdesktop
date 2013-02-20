/***************************************************************************
                          keducabuilder.h  -  description
                             -------------------
    begin                : Sat May 26 2001
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

#ifndef KEDUCABUILDER_H
#define KEDUCABUILDER_H

#include "../libkeduca/fileread.h"

#include <kmainwindow.h>

#include <klistbox.h>
#include <kaction.h>

#include <qtextview.h>
#include <qsplitter.h>

/**Main control documents
 * The Editor. This is the new main window of KEduca.
 * You can edit a KEduca Document (i.e. Test/Examen) here.
 *@author Javier Campos
 */

class KEducaBuilder : public KMainWindow  {
    Q_OBJECT

public:
    KEducaBuilder(QWidget* parent=0, const char *name=0, WFlags f = WType_TopLevel | WDestructiveClose);
    ~KEducaBuilder();
    /** start a dialog that asks the user what he wants to do to start*/
    bool startChooser();
    /** Init Internet Gallery Servers */
    void initGallery(const KURL &urlFile = KURL());

private:

    // Private methods

    /** Init graphical interface */
    void init();
    /** Init menu bar settings */
    void initMenuBar();
    /** Write settings */
    void configWrite();
    /** Read config data */
    void configRead();
    /** Check if the current document can save */
    bool checkSave();

    /** Save the file, return false if save failed for any reason */
    bool fileSave();
    /** Save the file under a user-chosen filename, return false if save as
        failed for any reason */
    bool fileSaveAs();

    /** Ask the user if he wants to save the file if it has been changed.
     Returns true if the file may be replaced (that is when he successfully saved it or said "Discard") */
    bool currentFileMayBeReplaced();

    // Private attributes

    /** KEDUCA file */
    FileRead *_keducaFile;
    /** Recent files */
    KRecentFilesAction *_recentFiles;
    /** List of answers */
    KListBox *_listAnswer;
    /** Preview */
    QTextView *_preview;
    /** Main splitter */
    QSplitter *_split;
    /** I open a file */
    bool _isOpenFile;
    QString _nativeFormat;

public slots:
    /** Open new file */
    void slotFileOpenURL( const KURL &url );
    /** Delete current document and start new */
    void slotFileOpenNew();

private slots:
    /** Open new document. */
    void slotFileOpen();
    /** Item select */
    void slotPreview( QListBoxItem *item );
    /** Close the window */
    bool queryClose();
    /** Edit question */
    void slotEdit();
    /** Add question */
    void slotAdd();
    /** Remove question */
    void slotRemove();
    /** Down record */
    void slotDown();
    /** Up record */
    void slotUp();
    /** Save As... */
    void slotFileSaveAs();
    /** Save file */
    void slotFileSave();
    /** Print file */
    void slotFilePrint();
    /** Header info */
    int slotHeader();
    /** Init Gallery */
    void slotGallery();
    /** Double click in list, edit question */
    void slotEditbyList(QListBoxItem *item);
};

#endif
