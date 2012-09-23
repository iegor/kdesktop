/***************************************************************************
                          keduca.h  -  description
                             -------------------
    begin                : mié may 23 20:36:15 CEST 2001
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

#ifndef KEDUCA_H
#define KEDUCA_H

/*
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
*/

#include <kapplication.h>
#include <kparts/mainwindow.h>
#include <kparts/part.h>
#include <kaction.h>

/** The KEduca TestMaster.
    This will execute the Test/Examen as defined by the KEduca Document. */
class Keduca : public KParts::MainWindow
{
    Q_OBJECT

public:
    /** construtor */
    Keduca(QWidget* parent=0, const char *name=0, WFlags f = WType_TopLevel | WDestructiveClose);
    /** destructor */
    ~Keduca();
  /** Open gallery window */
  void initGallery(const KURL &urlFile=KURL());

private:
    /** Configures the actions */
    void setupActions();
    /** Write settings */
    void configWrite();
    /** Read settings */
    void configRead();
    /** Close the window */
    bool queryClose();

    // Private attributes

    /** Recent files */
    KRecentFilesAction *_recentFiles;
    /** A KEDUCA file main view */
    KParts::ReadOnlyPart *_part;

public slots:
    /** Open a file */
    void slotFileOpenURL( const KURL &url );
    /** Open new file */
    void slotFileOpen();

private slots:
    /** Close actual document */
    /*void slotFileClose();*/
    /** Open gallery dialog */
    void slotGallery();
};

#endif
