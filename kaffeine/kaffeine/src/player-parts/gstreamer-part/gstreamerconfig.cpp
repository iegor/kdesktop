/*
 * gstreamerconfig.cpp - config dialog for gstreamer parameters
 *
 * Copyright (C) 2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <kcombobox.h>
#include <klineedit.h>
#include <kseparator.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>

#include "gstreamerconfig.h"
#include "gstreamerconfig.moc"


GStreamerConfig::GStreamerConfig(const QStringList& audioDrivers, const QStringList& videoDrivers) :
		KDialogBase(KDialogBase::IconList, i18n("GStreamer Engine Parameters"), KDialogBase::Ok|KDialogBase::Cancel,
		            KDialogBase::Cancel)

{
	setInitialSize(QSize(400,300), true);

	QFrame* frame = NULL;
	QGridLayout* layout = NULL;
	QLabel* label = NULL;

	//Audio Page
	frame = addPage(i18n("Audio"), i18n("Audio Options"), KGlobal::iconLoader()->loadIcon("sound", KIcon::Panel,
	                KIcon::SizeMedium));
	layout = new QGridLayout(frame, 10, 2);
	layout->setMargin(10);
	layout->setSpacing(10);
	m_audioDriverBox = new KComboBox(frame);
	m_audioDriverBox->insertStringList(audioDrivers);
	label = new QLabel(i18n("Prefered audio driver"), frame);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_audioDriverBox, 1, 1);
	layout->addMultiCellWidget(new KSeparator(KSeparator::Horizontal, frame), 2, 2, 0, 1);

	//Video Page
	frame = addPage(i18n("Video"), i18n("Video Options"), KGlobal::iconLoader()->loadIcon("video", KIcon::Panel,
	                KIcon::SizeMedium));
	layout = new QGridLayout(frame, 10, 2);
	layout->setMargin(10);
	layout->setSpacing(10);
	m_videoDriverBox = new KComboBox(frame);
	m_videoDriverBox->insertStringList(videoDrivers);
	label = new QLabel(i18n("Prefered video driver")+ "*", frame);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_videoDriverBox, 1, 1);
	layout->addMultiCellWidget(new KSeparator(KSeparator::Horizontal, frame), 2, 2, 0, 1);
	layout->addWidget(new QLabel(QString("<small>") + i18n("* Restart required!") + "</small>", frame), 10, 1);

	//Media page
	frame = addPage(i18n("Media"), i18n("Media Options"), KGlobal::iconLoader()->loadIcon("cdrom_unmount", KIcon::Panel,
	                KIcon::SizeMedium));
	layout = new QGridLayout(frame, 10, 2);
	layout->setMargin(10);
	layout->setSpacing(10);
	m_driveEdit = new KLineEdit(frame);
	label = new QLabel(i18n("CD, VCD, DVD drive"), frame);
	layout->addWidget(label, 1, 0);
	layout->addWidget(m_driveEdit, 1, 1);
	layout->addMultiCellWidget(new KSeparator(KSeparator::Horizontal, frame), 2, 2, 0, 1);
}

GStreamerConfig::~GStreamerConfig()
{}

QString GStreamerConfig::getAudioDriver() const
{
	return m_audioDriverBox->currentText();
}

QString GStreamerConfig::getVideoDriver() const
{
	return m_videoDriverBox->currentText();
}

QString GStreamerConfig::getDrive() const
{
	return m_driveEdit->text();
}

void GStreamerConfig::setDrive(const QString& drive)
{
	m_driveEdit->setText(drive);
}

void GStreamerConfig::setAudioDriver(const QString& name)
{
	m_audioDriverBox->setCurrentText(name);
}

void GStreamerConfig::setVideoDriver(const QString& name)
{
	m_videoDriverBox->setCurrentText(name);
}
