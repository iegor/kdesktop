/*
 * xineconfig.h - config dialog for xine parameters
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

#ifndef XINECONFIG_H
#define XINECONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class KLineEdit;
class KComboBox;
class QWidget;
class QHBox;
class QCheckBox;
class QSpinBox;
class QStringList;
class QGridLayout;

#include <xine.h>

/* stores a single config entry of the config file */
class XineConfigEntry : public QHBox
{
	Q_OBJECT
public:
	XineConfigEntry(QWidget* parent, QGridLayout* grid, int row, xine_cfg_entry_t* entry);
	~XineConfigEntry();

	bool valueChanged() const;  /* was the value changed by user? */
	const QString& getKey() const; /* the key (name) of the entry */
	int getNumValue() const;
	const QString& getStringValue() const;
	void setValueUnchanged()
	{
		m_valueChanged = false;
	}

private slots:
	void slotNumChanged(int);
	void slotBoolChanged(bool);
	void slotStringChanged(const QString&);

private:
	bool m_valueChanged;
	QString m_key;
	int m_numValue;
	int m_numDefault;
	QString m_stringValue;
	QString m_stringDefault;

	KLineEdit* m_stringEdit;
	KComboBox* m_enumEdit;
	QSpinBox* m_numEdit;
	QCheckBox* m_boolEdit;
};


class XineConfig : public KDialogBase
{
	Q_OBJECT
public:
	XineConfig(const xine_t* const xine);
	~XineConfig();

private slots:
	void slotOkPressed();
	void slotApplyPressed();

private:
	void createPage(const QString& cat, bool expert, QWidget* parent);
	const QStringList getCategories();

private:
	QPtrList<XineConfigEntry> m_entries;
	xine_t* m_xine;
};

#endif /* XINECONFIG_H */
