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

#ifndef IMAGESETTINGS_H
#define IMAGESETTINGS_H

#include <kdeprint/kprintdialogpage.h>

class QCheckBox;

class ImageSettings : public KPrintDialogPage
{
	Q_OBJECT
	public:
		ImageSettings( QWidget * parent = 0, const char * name = 0 );
		~ImageSettings();

		void setOptions( const QMap<QString, QString> & opts );
		void getOptions( QMap<QString, QString> & opts, bool include_def = false );
		bool isValid( QString & msg );

	private:
		QCheckBox * m_pFitImage;
		QCheckBox * m_pCenter;
};

// vim:sw=4:ts=4

#endif // IMAGESETTINGS_H

