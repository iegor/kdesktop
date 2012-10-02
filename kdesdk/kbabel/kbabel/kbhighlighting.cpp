/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2002 by Marco Wegner <mail@marcowegner.de>
	        2003 Trolltech AS
		2003 Lukas Tinkl <lukas@kde.org>
		2003-2005 Stanislav Visnovsky <visnovsky@kde.org>

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


#include <qcolor.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextedit.h>

#include "kapplication.h"
#include <kconfig.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kbabelsettings.h>
#include <kspell.h>

#include "kbhighlighting.h"
#include "resources.h"

KBabelHighlighter::KBabelHighlighter( QTextEdit * edit, KSpell *spell ) : QObject() 
    , _edit( edit )
    , syntaxHighlighting(true)
    , mSpell(0), alwaysEndsWithSpace(true)
{
  regexps << "(<[_:A-Za-z][-_.:A-Za-z0-9]*([\\s]*[_:A-Za-z][-_.:A-Za-z0-9]*=\\\\\"[^<>]*\\\\\")*[\\s]*/?>)|(</[_:A-Za-z][-_.:A-Za-z0-9]*[\\s]*>)";
  regexps << "(&[A-Za-z_:][A-Za-z0-9_.:-]*;)";
  regexps << "(%[\\ddioxXucsfeEgGphln]+)|(%\\d+\\$[dioxXucsfeEgGphln])";
  regexps << "(\\\\[abfnrtv'\"\?\\\\])|(\\\\\\d+)|(\\\\x[\\dabcdef]+)";

  colors.resize( 8 );
  colors[Normal]  = KGlobalSettings::textColor();
  colors[Tag]     = KBabelSettings::tagColor ();
  colors[Entity]  = KBabelSettings::tagColor ();
  colors[CFormat] = KBabelSettings::cformatColor ();
  colors[Masked]  = KBabelSettings::quotedColor ();
  colors[Accel]   = KBabelSettings::accelColor ();
  colors[Error]   = KBabelSettings::errorColor ();
  colors[SpellcheckError]   = KBabelSettings::spellcheckErrorColor ();
  
  _hasErrors = false;
  
  readSettings( );
  regexps << accelMarker + "[\\w]";
  
  connect( _edit, SIGNAL( textChanged( ) ), this, SLOT( highlight( ) ) );
  
  setSpellChecker(spell);
}

// spell check the current text and highlight (as red text) those words
// that fail the spell check
void KBabelHighlighter::highlight( )
{
  // no updates while we're highlighting
  _edit->blockSignals( true );
  _edit->setUpdatesEnabled( false );

  // store cursor position
  int cpara, cindex;
  _edit->getCursorPosition( &cpara, &cindex );

  _edit->selectAll( );
  _edit->setColor( _hasErrors ? colors[Error] : colors[Normal] );
  _edit->removeSelection( );

  // create a single line out of the text: remove "\n", so that we only
  // have to deal with one single line of text. 
  QString text = _edit->text( );
  text.replace( "\n", "" );

  QRegExp rx;
  int pos;
  
  if (syntaxHighlighting)
  {
    for ( uint i = 0; i < regexps.count( ); ++i ) {
      rx.setPattern( regexps[i] );
      pos = text.find( rx );
      while ( pos >= 0 ) {
        doHighlighting( (HighlightType)(i+1), pos, rx.matchedLength( ) );
        pos = text.find( rx, pos + rx.matchedLength( ) );
      }
    }
  }

  if( mSpell )
  {
    // spell-on-fly start
    if (!text.endsWith(" "))
        alwaysEndsWithSpace = false;
    else
        alwaysEndsWithSpace = true;

    //MessageHighlighter::highlightParagraph( text, endStateOfLastPara );

    int len = text.length();
    if (alwaysEndsWithSpace)
        len--;

    currentPos = 0;
    currentWord = "";
    for (int i = 0; i < len; i++) {
        if (text[i].isSpace() || text[i] == '-' || (text[i]=='\\' && text[i+1]=='n')) {
            flushCurrentWord();
	    if (text[i]=='\\') i++;
            currentPos = i + 1;
        } else {
            currentWord += text[i];
        }
    }

    // this was     if (!text[len - 1].isLetter())
    // but then the last word is never checked  
    if (text[len - 1].isLetter()) {
        kdDebug(KBABEL) << "flushing last Current word: " << currentWord << endl;
        flushCurrentWord();
    }
    else {
        kdDebug(KBABEL) << "not flushing last Current word: " << currentWord << endl;
    }
  } // spell-on-fly end
  
  _edit->setColor( colors[Normal] );

  //restore cursor position
  _edit->setCursorPosition( cpara, cindex );

  // allow updates again now that we're finished highlighting
  _edit->setUpdatesEnabled( true );
  _edit->blockSignals( false );
  _edit->updateContents( );
  _edit->ensureCursorVisible();
}

void KBabelHighlighter::doHighlighting( HighlightType type, int pos, int length )
{
  uint startPara = 0, endPara = 0, startIndex = pos, endIndex = pos+length;

  // transform the one-dimensional indexes into two-dimensional ones
  while ( startIndex > _edit->text( startPara ).length( ) )
    startIndex -= _edit->text( startPara++ ).length( ) - 1;
  while ( endIndex > _edit->text( endPara ).length( ) )
    endIndex -= _edit->text( endPara++ ).length( ) - 1;

  // and finally do the actual highlighting
  _edit->setSelection( startPara, startIndex, endPara, endIndex );
  _edit->setColor( colors[type] );
  _edit->removeSelection( );
}

void KBabelHighlighter::setHighlightColor( HighlightType type, QColor color )
{
  colors[type] = color;
}

void KBabelHighlighter::setHasErrors( bool err )
{
  _hasErrors = err;
}

void KBabelHighlighter::readSettings( )
{
  // FIXME: does not care about different projects yet
  KConfig * config = KGlobal::config( );
  config->setGroup( "Misc" );
  QString temp = config->readEntry( "AccelMarker", "&" );
  accelMarker = temp[0];
}

static int dummy, dummy2;
static int *Okay = &dummy;
static int *NotOkay = &dummy2;

void KBabelHighlighter::flushCurrentWord()
{
    while (currentWord[0].isPunct()) {
        currentWord = currentWord.mid(1);
        currentPos++;
    }

    QChar ch;
    while ((ch = currentWord[(int) currentWord.length() - 1]).isPunct()
           && ch != '(' && ch != '@')
        currentWord.truncate( currentWord.length() - 1 );
	
    // try to remove tags (they might not be fully compliant, but
    // we don't want to check them anyway
    QRegExp tags("(<[-_.:A-Za-z0-9]*([\\s]*[-_.:A-Za-z0-9]*=\\\\\"[^<>]*\\\\\")*[\\s]*/?>)|(</[-_.:A-Za-z0-9]*[\\s]*>)");
    if( tags.search (currentWord) != -1 )
    {
	currentPos += tags.matchedLength();
    }
    
    currentWord.replace ( tags, "" );

    if (!currentWord.isEmpty()) {
        bool isPlainWord = true;
        for (int i = 0; i < (int) currentWord.length(); i++) {
            QChar ch = currentWord[i];
            if (ch.upper() == ch) {
                isPlainWord = false;
                break;
            }
        }

        if (/*isPlainWord && currentWord.length() > 2 &&*/ isMisspelled(currentWord))
            doHighlighting(SpellcheckError,currentPos, currentWord.length());
    }
    currentWord = "";
}

QDict<int> KBabelHighlighter::dict(50021);

bool KBabelHighlighter::isMisspelled(const QString& wordRaw)
{
    // We have to treat ampersands (like in "&go" or "g&o") in a special way.
    // they must not break the word. And we cannot change the parameter, as
    // then the highlight would be one character short. So we have to copy the 
    // word first.
    QString word = wordRaw;
    kdDebug(KBABEL) << "isampersand: checking (raw):" << word << endl;
    word.replace("&", "" );
    kdDebug(KBABEL) << "isMisspelled: checking: " << word << endl;

    // Normally isMisspelled would look up a dictionary and return
    // true or false, but kspell is asynchronous and slow so things
    // get tricky...

    // "dict" is used as a cache to store the results of KSpell
    if (!dict.isEmpty() && dict[word] == NotOkay)
        return true;
    if (!dict.isEmpty() && dict[word] == Okay)
        return false;

    // there is no 'spelt correctly' signal so default to Okay
    kdDebug(KBABEL) << "Adding word " << word << endl;
    dict.replace(word, Okay);
    mSpell->checkWord(word, false);
    return false;
}

void KBabelHighlighter::slotMisspelling(const QString & originalword,
                                       const QStringList & suggestions, unsigned int)
{
    kdDebug(KBABEL) << "Misspelled " << originalword << ", " << suggestions << endl;
    dict.replace( originalword, NotOkay );

    // this is slow but since kspell is async this will have to do for now
    highlight();
}

void KBabelHighlighter::setSpellChecker( KSpell* spell )
{
    if( mSpell )
    {
	disconnect(mSpell, SIGNAL(misspelling(const QString &, const QStringList &, unsigned int)),
            this, SLOT(slotMisspelling(const QString &, const QStringList &, unsigned int)));

	// cleanup the cache
	dict.clear();
    }
    
    mSpell = spell;

    if( mSpell )
    {
	connect(mSpell, SIGNAL(misspelling(const QString &, const QStringList &, unsigned int)),
            this, SLOT(slotMisspelling(const QString &, const QStringList &, unsigned int)));

	// wait for KSpell to startup correctly
	kapp->processEvents(500);
    }    
    
    highlight();
}

void KBabelHighlighter::setSyntaxHighlighting( bool enable )
{
    syntaxHighlighting = enable;
    
    // update highlighting
    highlight();
}
#include "kbhighlighting.moc"
