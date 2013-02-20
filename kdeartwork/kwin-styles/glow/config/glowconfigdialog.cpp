/***************************************************************************
                          glowconfigdialog.cpp  -  description
                             -------------------
    begin                : Thu Sep 12 2001
    copyright            : (C) 2001 by Henning Burchardt
    email                : h_burchardt@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qbitmap.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qsignalmapper.h>
#include <qstringlist.h>
#include <kconfig.h>
#include <kcolorbutton.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kstandarddirs.h>
#include "bitmaps.h"
#include "glowconfigdialog.h"
#include "../resources.h"

#define NUMBER_OF_BUTTONS 5

extern "C"
{
	KDE_EXPORT QObject* allocate_config( KConfig* conf, QWidget* parent )
	{
		return(new GlowConfigDialog(conf, parent));
	}
}

GlowConfigDialog::GlowConfigDialog( KConfig * conf, QWidget * parent )
	: QObject(parent)
{
	_glowConfig = new KConfig("kwinglowrc");
	KGlobal::locale()->insertCatalogue("kwin_glow_config");

	_main_group_box = new QWidget(parent);
	QVBoxLayout *main_group_boxLayout = new QVBoxLayout(_main_group_box);
	main_group_boxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	main_group_boxLayout->setSpacing(6);

	//-------------------------------------------------------------------------
	// themes

	_theme_list_view = new QListView (_main_group_box, "theme_list_view");
	_theme_list_view->addColumn (i18n("Theme"));
	_theme_list_view->addColumn (i18n("Button Size"));
        _theme_list_view->setAllColumnsShowFocus(true);
        _theme_list_view->setResizeMode(QListView::AllColumns);

	main_group_boxLayout->addWidget (_theme_list_view);
	QObject::connect (_theme_list_view, SIGNAL(selectionChanged()),
			this, SLOT(slotThemeListViewSelectionChanged()));
	slotLoadThemeList();

	_button_glow_color_group_box = new QGroupBox(
		0, Qt::Horizontal, i18n("Button Glow Colors"), _main_group_box);
	QHBoxLayout *colorHBoxLayout =
		new QHBoxLayout(_button_glow_color_group_box->layout());

	// create buttons
	QSize buttonSize(BITMAP_SIZE, BITMAP_SIZE);
	QPixmap pm(buttonSize);
	pm.fill(Qt::black);

	_stickyButton = new QPushButton(_button_glow_color_group_box);
	pm.setMask(QBitmap(buttonSize, stickyoff_bits, true));
	_stickyButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_stickyButton);
	_titleButtonList.push_back(_stickyButton);

	_helpButton = new QPushButton(_button_glow_color_group_box);
	pm.setMask(QBitmap(buttonSize, help_bits, true));
	_helpButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_helpButton);
	_titleButtonList.push_back(_helpButton);

	_iconifyButton = new QPushButton(_button_glow_color_group_box);
	pm.setMask(QBitmap(buttonSize, minimize_bits, true));
	_iconifyButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_iconifyButton);
	_titleButtonList.push_back(_iconifyButton);

	_maximizeButton = new QPushButton(_button_glow_color_group_box);
	pm.setMask(QBitmap(buttonSize, maximizeoff_bits, true));
	_maximizeButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_maximizeButton);
	_titleButtonList.push_back(_maximizeButton);

	_closeButton = new QPushButton(_button_glow_color_group_box);
	pm.setMask(QBitmap(buttonSize, close_bits, true));
	_closeButton->setPixmap(pm);
	colorHBoxLayout->addWidget(_closeButton);
	_titleButtonList.push_back(_closeButton);

	// create signal mapper
	_titleButtonMapper = new QSignalMapper(this);
	for( uint i=0; i<_titleButtonList.size(); i++ ) {
		_titleButtonMapper->setMapping(_titleButtonList[i], i);
		connect(_titleButtonList[i], SIGNAL(clicked()),_titleButtonMapper, SLOT(map()));
	}
	connect(_titleButtonMapper, SIGNAL(mapped(int)),this, SLOT(slotTitleButtonClicked(int)));

	_colorButton = new KColorButton(_button_glow_color_group_box);
	_colorButton->setEnabled(false);
	connect(_colorButton, SIGNAL(changed(const QColor&)),
		this, SLOT(slotColorButtonChanged(const QColor&)));

	colorHBoxLayout->addItem(new QSpacerItem(
		200, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
	colorHBoxLayout->addWidget(_colorButton);

	main_group_boxLayout->addWidget(_button_glow_color_group_box);

	QHBoxLayout *titlebarGradientTypeLayout = new QHBoxLayout();
	_titlebarGradientTypeComboBox = new QComboBox(_main_group_box);

    KConfig *c = KGlobal::config();
    KConfigGroupSaver cgs( c, QString::fromLatin1("WM") );
    QColor activeBackground = c->readColorEntry("activeBackground");
    QColor activeBlend = c->readColorEntry("activeBlend");

	// If the colors are equal, change one to get a gradient effect
	if (activeBackground==activeBlend) {
		activeBackground = activeBackground.dark();
	}
	for (int i=0; i< KPixmapEffect::EllipticGradient; i++ ) {
		KPixmap gradPixmap(QSize(196,20));
		KPixmapEffect::gradient(gradPixmap, activeBackground,
								activeBlend, (KPixmapEffect::GradientType) i);

		_titlebarGradientTypeComboBox->insertItem(gradPixmap, i);
	}

	connect(_titlebarGradientTypeComboBox, SIGNAL(activated(int)),
		this, SLOT(slotTitlebarGradientTypeChanged(int)));
	titlebarGradientTypeLayout->addWidget(
		new QLabel(i18n("Titlebar gradient:"), _main_group_box));
	titlebarGradientTypeLayout->addWidget(_titlebarGradientTypeComboBox, 0, Qt::AlignLeft);
        titlebarGradientTypeLayout->addStretch(10);
	main_group_boxLayout->addLayout(titlebarGradientTypeLayout);


	_showResizeHandleCheckBox = new QCheckBox(
			i18n("Show resize handle"),	_main_group_box);
	connect(_showResizeHandleCheckBox, SIGNAL(clicked()),
		this, SLOT(slotResizeHandleCheckBoxChanged()));
	main_group_boxLayout->addWidget(_showResizeHandleCheckBox);

	// load config and update user interface
	load(conf);

	_main_group_box->show();
}

GlowConfigDialog::~GlowConfigDialog()
{
	delete _main_group_box;
	delete _glowConfig;
	delete[] _buttonConfigMap;
}

void GlowConfigDialog::load( KConfig* /* conf */ )
{
	QColor color;
	const QColor defaultCloseButtonColor(DEFAULT_CLOSE_BUTTON_COLOR);
	const QColor defaultMaximizeButtonColor(DEFAULT_MAXIMIZE_BUTTON_COLOR);
	const QColor defaultIconifyButtonColor(DEFAULT_ICONIFY_BUTTON_COLOR);
	const QColor defaultHelpButtonColor(DEFAULT_HELP_BUTTON_COLOR);
	const QColor defaultStickyButtonColor(DEFAULT_STICKY_BUTTON_COLOR);

	_glowConfig->setGroup("General");

	_buttonConfigMap = new QColor[NUMBER_OF_BUTTONS];
	color = _glowConfig->readColorEntry("stickyButtonGlowColor",
			&defaultStickyButtonColor);
	_buttonConfigMap[stickyButton] = color;

	color = _glowConfig->readColorEntry("helpButtonGlowColor",
			&defaultHelpButtonColor);
	_buttonConfigMap[helpButton] = color;

	color = _glowConfig->readColorEntry("iconifyButtonGlowColor",
			&defaultIconifyButtonColor);
	_buttonConfigMap[iconifyButton] = color;

	color = _glowConfig->readColorEntry("maximizeButtonGlowColor",
			&defaultMaximizeButtonColor);
	_buttonConfigMap[maximizeButton] = color;

	color = _glowConfig->readColorEntry("closeButtonGlowColor",
			&defaultCloseButtonColor);
	_buttonConfigMap[closeButton] = color;

	_showResizeHandle = _glowConfig->readBoolEntry("showResizeHandle", true);
	_titlebarGradientType = static_cast<KPixmapEffect::GradientType>
							(_glowConfig->readNumEntry("titlebarGradientType",
							KPixmapEffect::DiagonalGradient));

	_showResizeHandleCheckBox->setChecked(_showResizeHandle);
	_titlebarGradientTypeComboBox->setCurrentItem(_titlebarGradientType);

	_theme_name = _glowConfig->readEntry ("themeName", "default");
	_theme_list_view->setSelected (
			_theme_list_view->findItem (_theme_name, 0), true);
        slotTitleButtonClicked(0);
}

void GlowConfigDialog::save( KConfig* /* conf */ )
{
	_glowConfig->setGroup("General");

	_glowConfig->writeEntry("stickyButtonGlowColor", _buttonConfigMap[stickyButton]);
	_glowConfig->writeEntry("helpButtonGlowColor", _buttonConfigMap[helpButton]);
	_glowConfig->writeEntry("iconifyButtonGlowColor", _buttonConfigMap[iconifyButton]);
	_glowConfig->writeEntry("maximizeButtonGlowColor", _buttonConfigMap[maximizeButton]);
	_glowConfig->writeEntry("closeButtonGlowColor", _buttonConfigMap[closeButton]);

	_glowConfig->writeEntry("showResizeHandle", _showResizeHandle);
	_glowConfig->writeEntry("titlebarGradientType", _titlebarGradientType);

	_glowConfig->writeEntry ("themeName", _theme_name);

	_glowConfig->sync();
}

void GlowConfigDialog::defaults()
{
	const QColor defaultCloseButtonColor = DEFAULT_CLOSE_BUTTON_COLOR;
	const QColor defaultMaximizeButtonColor(DEFAULT_MAXIMIZE_BUTTON_COLOR);
	const QColor defaultIconifyButtonColor(DEFAULT_ICONIFY_BUTTON_COLOR);
	const QColor defaultHelpButtonColor(DEFAULT_HELP_BUTTON_COLOR);
	const QColor defaultStickyButtonColor(DEFAULT_STICKY_BUTTON_COLOR);

	_buttonConfigMap[stickyButton] = defaultStickyButtonColor;
	_buttonConfigMap[helpButton] = defaultHelpButtonColor;
	_buttonConfigMap[iconifyButton] = defaultIconifyButtonColor;
	_buttonConfigMap[maximizeButton] = defaultMaximizeButtonColor;
	_buttonConfigMap[closeButton] = defaultCloseButtonColor;

	_showResizeHandle = true;
	_titlebarGradientType = KPixmapEffect::DiagonalGradient;

	_showResizeHandleCheckBox->setChecked(_showResizeHandle);
	_titlebarGradientTypeComboBox->setCurrentItem(_titlebarGradientType);

	_theme_list_view->setSelected (
			_theme_list_view->findItem("default", 0), true);
}

void GlowConfigDialog::slotLoadThemeList ()
{
	QStringList dir_list=KGlobal::dirs()->findDirs("data", "kwin/glow-themes");

	QStringList::ConstIterator it;

	_theme_list_view->clear();
	new QListViewItem (_theme_list_view, "default", "17x17");

	for (it=dir_list.begin(); it!=dir_list.end(); ++it)
	{
		QDir dir (*it, QString("*"), QDir::Unsorted,
				QDir::Dirs | QDir::Readable);
		if (dir.exists())
		{
			QFileInfoListIterator it2(*dir.entryInfoList());
			QFileInfo * finfo;

			while ((finfo=it2.current()))
			{
				if (finfo->fileName() == "." || finfo->fileName() == "..") {
					++it2;
					continue;
				}

				if (! _theme_list_view->findItem (finfo->fileName(), 0))
				{
					KConfig conf (dir.path() + "/" + finfo->fileName() + "/" +
							finfo->fileName() + ".theme");
					QSize button_size = conf.readSizeEntry (
						"buttonSize", new QSize (-1, -1));
					if (button_size.width() == -1)
					{
						++it2;
						continue;
					}
					QString size_string = QString("") +
						QString::number(button_size.width()) +
						"x" + QString::number(button_size.height());
					new QListViewItem (_theme_list_view,
							finfo->fileName(), size_string);
				}

				++it2;
			}
		}
	}
}

void GlowConfigDialog::slotTitlebarGradientTypeChanged(int index)
{
	_titlebarGradientType = static_cast<KPixmapEffect::GradientType>(index);
	emit changed();
}

void GlowConfigDialog::slotResizeHandleCheckBoxChanged()
{
	_showResizeHandle = _showResizeHandleCheckBox->isChecked();
	emit changed();
}

void GlowConfigDialog::slotTitleButtonClicked(int index)
{
	for( int i=0; i< ((int) _titleButtonList.size()); i++ ) {
		_titleButtonList[i]->setDown(i==index);
	}
	_colorButton->setEnabled(true);
	_colorButton->setColor(_buttonConfigMap[index]);
}

void GlowConfigDialog::slotColorButtonChanged(const QColor& glowColor)
{
	if( _stickyButton->isDown() ) {
		_buttonConfigMap[stickyButton] = glowColor;
	} else if( _helpButton->isDown() ) {
		_buttonConfigMap[helpButton] = glowColor;
	} else if( _iconifyButton->isDown() ) {
		_buttonConfigMap[iconifyButton] = glowColor;
	} else if( _maximizeButton->isDown() ) {
		_buttonConfigMap[maximizeButton] = glowColor;
	} else {
		_buttonConfigMap[closeButton] = glowColor;
	}
	emit changed();
}

void GlowConfigDialog::slotThemeListViewSelectionChanged ()
{
	if( _theme_list_view->selectedItem() != 0 ) {
		_theme_name = _theme_list_view->selectedItem()->text (0);

		emit changed();
	}
}

#include "glowconfigdialog.moc"
