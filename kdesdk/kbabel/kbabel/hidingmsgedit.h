/***************************************************************************
                          hidingmsgedit.h  -  description
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

 *                                                                         *
 ***************************************************************************/

#ifndef HIDINGMSGEDIT_H
#define HIDINGMSGEDIT_H

#include <qwidgetstack.h>
#include <qguardedptr.h>
#include <qptrlist.h>

#include "mymultilineedit.h"

class KSpell;
class QTabWidget;
class QPopupMenu;

namespace KBabel
{
    class EditCommand;
}

/**
  *@author Stanislav Visnovsky <visnovsky@kde.org>
  */

class HidingMsgEdit : public QWidgetStack  {
   Q_OBJECT
public: 
  HidingMsgEdit(uint numberOfPlurals, QWidget* eventFilter=0, KSpell* spell=0, QWidget *parent=0, const char *name=0);
  ~HidingMsgEdit();
  
  void setNumberOfPlurals(uint numberOfPlurals);
  
  // return index number for a currently shown plural form (0=singular or no plurals)
  uint currentForm();

  // MsgMultiLineEdit interface
  bool isOverwriteMode() const { return _currentEdit->isOverwriteMode(); }
  bool isModified();
  bool hasSelectedText() const { return _currentEdit->hasSelectedText(); }
  QString selectedText () const { return _currentEdit->selectedText(); }
  void processCommand(KBabel::EditCommand* cmd, bool undo=false);
  void offset2Pos(int offset, int &row, int &col) const
	{ return _currentEdit->offset2Pos(offset, row, col ); }
  void getCursorPosition(int *para, int *index) const
	{ _currentEdit->getCursorPosition(para,index); }
  int currentIndex() const
	{ return _currentEdit->currentIndex(); }
  int beginOfLastMarkedText() const
    { return _currentEdit->beginOfLastMarkedText(); }
  virtual void setFont ( const QFont & );
  void setCurrentColor(const MsgMultiLineEdit::TextColor color);
  bool spacePoints() const { return _currentEdit->spacePoints(); }
  void setSpacePoints(bool on);
  bool quotes() const { return _currentEdit->quotes(); }
  void setQuotes(bool on);
  void setBgColor( const QColor& color);
  bool highlightBg() const { return _currentEdit->highlightBg(); }
  bool highlightSyntax() const { return _currentEdit->highlightSyntax(); }
  void setHighlightColors(const QColor& quoteColor, const QColor& unquoteColor
        , const QColor& cformatColor, const QColor& accelColor, const QColor& tagColor); 
  int beginOfMarkedText() { return _currentEdit->beginOfMarkedText(); }
  virtual void insertAt ( const QString & s, int line, int col, bool mark = false )
        { _currentEdit->insertAt( s, line, col, mark ); }

  void setDiffMode(bool on);
  void setDiffDisplayMode(bool underlineAdded, bool strikeOutDeleted);
  void setDiffColors(const QColor& addColor, const QColor& delColor);     
  QString text(int para) { return _currentEdit->text(para); }
 
  void setSpellChecker(KSpell* spell);
 
  void selectTag(int start, int length) { _currentEdit->selectTag(start,length); }

  // reiplemented to return correct value
  bool hasFocus ();
public slots: // Public slots
  void setText(QStringList texts, QString msgctxt = QString::null);
  void showSingle();
  void showMultiple();
  void showPlurals( bool on );
  void showForm(int form);
  virtual void setFocus() { _currentEdit->setFocus(); }
  void forceUpdate() { _currentEdit->forceUpdate(); }
  
  // MsgMultiLineEdit interface
  virtual void setReadOnly( bool b );
  virtual void setOverwriteMode( bool b );
  virtual void setModified( bool b );
  void setCleverEditing( bool on );
  void setHighlightBg( bool on );
  void setHighlightSyntax( bool on );
  virtual void clear() { _currentEdit->clear(); }
  virtual void cut() { _currentEdit->cut(); }
  virtual void copy() { _currentEdit->copy(); }
  virtual void paste() { _currentEdit->paste(); }
  virtual void setSelection( int paraFrom, int indexFrom, int paraTo, int indexTo, int selNum = 0 )
	{ _currentEdit->setSelection( paraFrom, indexFrom, paraTo, indexTo, selNum) ; }
  virtual void selectAll(bool select=true) { _currentEdit->selectAll(select); }
  virtual void setCursorPosition ( int para, int index )
	{ _currentEdit->setCursorPosition(para,index); }
  virtual void setContextMenu( QPopupMenu *menu );

signals:
  void signalUndoCmd(KBabel::EditCommand*);
  void textChanged();
  void textChanged(const QString&);
  void cursorPositionChanged ( int para, int pos );
  void currentFormChanged ( uint form );

private slots:
  
  void emitTextChanged();
  
  // invoked if TabWidget changes the shown widget
  void newCurrentMultiple( QWidget * );  
  // invoked by inner cursorPositionChanged() to transform line/col for wrapping
  void emitCursorPositionChanged( int para, int pos );

private: // Private attributes
  /** Used for editting non-plural messages */
  MsgMultiLineEdit* _singleEdit;
  /** Used for editting plural forms */
  QTabWidget* _multipleEdit;
  QWidget* _eventFilter;
  
  MsgMultiLineEdit* _currentEdit;
  QPtrList<MsgMultiLineEdit> _allEdits;
  
  uint _numberOfPlurals;
  
  QGuardedPtr<KSpell> _spell;
};

#endif
