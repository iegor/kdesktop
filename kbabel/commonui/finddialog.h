/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.

**************************************************************************** */
#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <qstring.h>
#include <qstringlist.h>
#include <kdialogbase.h>

class QButtonGroup;
class QCheckBox;
class QLabel;
class KComboBox;

#include "findoptions.h"

class KDE_EXPORT FindDialog : public KDialogBase
{
	Q_OBJECT
public:
   /**
   * Constructor
   * @param replaceDlg flag, if this is a replace dialog
   */
	FindDialog(bool replaceDlg, QWidget* parent);
	~FindDialog();
	
	/**
	* shows the dialog
	* @param initialStr string to display in find field.
	* If empty, the last used string is used.
	*
	* @return the result code of the dialog
	*/
	int show(QString initialStr);
	
	/**
	* executes the dialog as modal
	* @param initialStr string to display in find field.
	* If empty, the last used string is used.
	*
	* @return the result code of the dialog
	*/
	int exec(QString initialStr);
	KBabel::FindOptions findOpts();
	void setFindOpts(KBabel::FindOptions options);
	KBabel::ReplaceOptions replaceOpts();
	void setReplaceOpts(KBabel::ReplaceOptions options);
	
protected:
   void readSettings();
   void saveSettings();
   
   bool isReplaceDialog() { return _replaceDlg; }

private slots:
	void inButtonsClicked(int id);
	void regExpButtonClicked();

private:
	KComboBox *_findCombo;
	KComboBox *_replaceCombo;
	QLabel *_replaceLabel;
	
	QButtonGroup *_buttonGrp;
	QCheckBox *_inMsgid;
	QCheckBox *_inMsgstr;
	QCheckBox *_inComment;
	
	QCheckBox *_caseSensitive;
	QCheckBox *_wholeWords;
    QCheckBox *_ignoreAccelMarker;
    QCheckBox *_ignoreContextInfo;
	QCheckBox *_backwards;
	QCheckBox *_fromCursor;
	QCheckBox *_isRegExp;
	QCheckBox *_askForReplace;
	
	QPushButton *_regExpButton;
	QDialog *_regExpEditDialog;
	
	KBabel::FindOptions _findOptions;
	KBabel::ReplaceOptions _replaceOptions;
	
	QStringList _findList;
	QStringList _replaceFindList;
	QStringList _replaceList;
	
	bool _replaceDlg;
};

class KDE_EXPORT ReplaceDialog : public KDialogBase
{
	Q_OBJECT
public:
	ReplaceDialog(QWidget* parent);
	~ReplaceDialog();

signals:
	void replace();
	void replaceAll();
	void next();	
	
};

#endif // FINDDIALOG_H
