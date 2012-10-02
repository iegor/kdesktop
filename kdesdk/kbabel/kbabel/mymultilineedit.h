/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2003 by Stanislav Visnovsky
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
#ifndef MYMULTILINEEDIT_H
#define MYMULTILINEEDIT_H

#include <ktextedit.h>
#include <qptrvector.h>

namespace KBabel
{
    class EditCommand;
}

class KBabelHighlighter;
class KSpell;
class QPixmap;

class MyMultiLineEdit : public KTextEdit
{
   Q_OBJECT
public:
   MyMultiLineEdit(int ID,QWidget* parent,const char* name=0);

   /**
      applies cmd to the displayed text, but does not emit
      signalUndoCommand
   */
   void processCommand(KBabel::EditCommand* cmd, bool undo=false);

   /** 
	* @returns the position in text, where the marked text begins
	* -1, if there is no marked text
	*/
   int beginOfMarkedText();

    /** 
    * @returns the position in text, where the last marked text began
    * or the current cursor position if there was no marked text.
    * This is used for getting the start position for a text replacement
    */
   int beginOfLastMarkedText();

    /** 
    * @returns the position in text, where the last marked text ended
    * or the current cursor position if there was no marked text.
    * This is used for getting the end position for a text replacement
    */
   int endOfLastMarkedText();

   virtual void insertAt ( const QString & s, int line, int col, bool mark = false );
   virtual void removeLine ( int line );

   int pos2Offset(uint paragraph, uint index);
   void offset2Pos(int offset, int &row, int &col) const;
   /** 
	* @returns the current position in text, where the cursor is
	*/

   int currentIndex();
   /** 
   * processes Del key
   */
   void my_del();
   void my_backspace();
   
   /**
    * need to override deleting of popup menus :-(
    */
   void contentsContextMenuEvent( QContextMenuEvent *e );
   
   /**
    * need to reimplement overwrite mode :-(
    */
   bool isOverwriteMode() { return _overwrite; }    

public slots:
   
   virtual void clear();
   virtual void paste();
   virtual void setReadOnly(bool on);
   virtual void setContextMenu( QPopupMenu *menu );
   virtual void setText(const QString& s);
   virtual void doKeyboardAction( KeyboardAction action );
   virtual void removeSelectedText(int selNum = 0);

   virtual void onSelectionChanged();
   
   /**
   reimplemented overwrite mode, since QTextEdit handles this internally and does
   not use any accessible virtual methods :-((.
   */
   virtual void setOverwriteMode(bool b);

protected:

   virtual void focusInEvent(QFocusEvent*);
   virtual QPopupMenu *createPopupMenu();
   virtual QPopupMenu *createPopupMenu(const QPoint &pos);
   
   /* the parent handles this */
   virtual void wheelEvent(QWheelEvent*);

   bool emitUndo;

   /* First and the last line of the last change. They are only approximate. Used for faster display
    * highlighting etc.
    */
   uint _firstChangedLine;
   uint _lastChangedLine;
   
   /* This is a cache. _lastPragraphOffset always correctly corresponds to _lastParagraphOffset
    */
   uint _lastParagraph;
   uint _lastParagraphOffset;

   /* We save the last selection positions. This is needed when a tag is inserted to get the
      left cursor position of the originally used selection */
   int _lastSelectionStart;
   int _lastSelectionEnd;

   /* flag to skip any work on updating, since it will be more changes */
   bool _dontUpdate;

protected slots:
   virtual void insert ( const QString & text, bool indent = FALSE, bool checkNewLine = TRUE, bool removeSelected = TRUE );
   virtual void emitCursorPosition();

signals:
   void signalUndoCmd(KBabel::EditCommand*);
   void signalSyntaxHighlightingChanged (bool enable);

protected:
   int _myID;
   
private:
   QPopupMenu *_menu;
   bool _overwrite;
};


class MsgMultiLineEdit : public MyMultiLineEdit
{   
   Q_OBJECT
public:
   enum TextColor { NormalColor, ErrorColor };

   MsgMultiLineEdit(int ID, KSpell* spell=0, QWidget* parent=0,const char* name=0);
   virtual ~MsgMultiLineEdit();

   /** is displaying surrounding quotes enabled? */
   bool quotes() const { return _quotes;}
   /** enable or disable displaying of surrounding quotes */
   void setQuotes(bool on);

   /** is clever editing enabled? */
   bool cleverEditing() const { return _cleverEditing; }
   /** enable or disable clever editing */
   void setCleverEditing(bool on);
   /** is highlighting background enabled? */
   bool highlightBg() const { return _highlightBg; }
   /** enable or disable highlighting background*/
   void setHighlightBg(bool on);
   QColor bgColor() const { return _bgColor; }
   void setBgColor(const QColor& color);

   bool spacePoints() const { return _spacePoints; }
   void setSpacePoints(bool on);

   bool highlightSyntax() const { return _hlSyntax; }
   void highlight();
   void setHighlightSyntax(bool on);
   void setHighlightColors(const QColor& quoteColor, const QColor& unquoteColor
             , const QColor& cformatColor, const QColor& accelColor, const QColor& tagColor);

   void setFont(const QFont& font);

   void setDiffMode(bool on);
   void setDiffDisplayMode(bool underlineAdded, bool strikeOutDeleted);
   void setDiffColors(const QColor& addColor, const QColor& delColor);
   
   void setTextColor(const QColor &color);
   void setErrorColor(const QColor &color);
   
   void setCurrentColor(const TextColor color);
   
   void setSpellChecker(KSpell* spell);
   
   void selectTag(int start, int length);
   
public slots:
   virtual void setText(const QString& s);
   void paintSpacePoints();
   void paintSpacePoints( int para, int pos ); // overloaded for signal QTextEdit::cursorPositionChanged

   /**
   *  reimplemented to call highlight()
   */
   void repaint(); 
   void forceUpdate();
   void emittedTextChanged();
   
   /**
   * reimplemented to skip in case of disabled updates
   */
   void ensureCursorVisible();
   
protected:
   virtual void paintEvent (QPaintEvent * event );
   virtual void drawContents( QPainter *painter, int clipx, int clipy, int clipw, int cliph );
   
   virtual void keyPressEvent(QKeyEvent*);
   virtual void keyReleaseEvent(QKeyEvent*);

private:
   /**
   * Computes the pixel position in line which corresponds to
   * character position xIndex
   */
   QRect mapToView( int para, int index );

   /**
    * tests if the character in string str at position col is masked with
    * '\' by counting the number of '\' backwards
    */
   static bool isMasked(QString *str,uint col);

private:
   bool _quotes;
   bool _cleverEditing;
   bool _highlightBg;
   bool _spacePoints;
   QColor _bgColor;
   QColor _textColor;
   QColor _errorColor;
   QColor _currentColor;
   
   QPixmap* _whitespace;
   QPixmap* _whitespaceNB;
   QPixmap* _errorWhitespace;
   QPixmap* _errorWhitespaceNB;
   
   int _wsOffsetX;
   int _wsOffsetY;

   bool _hlSyntax;
   QColor _quoteColor;
   QColor _unquoteColor;
   QColor _cformatColor;
   QColor _accelColor;
   QColor _tagColor;

   struct DiffInfo
   {
       bool add;
       int begin;
       int end;
   };

   QPtrVector< QValueList<DiffInfo> > diffPos;
   bool _showDiff;
   bool _diffUnderlineAdd;
   bool _diffStrikeOutDel;
   QColor _diffAddColor;
   QColor _diffDelColor;
   
   // for Alt+123 feature
   int _currentUnicodeNumber;
   
   KBabelHighlighter * highlighter;
   
   // next tag highlighting
   int _tagStartPara, _tagStartIndex, _tagEndPara, _tagEndIndex;
};

#endif // MYMULTILINEEDIT_H
