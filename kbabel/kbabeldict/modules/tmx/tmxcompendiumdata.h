/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2001 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002 by Stanislav Visnovsky
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
#ifndef TMXCOMPENDIUMDATA_H
#define TMXCOMPENDIUMDATA_H


#include <kurl.h>
#include <qdict.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qvaluevector.h>
#include <qptrlist.h>

class TmxCompendiumData : public QObject
{
    Q_OBJECT

public:
    TmxCompendiumData(QObject *parent=0);

    bool load(const KURL& url, const QString& language);

    const int *exactDict(const QString& text) const;
    const QValueList<int> *allDict(const QString& text) const;
    const QValueList<int> *wordDict(const QString& text) const;
    
    const QString msgid(const int index) const;
    const QString msgstr(const int index) const;
    const int numberOfEntries() const { return _exactDict.count(); }

    bool active() const { return _active; }
    bool initialized() const { return _initialized; }
    bool hasErrors() const { return _error; }
    QString errorMsg() const { return _errorMsg; }

    /** registers an object, that uses this data */
    void registerObject(QObject *);
    /** 
     * unregisters an object, that uses this data 
     * 
     * @return true, if this was the last object
     */
    bool unregisterObject(QObject *);

    bool hasObjects() const;


    static QString simplify(const QString& text);
    static QStringList wordList(const QString& text);
    
signals:
    void progressStarts(const QString&);
    void progressEnds();
    void progress(int);

    
private:
    bool _active;
    bool _error;
    bool _initialized;
    QString _errorMsg;

    QDict<int> _exactDict;
    QDict< QValueList<int> > _allDict;
    QDict< QValueList<int> > _wordDict;
    
    QValueVector<QString> _originals;
    QValueVector<QString> _translations;

    QPtrList<QObject> _registered;
};

#endif
