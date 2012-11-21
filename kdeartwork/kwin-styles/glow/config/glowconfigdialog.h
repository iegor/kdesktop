/***************************************************************************
                          glowconfigdialog.h  -  description
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

#ifndef GLOW_CONFIG_DIALOG_H
#define GLOW_CONFIG_DIALOG_H

#include <qvaluevector.h>
#include <qobject.h>

class QListView;
class QPushButton;
class QSignalMapper;
class QCheckBox;
class QComboBox;
class KConfig;
class KColorButton;

class GlowConfigDialog : public QObject
{
	Q_OBJECT

public:
	GlowConfigDialog( KConfig* conf, QWidget* parent );
	~GlowConfigDialog();

signals:
	void changed();

public slots:
	void load( KConfig* conf );
	void save( KConfig* conf );
	void defaults();

protected slots:
	void slotTitleButtonClicked(int);
	void slotColorButtonChanged(const QColor&);
	void slotTitlebarGradientTypeChanged(int);
	void slotResizeHandleCheckBoxChanged();
	void slotThemeListViewSelectionChanged ();

private slots:
	void slotLoadThemeList ();
	
private:
	enum ButtonType{stickyButton, helpButton, iconifyButton,
		maximizeButton, closeButton };

	KConfig *_glowConfig;

	bool _showResizeHandle;
	KPixmapEffect::GradientType _titlebarGradientType;
	QString _theme_name;

	QWidget *_main_group_box;
	QGroupBox *_button_glow_color_group_box;
	QGroupBox *_theme_group_box;

	QListView * _theme_list_view;

	QCheckBox *_showResizeHandleCheckBox;
	QComboBox *_titlebarGradientTypeComboBox;

	QPushButton *_stickyButton;
	QPushButton *_helpButton;
	QPushButton *_iconifyButton;
	QPushButton *_maximizeButton;
	QPushButton *_closeButton;
	QSignalMapper *_titleButtonMapper;

	QColor* _buttonConfigMap;
	QValueVector<QPushButton*> _titleButtonList;
	
	KColorButton *_colorButton;
};

#endif

