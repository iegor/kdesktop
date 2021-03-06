/***************************************************************************
 *   Copyright (C) 2002 by Bernd Gehrmann                                  *
 *   bernd@kdevelop.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _PYTHONCONFIGWIDGET_H_
#define _PYTHONCONFIGWIDGET_H_

#include "pythonconfigwidgetbase.h"
#include <qdom.h>


class PythonConfigWidget : public PythonConfigWidgetBase
{ 
    Q_OBJECT

public:
    PythonConfigWidget( QDomDocument &projectDom, QWidget *parent=0, const char *name=0 );
    ~PythonConfigWidget();

public slots:
    void accept();

private:
    QDomDocument &dom;
};

#endif
