/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer <matthias.kiefer@gmx.de>
		2002-2003 by Stanislav Visnovsky <visnovsky@kde.org>
  Copyright (C) 2006 by Nicolas GOUTTE <goutte@kde.org> 

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
#ifndef CATALOGITEM_H
#define CATALOGITEM_H

#include <qstringlist.h>
#include <qptrlist.h>

#include "pluralforms.h"
#include "editcmd.h"
#include "catalogfileplugin.h"
#include "kbproject.h"

class QTextStream;
class GettextFlexLexer;

namespace KBabel
{

class RegExpExtractor;
class CatalogItemPrivate;

/**
* This class represents an entry in a catalog.
* It contains the comment, the Msgid and the Msgstr.
* It defines some functions to query the state of the entry
* (fuzzy, untranslated, cformat).
*
* @short Class, representing an entry in a catalog
* @author Matthias Kiefer <matthias.kiefer@gmx.de>
*/
class KDE_EXPORT CatalogItem
{

public:
    /** Constructor for empty item*/
    CatalogItem(Project::Ptr project = 0);
    /** Copy constructor */
    CatalogItem(const CatalogItem&);
    /** Destructor */
    ~CatalogItem();

    /**
    * returns true, if the translation is fuzzy, this means
    * if the string ", fuzzy" is contained in the comment
    */
    bool isFuzzy() const;
    
    /**
     * returns true, if the translation in c-format, this means
     * if the string ", c-format" is contained in the comment
     * @since 1.11.2 (KDE 3.5.2) not only "c-format" is supported but also
     * "possible-c-format" (from the debug parameter of xgettext)
     */
    bool isCformat() const;

    /**
    * returns true, if the string ", no-c-format" is contained in the comment
    */
    bool isNoCformat() const;

    /**
     * returns true, if the translation in qt-format, this means
     * if the string ", qt-format" is contained in the comment
     * @since 1.11.2 (KDE 3.5.2)
     */
    bool isQtformat() const;

    /**
     * returns true, 
     * if the string ", no-qt-format" is contained in the comment
     * @since 1.11.2 (KDE 3.5.2)
     */
    bool isNoQtformat() const;

    /**
    * returns true, if the Msgid is untranslated, this means the
    * Msgstr is empty
    */
    bool isUntranslated() const;

    /**
     * @return type of the plural form, is any
     */
    PluralFormType pluralForm() const;

    bool isValid() const;
    
    void setSyntaxError(bool);

    /** returns the number of lines, the entry will need in a file */
    int totalLines() const;

    /**
    * removes the string ", fuzzy" from the comment
    * @param doIt if false, the comment is not changed, just the
    * commands for doing it are calculated
    */
    QPtrList<EditCommand> removeFuzzy(bool doIt=true);
 
    /**
    * adds the string ", fuzzy" to the comment
    * @param doIt if false, the comment is not changed, just the
    * commands for doing it are calculated
    */
    QPtrList<EditCommand> addFuzzy(bool doIt=true);


    /** cleares the item */
    void clear();


    /** returns the comment of this entry */
    QString comment() const;
    /** returns the msgctxt of this entry */
    QString msgctxt(const bool noNewlines = false) const;
    /** returns the msgid of the entry */
    QStringList msgid(const bool noNewlines = false) const;
    /** returns the msgstr of the entry */
    QStringList msgstr(const bool noNewlines = false) const;

    /**
     * @param pluralNr If this item is a gettext plural form item,
     * it returns the plural form number pluralNr, else always the msgstr
     * is returned
     * @return the msgstr as list
     */
    QStringList msgstrAsList(int pluralNr=0) const;

    void setComment(QString com);
    void setMsgctxt(QString msg);
    void setMsgid(QString msg);
    void setMsgid(QStringList msg);
    void setMsgstr(QString msg);
    void setMsgstr(QStringList msg);
    
    void setGettextPluralForm( bool _gettextPlurals );

    void processCommand(EditCommand* cmd, bool undo=false);
    
    Project::Ptr project() const;


    /** @return a list of tags in the msgid */
    QStringList tagList( RegExpExtractor& tagExtractor );

    /** @return a list of args in the msgid */
    QStringList argList( RegExpExtractor& argExtractor);

	/**
	 * @return the list of all errors of this item 
	 */
	QStringList errors() const;
	
	QString nextError() const;
	void clearErrors();
	void removeError(const QString& error);
	void appendError(const QString& error);

	/**
	 * makes some sanity checks and set status accordingly
	 * @return the new status of this item
	 * @see CatalogItem::Error
	 * @param accelMarker a char, that marks the keyboard accelerators
	 * @param contextInfo a regular expression, that determines what is 
	 * the context information
	 * @param singularPlural a regular expression, that determines what is 
	 * string with singular and plural form
     * @param neededLines how many lines a string with singular-plural form
     * must have
	 */
	int checkErrors(QChar accelMarker, const QRegExp& contextInfo
            , const QRegExp& singularPlural, const int neededLines);
	
    
    void operator=(const CatalogItem& rhs);

private:
    void setProject( Project::Ptr project );
    
    friend class Catalog;

private:
   CatalogItemPrivate* d;
};

}

#endif // CATALOGITEM_H
