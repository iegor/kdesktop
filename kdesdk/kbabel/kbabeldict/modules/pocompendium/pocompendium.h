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

#ifndef POCOMPENDIUM_H
#define POCOMPENDIUM_H

#include <poinfo.h>

#include "compendiumdata.h"
#include "searchengine.h"

#include <qdict.h>
#include <qfile.h>
#include <qguardedptr.h>
#include <qptrlist.h>

class CompendiumPreferencesWidget;
class KConfigBase;
class QTimer;

class PoCompendium : public SearchEngine
{
	Q_OBJECT

public:
	PoCompendium(QObject *parent=0, const char *name=0);
	virtual ~PoCompendium();

	virtual bool isReady() const;
    
    virtual QString translate(const QString&text, uint pluralForm);
    virtual QString searchTranslation(const QString&, int &score, const uint pluralForm);
    virtual QString fuzzyTranslation(const QString&, int &score, const uint pluralForm);

	virtual bool isSearching() const;

	virtual void saveSettings(KConfigBase *config);
	virtual void readSettings(KConfigBase *config);

	virtual PrefWidget *preferencesWidget(QWidget *parent);

	virtual const KAboutData *about() const;

	virtual QString name() const;
	
	virtual QString id() const;

	virtual QString lastError();


public slots:
	virtual bool startSearch(const QString& s, uint pluralForm = 0, const SearchFilter* filter = 0);
	virtual void stopSearch();
    virtual void setLanguageCode(const QString& lang);

protected slots:
	/** reads the current settings from the preferences dialog */
	void applySettings();
    
	/** sets the current settings in the preferences dialog */
	void restoreSettings();

    void slotLoadCompendium();

    void recheckData();
    void removeData();

protected:
    void loadCompendium();
    void addResult(SearchResult *, QPtrList<SearchResult>& allResults);
    QString maskString(QString string);

    void registerData();
    void unregisterData();
    
    bool searchExact(const QString& searchString, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices );
    bool searchTextOnly(const QString& searchString, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices );
    bool searchCaseInsensitive(const QString& searchString, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices );
    bool searchWords(const QString& searchString, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices );
    bool searchNGram(const QString& searchString, uint pluralForm, QPtrList<SearchResult>& results, QValueList<int>& foundIndices, QValueList<int>& checkedIndices );

private:
    QGuardedPtr<CompendiumPreferencesWidget> prefWidget;
    CompendiumData *data;
    KBabel::PoInfo catalogInfo;
    QTimer *loadTimer;

    QString url;
    QString realURL;
    QString langCode;
	
    bool caseSensitive;
    bool ignoreFuzzy;
    bool wholeWords;

    bool matchEqual;
    bool matchIsContained;
    bool matchContains;
    bool matchWords;
    bool matchNGram;
    
    bool buildIndex;
    uint freeMemDelay;

	bool error;
	QString errorMsg;

	bool stop;
	bool active;
	bool initialized;
    bool loading;

    static QDict<CompendiumData> *_compDict;
    static QDict<CompendiumData> *compendiumDict();
};

#endif
