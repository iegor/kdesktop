/***************************************************************************
                          kradioeduca.h  -  description
                             -------------------
    begin                : Wed Sep 6 2000
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

#ifndef KRADIOEDUCA_H
#define KRADIOEDUCA_H


#include <qradiobutton.h>
#include <qsimplerichtext.h>

/**QRadioButton with wordwrap and richtext features
 *@author Javier Campos Morales
 */
class KRadioEduca : public QRadioButton {
    Q_OBJECT

public:
    KRadioEduca(QWidget *parent=0, const char *name=0);
    ~KRadioEduca();

protected:

    // Protected methods

    /** reimpl */
    void drawButtonLabel( QPainter *p);

private:

    // Private attributes

    /** Rich Text */
    QSimpleRichText *_doc;

};

#endif
