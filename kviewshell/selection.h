// -*- C++ -*-
//
// selection.h
//
// (C) 2001-20004 Stefan Kebekus
// Distributed under the GPL

#ifndef selection_h
#define selection_h

#include "pageNumber.h"

#include <qstring.h>


/* The --very simple-- class TextSelection represents text that the
   user has selected on a certain page of a document. The class stores

   the page number,

   two Q_INT32 numbers, 'selectedTextStart' and 'selectedTextEnd' that
   should hold indiced referring to the start and the end of the
   selected text in the appropriate documentPage's textLinkList

   a QString, 'selectedText' that holds the text that is selected

   these values can be stored, retrieved, it can be checked if the
   selection is empty, and signals are emitted when the values change
 */

class TextSelection
{

public:
  // Constructs an empty selection with an invalid page number, and an
  // empty text string. The values 'selectedTextStart' and
  // 'selectedTextEnd' are set to 0. None of the signals pageChanged()
  // or selectionIsNotEmpty() is emitted at construction time
  TextSelection();

  void          clear();

  // Use this method to set the data that is described above. Note
  // that the consistency of 'pageNr' 'selectedTextStart' and
  // 'selectedTextEnd' are not checked; it is entirely of the program
  // that calls this method to make sure that it sets reasonable
  // values; it is not even checked if selectedTextStart <=
  // selectedTextEnd! The signals pageChanged() and
  // selectionIsNotEmpty() are emitted if appropriate.
  void          set(const PageNumber& pageNr, Q_INT32 selectedTextStart, Q_INT32 selectedTextEnd, const QString& text);
  
  // This method can be used to find out if the selection is empty,
  // i.e. if the page number is invalid or the selected text is empty.
  bool          isEmpty() const {return (!page.isValid() || selectedText.isEmpty());}

  // Method used to retrieve the data described above
  Q_INT32       getSelectedTextStart() const {return selectedTextStart;}

  // Method used to retrieve the data described above
  Q_INT32       getSelectedTextEnd() const {return selectedTextEnd;}

  // Method used to retrieve the data described above
  const QString &getSelectedText() const {return selectedText;}

  // Method used to retrieve the data described above
  PageNumber    getPageNumber() const {return page;}

  // If the selection is not empty, this method copies the text to the
  // system clipboard. If the selection is empty, nothing is done.
  void          copyText() const;

  bool operator== (const TextSelection&) const;
  bool operator!= (const TextSelection&) const;

 private:
  // Described above
  PageNumber    page;

  // Described above
  Q_INT32       selectedTextStart, selectedTextEnd;

  // Described above
  QString       selectedText;
};

#endif
