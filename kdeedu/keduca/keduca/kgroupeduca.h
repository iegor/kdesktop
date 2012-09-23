/***************************************************************************
                          kgroupeduca.h  -  description
                             -------------------
    begin                : Thu Sep 7 2000
    copyright            : (C) 2000 by Javier Campos Morales
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

#ifndef KGROUPEDUCA_H
#define KGROUPEDUCA_H

#include <qvbuttongroup.h>
#include <qscrollview.h>
#include <qvbox.h>

/**
 *@author Javier Campos Morales
 */
class KGroupEduca : public QVButtonGroup  {
    Q_OBJECT

public:
    enum ButtonType { Radio, Check };

    KGroupEduca(QWidget *parent=0, const char *name=0);
    ~KGroupEduca();
    /** Insert a check or radio button */
    void insertAnswer( const QString& text);
    /** Set type */
    void setType(ButtonType Type);
    /** Clear all kradio or kcheck answers */
    void clearAnswers();
    /** Return if is checked radio or check buttons */
    bool isChecked(int id);

protected:
    QVBox* _vbox2;

private:

    // Private attributes

    QScrollView *_sv;
    /** Type mode */
    ButtonType _typeMode;
};

#endif
