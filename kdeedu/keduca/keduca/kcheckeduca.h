/***************************************************************************
                          kcheckeduca.h  -  description
                             -------------------
    begin                : Tue Sep 5 2000
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

#ifndef KCHECKEDUCA_H
#define KCHECKEDUCA_H

#include <qcheckbox.h>

#include <qsimplerichtext.h>

/**KCheck with wordwrap and rich text support
 *@author Javier Campos Morales
 */

class KCheckEduca : public QCheckBox  {
    Q_OBJECT

public:
    KCheckEduca(QWidget *parent=0, const char *name=0);
    ~KCheckEduca();

protected:
    void drawButtonLabel( QPainter *);

private:
    /** Rich text */
    QSimpleRichText *_doc;
};

#endif
