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

// $Id: kviewpresenterconfmodule.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef KVIEWPRESENTERCONFMODULE_H
#define KVIEWPRESENTERCONFMODULE_H

#include "kpreferencesmodule.h"

class QCheckBox;

class KViewPresenterConfModule : public KPreferencesModule
{
	Q_OBJECT
	public:
		KViewPresenterConfModule( QObject * parent );
		~KViewPresenterConfModule();

	signals:
		void configChanged();

	protected:
		virtual void applyChanges();
		virtual void reset();
		virtual void createPage( QFrame * page );

	private:
		QCheckBox * m_pCheckBox;
};

// vim:sw=4:ts=4

#endif // KVIEWPRESENTERCONFMODULE_H
