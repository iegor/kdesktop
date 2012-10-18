/*
 * pref.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2005-2007 Christophe Thommeret <hftom@free.fr>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "pref.h"
#include "pref.moc"

#include <kglobal.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlineedit.h>

#include <qtextcodec.h>

#include <kcharsets.h>

KaffeinePreferences::KaffeinePreferences() : KDialogBase(IconList,i18n("Kaffeine Setup"), Ok|Apply|Cancel, Ok)
{
	QGroupBox *gb;
	QVBoxLayout *vb;
	QGridLayout *grid;
	KIconLoader *icon = new KIconLoader();

	setInitialSize(QSize(400,400), false);

//-----------behavior----------------
	QFrame *behavior = addPage(i18n("Behavior"), i18n("Behavior"), KGlobal::iconLoader()->loadIcon("kaffeine", KIcon::Panel, KIcon::SizeMedium));
	vb = new QVBoxLayout( behavior, 6, 6 );
	gb = new QGroupBox( "", behavior );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );
	m_pauseVideo = new QCheckBox(i18n("Pause video when window is minimized"), gb);
	grid->addMultiCellWidget(m_pauseVideo, 0, 0, 0, 1);
	vb->addWidget( gb );

	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

//-----------appearance--------------
	QFrame *looknfeel = addPage(i18n("Appearance"), i18n("Appearance"),KGlobal::iconLoader()->loadIcon("looknfeel", KIcon::Panel, KIcon::SizeMedium));
	vb = new QVBoxLayout( looknfeel, 6, 6 );
	gb = new QGroupBox( "", looknfeel );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );
	m_systemTray = new QCheckBox(i18n("Embed in system tray"), gb);
	connect(m_systemTray, SIGNAL(toggled(bool)), this, SLOT(slotEmbedInTrayToggled(bool)));
	grid->addMultiCellWidget(m_systemTray, 0, 0, 0, 1);
	m_osdTimeout = new QSpinBox(gb);
	m_osdTimeout->setMinValue(0);
	m_osdTimeout->setMaxValue(60);
	m_osdTimeout->setSuffix(i18n(" sec"));
	m_osdTimeout->setSpecialValueText(i18n("off"));
	grid->addWidget(m_osdTimeout, 1,0);
	QLabel* osdTimeoutLabel = new QLabel(i18n("Duration of title announcement in system tray"), gb);
	osdTimeoutLabel->setDisabled(true);
	connect(m_systemTray, SIGNAL(toggled(bool)), osdTimeoutLabel, SLOT(setEnabled(bool)));
	grid->addWidget(osdTimeoutLabel, 1,1);
	vb->addWidget( gb );

	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

//-----------dvbclient--------------
	QFrame *dc = addPage(i18n("DVB Client"), i18n("DVB Client"),KGlobal::iconLoader()->loadIcon("network_local", KIcon::Panel, KIcon::SizeMedium));
	vb = new QVBoxLayout( dc, 6, 6 );
	m_dcEnabled = new QCheckBox(i18n("Enable DVB client"), dc);
	vb->addWidget( m_dcEnabled );
	gb = new QGroupBox( "", dc );
	gb->setDisabled(true);
	connect(m_dcEnabled, SIGNAL(toggled(bool)), gb, SLOT(setEnabled(bool)));
	grid = new QGridLayout( gb, 1, 1, 20, 6 );
	QLabel *lab = new QLabel( i18n("Broadcast address:"), gb );
	grid->addWidget( lab, 0, 0 );
	m_dcAddress = new QLineEdit( gb );
	grid->addWidget( m_dcAddress, 0, 1 );
	lab = new QLabel( i18n("Broadcast port:"), gb );
	grid->addWidget( lab, 1, 0 );
	m_dcPort = new QSpinBox( 1, 65535, 1, gb );
	grid->addWidget( m_dcPort, 1, 1 );
	lab = new QLabel( i18n("Info port:"), gb );
	grid->addWidget( lab, 2, 0 );
	m_dcInfo = new QSpinBox( 1, 65535, 1, gb );
	grid->addWidget( m_dcInfo, 2, 1 );

	lab = new QLabel( i18n("Time shifting directory:"), gb );
	grid->addWidget( lab, 3, 0 );
	m_shiftDirLe = new QLineEdit( gb );
	m_shiftDirLe->setReadOnly( true );
	grid->addWidget( m_shiftDirLe, 3, 1 );
	m_shiftDirBtn = new QToolButton( gb );
	m_shiftDirBtn->setIconSet( icon->loadIconSet("fileopen", KIcon::Small) );
	grid->addWidget( m_shiftDirBtn, 3, 2 );
	connect( m_shiftDirBtn, SIGNAL(clicked()), this, SLOT(setShiftDir()) );

	vb->addWidget( gb );
	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

//-----------misc----------------
	QFrame *misc = addPage(i18n("Misc"), i18n("Miscellaneous Options"),KGlobal::iconLoader()->loadIcon("misc", KIcon::Panel, KIcon::SizeMedium));
	vb = new QVBoxLayout( misc, 6, 6 );
	gb = new QGroupBox(1, Qt::Horizontal, "", misc);
	gb->setInsideMargin(20);
	gb->setInsideSpacing(6);

	m_useAlternateEncoding = new QCheckBox(i18n("Use alternate (non-Unicode) encoding for Meta tags"), gb);
	connect(m_useAlternateEncoding, SIGNAL(toggled(bool)), this, SLOT(slotUseAlternateEncodingToggled(bool)));

	m_alternateEncoding = new QComboBox(gb);
	QStringList codecNames = KGlobal::charsets()->descriptiveEncodingNames();
	codecNames.sort();
	m_alternateEncoding->insertStringList(codecNames);
	vb->addWidget( gb );

	gb = new QGroupBox( "", misc );
	grid = new QGridLayout( gb, 1, 1, 20, 6 );
	KPushButton* clearRecent = new KPushButton( i18n("Clear"), gb);
	clearRecent->setSizePolicy( QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Fixed));
	connect(clearRecent, SIGNAL(clicked()), this, SIGNAL(signalClearRecent()));
	grid->addWidget(clearRecent, 0, 0);
	QLabel* clearLabel = new QLabel(i18n("Clear recent files list"), gb);
	grid->addWidget(clearLabel, 0, 1);
	vb->addWidget( gb );

	vb->addItem( new QSpacerItem( 20, 20, QSizePolicy::Ignored, QSizePolicy::Ignored ) );

	delete icon;

	connect(this, SIGNAL(okClicked()), SLOT(slotOkPressed()));
	connect(this, SIGNAL(applyClicked()), SLOT(slotApplyPressed()));
}

void KaffeinePreferences::setConfig(bool pauseVideo, bool tray, uint duration, bool useEncoding, const QString& encoding)
{
	m_osdTimeout->setValue(duration);
	m_osdTimeout->setEnabled(tray);
	m_systemTray->setChecked(tray);

	bool m_useEncoding = false;
	for (int i = 0; i < m_alternateEncoding->count(); ++i) {
		if (KGlobal::charsets()->encodingForName(m_alternateEncoding->text(i)).lower() == encoding.lower()) {
			m_useEncoding = useEncoding;
			m_alternateEncoding->setCurrentItem(i);
			break;
		}
	}

	m_useAlternateEncoding->setChecked(m_useEncoding);
	m_alternateEncoding->setEnabled(m_useEncoding);
	m_pauseVideo->setChecked(pauseVideo);
}

void KaffeinePreferences::setDvbClient( bool enabled, const QString &address, int port, int info, const QString &tspath )
{
	m_dcEnabled->setChecked(enabled);
	m_dcAddress->setText(address);
	m_dcPort->setValue(port);
	m_dcInfo->setValue(info);
	m_shiftDirLe->setText(tspath);
}

void KaffeinePreferences::slotApplyPressed()
{
	emit signalEmbedSystemTray(m_systemTray->isChecked());
	emit signalUseAlternateEncoding(m_useAlternateEncoding->isChecked());
	emit signalAlternateEncoding(KGlobal::charsets()->encodingForName(m_alternateEncoding->currentText()));
	emit signalSetOSDTimeout(m_osdTimeout->value());
	emit signalPauseVideo(m_pauseVideo->isChecked());
	emit signalDvbClient(m_dcEnabled->isChecked(), m_dcAddress->text().stripWhiteSpace(), m_dcPort->value(), m_dcInfo->value(), m_shiftDirLe->text() );
}

void KaffeinePreferences::slotOkPressed()
{
	slotApplyPressed();
	hide();
}

void KaffeinePreferences::setShiftDir()
{
	QString s = KFileDialog::getExistingDirectory( m_shiftDirLe->text().stripWhiteSpace() );
	if ( !s.isEmpty() )
		m_shiftDirLe->setText( s );
}

void KaffeinePreferences::slotUseAlternateEncodingToggled(bool on)
{
	m_alternateEncoding->setEnabled(on);
}

void KaffeinePreferences::slotEmbedInTrayToggled(bool on)
{
	m_osdTimeout->setEnabled(on);
}
