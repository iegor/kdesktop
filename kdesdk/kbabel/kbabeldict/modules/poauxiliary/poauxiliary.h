/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
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

#ifndef POAUXILIARY_H
#define POAUXILIARY_H

#include "searchengine.h"

#include <qguardedptr.h>
#include <qdict.h>
class QTimer;

namespace KBabel
{
    class Catalog;
}

class AuxiliaryPreferencesWidget;

class PoAuxiliary : public SearchEngine
{
    Q_OBJECT

public:
    PoAuxiliary(QObject *parent=0, const char *name=0);
    virtual ~PoAuxiliary();

    virtual bool isReady() const;

    virtual QString translate(const QString& text, uint pluralForm);

    virtual bool isSearching() const;

    virtual void saveSettings(KConfigBase *config);
    virtual void readSettings(KConfigBase *config);

    virtual PrefWidget *preferencesWidget(QWidget *parent);

    virtual const KAboutData *about() const;

    virtual QString name() const;
    
    virtual QString id() const;

    virtual QString lastError();

    virtual bool usesRichTextResults();

public slots:
    virtual bool startSearch(const QString& text, unsigned int pluralForm
	, const SearchFilter*filter);
    virtual bool startSearchInTranslation(const QString& text);
    
    virtual void stopSearch();

    virtual void setEditedFile(const QString&);
    virtual void setEditedPackage(const QString&);
    virtual void setLanguageCode(const QString&);
        

protected slots:
    /** reads the current settings from the preferences dialog */
    void applySettings();
    
    /** sets the current settings in the preferences dialog */
    void restoreSettings();

    void loadAuxiliary();
    
private:
    QGuardedPtr<AuxiliaryPreferencesWidget> prefWidget;
    KBabel::Catalog *catalog;
    QString auxPackage;
    QString auxTranslator;
    QString auxURL;

    QString url;
    bool ignoreFuzzy;

    QString editedFile;
    QString package;
    QString langCode;

    bool error;
    QString errorMsg;

    bool stop;
    bool active;
    bool loading;
    bool initialized;

    QTimer *loadTimer;

    struct Entry
    {
        QString orig;
        QString translation;
        QString comment;
        bool fuzzy;
    };

    QDict<Entry> msgidDict;
    QDict<Entry> msgstrDict;
};

#endif
