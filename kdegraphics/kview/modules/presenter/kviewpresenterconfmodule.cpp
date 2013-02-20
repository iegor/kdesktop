/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// $Id: kviewpresenterconfmodule.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "kviewpresenterconfmodule.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qframe.h>

#include <klocale.h>
#include <kdialog.h>
#include <kglobal.h>
#include <kconfig.h>

KViewPresenterConfModule::KViewPresenterConfModule( QObject * parent )
	: KPreferencesModule( "kviewpresenter", parent, "KView Presenter Config Module" )
{
}

KViewPresenterConfModule::~KViewPresenterConfModule()
{
}

void KViewPresenterConfModule::applyChanges()
{
	emit configChanged();
}

void KViewPresenterConfModule::reset()
{
}

void KViewPresenterConfModule::createPage( QFrame * page )
{
	QBoxLayout * layout = new QVBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );
	layout->setAutoAdd( true );

	m_pCheckBox = new QCheckBox( "This is only for testing...", page );
}

// vim:sw=4:ts=4

#include "kviewpresenterconfmodule.moc"
