/***************************************************************************
                          siggenerator.h  -  description
                             -------------------
    begin                : Sat Jul 20 2002
    copyright            : (C) 2002 by Scott Wheeler
    email                : wheeler@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIGGENERATOR_H
#define SIGGENERATOR_H

#include <qstring.h>
#include <qdom.h>

class SigGenerator 
{
public: 
    SigGenerator();
    ~SigGenerator();

    QString random() const;
    QString daily() const;

private:

    QString attachStandard(const QString &text) const;

    QDomDocument doc;
    QDomNodeList signatures;
    QString header;
    QString footer;
};

#endif
