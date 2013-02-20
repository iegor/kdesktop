/*
 *  This file is part of the KDE libraries
 *  Copyright (c) 2005 Helge Deller <deller@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include "kfax_printsettings.h"

#include <klocale.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>

KFAXPrintSettings::KFAXPrintSettings(QWidget *parent, const char *name)
: KPrintDialogPage(parent, name)
{
	QString whatsThisScaleFullPage = i18n( "<qt>"
		"<p><strong>'Ignore Paper Margins'</strong></p>"
		"<p>"
		"If this checkbox is enabled, the paper margins will be ignored "
		"and the fax will be printed on the full paper size."
		"</p>"
		"<p>"
		"If this checkbox is disabled, KFax will respect the standard paper "
		"margins and print the fax inside this printable area."
		"</p>"
						" </qt>" );
	QString whatsThisCenterHorz = i18n( "<qt>"
		"<p><strong>'Horizontal centered'</strong></p>"
		"<p>"
		"If this checkbox is enabled, the fax will be centered horizontally "
		"on the page."
		"</p>"
		"<p>"
		"If this checkbox is disabled, the fax will be printed at the left "
		"side of the page."
		"</p>"
						" </qt>" );
	QString whatsThisCenterVert = i18n( "<qt>"
		"<p><strong>'Vertical centered'</strong></p>"
		"<p>"
		"If this checkbox is enabled, the fax will be centered vertically "
		"on the page."
		"</p>"
		"<p>"
		"If this checkbox is disabled, the fax will be printed at the top "
		"of the page."
		"</p>"
						" </qt>" );

	setTitle(i18n("&Layout"));

	m_scaleFullPage = new QCheckBox(i18n("Ignore paper margins"), this);
	QWhatsThis::add(m_scaleFullPage, whatsThisScaleFullPage);
	m_center_horz = new QCheckBox(i18n("Horizontal centered"), this);
	QWhatsThis::add(m_center_horz, whatsThisCenterHorz);
	m_center_vert = new QCheckBox(i18n("Vertical centered"), this);
	QWhatsThis::add(m_center_vert, whatsThisCenterVert);

	QVBoxLayout *l0 = new QVBoxLayout(this, 0, 10);
	l0->addWidget(m_scaleFullPage);
	l0->addWidget(m_center_horz);
	l0->addWidget(m_center_vert);
	l0->addStretch(1);
}

KFAXPrintSettings::~KFAXPrintSettings()
{
}

void KFAXPrintSettings::getOptions(QMap<QString,QString>& opts, bool /*incldef*/)
{
	opts[APP_KFAX_SCALE_FULLPAGE] = (m_scaleFullPage->isChecked() ? "true" : "false");
	opts[APP_KFAX_CENTER_HORZ]    = (m_center_horz->isChecked() ? "true" : "false");
	opts[APP_KFAX_CENTER_VERT]    = (m_center_vert->isChecked() ? "true" : "false");
}

void KFAXPrintSettings::setOptions(const QMap<QString,QString>& opts)
{
	m_scaleFullPage->setChecked(opts[APP_KFAX_SCALE_FULLPAGE] == "true");
	m_center_horz->setChecked(opts[APP_KFAX_CENTER_HORZ] != "false");
	m_center_vert->setChecked(opts[APP_KFAX_CENTER_VERT] == "true");
}

#include "kfax_printsettings.moc"
