/***************************************************************************
                          hidingmsgedit.cpp  -  description
                             -------------------
    begin                : So nov 2 2002
    copyright            : (C) 2002 by Stanislav Visnovsky
    email                : visnovsky@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
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

 ***************************************************************************/

#include "resources.h"
#include "hidingmsgedit.h"
#include "mymultilineedit.h"
#include "editcmd.h"

#include <qptrlist.h>
#include <qsizepolicy.h>
#include <qstringlist.h>
#include <qtabwidget.h>

#include <kdebug.h>
#include <klocale.h>

using namespace KBabel;

HidingMsgEdit::HidingMsgEdit(uint numberOfPlurals, QWidget* eventFilter, KSpell* spell, QWidget *parent, const char *name ) : 
    QWidgetStack(parent,name)
    , _singleEdit(0)
    , _multipleEdit(0) 
    , _eventFilter(eventFilter)
    , _currentEdit(0)
    , _numberOfPlurals(numberOfPlurals)
    , _spell(spell)
{

  _allEdits.clear();
  setNumberOfPlurals( _numberOfPlurals );
  
  connect( _multipleEdit, SIGNAL(currentChanged( QWidget* )),
    this, SLOT( newCurrentMultiple( QWidget* )));
    
  showSingle();
}

HidingMsgEdit::~HidingMsgEdit(){
}

void HidingMsgEdit::setText(QStringList texts, QString msgctxt){
  if( texts.count() == 0 )
  {
      kdWarning() << "HidingMsgEdit::setText with empty text" << endl;
      _singleEdit->clear();
      showSingle();
      return;
  }

  if (!msgctxt.isEmpty())
      msgctxt = "\n>>>>> " + i18n("Context inserted by KBabel, do not translate:") + "\n" + msgctxt;
  
  if( texts.count() == 1 )
  {
      _singleEdit->setText(*texts.at(0) + msgctxt);
      showSingle();
  }
  else
  {
      if( _numberOfPlurals )
      {
          QStringList::iterator text = texts.begin();
	  uint i;
          for( i=0 ; i < _numberOfPlurals && text!= texts.end() ; i++, text++ )
	  {
	      static_cast<MsgMultiLineEdit *>(_multipleEdit->page(i))->setText(*text + msgctxt);
	  }
	  
	  // clean the non-initialized ones
	  while (i < _numberOfPlurals)
	  {
	      static_cast<MsgMultiLineEdit *>(_multipleEdit->page(i))->setText("");
	    i++;
	  }
      }
      showMultiple();
  }
}

void HidingMsgEdit::showSingle(){
    raiseWidget(_singleEdit);
    _currentEdit=_singleEdit;
    _currentEdit->setFocus();

    emit currentFormChanged ( 0 );
}

void HidingMsgEdit::showMultiple(){
    raiseWidget(_multipleEdit);
    _currentEdit=static_cast<MsgMultiLineEdit*>(_multipleEdit->currentPage());
    _currentEdit->setFocus();

    emit currentFormChanged ( _multipleEdit->currentPageIndex () );
}

void HidingMsgEdit::showPlurals( bool on )
{
    if( on ) showMultiple();
    else showSingle();
}

void HidingMsgEdit::showForm(int form)
{
    if( _currentEdit==_singleEdit && form>0 )
    {
	showMultiple();
	_multipleEdit->setCurrentPage(form);
	_currentEdit=static_cast<MsgMultiLineEdit*>(_multipleEdit->currentPage());
	emit currentFormChanged ( form );
    }
    else
    if( _currentEdit!=_singleEdit )
    {
	_multipleEdit->setCurrentPage(form);
	_currentEdit=static_cast<MsgMultiLineEdit*>(_multipleEdit->currentPage());
	emit currentFormChanged ( 0 );
    }
    _currentEdit->setFocus();
}

void HidingMsgEdit::setNumberOfPlurals( uint numberOfPlurals )
{
  _numberOfPlurals = numberOfPlurals;
  
  // find out the current shown version
  bool plurals = _currentEdit != _singleEdit;
  bool readonly = _currentEdit  ? _currentEdit->isReadOnly () : true;
  
  // cleanup old
  _currentEdit=0;
  _allEdits.clear();
  if( _singleEdit ) 
  {
    removeWidget( _singleEdit );
    delete _singleEdit;
  }
  
  if( _multipleEdit )
  {
    removeWidget( _multipleEdit );
    delete _multipleEdit;
  }
  
  // create new
  _singleEdit = new MsgMultiLineEdit( 0, _spell, this, "singleEdit" );
  _allEdits.append( _singleEdit );
  if( _eventFilter )
	_singleEdit->installEventFilter(_eventFilter);
  
  _multipleEdit = new QTabWidget( this );

  MsgMultiLineEdit* pl;
  for(uint i=0 ; i< _numberOfPlurals ; i++)
  {
     pl = new MsgMultiLineEdit( i, _spell, _multipleEdit, QString("multipleEdit %1").arg(i).local8Bit());
     _allEdits.append(pl);
     _multipleEdit->addTab( pl, i18n("Plural %1").arg(i+1));
     if( _eventFilter )
        pl->installEventFilter(_eventFilter);
  }
  
  addWidget(_singleEdit);
  addWidget(_multipleEdit);
  
  for( MsgMultiLineEdit* e = _allEdits.first() ; e ; e = _allEdits.next() )
  {
      connect( e, SIGNAL( signalUndoCmd( KBabel::EditCommand* )), 
        this, SIGNAL( signalUndoCmd( KBabel::EditCommand* )));
      connect( e, SIGNAL( textChanged() ) , this, SIGNAL( textChanged() ));
      connect( e, SIGNAL( textChanged() ) , this, SLOT( emitTextChanged() ));
      connect( e, SIGNAL( cursorPositionChanged( int, int )), 
        this, SLOT( emitCursorPositionChanged( int, int )) );
  }
  
  showPlurals( plurals );
  
  _currentEdit->setReadOnly (readonly);
}

void HidingMsgEdit::emitTextChanged()
{
    emit textChanged(_currentEdit->text());
}

uint HidingMsgEdit::currentForm()
{
    if( _currentEdit == _singleEdit ) return 0;
    else return _multipleEdit->currentPageIndex();
}

void HidingMsgEdit::newCurrentMultiple( QWidget* widget )
{
    _currentEdit = dynamic_cast<MsgMultiLineEdit*>(widget);
    emit currentFormChanged ( _multipleEdit->currentPageIndex () );
}

bool HidingMsgEdit::isModified() {
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	if( e->isModified() ) 
	{
	    return true;
	}
    }
    return false;
}

void HidingMsgEdit::setReadOnly(bool on) {
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setReadOnly( on );
    }
}

void HidingMsgEdit::processCommand(EditCommand* cmd, bool undo)
{
    if( _currentEdit == _singleEdit )
    {
	_singleEdit->processCommand(cmd,undo);
    }
    else
    {
	if( cmd->terminator() == 0)
	{
	    static_cast<MsgMultiLineEdit*>(
		_multipleEdit->page(static_cast<DelTextCmd*>(cmd)->pluralNumber)
	    )->processCommand(cmd,undo);
	}
    }
}

void HidingMsgEdit::setSpacePoints(bool on)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setSpacePoints( on );
    }
}

void HidingMsgEdit::setHighlightSyntax( bool on )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setHighlightSyntax( on );
    }
}

void HidingMsgEdit::setQuotes(bool on)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setQuotes( on );
    }
}

void HidingMsgEdit::setBgColor( const QColor& color)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setBgColor( color );
    }
}

void HidingMsgEdit::setHighlightColors(const QColor& quoteColor, const QColor& unquoteColor
        , const QColor& cformatColor, const QColor& accelColor, const QColor& tagColor)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setHighlightColors( quoteColor, unquoteColor, cformatColor,
	    accelColor, tagColor );
    }
}

void HidingMsgEdit::setOverwriteMode( bool b )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setOverwriteMode( b );
    }
}

void HidingMsgEdit::setModified( bool b )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setModified( b );
    }
}

void HidingMsgEdit::setCleverEditing( bool on )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setCleverEditing( on );
    }
}

void HidingMsgEdit::setHighlightBg( bool on )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setHighlightBg( on );
    }
}

void HidingMsgEdit::setContextMenu( QPopupMenu *menu )
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setContextMenu( menu );
    }
}

void HidingMsgEdit::setCurrentColor( const MsgMultiLineEdit::TextColor color)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setCurrentColor( color );
    }
}

bool HidingMsgEdit::hasFocus ()
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	if( e->hasFocus() ) return true;
    }
    
    return _multipleEdit->hasFocus() || QWidgetStack::hasFocus();
}

void HidingMsgEdit::setDiffColors(const QColor& addColor, const QColor& delColor)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setDiffColors( addColor, delColor );
    }
}

void HidingMsgEdit::setDiffMode(bool on)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setDiffMode( on );
    }
}

void HidingMsgEdit::setDiffDisplayMode(bool underlineAdded, bool strikeOutDeleted)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setDiffDisplayMode( underlineAdded, strikeOutDeleted );
    }
}
	 
void HidingMsgEdit::setFont(const QFont& font)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setFont( font );
    }
}
	 
void HidingMsgEdit::setSpellChecker(KSpell* spell)
{
    for( MsgMultiLineEdit* e = _allEdits.first(); e ; e = _allEdits.next() )
    {
	e->setSpellChecker( spell );
    }
    _spell = spell;
}

void HidingMsgEdit::emitCursorPositionChanged(int line, int col ) 
{
   int linestart = 0;
    int indexline = _currentEdit->lineOfChar( line, col );
    if ( indexline > 0 )
    {
        int min = 0, max = col;
        int i = (min + max)/2;
        int iline = _currentEdit->lineOfChar( line, i );
        while ( iline != indexline-1 ||
                _currentEdit->lineOfChar( line, i+1 ) != indexline )
        {
            Q_ASSERT( min != max && min != i && max != i );
            if ( iline < indexline )
                min = i;
            else
                max = i;
            i = (min + max)/2;
            iline = _currentEdit->lineOfChar( line, i );
        }
        linestart = i+1;
    }
    Q_ASSERT( linestart >= 0 );

    emit cursorPositionChanged(line + _currentEdit->lineOfChar( line, col ) ,col-linestart);
}
	 
#include "hidingmsgedit.moc"

