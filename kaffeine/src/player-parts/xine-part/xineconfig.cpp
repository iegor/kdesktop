/*
 * xineconfig.cpp - config dialog for xine parameters
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
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

#include <kglobal.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kdialogbase.h>
#include <klocale.h>
#include <kdebug.h>
#include <kseparator.h>

#include <qcolor.h>
#include <qfont.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qscrollview.h>
#include <qsignalmapper.h>

#include "xineconfig.h"
#include "xineconfig.moc"

#define NON_EXPERT_OPTIONS_DEPRECATED "audio.speaker_arrangement;audio.driver;audio.mixer_software;video.driver;dxr3.device_number;dxr3.enc_add_bars;dxr3.enc_alt_play_mode;input.dvd_language;input.dvd_region;input.cdda_device;input.cdda_use_cddb;input.drive_slowdown;input.dvd_device;input.vcd_device;input.http_no_proxy;input.http_proxy_host;input.http_proxy_password;input.http_proxy_port;input.http_proxy_user;codec.real_codecs_path;codec.win32_path;post.goom_fps;post.goom_height;post.goom_width;misc.spu_subtitle_size;misc.spu_vertical_offset;misc.spu_src_encoding;misc.sub_timeout;osd.osd_messages;vcd.default_device;"

#define NON_EXPERT_OPTIONS_NEW "audio.output.speaker_arrangement;audio.driver;audio.mixer_software;video.driver;dxr3.device_number;dxr3.encoding.add_bars;dxr3.encoding.alt_play_mode;media.dvd.language;media.dvd.region;media.audio_cd.device;media.audio_cd.use_cddb;media.audio_cd.drive_slowdown;media.dvd.device;media.vcd.device;media.network.http_no_proxy;media.network.http_proxy_host;media.network.http_proxy_password;media.network.http_proxy_port;media.network.http_proxy_user;decoder.external.real_codecs_path;decoder.external.win32_codecs_path;effects.goom.csc_method;effects.goom.fps;effects.goom.height;effects.goom.width;subtitles.separate.subtitle_size;subtitles.separate.vertical_offset;subtitles.separate.src_encoding;subtitles.separate.timeout;media.vcd.device;osd.osd_messages;osd.osd_size"

#define NON_EXPERT_OPTIONS NON_EXPERT_OPTIONS_NEW NON_EXPERT_OPTIONS_DEPRECATED

XineConfigEntry::XineConfigEntry(QWidget* parent, QGridLayout* grid, int row, xine_cfg_entry_t* entry) :
		m_valueChanged(false), m_key(QString(entry->key)), m_numValue(entry->num_value),
		m_numDefault(entry->num_default), m_stringValue(entry->str_value), m_stringDefault(entry->str_default),
		m_stringEdit(NULL), m_enumEdit(NULL), m_numEdit(NULL), m_boolEdit(NULL)
{
	switch (entry->type)
	{
		case XINE_CONFIG_TYPE_UNKNOWN:
		break;
		case XINE_CONFIG_TYPE_STRING:
		{
			m_stringEdit = new KLineEdit(entry->str_value, parent);
			if (!strcmp(entry->str_value,entry->str_default))
				m_stringEdit->setPaletteForegroundColor(QColor(darkMagenta));
			else
				m_stringEdit->setPaletteForegroundColor(QColor(black));
			grid->addWidget(m_stringEdit, row ,0);
			connect(m_stringEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotStringChanged(const QString&)));
			break;
		}
		case XINE_CONFIG_TYPE_ENUM:
		{
			m_enumEdit = new KComboBox(parent);
			int i = 0;
			while (entry->enum_values[i])
			{
				m_enumEdit->insertItem(entry->enum_values[i]);
				i++;
			}
			m_enumEdit->setCurrentItem(entry->num_value);
			if (entry->num_value == entry->num_default)
				m_enumEdit->setPaletteForegroundColor(QColor(darkMagenta));
			else
				m_enumEdit->setPaletteForegroundColor(QColor(black));
			grid->addWidget(m_enumEdit, row, 0);
			connect(m_enumEdit, SIGNAL(activated(int)), this, SLOT(slotNumChanged(int)));
			break;
		}
		case XINE_CONFIG_TYPE_NUM:
		{
			m_numEdit = new QSpinBox(-999999, 999999, 1, parent);
			m_numEdit->setValue(entry->num_value);
			if (entry->num_value == entry->num_default)
				m_numEdit->setPaletteForegroundColor(QColor(darkMagenta));
			else
				m_numEdit->setPaletteForegroundColor(QColor(black));
			grid->addWidget(m_numEdit, row, 0);
			connect(m_numEdit, SIGNAL(valueChanged(int)), this, SLOT(slotNumChanged(int)));
			break;
		}
		case XINE_CONFIG_TYPE_RANGE:
		{
			m_numEdit = new QSpinBox(parent);
			m_numEdit->setValue(entry->num_value);
			m_numEdit->setRange(entry->range_min, entry->range_max);
			if (entry->num_value == entry->num_default)
				m_numEdit->setPaletteForegroundColor(QColor(darkMagenta));
			else
				m_numEdit->setPaletteForegroundColor(QColor(black));
			grid->addWidget(m_numEdit, row, 0);
			connect(m_numEdit, SIGNAL(valueChanged(int)), this, SLOT(slotNumChanged(int)));
			break;
		}
		case XINE_CONFIG_TYPE_BOOL:
		{
			m_boolEdit = new QCheckBox(parent);
			m_boolEdit->setChecked(entry->num_value);
			if (entry->num_value == entry->num_default)
				m_boolEdit->setPaletteForegroundColor(QColor(darkMagenta));
			else
				m_boolEdit->setPaletteForegroundColor(QColor(black));
			grid->addWidget(m_boolEdit, row, 0);
			connect(m_boolEdit, SIGNAL(toggled(bool)), this, SLOT(slotBoolChanged(bool)));
			break;
		}
	}

	QString m_keyName(entry->key);
	m_keyName.remove( 0, m_keyName.find(".") + 1 );

	QLabel* description = new QLabel(m_keyName + "\n" + QString::fromLocal8Bit(entry->description), parent);
	description->setAlignment( QLabel::WordBreak | QLabel::AlignVCenter );
	grid->addWidget(description, row, 1);

	KSeparator* separator = new KSeparator(KSeparator::Horizontal, parent);
	grid->addMultiCellWidget(separator, row+1, row+1, 0, 1);
}

XineConfigEntry::~XineConfigEntry()
{}

void XineConfigEntry::slotNumChanged(int val)
{
	m_numValue = val;
	m_valueChanged = true;

	if (m_numValue == m_numDefault)
	{
		if (m_numEdit)
		{
			m_numEdit->setPaletteForegroundColor(darkMagenta);
			m_numEdit->update();
		}
		else
		{
			m_enumEdit->setPaletteForegroundColor(darkMagenta);
			m_enumEdit->update();
		}
	}
	else
	{
		if (m_numEdit)
		{
			m_numEdit->setPaletteForegroundColor(black);
			m_numEdit->update();
		}
		else
		{
			m_enumEdit->setPaletteForegroundColor(black);
			m_enumEdit->update();
		}
	}
}

void XineConfigEntry::slotBoolChanged(bool val)
{
	m_numValue = val;
	m_valueChanged = true;
	if (m_numValue == m_numDefault)
	{
		m_boolEdit->setPaletteForegroundColor(QColor(darkMagenta));
		m_boolEdit->update();
	}
	else
	{
		m_boolEdit->setPaletteForegroundColor(QColor(black));
		m_boolEdit->update();
	}
}

void XineConfigEntry::slotStringChanged(const QString& val)
{
	m_stringValue = val;
	m_valueChanged = true;
	if (m_stringValue == m_stringDefault)
	{
		m_stringEdit->setPaletteForegroundColor(QColor(darkMagenta));
		m_stringEdit->update();
	}
	else
	{
		m_stringEdit->setPaletteForegroundColor(QColor(black));
		m_stringEdit->update();
	}
}

bool XineConfigEntry::valueChanged() const
{
	return m_valueChanged;
}

const QString& XineConfigEntry::getKey() const
{
	return m_key;
}

int XineConfigEntry::getNumValue() const
{
	return m_numValue;
}

const QString& XineConfigEntry::getStringValue() const
{
	return m_stringValue;
}


/************** XINE CONFIG CLASS **************************/

XineConfig::XineConfig(const xine_t* const xine) :
		KDialogBase(KDialogBase::IconList, i18n("xine Engine Parameters"),
		            KDialogBase::Ok|KDialogBase::Apply|KDialogBase::Cancel, KDialogBase::Cancel)

{
	setInitialSize(QSize(650,500), true);

	m_xine = (xine_t*)xine;

	QStringList cats = getCategories();
	QTabWidget* tabWidget = NULL;
	QFrame* xineFrame = NULL;
	QVBoxLayout* xineLayout = NULL;
	QVBox* xineBeginnerPage = NULL;
	QVBox* xineExpertPage = NULL;
	QString icon;

	QStringList::ConstIterator end ( cats.end());
	for (QStringList::ConstIterator it = cats.begin(); it != end; ++it)
	{
		//  kdDebug() << "XineConfig: add page: " << *it << endl;
		if (*it == "audio")
			icon = "sound";
		else if (*it == "video")
			icon = "video";
		else if (*it == "vcd")
			icon = "cdrom_unmount";
		else if (*it == "input")
			icon = "connect_established";
		else if (*it == "effects")
			icon = "wizard";
		else if (*it == "media")
			icon = "cdrom_unmount";
		else if (*it == "subtitles")
			icon = "font_bitmap";
		else if (*it == "osd")
			icon = "font_bitmap";
		else if (*it == "engine")
			icon = "exec";
		else
			icon = "edit";

		xineFrame = addPage(*it, i18n("%1 Options").arg(*it), KGlobal::iconLoader()->loadIcon(icon, KIcon::Panel,
		                    KIcon::SizeMedium));
		xineLayout = new QVBoxLayout(xineFrame, marginHint(), spacingHint());
		tabWidget = new QTabWidget(xineFrame);
		xineLayout->addWidget(tabWidget);
		xineBeginnerPage = new QVBox(tabWidget);
		xineBeginnerPage->setMargin(5);
		tabWidget->addTab(xineBeginnerPage, i18n("Beginner Options"));
		createPage(*it, false, xineBeginnerPage);
		xineExpertPage = new QVBox(tabWidget);
		xineExpertPage->setMargin(5);
		tabWidget->addTab(xineExpertPage, i18n("Expert Options"));
		createPage(*it, true, xineExpertPage);
	}

	connect(this, SIGNAL(okClicked()), SLOT(slotOkPressed()));
	connect(this, SIGNAL(applyClicked()), SLOT(slotApplyPressed()));
}

XineConfig::~XineConfig()
{
	m_entries.setAutoDelete(true);
	m_entries.clear();
	kdDebug() << "XineConfig: destructed" << endl;
}

void XineConfig::createPage(const QString& cat, bool expert, QWidget* parent)
{
	xine_cfg_entry_t* ent;

	QScrollView* sv = new QScrollView(parent);
	sv->setResizePolicy(QScrollView::AutoOneFit);
	parent = new QWidget(sv->viewport());
	sv->addChild(parent);

	QGridLayout* grid = new QGridLayout(parent, 20 ,2);
	grid->setColStretch(1,8);
	grid->setSpacing(10);
	grid->setMargin(10);

	uint row = 0;
	QString entCat;

	/*********** read in xine config entries ***********/
	ent = new xine_cfg_entry_t;
	xine_config_get_first_entry(m_xine, ent);

	do
	{
		entCat = QString(ent->key);
		entCat = entCat.left(entCat.find("."));
		if (entCat == cat)
		{
			if (((!expert) && (QString(NON_EXPERT_OPTIONS).contains(ent->key))) ||
			        ((expert) && (!QString(NON_EXPERT_OPTIONS).contains(ent->key))))
			{
				m_entries.append(new XineConfigEntry(parent, grid, row, ent));
				delete ent;
				ent = new xine_cfg_entry_t;
				row += 2;
			}
		}
	}
	while(xine_config_get_next_entry(m_xine, ent));

	delete ent;
}

const QStringList XineConfig::getCategories()
{
	QStringList cats;
	xine_cfg_entry_t* ent = new xine_cfg_entry_t;
	if (!xine_config_get_first_entry(m_xine, ent))
		return cats;
	QString entCat;

	do
	{
		entCat = QString(ent->key);
		entCat = entCat.left(entCat.find("."));
		if (cats.findIndex(entCat) == -1)
		{
			// kdDebug() << "XineConfig: new category: " << entCat << endl;
			cats.append(entCat);
		}
		delete ent;
		ent = new xine_cfg_entry_t;

	}
	while(xine_config_get_next_entry(m_xine, ent));

	delete ent;
	return cats;
}

void XineConfig::slotOkPressed()
{
	slotApplyPressed();
	QDialog::close();
}

/*************** apply changed entries *****************/

void XineConfig::slotApplyPressed()
{
	xine_cfg_entry_t *entry;


	for(uint i = 0; i < m_entries.count(); i++) /* check all entries */
	{
		if (m_entries.at(i)->valueChanged())  /* changed? */
		{
			entry = new xine_cfg_entry_t;
			if (xine_config_lookup_entry(m_xine, m_entries.at(i)->getKey().ascii(), entry))
			{
				kdDebug() << "XineConfig: Apply: " << m_entries.at(i)->getKey() << "\n";

				entry->num_value = m_entries.at(i)->getNumValue();

				if (m_entries.at(i)->getStringValue().ascii())
					entry->str_value = (char*) (const char*)m_entries.at(i)->getStringValue().latin1();

				xine_config_update_entry(m_xine, entry);
				delete entry;

				m_entries.at(i)->setValueUnchanged();
			}
		}
	}
}
