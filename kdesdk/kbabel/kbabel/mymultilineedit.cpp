/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2001-2004 by Stanislav Visnovsky
			    <visnovsky@kde.org>

  Alt+123 feature idea taken from KOffice by David Faure <david@mandrakesoft.com>.
  Word wrap support by Jarno Elonen <elonen@iki.fi>, 2003

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


#include "mymultilineedit.h"
#include "editcmd.h"
#include "resources.h"

#include <qpixmap.h>
#include <qpainter.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qdragobject.h>
//#include <private/qrichtext_p.h>
#include <qpopupmenu.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kstdaccel.h>

#include "kbhighlighting.h"

using namespace KBabel;

MyMultiLineEdit::MyMultiLineEdit(int ID, QWidget* parent,const char* name)
                :KTextEdit(parent,name), emitUndo(true), 
		_firstChangedLine(0), 
		_lastChangedLine(0), 
        	_lastParagraph(0), 
		_lastParagraphOffset(0), 
		_lastSelectionStart(-1), 
		_lastSelectionEnd(-1),
		_dontUpdate(false), _myID (ID),
		 _menu(0), _overwrite(false)
{
   setUndoRedoEnabled(false); // we handle this ourselves
   setWordWrap( WidgetWidth );
   viewport()->setAcceptDrops( false ); // we need our parent to get drops

   connect(this, SIGNAL(selectionChanged()), this, SLOT( onSelectionChanged() ) );
}

void MyMultiLineEdit::onSelectionChanged()
{
    kdDebug(KBABEL) << "MyMultiLineEdit::onSelectionChanged" << endl;
    int parFrom, parTo, indexFrom, indexTo;
    if ( hasSelectedText() ) {
        getSelection( &parFrom, &indexFrom, &parTo, &indexTo );
        kdDebug(KBABEL) << "parFrom=" << parFrom << "indexFrom=" << indexFrom << "parTo=" << parTo << "indexTo=" << indexTo << endl;
        _lastSelectionStart = beginOfMarkedText();
    }
    else
    {
        _lastSelectionStart = -1; // no selection
        _lastSelectionEnd = -1;
    }

    //kdDebug(KBABEL) << "_lastSelectionStart=" << _lastSelectionStart << endl;
}

void MyMultiLineEdit::processCommand(EditCommand* cmd, bool undo)
{
   if(cmd->terminator()!=0)
      return;

   DelTextCmd* delcmd = (DelTextCmd*) cmd;
   bool ins =  true;
   if (delcmd->type() == EditCommand::Delete )
      ins = undo;
   else if (delcmd->type() == EditCommand::Insert )
      ins = !undo;
   else
   {
      return;
   }

  // avoid duplicate update of catalog
  bool oldEmitUndo = emitUndo;
  emitUndo = false;

  QPalette _visibleHighlight( palette() );
  QPalette _invisibleHighlight( palette() );
  QColorGroup newcg( colorGroup() );
  newcg.setColor( QColorGroup::HighlightedText, newcg.text() );
  newcg.setColor( QColorGroup::Highlight, newcg.base() );
  if( hasFocus() ) _invisibleHighlight.setActive( newcg );
  else _invisibleHighlight.setInactive( newcg );
  setPalette( _invisibleHighlight );   
   
  if(delcmd->offset <= (int)_lastParagraphOffset)
  {
    _lastParagraph=0;
    _lastParagraphOffset=0;
  }
  
   if ( ins )
   {
      int row, col;
      
      offset2Pos( delcmd->offset, row, col );
      setCursorPosition( row, col );
      
      _firstChangedLine=row;
      if(delcmd->str.find("\n")>0 )_lastChangedLine=row+delcmd->str.contains("\n");
      else _lastChangedLine=row;
      
      KTextEdit::insert( delcmd->str ); 

      offset2Pos( delcmd->offset+delcmd->str.length(), row, col );
      setCursorPosition( row, col);
   }
   else
   { // del

      int row, col, rowEnd, colEnd;

      offset2Pos( delcmd->offset, row, col );
      offset2Pos( delcmd->offset + delcmd->str.length(), rowEnd, colEnd );
      
      setSelection( row, col, rowEnd, colEnd, 0 );
      _firstChangedLine=_lastChangedLine=row;
      KTextEdit::removeSelectedText();
   }


   setPalette( _visibleHighlight );   

   emitUndo = oldEmitUndo;
      
   emitCursorPosition();
}

int MyMultiLineEdit::beginOfLastMarkedText()
{
    if ( _lastSelectionStart != -1 )
        return _lastSelectionStart;
    else
        return currentIndex();
}

int MyMultiLineEdit::endOfLastMarkedText()
{
    if ( _lastSelectionEnd != -1 )
        return _lastSelectionEnd;
    else
        return currentIndex();
}

int MyMultiLineEdit::beginOfMarkedText()
{
	int beginX=0;
	int beginY=0;
	int endX=0;
	int endY=0;

	int pos=-1;
	
	getSelection(&beginY,&beginX,&endY,&endX);
	if( hasSelectedText() )
	{
		pos = pos2Offset(beginY,beginX);
	}

	return pos;
}

void MyMultiLineEdit::emitCursorPosition()
{
    int line=0;
    int col=0;
    getCursorPosition(&line,&col);
    
    emit cursorPositionChanged(line, col);
}

void MyMultiLineEdit::wheelEvent(QWheelEvent *e)
{
    e->ignore();
}

void MyMultiLineEdit::focusInEvent(QFocusEvent *e)
{
    KTextEdit::focusInEvent(e);
    emitCursorPosition();
}

void MyMultiLineEdit::contentsContextMenuEvent( QContextMenuEvent * e)
{
    e->accept();
    if( _menu ) _menu->exec( e->globalPos() );
}

QPopupMenu * MyMultiLineEdit::createPopupMenu()
{
    return _menu;
}

QPopupMenu * MyMultiLineEdit::createPopupMenu(const QPoint &)
{
    return 0;
}

void MyMultiLineEdit::setContextMenu( QPopupMenu * menu )
{
    _menu = menu;
}

void MyMultiLineEdit::doKeyboardAction( KeyboardAction action )
{
  int row,col;
  getCursorPosition(&row, &col);
  
  switch( action ) {
      case ActionDelete: 
        _firstChangedLine=_lastChangedLine=row;
	my_del(); break;

      case ActionBackspace:
        _firstChangedLine=_lastChangedLine=row;
	my_backspace(); break;

      case ActionReturn:
	if( emitUndo) 
	    emit signalUndoCmd( new InsTextCmd(currentIndex(), "\n", _myID) );
	break;
  
      case ActionKill:
        _firstChangedLine=_lastChangedLine=row;
        if(emitUndo)
	{
	    int x,y;
	    getCursorPosition( &x, &y );
	    QString s = text(x);
	    if( y < (int)s.length()-1  ) // not the end of paragraph
	    {
		QString delText = s.mid( y, s.length()-y-1);
	        emit signalUndoCmd( new DelTextCmd(currentIndex(), delText, _myID ) );
	    } else 
		if( x < paragraphs()-1 ) // not the end of text
		    emit signalUndoCmd( new DelTextCmd(currentIndex(), "\n", _myID ) );
	}
        break;
     default: break;
  }

  KTextEdit::doKeyboardAction( action );

  emitCursorPosition();
}

void MyMultiLineEdit::setText(const QString& s)
{
    _lastParagraph=0;
    _lastParagraphOffset=0;
    // workaround, since insert does not interpret markup
    setTextFormat( Qt::PlainText );
    _firstChangedLine=_lastChangedLine=0;
    KTextEdit::setText(s);
    setTextFormat( Qt::AutoText );
    // now the number of lines is known, let's do highlight
    _lastChangedLine=paragraphs();
    emit textChanged();
    emitCursorPosition();
}

void MyMultiLineEdit::insertAt( const QString & s, int line, int col, bool mark )
{
   // it will invoke insert, don't need to send InsTextCmd
   KTextEdit::insertAt(s,line,col);
   
   // code from QMultiLineEdit
   if( mark )  
	setSelection( line, col, line, col + s.length(), 0 );
   // end of copied code
   
   emitCursorPosition();
}

void MyMultiLineEdit::insert( const QString & text, bool indent, bool checkNewLine, bool removeSelected )
{
   int row,col;

   bool noSelectionRemoved = true;
   setUpdatesEnabled(false);
   if( removeSelected && hasSelectedText() )
   {
	int endRow,endCol;
	getSelection(&row,&col,&endRow,&endCol);

	if( row < (int)_lastParagraph )
	{
	    _lastParagraph=0;
	    _lastParagraphOffset=0;
	}
	
	_firstChangedLine=_lastChangedLine=row;
	removeSelectedText();
	noSelectionRemoved = false;
   }
	
   getCursorPosition(&row,&col);
   _firstChangedLine=row;
   _lastChangedLine=row;

   if( emitUndo)
   {
	emit signalUndoCmd( new BeginCommand(-1,UndefPart));
	// reimplemented overwrite
	if( _overwrite && noSelectionRemoved) 
	{
	    doKeyboardAction( ActionDelete );
	}
	
	emit signalUndoCmd( new InsTextCmd(currentIndex(), text, _myID) );
	emit signalUndoCmd( new EndCommand(-1,UndefPart));
   }
   
   int n=text.find("\n");
   if( n > 0 ) _lastChangedLine+=n;
   
   // setup palettes
   
   QPalette _visibleHighlight( palette() );
   QPalette _invisibleHighlight( palette() );
   QColorGroup newcg( colorGroup() );
   newcg.setColor( QColorGroup::HighlightedText, newcg.text() );
   newcg.setColor( QColorGroup::Highlight, newcg.base() );
   if( hasFocus() ) _invisibleHighlight.setActive( newcg );
   else _invisibleHighlight.setInactive( newcg );
   setPalette( _invisibleHighlight );   
   KTextEdit::insert(text, indent, checkNewLine, removeSelected);
   setPalette( _visibleHighlight );   
 
   setUpdatesEnabled(true);

   emitCursorPosition();
}

void MyMultiLineEdit::removeLine ( int line )
{
   kdDebug(KBABEL) << "removeLine invoked" << endl;

   KTextEdit::removeParagraph(line);
   emitCursorPosition();
}

void MyMultiLineEdit::clear()
{
    _lastParagraph=0;
    _lastParagraphOffset=0;
    
    _dontUpdate=true;

   QString s = text();
   if( !s.isEmpty() && emitUndo ) {
	emit signalUndoCmd( new BeginCommand(-1,UndefPart) );
	emit signalUndoCmd( new DelTextCmd(0,s,_myID) );
	emit signalUndoCmd( new EndCommand(-1,UndefPart) );
   }
   
   KTextEdit::clear();
   
   _dontUpdate=false;
   
   _firstChangedLine=_lastChangedLine=0;
   emitCursorPosition();
}


void MyMultiLineEdit::my_backspace()
{

   int cursorY, cursorX;
   getCursorPosition( &cursorY, &cursorX );

   if( hasSelectedText())
   {
	  Q_ASSERT( "backspace: This should never happen, why is not invoked removeSelectedText()?");
   }
   else if(! (cursorY==0 && cursorX==0) )
   {
      if(emitUndo)
      {
         int offset = currentIndex();

         QString s= text(cursorY);
         if(cursorX != 0)
         {
            QString delTxt(s[cursorX-1]);
            emit signalUndoCmd(new DelTextCmd(offset-1,delTxt,_myID));
         }
         else if( cursorY > 0 || cursorX > 0 ) // not at the beginning
         {
            emit signalUndoCmd(new DelTextCmd(offset-1,"\n",_myID));
         }
      }
   }
}

void MyMultiLineEdit::my_del()
{

   int cursorY, cursorX;
   getCursorPosition( &cursorY, &cursorX );

   if( hasSelectedText())
   {
	  Q_ASSERT( "del: This should never happen, why is not invoked removeSelectedText()?");
   }
   else if(! (cursorY==paragraphs()-1 && cursorX==paragraphLength( cursorY )) )
   {
      if(emitUndo)
      {
         int offset = pos2Offset(cursorY, cursorX);

         QString s=text(cursorY);
         if(cursorX != (int)s.length()-1)
         {
            QString delTxt(s[cursorX]);
            emit signalUndoCmd(new DelTextCmd(offset,delTxt,_myID));
         }
         else if( cursorY < (int)paragraphs()-1 || ( (cursorY == (int)paragraphs()-1) && (cursorX < (int)text( paragraphs()-1 ).length()-1 ) ) )// !atEnd() )
         {
            emit signalUndoCmd(new DelTextCmd(offset,"\n",_myID));
         }
      }
   }
}

void MyMultiLineEdit::removeSelectedText(int selNum)
{
    if( selNum != 0 ) 
    {
	_lastParagraph=0;
	_lastParagraphOffset=0;
	
	KTextEdit::removeSelectedText(selNum);
    }
    else 
    {
	  int paraFrom, idxFrom, paraTo, idxTo;
	  KTextEdit::getSelection( &paraFrom, &idxFrom, &paraTo, &idxTo );
	  
	  if( paraFrom < (int)_lastParagraph )
	  {
	    _lastParagraph=0;
	    _lastParagraphOffset=0;
	  }

	  int offset = pos2Offset( paraFrom, idxFrom );
	  emit signalUndoCmd(new DelTextCmd( offset, selectedText(), _myID ) );
	  KTextEdit::removeSelectedText(selNum);
    }

    emitCursorPosition();
}

void MyMultiLineEdit::paste()
{
    KTextEdit::paste();    
    emitCursorPosition();
}

int MyMultiLineEdit::currentIndex()
{
    int para; // paragraph of current position
    int index; // index in the current paragraph
    
    KTextEdit::getCursorPosition(&para,&index);
    
    return pos2Offset( para, index );
}


void MyMultiLineEdit::offset2Pos(int offset, int &paragraph, int &index) const
{
    if (offset <= 0)
    {
	paragraph = 0;
	index = 0;
	return;
    }
    else
    {
       int charsLeft = offset;
       int i;

       for( i = 0; i < paragraphs(); ++i )
       {
           if (paragraphLength( i ) < charsLeft)
              charsLeft -= paragraphLength( i );
           else
           {
              paragraph = i;
              index = charsLeft;
              return;
           }
	   --charsLeft;
       }
    
          paragraph = i-1;
          index = charsLeft;
       return;
    }
}

int MyMultiLineEdit::pos2Offset(uint paragraph, uint index)
{
    paragraph = QMAX( QMIN( (int)paragraph, paragraphs() - 1), 0 ); // Sanity check
    index  = QMAX( QMIN( (int)index,  paragraphLength( paragraph )), 0 ); // Sanity check

    {
        uint lastI;
	lastI  = paragraphLength( paragraph );
	uint i = 0; 
	uint tmp = 0;

	if( paragraph>=_lastParagraph )
	{
	    tmp = _lastParagraphOffset;
	    i  = _lastParagraph;
	}

	for( ;i < paragraph ; i++ )
	{
	    tmp += paragraphLength( i ) + 1;
	}
	
	_lastParagraphOffset=tmp;
	_lastParagraph=paragraph;

	tmp += QMIN( lastI, index );

	return tmp;
    }
}

void MyMultiLineEdit::setReadOnly(bool on)
{
	// I want this backgroundmode, also when readonly==true
	if(on) 
	{
		setBackgroundMode(PaletteBase);
	}
	
	QTextEdit::setReadOnly(on);
}

void MyMultiLineEdit::setOverwriteMode( bool b )
{
    _overwrite = b;
}

/*******************************************************************************/
MsgMultiLineEdit::MsgMultiLineEdit(int ID, KSpell* spell, QWidget* parent,const char* name)
                :MyMultiLineEdit(ID, parent,name),
                _quotes(false),
                _cleverEditing(false),
                _highlightBg(false),
                _spacePoints(false),
                _bgColor(colorGroup().base().dark(110)),
		_textColor(KGlobalSettings::textColor()),
		_errorColor(Qt::red),
		_currentColor(KGlobalSettings::textColor()),
		_whitespace(0),
                _hlSyntax(true),
                _quoteColor(Qt::darkGreen),
                _unquoteColor(Qt::red),
                _cformatColor(Qt::blue),
                _accelColor(Qt::darkMagenta),
                _showDiff(false),
                _diffUnderlineAdd(true),
                _diffStrikeOutDel(true),
                _diffAddColor(Qt::darkGreen),
                _diffDelColor(Qt::darkRed),
		_currentUnicodeNumber(0),
		highlighter(0),
		_tagStartPara(0), _tagStartIndex(0), _tagEndPara(0), _tagEndIndex(0)
{
   diffPos.setAutoDelete(true);
   diffPos.clear();
   
   _whitespace = new QPixmap(2,2,-1,QPixmap::BestOptim);
   _whitespace->fill(_textColor);
   _errorWhitespace = new QPixmap(2,2,-1,QPixmap::BestOptim);
   _errorWhitespace->fill(_errorColor);
   
   _whitespaceNB = new QPixmap(3,3,-1,QPixmap::BestOptim);
   _whitespaceNB->fill();
   _errorWhitespaceNB = new QPixmap(3,3,-1,QPixmap::BestOptim);
   _errorWhitespaceNB->fill();
   
   QPainter p(_whitespaceNB);
   p.setPen( _textColor );
   p.drawEllipse(_whitespaceNB->rect());
   
   QPainter q(_errorWhitespaceNB);
   q.setPen( _errorColor );
   q.drawEllipse(_errorWhitespaceNB->rect());

   // this will setup bitBlt pixmaps
   setFont( font() );
   highlighter = new KBabelHighlighter( this, spell );
   connect( this, SIGNAL( signalSyntaxHighlightingChanged( bool ) ), highlighter, SLOT( setSyntaxHighlighting( bool ) ) );
   
   connect( this, SIGNAL( selectionChanged() ), this, SLOT( paintSpacePoints() ) );   
   connect( this, SIGNAL( cursorPositionChanged( int, int ) ), this, SLOT( paintSpacePoints(int, int) ) );
   connect( this, SIGNAL( textChanged() ), this, SLOT( emittedTextChanged() ) );
}

MsgMultiLineEdit::~MsgMultiLineEdit ()
{
    if(highlighter)
	delete highlighter;
}

void MsgMultiLineEdit::setText(const QString& s)
{
    QString str = s;
    
    if(_showDiff)
    {
        diffPos.clear();
        int lines = s.contains('\n');
        diffPos.resize(lines+1);

        QStringList lineList = QStringList::split('\n',s,true);
        
        int lineCounter=-1;
        bool haveAdd=false;
        bool haveDel=false;
        bool multiline=false;
        QStringList::Iterator it;
        for(it = lineList.begin(); it != lineList.end(); ++it)
        {
            lineCounter++;
            
            int lastPos=0;
            bool atEnd=false;

            while(!atEnd)
            {            
                int addPos=-1;
                int delPos=-1;
         
                if(haveAdd && multiline)
                {
                    addPos=0;
                }
                else
                {
                    addPos = (*it).find("<KBABELADD>",lastPos);
                }
            
                if(haveDel && multiline)
                {
                    delPos=0;
                }
                else
                {
                    delPos = (*it).find("<KBABELDEL>",lastPos);
                }

                if(delPos >= 0 && addPos >= 0)
                {
                    if(delPos <= addPos)
                    {
                        haveDel=true;
                        haveAdd=false;
                    }
                    else
                    {
                        haveDel=false;
                        haveAdd=true;
                    }
                }
                else if(delPos >= 0)
                {
                    haveDel=true;
                    haveAdd=false;
                }
                else if(addPos >= 0)
                {
                    haveDel=false;
                    haveAdd=true;
                }
                else
                {
                    atEnd=true;
                    haveAdd=false;
                    haveDel=false;
                }
                
                DiffInfo di;
                di.begin=-1;
                
                if(haveAdd)
                {
                    if(!multiline)
                    {
                        (*it).remove(addPos,11);                    
                    }
                    
                    int endPos = (*it).find("</KBABELADD>",addPos);
                    if(endPos < 0)
                    {
                        endPos = (*it).length();
                        atEnd=true;
                        multiline=true;
                    }
                    else
                    {
                        (*it).remove(endPos,12);
                        haveAdd=false;
                        multiline=false;
                    }
                    
                    lastPos=endPos;

                    di.begin=addPos;
                    di.end=endPos-1;
                    di.add=true;
                }
                else if(haveDel)
                {
                    if(!multiline)
                    {
                        (*it).remove(delPos,11); 
                    }
                    
                    int endPos = (*it).find("</KBABELDEL>",delPos);
                    if(endPos < 0)
                    {
                        endPos = (*it).length();
                        atEnd=true;
                        multiline=true;
                    }
                    else
                    {
                        (*it).remove(endPos,12);
                        haveDel=false;
                        multiline=false;
                    }
                    
                    lastPos=endPos;

                    di.begin=delPos;
                    di.end=endPos-1;
                    di.add=false;
                }

                if(di.begin >= 0)
                {
                    QValueList<DiffInfo> *list = diffPos[lineCounter];
                    if(!list)
                    {
                        list = new QValueList<DiffInfo>;
                        diffPos.insert(lineCounter,list);
                    }

                    list->append(di);
                }
                
            }
        }
        
        QRegExp reg("</?KBABELADD>");
        str.replace(reg,"");
        reg.setPattern("</?KBABELDEL>");
        str.replace(reg,"");
    }
    
    MyMultiLineEdit::setText(str);
    paintSpacePoints();
}

void MsgMultiLineEdit::setQuotes(bool on)
{
   _quotes=on;
   update();
}

void MsgMultiLineEdit::setCleverEditing(bool on)
{
   _cleverEditing=on;
}


void MsgMultiLineEdit::setHighlightBg(bool on)
{
   _highlightBg=on;
   update();
}


void MsgMultiLineEdit::setBgColor(const QColor& color)
{
   _bgColor=color;

   if(_highlightBg)
      update();
}

void MsgMultiLineEdit::setSpacePoints(bool on)
{
   _spacePoints=on;

   update();
}

void MsgMultiLineEdit::setHighlightSyntax(bool on)
{
   _hlSyntax=on;
   
   emit signalSyntaxHighlightingChanged (on);
   
   update();
}

void MsgMultiLineEdit::setHighlightColors(const QColor& quoteColor, const QColor& unquoteColor
                 , const QColor& cformatColor, const QColor& accelColor, const QColor& tagColor)
{
   _quoteColor=quoteColor;
   _unquoteColor=unquoteColor;
   _cformatColor=cformatColor;
   _accelColor=accelColor;
   _tagColor=tagColor;

   highlighter->setHighlightColor( KBabelHighlighter::Tag, tagColor );
   highlighter->setHighlightColor( KBabelHighlighter::Entity, accelColor );
   highlighter->setHighlightColor( KBabelHighlighter::CFormat, cformatColor );
   highlighter->setHighlightColor( KBabelHighlighter::Masked, quoteColor );
   
   update();
}


void MsgMultiLineEdit::setFont(const QFont& font)
{
   KTextEdit::setFont(font);
   
   // we don't need to calculate a special offset for non-breaking space, since
   // they are very similar in size
   QFontMetrics fm(font);
   _wsOffsetX = QMAX(fm.width(' ')/2-2,1);
   _wsOffsetY = QMAX(fm.height()/2-1,0);
   
   repaint();
}

void MsgMultiLineEdit::setDiffDisplayMode(bool addUnderline, bool delStrikeOut)
{
    _diffUnderlineAdd = addUnderline;
    _diffStrikeOutDel = delStrikeOut;
    
    if(_showDiff)
        update();
}

void MsgMultiLineEdit::setDiffColors(const QColor& addColor
        , const QColor& delColor)
{
    _diffAddColor = addColor;
    _diffDelColor = delColor;

    if(_showDiff)
        update();
}

void MsgMultiLineEdit::setTextColor(const QColor &color )
{
  QPalette p( palette() );
  QColorGroup newcg( colorGroup() );
  newcg.setColor( QColorGroup::Text, color );
  if( hasFocus() ) p.setActive( newcg );
  else p.setInactive( newcg );
  setPalette( p );
  _textColor = color;
  highlighter->setHighlightColor( KBabelHighlighter::Normal, color );
}

void MsgMultiLineEdit::setErrorColor(const QColor &color )
{
  _errorColor = color;
  highlighter->setHighlightColor( KBabelHighlighter::Error, color );
}

void MsgMultiLineEdit::setCurrentColor(const TextColor color)
{
  if( color == NormalColor ) {
    _currentColor = _textColor;
    highlighter->setHasErrors( false );
  } else {
    _currentColor = _errorColor;
    highlighter->setHasErrors( true );
  }
  
  /*
  setUpdatesEnabled(false);
  // need to block signals (especially textChanged() to avoid recursion with KBabelView::autoCheck
  blockSignals(true);
  selectAll();
  setColor( _currentColor );
  removeSelection();
  setColor(_currentColor);
  blockSignals(false);
  setUpdatesEnabled(true);
  */
  forceUpdate();
}

void MsgMultiLineEdit::setSpellChecker(KSpell* spell)
{
    highlighter->setSpellChecker(spell);
}

void MsgMultiLineEdit::paintSpacePoints(int, int )
{
    paintSpacePoints();
}

void MsgMultiLineEdit::paintSpacePoints()
{
    QRect r;
    QPainter painter(viewport() );
    const QFontMetrics& fm = fontMetrics();

    int paranum = paragraphAt(QPoint(contentsX(), contentsY()));    

    if( _spacePoints )
    {
        int curpara = paranum;
        painter.setPen( _currentColor );
        QPixmap* ws, *wsnb;

        if( _currentColor== _errorColor )
        {
            ws = _errorWhitespace;
            wsnb = _errorWhitespaceNB;
        }
        else
        {
            ws = _whitespace;
            wsnb = _whitespaceNB;
        }

        while( curpara < paragraphs())
        {
            if ( paragraphRect( curpara ).top() > contentsY()+visibleHeight()) break;

            const QString& s = text(curpara);
            int i = s.find( " " );
            while( (i >= 0) && (i < (int)s.length()-1) ) // -1 because text will end by EOLN
            {
                QPixmap* pm = ( s.at(i).unicode() == 0x00A0U ) ? wsnb : ws;
                QRect r = mapToView( curpara, i );
                r.moveBy( r.width()/2, (r.height() - fm.descent())/2 );
                r.moveBy( -pm->rect().width()/2, -pm->rect().height()/2-1 );
                bitBlt(viewport(), r.topLeft(), pm, pm->rect(), Qt::CopyROP);
                i = s.find( " ", i+1 );
            }
            ++curpara;
        }
    }

    if( _quotes )
    {
        QFontMetrics fm( font());
        QRect qs = fm.boundingRect("\"");

        for( int curpara = paranum; curpara < paragraphs() ; curpara++ )
        {
            r = paragraphRect(curpara);
            if( r.y() > contentsY()+visibleHeight() ) break;

            painter.drawText( QPoint( 0, mapToView( curpara, 0 ).top()) +
                    QPoint(0, qs.height() + 4), "\""); // 4 = hardcoded margin in QT
            painter.drawText( mapToView( curpara, QMAX( 0,
                ((int)text( curpara ).length())-1)).topRight() +
                QPoint(0, qs.height() + 4), "\"");  // 4 = hardcoded margin in QT
        }
    }

    if( _showDiff && (!_diffUnderlineAdd || !_diffStrikeOutDel) )
    {
        if( paragraphs() == (int)diffPos.size() ) // sanity check
        {
            painter.setRasterOp( Qt::AndROP );
            for( int curpara = paranum; curpara < paragraphs() ; curpara++ )
            {
                r = paragraphRect(curpara);
                if( r.y() > contentsY()+visibleHeight() ) break;

                QValueList<DiffInfo> *list = diffPos[curpara];
                if(list)
                {
                    QValueList<DiffInfo>::ConstIterator it;
                    for(it = list->begin(); it != list->end(); ++it)
                    {
                        QRect beg =  mapToView( curpara, (*it).begin );
                        QRect end =  mapToView( curpara, (*it).end );

                        QColor* c = 0;
                        if( (*it).add  && !_diffUnderlineAdd)
                            c = &_diffAddColor;
                        else if(!(*it).add && !_diffStrikeOutDel)
                            c = &_diffDelColor;

                        if ( c != 0 )
                        {
                            // Single or multiple lines?
                            if ( beg.top() == end.top())
                            {
                                painter.fillRect( QRect( beg.topLeft(),
                                    QPoint( end.right(), end.bottom())), *c );
                            }
                            else
                            {
                                painter.fillRect( QRect(
                                    beg.topLeft(),
                                    QPoint( r.right(), beg.bottom())), *c );
				if( end.top()-beg.bottom() > 2 ) {
				    // there is a line, not only thin space
                            	    painter.fillRect( QRect(
                                	QPoint( r.left(), beg.bottom()),
                                	QPoint( r.right(), end.top())), *c );
				}
                                painter.fillRect( QRect(
                                    QPoint( r.left(), end.top()),
                                    QPoint( end.right(), end.bottom())), *c );
                            }
                        }
                    }
                }
            }
        }
    }

    if( _showDiff && (_diffUnderlineAdd || _diffStrikeOutDel) )
    {
        if( paragraphs() == (int)diffPos.size() ) // sanity check
        {
            for( int curpara = paranum; curpara < paragraphs() ; curpara++ )
            {
                r = paragraphRect(curpara);
                if( r.y() > contentsY()+visibleHeight() ) break;

                QValueList<DiffInfo> *list = diffPos[curpara];
                if(list)
                {
                    QPen addPen(_diffAddColor,2);
                    QPen delPen(_diffDelColor,2);
                    QValueList<DiffInfo>::ConstIterator it;
                    for(it = list->begin(); it != list->end(); ++it)
                    {
                        QRect beg = mapToView( curpara, (*it).begin );
                        QRect end = mapToView( curpara, (*it).end );

                        QPen* p = 0;
                        int dy = 0;
                        if( (*it).add && _diffUnderlineAdd)
                            p = &addPen;
                        else if(!(*it).add && _diffStrikeOutDel)
                        {
                            p = &delPen;
                            dy = fm.ascent()/2-1;
                        }

                        if ( p != 0 )
                        {
                            painter.setPen( *p );

                            // Single or multiple lines?
                            if ( beg.top() == end.top())
                                painter.drawLine(
                                    beg.topLeft() + QPoint(0, fm.ascent()-dy),
                                    end.topRight()+ QPoint(0, fm.ascent()-dy));
                            else
                            {
                                int y = beg.top() + fm.ascent();
                                painter.drawLine(
                                    QPoint(beg.left(), y),
                                    QPoint(r.right(), y));
                                y += fm.lineSpacing();
                                while (y < end.top() + fm.ascent())
                                {
                                    painter.drawLine(
                                        QPoint(r.left(), y),
                                        QPoint(r.right(), y));
                                    y += fm.lineSpacing();
                                }
                                painter.drawLine(
                                    QPoint(r.left(), end.top() + fm.ascent()),
                                    QPoint(end.right(), end.top() + fm.ascent()));
                            }
                        }
                    }
                }
            }
        }
    }
}

void MsgMultiLineEdit::repaint()
{
    highlight();
    MyMultiLineEdit::repaint();
}

void MsgMultiLineEdit::forceUpdate()
{
    _firstChangedLine=0;
    _lastChangedLine=paragraphs()-1;
    highlighter->highlight();
    MyMultiLineEdit::repaint();
}

void MsgMultiLineEdit::ensureCursorVisible()
{
    if( isUpdatesEnabled() )
	MyMultiLineEdit::ensureCursorVisible();
}

void MsgMultiLineEdit::highlight()
{
/*    if( _dontUpdate ) return;
    
    QColor bg;
    if( _highlightBg ) bg = _bgColor;
    else bg = colorGroup().base(); 

    for( int i = 0 ; i < paragraphs() ; i++ )
	    setParagraphBackgroundColor( i, bg );
	

    if(_hlSyntax) 
    {
	blockSignals(true); // block signals to avoid recursion
	setUpdatesEnabled(false);
	int cursorParagraph, cursorIndex;

	getCursorPosition( &cursorParagraph, &cursorIndex );

	// setup new colors
	
	uint i;
	
	QRegExp markup("(\\\\)|(\")|(\\\\[abfnrtv'\"\?\\\\])|(\\\\\\d+)|(\\\\x[\\dabcdef]+)"
	    "|(%[\\ddioxXucsfeEgGphln]+)|(&[^\\s])|(&[\\w-]+;)");

	for( i = QMAX(_firstChangedLine,0) ; i < QMIN(_lastChangedLine+1,(uint)paragraphs()) ; i++ ) {
	
	QString line=text(i);

	//remove old highlighting
	setSelection(i,0,i,line.length());
	setColor( _currentColor );
	removeSelection();
	
	QColor colorToUse;

	int index=0;
	index=markup.search( line, index );
	while(index>=0)
	{
	    switch( line[index].latin1() )
	    {
		case '\\':
		    if( markup.matchedLength() == 1 ) colorToUse=_unquoteColor;
		    else colorToUse=_quoteColor;
		    break;
		case '\"':
		    colorToUse=_unquoteColor;
		    break;
		case '%':
		    colorToUse=_cformatColor;
		    break;
		case '&':
		    colorToUse=_accelColor;
		    break;
	    }
	    
	    setSelection( i, index, i, index+markup.matchedLength(), 0);
	    setColor( colorToUse );
	    removeSelection();
	    index=markup.search( line, index+markup.matchedLength() );
	}
	}

	// Color XML and HTML tags
	
	int tagindex=0;
	int taglength=0;
	int lineindex=0;
	uint index=0;
	int startPara, endPara, startIndex, endIndex;
	QString t= text();
	
	if(_lastParagraph <= _firstChangedLine)
	{
	    index=_lastParagraph;
	    lineindex=_lastParagraphOffset;
	}
	
	for( ; index<_firstChangedLine ; index++)
	    lineindex+=paragraphLength(index)+1;
	
        QRegExp re("<.*>");
	re.setMinimal(true);

	if( _firstChangedLine >0 )
	{
	    QColor c;
	    QFont f;
	    VerticalAlignment v;
	    getFormat(_firstChangedLine-1, paragraphLength(_firstChangedLine-1)-1, &f, &c, &v);
	    QString l = text(_firstChangedLine-1);
	    if( c==_tagColor  && !l.endsWith(">") ) // hope _tagColor will be different than other colors
	    {
		QRegExp endtag("[^<]*>");
		tagindex=endtag.search(t, lineindex);
		taglength=endtag.matchedLength();
	    } else {
		tagindex=re.search(t, lineindex);
		taglength=re.matchedLength();
	    }
	} else 	{
	    tagindex=re.search( t, lineindex );
	    taglength=re.matchedLength();
	}
	
	while( tagindex >= 0 && (int)index<paragraphs())
	{
	    while( tagindex>=lineindex  && index<_lastChangedLine+2) 
		lineindex+=paragraphLength(index++)+1;
	    if(index==_lastChangedLine+2) break;
	    lineindex-=paragraphLength(index-1);
	    lineindex--;
	    index--;
		
	    startPara=index;
	    startIndex=tagindex-lineindex;
	    
	    tagindex+=taglength;

	    while( tagindex>=lineindex && (int)index<paragraphs()) 
		lineindex+=paragraphLength(index++)+1;
	    lineindex-=paragraphLength(index-1);
	    lineindex--;
	    index--;
	    
	    endPara=index;
	    endIndex=tagindex-lineindex;

	    setSelection( startPara, startIndex, endPara, endIndex, 0 );
            setColor( _tagColor );
            removeSelection();

	    if(index>_lastChangedLine) break;
            tagindex=re.search( t, tagindex );
	    taglength=re.matchedLength();
        }
																																																
	setCursorPosition( cursorParagraph, cursorIndex );
	setColor( _textColor );
	setUpdatesEnabled(true);
	blockSignals(false); // block signals to avoid recursion
	updateContents();
    }
    ensureCursorVisible();
    */
}

void MsgMultiLineEdit::drawContents( QPainter *painter, int clipx, int clipy, int clipw, int cliph )
{
    MyMultiLineEdit::drawContents( painter, clipx, clipy, clipw, cliph );
    paintSpacePoints();
}

void MsgMultiLineEdit::paintEvent( QPaintEvent *event )
{
    MyMultiLineEdit::paintEvent( event );
    paintSpacePoints();
}

QRect MsgMultiLineEdit::mapToView( int para, int index )
{
    if( para < 0 || para > paragraphs() ||
        index < 0 || index > paragraphLength(para) )
            return QRect(); //invalid rectangle

    const QFontMetrics& fm = fontMetrics();
    const QString& paratext = text(para);

    // Find index of the first character on the same line as parameter
    // 'index' using binary search. Very fast, even for long texts.
    int linestart = 0;
    int indexline = lineOfChar( para, index );
    if ( indexline > 0 )
    {
        int min = 0, max = index;
        int i = (min + max)/2;
        int iline = lineOfChar( para, i );
        while ( iline != indexline-1 ||
                lineOfChar( para, i+1 ) != indexline )
        {
            Q_ASSERT( min != max && min != i && max != i );
            if ( iline < indexline )
                min = i;
            else
                max = i;
            i = (min + max)/2;
            iline = lineOfChar( para, i );
        }
        linestart = i+1;
    }
    Q_ASSERT( linestart >= 0 );
    
    int linewidth;
    
    // if the tag is not valid, easy
    if( (_tagStartPara == _tagEndPara) && (_tagStartIndex == _tagEndIndex) ) {
	linewidth = fm.width( paratext.mid( linestart, index-linestart ));
    } else {
	int tso = pos2Offset( _tagStartPara, _tagStartIndex );
	int teo = pos2Offset( _tagEndPara, _tagEndIndex );
	int off = pos2Offset( para, index );
    
	if( off < tso ) {
	    // it is surely before the tag
	    linewidth = fm.width( paratext.mid( linestart, index-linestart ));
	} else if( off >= teo ) {
	    // it is surely after the tag
	
	    // is it on the same line as the end of the tag?
	    if( _tagEndPara < para || lineOfChar( _tagEndPara, _tagEndIndex ) < indexline ) {
		// no tag on the line, no bold
		linewidth = fm.width( paratext.mid( linestart, index-linestart ));
	    } else {
		QFont f( font() );
		f.setBold( true );
		QFontMetrics bfm( f );
		// is tag single displayed line?
		if( _tagStartPara == _tagEndPara 
		    && lineOfChar( _tagStartPara, _tagStartIndex ) == lineOfChar( _tagEndPara, _tagEndIndex ) ) 
		{
		    // yes, count the non-bold before the tag start
		    linewidth = fm.width( paratext.mid( linestart, _tagStartIndex-linestart ) )
			+ bfm.width( paratext.mid( _tagStartIndex, _tagEndIndex-_tagStartIndex ) );
		} 
		else
		{
		    // count the part of the tag itself
		    linewidth = bfm.width( paratext.mid( linestart, _tagEndIndex-linestart ) );
		}
	    
		// add the rest from tag to the index
		linewidth += fm.width( paratext.mid( _tagEndIndex, index-_tagEndIndex ) );
	    }
	}
	else {
	    // in tag
	    QFont f( font() );
	    f.setBold( true );
	    QFontMetrics bfm( f );
	    // is it the first line of the tag?
	    if( para == _tagStartPara && indexline == lineOfChar( _tagStartPara, _tagStartIndex ) ) {
		// start of the line is normal
		linewidth = fm.width( paratext.mid( linestart, _tagStartIndex-linestart ) )
		    + bfm.width( paratext.mid( _tagStartIndex, index-_tagStartIndex ) );
	    } else {
		// whole is bold
		linewidth = bfm.width( paratext.mid( linestart, index-linestart ) );
	    }
	}
    }
    
    // FIXME as soon as it's possible to ask real margins from QTextEdit:
    const int left_margin = 4;
    // const int top_margin = 4;
    
    QPainter painter( viewport());
    const QRect& linerect = paragraphRect(para);
    return QRect(
        contentsToViewport( QPoint(
            left_margin + linerect.left() + linewidth , 
            /*top_margin + */linerect.top() + indexline * fm.lineSpacing() + fm.leading())),
        QSize(
            fm.charWidth( paratext, index ),
            fm.lineSpacing()
        ));
}

void MsgMultiLineEdit::keyPressEvent(QKeyEvent *e)
{
    if(!_cleverEditing || isReadOnly())
    {
        MyMultiLineEdit::keyPressEvent(e);
        return;
    }

    KKey key( e );
    
    if(e->key() == Key_Return || e->key() == Key_Enter)
    { 
        emit signalUndoCmd(new BeginCommand(-1,UndefPart));

        int row, col;
        getCursorPosition(&row,&col);
        QString str=text(row);

        if(e->state() & ShiftButton)
        {            
            if(col > 0 && !str.isEmpty())
            {
                if(str.at(col-1) == '\\' && !isMasked(&str,col-1))
                {
                    insert("n",false);
                }
                else
                {
                    insert("\\n",false);
                }
            }
            else
            {
                insert("\\n",false);
            }
        }    
        else if(!(e->state() & ControlButton))
        {
            if(col > 0 && !str.isEmpty() && !str.at(col-1).isSpace())
            {
                if(str.at(col-1)=='\\' && !isMasked(&str,col-1))
                {
                    insert("\\",false);
                }
                
                // if there is not a new line at the end
                if(col < 2 || str.mid(col-2,2)!="\\n")
                {
                    insert(" ",false);
                }
            }
            else if(str.isEmpty())
            {
                insert("\\n",false);
            }
        }    
     
        if( !str.isEmpty())
        {
	    // construct new event without modifiers
    	    MyMultiLineEdit::keyPressEvent( new QKeyEvent(e->type(), e->key(), e->ascii(), 0,
		e->text(), e->isAutoRepeat(), e->count() ) );
	    e->accept();
        }

        emit signalUndoCmd(new EndCommand(-1,UndefPart));
        return;
    }
    else if(e->key() == Key_Tab)
    {
        insert("\\t",false);
        emit textChanged();
        e->accept();
        return;
    }
    else if((e->key() == Key_Delete && !(e->state() & ControlButton))
            || ((e->state() & ControlButton) && e->key() == Key_D) )
    {
        emit signalUndoCmd(new BeginCommand(-1,UndefPart));
        
        if(!hasSelectedText())
        {
            int row, col;
            getCursorPosition(&row,&col);
            QString str=text(row);

            if(!str.isEmpty() && col < (int)str.length() && str.at(col) == '\\'
                        && !isMasked(&str,col))
            {
                QString spclChars="abfnrtv'\"?\\";
                if(col < (int)str.length()-1 
                        && spclChars.contains(str.at(col+1)))
                {
                    del();
                }
            }
        }
        
        del();

        emit signalUndoCmd(new EndCommand(-1,UndefPart));
        emit textChanged();
        e->accept();
        return;
    }
    else if(e->key() == Key_BackSpace
            || ((e->state() & ControlButton) && e->key() == Key_H) )
    {
        emit signalUndoCmd(new BeginCommand(-1,UndefPart)); 
        
        if(!hasSelectedText())
        {
            int row, col;
            getCursorPosition(&row,&col);
            QString str=text(row);

            QString spclChars="abfnrtv'\"?\\";
            if(!str.isEmpty() && col > 0 && spclChars.contains(str.at(col-1)))
            {
                if(col > 1 && str.at(col-2)=='\\' && !isMasked(&str,col-2))
                {
                    MyMultiLineEdit::keyPressEvent(e);
                }
            }

        }
        
        MyMultiLineEdit::keyPressEvent(e);

        emit signalUndoCmd(new EndCommand(-1,UndefPart));

        e->accept();
        return;
    }
    else if(e->text() == "\"")   
    {
        emit signalUndoCmd(new BeginCommand(-1,UndefPart));

        int row, col;
        getCursorPosition(&row,&col);
        QString str=text(row);
    
        if(col == 0 || str.at(col-1) != '\\' || isMasked(&str,col-1) )
        {
            insert("\\\"",false);
        }
        else
        {
            insert("\"",false);
        }

        e->accept();

        emit signalUndoCmd(new EndCommand(-1,UndefPart));
        return;
    }
    else if(e->key() == Key_Space && ( e->state() & AltButton ) )
    {
	insert( QChar( 0x00a0U ) );
	e->accept();
	return;
    }
    // ALT+123 feature
    else if(( e->state() & AltButton ) && e->text()[0].isDigit() )
    {
	QString text=e->text();
	while ( text[0].isDigit() ) {
	    _currentUnicodeNumber = 10*_currentUnicodeNumber+(text[0].digitValue());
	    text.remove( 0, 1 );
	}
    }
    else
    { 
        MyMultiLineEdit::keyPressEvent(e);
    }
}

void MsgMultiLineEdit::keyReleaseEvent(QKeyEvent* e)
{
    if ( e->key() == Key_Alt && _currentUnicodeNumber >= 32 )
    {
        QString text = QChar( _currentUnicodeNumber );
	_currentUnicodeNumber=0;
        insert( text );
    }
}

void MsgMultiLineEdit::setDiffMode(bool on)
{
    _showDiff=on;
    
    if(!on)
    {
        diffPos.clear();
    }
}

bool MsgMultiLineEdit::isMasked(QString *str, uint col)
{
    if(col == 0 || !str)
        return false;

    uint counter=0;
    int pos=col;
    
    while(pos >= 0 && str->at(pos) == '\\')
    {
        counter++;
        pos--;
    }
    
    return !(bool)(counter%2);
}

void MsgMultiLineEdit::emittedTextChanged()
{
    highlight();
    paintSpacePoints();    
}

void MsgMultiLineEdit::selectTag(int start, int length)
{
    setUpdatesEnabled(false);
    setSelection( _tagStartPara, _tagStartIndex, _tagEndPara, _tagEndIndex);
    setBold( false );

    offset2Pos(start, _tagStartPara, _tagStartIndex);
    offset2Pos(start+length, _tagEndPara, _tagEndIndex);
    
    setSelection( _tagStartPara, _tagStartIndex, _tagEndPara, _tagEndIndex);
    setBold( true );
    setUpdatesEnabled(true);
}

#include "mymultilineedit.moc"
