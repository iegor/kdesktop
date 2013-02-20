/***************************************************************************
                          standardtext.cpp  -  description
                             -------------------
    copyright            : (C) 2003 by Scott Wheeler
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

#include <ktextedit.h>

#include <qlabel.h>

#include "standardtext.h"

StandardText::StandardText(QWidget *parent, const char *name, bool modal, WFlags fl) : StandardTextBase(parent, name, modal, fl)
{

}

StandardText::~StandardText()
{

}

void StandardText::setLabel(const QString &text)
{
    label->setText(text);
}

void StandardText::setText(const QString &text)
{
    textEdit->setText(text);
}

void StandardText::done(int r)
{
    if(r == QDialog::Accepted)
	emit textUpdated(textEdit->text());
 
    StandardTextBase::done(r);
}

#include "standardtext.moc"
