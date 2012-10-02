/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2005 by Stanislav Visnovsky
			    <visnovsky@kde.org>

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
#ifndef PROJECTPREFWIDGETS_H
#define PROJECTPREFWIDGETS_H

#include <ktabctl.h>
#include <qptrlist.h>
#include "projectsettings.h"


class KLineEdit;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QListBox;
class QRadioButton;
class QSpinBox;
class CmdEdit;
class KFontChooser;
class KColorButton;
class KComboBox;
class KSpellConfig;
class KURLRequester;
class QPushButton;
class QGroupBox;
class KListEditor;
class ToolSelectionWidget;
class KIntNumInput;
class KURLRequester;

struct ModuleInfo;

class KDE_EXPORT SavePreferences : public KTabCtl
{
   Q_OBJECT
public:
   SavePreferences(QWidget* parent=0);
   void defaults(const KBabel::SaveSettings& settings);
   void setAutoSaveVisible(const bool on);

private:
   QCheckBox* _lastButton;
   QCheckBox* _revisionButton;
   QCheckBox* _languageButton;
   QCheckBox* _charsetButton;
   QCheckBox* _encodingButton;
   QCheckBox* _projectButton;

   QCheckBox* _updateButton;
   QCheckBox* _autoCheckButton;
   QCheckBox* _saveObsoleteButton;

   QComboBox* _encodingBox;
   QCheckBox* _oldEncodingButton;

   QRadioButton* _defaultDateButton;
   QRadioButton* _localDateButton;
   QRadioButton* _customDateButton;
   QLineEdit* _dateFormatEdit;
   
   QLineEdit* _projectEdit;

   QRadioButton* _removeFSFButton;
   QRadioButton* _updateFSFButton;
   QRadioButton* _nochangeFSFButton;
   QCheckBox* _translatorCopyrightButton;

   QCheckBox* _descriptionButton;
   QLineEdit* _descriptionEdit;
   
   KIntNumInput * _autoSaveDelay;
   QGroupBox * _autoSaveBox;

private slots:
   void customDateActivated(bool on);
};


class IdentityPreferences : public QWidget
{
    Q_OBJECT
public:
    IdentityPreferences(QWidget *parent = 0, const QString& project = "");
    virtual bool eventFilter(QObject *, QEvent*);
    void defaults(const KBabel::IdentitySettings& settings);

private slots:
    void checkTestPluralButton();
    void testPluralForm();
    void lookupGnuPluralFormHeader();
    
private:
   QLineEdit* _nameEdit;
   QLineEdit* _localNameEdit;
   QLineEdit* _mailEdit;
   QLineEdit* _langEdit;
   QLineEdit* _langCodeEdit;
   QLineEdit* _listEdit;

   QLineEdit* _timezoneEdit;
   QSpinBox *_pluralFormsBox;
   QPushButton *_testPluralButton;
   QCheckBox* _checkPluralArgumentBox;
   QLineEdit* _gnuPluralFormHeaderEdit;
   QPushButton *_testGnuPluralFormButton;
};


class MiscPreferences : public QWidget
{
   Q_OBJECT
public:
    MiscPreferences(QWidget *parent=0);
    void defaults(const KBabel::MiscSettings& settings);

private slots:
   void regExpButtonClicked();
   
private:
   void setContextInfo(QString reg);
   QString contextInfo() const;
   
   KLineEdit *contextInfoEdit;
   KLineEdit *accelMarkerEdit;
   
   QDialog *_regExpEditDialog;
   QPushButton *_regExpButton;

   QRadioButton* bzipButton;
   QRadioButton* gzipButton;
   QCheckBox* compressSingle;
};

class SpellPreferences : public QWidget
{
    Q_OBJECT
public: 
    SpellPreferences(QWidget *parent=0);
    
    void mergeSettings(KBabel::SpellcheckSettings& set) const;
    void updateWidgets(const KBabel::SpellcheckSettings& settings);
    void defaults(const KBabel::SpellcheckSettings& settings);

signals:
    void settingsChanged();

private:
    KSpellConfig* spellConfig;
    QCheckBox* remIgnoredBtn;
    QCheckBox* onFlyBtn;
    KURLRequester* ignoreURLEdit;

};

class CatmanPreferences : public QWidget
{
    Q_OBJECT
public:
    CatmanPreferences(QWidget *parent = 0);
    void defaults(const KBabel::CatManSettings& settings);

private:
    KURLRequester* _poDirEdit;
    KURLRequester* _potDirEdit;

    QCheckBox* _openWindowButton;

    QCheckBox* _killButton;
    QCheckBox* _indexButton;
    QCheckBox* m_msgfmtButton;
};

class DirCommandsPreferences : public QWidget
{
    Q_OBJECT
public:
    DirCommandsPreferences(QWidget *parent = 0);
    virtual ~DirCommandsPreferences();

    void mergeSettings(KBabel::CatManSettings& settings) const;
    void updateWidgets(const KBabel::CatManSettings&);
    void defaults(const KBabel::CatManSettings& settings);

signals:
    void settingsChanged();

private:
    CmdEdit* _dirCmdEdit;
};

class FileCommandsPreferences : public QWidget
{
    Q_OBJECT
public:
    FileCommandsPreferences(QWidget *parent = 0);
    virtual ~FileCommandsPreferences();

    void mergeSettings(KBabel::CatManSettings& settings) const;
    void updateWidgets(const KBabel::CatManSettings& settings);
    void defaults(const KBabel::CatManSettings& settings);

signals:
    void settingsChanged();

private:
    CmdEdit* _fileCmdEdit;
};

class ViewPreferences : public QWidget
{
    Q_OBJECT
public:
    ViewPreferences(QWidget *parent = 0);
    void defaults(const KBabel::CatManSettings& settings);

private:
    QCheckBox* _flagColumnCheckbox;
    QCheckBox* _fuzzyColumnCheckbox;
    QCheckBox* _untranslatedColumnCheckbox;
    QCheckBox* _totalColumnCheckbox;
    QCheckBox* _cvsColumnCheckbox;
    QCheckBox* _revisionColumnCheckbox;
    QCheckBox* _translatorColumnCheckbox;
};

/**
* This class implements preference widget for source context
*
* @short Class for setting preferences for source context
* @author Stanislav Visnovsky <visnovsky@kde.org>
*/
class SourceContextPreferences : public QWidget
{
    Q_OBJECT
public:
    SourceContextPreferences(QWidget* parent=0); 
    virtual ~SourceContextPreferences();
    
    void mergeSettings(KBabel::SourceContextSettings& settings) const;
    void updateWidgets(const KBabel::SourceContextSettings& settings);
    void defaults(const KBabel::SourceContextSettings& settings);

    virtual bool eventFilter(QObject *, QEvent*);
    
signals:
    void itemsChanged ();

private:
    KURLRequester* _coderootEdit;
    KListEditor* _pathsEditor;
};

#endif // PROJECTPREFWIDGETS_H
