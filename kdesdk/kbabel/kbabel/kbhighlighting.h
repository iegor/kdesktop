/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Marco Wegner <mail@marcowegner.de>
	    (C) 2003 TrollTech AS
	    (C) 2003 Lukas Tinkl <lukas@kde.org>
	    (C) 2003-2005 Stanislav Visnovsky <visnovsky@kde.org>

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


#ifndef KBHIGHLIGHTING_H
#define KBHIGHLIGHTING_H

#include <qmemarray.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qguardedptr.h>

class KSpell;
class QColor;
class QString;
class QTextEdit;

class KBabelHighlighter : public QObject
{
  Q_OBJECT

  public:
    enum HighlightType { 
	Normal = 0, 
	Tag, 
	Entity, 
	CFormat, 
	Masked, 
	Accel, 
	Error, 
	SpellcheckError 
    };

    KBabelHighlighter( QTextEdit * edit, KSpell *spell );

    void setHighlightColor( HighlightType type, QColor color );
    void setHasErrors( bool err );
    void setSpellChecker( KSpell* spell);

    bool isMisspelled(const QString& word);
    
  public slots:
    void highlight( );
    void setSyntaxHighlighting( bool enable );
    
  protected slots:
     void slotMisspelling (const QString & originalword,
                           const QStringList & suggestions, unsigned int pos);

  private:
    void doHighlighting( HighlightType type, int pos, int length );
    void readSettings( );
    void flushCurrentWord();
  
  private:
    QTextEdit * _edit;
    bool syntaxHighlighting;
    
    QStringList regexps;
    QMemArray<QColor> colors;
    
    bool _hasErrors;
    QString accelMarker;

    static QDict<int> dict;
    QGuardedPtr<KSpell> mSpell;
    QString currentWord;
    int currentPos;
    bool alwaysEndsWithSpace;
};

#endif // KBHIGHLIGHTING_H
