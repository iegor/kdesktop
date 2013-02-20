/***************************************************************************
                          siggenerator.cpp  -  description
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

#include "siggenerator.h"
#include "siglistviewitem.h"

#include <kstandarddirs.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>

#include <qfile.h>
#include <qdatetime.h>

SigGenerator::SigGenerator()
{
    QString dir = KGlobal::dirs()->saveLocation("appdata");
    QFile file(dir + "sigs.sigml");

    if(file.open(IO_ReadOnly) && doc.setContent(&file)) {
	signatures = doc.elementsByTagName("signature");
	file.close();
    }

    KConfig *config = KGlobal::config();
    {
	KConfigGroupSaver saver(config, "Settings");
	header = config->readEntry("Header");
	footer = config->readEntry("Footer");
    }
}

SigGenerator::~SigGenerator()
{

}


QString SigGenerator::random() const
{
    if(signatures.count() <= 0)
	return(QString::null);

    int i = KApplication::random() % signatures.count();
    QString text;
    SigListViewItem::nodeToText(signatures.item(i), text);
    
    return attachStandard(text);
}

QString SigGenerator::daily() const
{
    if(signatures.count() <= 0)
	return(QString::null);

    QDate date;
    int i = date.dayOfYear() % signatures.count();
    QString text;
    SigListViewItem::nodeToText(signatures.item(i), text);
    return attachStandard(text);
}

QString SigGenerator::attachStandard(const QString &text) const
{
    QString s = text;

    if(!header.isEmpty())
	s = header + "\n" + s;
    if(!footer.isEmpty())
	s = s + "\n" + footer + "\n";

    return s;
}
