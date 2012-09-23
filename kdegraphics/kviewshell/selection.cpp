//
// selection.cpp
//
// Part of KDVI - A previewer for TeX DVI files.
//
// (C) 2001--2004 Stefan Kebekus
// Distributed under the GPL

#include <config.h>

#include <qapplication.h>
#include <qclipboard.h>

#include "selection.h"

TextSelection::TextSelection()
  : page(PageNumber::invalidPage),
    selectedText(QString::null)
{}


void TextSelection::set(const PageNumber& pageNr, Q_INT32 start, Q_INT32 end, const QString& text)
{
  page              = pageNr;
  selectedTextStart = start;
  selectedTextEnd   = end;
  if (page != 0)
    selectedText = text;
  else
    selectedText = QString::null;

  if (page != 0) {
    QApplication::clipboard()->setSelectionMode(true);
    QApplication::clipboard()->setText(selectedText);
  }
}


bool TextSelection::operator== (const TextSelection& s) const
{
  // We don't check if the strings are equal because for corretly constructed selection objects this is always
  // the case if the following condition holds.
  return (page == s.page && selectedTextStart == s.selectedTextStart && selectedTextEnd == s.selectedTextEnd);
}


bool TextSelection::operator!= (const TextSelection& s) const
{
  return !(*this == s);
}


void TextSelection::copyText() const
{
  if (!isEmpty()) {
    QApplication::clipboard()->setSelectionMode(false);
    QApplication::clipboard()->setText(selectedText);
  }
}


void TextSelection::clear()
{
  set(0, -1, -1, QString::null);
}

