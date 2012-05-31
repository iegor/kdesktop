//
// Class: RenderedDocumentPage
//
// Widget for displaying TeX DVI files.
// Part of KDVI- A previewer for TeX DVI files.
//
// (C) 2004 Stefan Kebekus. Distributed under the GPL.

#include <config.h>

#include <kdebug.h>

#include "renderedDocumentPage.h"
#include "hyperlink.h"
#include "selection.h"
#include "textBox.h"


RenderedDocumentPage::RenderedDocumentPage()
{
  textBoxList.reserve(250);
  pageNr = 0;
  isEmpty = true;
  pageText = QString::null;
}


RenderedDocumentPage::~RenderedDocumentPage()
{
  ;
}


void RenderedDocumentPage::setPageNumber(const PageNumber& pnr)
{
  pageNr = pnr;
  clear();
}


void RenderedDocumentPage::clear()
{
#ifdef DEBUG_DOCUMENTPAGE
  kdDebug(1223) << "RenderedDocumentPage::clear() called for page #" << pageNumber << endl;
#endif

  textBoxList.clear();
  hyperLinkList.clear();
  pageText = QString::null;

  isEmpty = true;
}


QRegion RenderedDocumentPage::selectedRegion(const TextSelection& selection)
{
  if (selection.isEmpty() || selection.getPageNumber() != pageNr)
    return QRegion();

  int startIndex = selection.getSelectedTextStart();
  int endIndex = selection.getSelectedTextEnd();

  QValueVector<QRect> wordBox;

  QRect currentWordBox;
  //unsigned int currentBaseline = 0;

  // Merge character boxes into boxes containing complete words.
  // Note: A word in this context is defined as a string of boxes
  // with the same baseline.
  for (int i = startIndex; i <= endIndex; i++)
  {
    if (i == 0)
    {
      // start first word
      currentWordBox = textBoxList[i].box;
      //currentBaseline = textBoxList[i].baseline;
      continue;
    }

    /*if (currentBaseline == textBoxList[i].baseline)
    {
      currentWordBox = currentWordBox.unite(textBoxList[i].box);
    }
    else*/
    {
      // start next word
      wordBox.push_back(currentWordBox);
      currentWordBox = textBoxList[i].box;
      //currentBaseline = textBoxList[i].baseline;
    }
  }
  // we still need to store the last word
  wordBox.push_back(currentWordBox);

  QValueVector<QRect> lineBox;

  // Merge word boxes into boxes containing whole lines.
  // We start a new line if we encounter a wordbox which does not
  // vertically overlap which the current lineBox.
  QRect currentLineBox;

  for (unsigned int i = 0; i < wordBox.size(); i++)
  {
    if (!currentLineBox.isValid())
    {
      // start first line
      currentLineBox = wordBox[i];
      continue;
    }

    // check if there is vertical overlap
    if (wordBox[i].top() <= currentLineBox.bottom() && wordBox[i].bottom() >= currentLineBox.top())
    {
      // the word belongs to the current line
      currentLineBox = currentLineBox.unite(wordBox[i]);
    }
    else
    {
      // start next line
      //kdDebug() << "push line (" << currentLineBox.top() << ", " << currentLineBox.bottom() << ")" << endl;
      lineBox.push_back(currentLineBox);
      currentLineBox = wordBox[i];
    }
  }
  // we still need to store the last line
  //kdDebug() << "push line (" << currentLineBox.top() << ", " << currentLineBox.bottom() << ")" << endl;
  lineBox.push_back(currentLineBox);

  //kdDebug() << "Number of lineboxes = " << lineBox.size() << endl;

  // Now we increase the height of the lines if necessary to obtain a connected region
  // for our selection.
  for (unsigned int i = 0; i < lineBox.size() - 1; i++)
  {
      if (lineBox[i+1].top() >= lineBox[i].bottom())
      {
        int midPoint = (lineBox[i].bottom() + lineBox[i+1].top()) / 2;

        lineBox[i].setBottom(midPoint);
        lineBox[i+1].setTop(midPoint+1);
      }
  }

  // Add the lineboxes to a Region
  QRegion selectedRegion;
  for (unsigned int i = 0; i < lineBox.size(); i++)
  {
    selectedRegion += QRegion(lineBox[i]);
  }

  return selectedRegion;
}


TextSelection RenderedDocumentPage::select(const QRect& selectedRectangle)
{
  int selectedTextStart = -1;
  int selectedTextEnd   = -1;

  // Find the smallest and biggest index for which the corresponding
  // textBoxList entry intersects the selected rectangle.
  for (unsigned int i=0; i<textBoxList.size(); i++)
  {
    if (selectedRectangle.intersects(textBoxList[i].box))
    {
      if (selectedTextStart == -1)
        selectedTextStart = i;
      selectedTextEnd = i;
    }
  }

  TextSelection selection;

  QString selectedText;

  if (selectedTextStart != -1)
  {
    for (int i = selectedTextStart; (i <= selectedTextEnd) && (i < (int)textBoxList.size()); i++)
    {
      selectedText += textBoxList[i].text;
    }
    selection.set(pageNr, selectedTextStart, selectedTextEnd, selectedText);
    return selection;
  }
  // return empty selection
  return selection;
}

TextSelection RenderedDocumentPage::select(const QPoint& point)
{
  int selectedTextStart = -1;
  int selectedTextEnd   = -1;

  for (unsigned int i=0; i<textBoxList.size(); i++)
  {
    if (textBoxList[i].box.contains(point))
    {
      selectedTextStart = i;
      selectedTextEnd = i;
      break;
    }
  }

  TextSelection selection;

  QString selectedText;

  if (selectedTextStart != -1)
  {
    selectedText = textBoxList[selectedTextStart].text;
    selection.set(pageNr, selectedTextStart, selectedTextEnd, selectedText);
    return selection;
  }
  // return empty selection
  return selection;
}

TextSelection RenderedDocumentPage::find(const QString& str, int index, bool caseSensitive)
{
  if (pageText.isNull())
  {
    // Create the pageText by joining all entries of textBoxList.
    for (QValueVector<TextBox>::Iterator i = textBoxList.begin(); i != textBoxList.end(); i++)
    {
      pageText = pageText + i->text;
    }
  }

  // Create empty selection;
  TextSelection selection;

  // If the page contains no searchable text
  if (pageText.isNull())
    return selection;

  // Compute the corresponding pageText index
  unsigned int subIndex = 0;
  for (int i = 0; i < index; i++)
  {
    subIndex += textBoxList[i].text.length();
  }

  int textIndex = pageText.find(str, subIndex, caseSensitive);

  if (textIndex == -1)
    return selection;

  // Because a single Hyperlink structure can possible store more then
  // one character we now have to search for the Indices in the
  // textBoxList Vector which corresponds to the found index in the
  // String pageText.  FIXME: It would be faster to search directly in
  // the textBoxList.
  int counter = 0;
  int startIndex = 0;
  while (counter < textIndex)
  {
    counter += textBoxList[startIndex].text.length();
    // If the string we searched for starts in the middle of some text element we better return a
    // selection that it somewhat bigger.
    if (counter > textIndex)
      break;

    startIndex++;

    // safety check
    if (startIndex >= (int)textBoxList.size())
      return selection;
  }

  // Search for the end index.
  // TODO: This algorithm is not entirely correct if str does not start exactly at the beginning of some text element.
  counter = 0;
  int endIndex = startIndex;
  while (counter < (int)str.length())
  {
    counter += textBoxList[endIndex].text.length();
    if (counter >= (int)str.length())
      break;

    endIndex++;

    // safety check
    if (endIndex >= (int)textBoxList.size())
      return selection;
  }

  // Set the selection
  selection.set(pageNr, startIndex, endIndex, str);
  return selection;
}


TextSelection RenderedDocumentPage::findRev(const QString& str, int index, bool caseSensitive)
{
  // Negative index means we start the search at the end of the text.
  if (index < 0)
  {
    index = textBoxList.size();
  }

  if (pageText.isNull())
  {
    // Create the pageText by joining all entries of textBoxList.
    for (QValueVector<TextBox>::Iterator i = textBoxList.begin(); i != textBoxList.end(); i++)
    {
      pageText = pageText + i->text;
    }
  }

  // Create empty selection;
  TextSelection selection;

  // If the page contains no searchable text
  if (pageText.isNull())
    return selection;

  // Compute the corresponding pageText index
  unsigned int subIndex = 0;
  for (int i = 0; i < index; i++)
  {
    subIndex += textBoxList[i].text.length();
  }

  int textIndex = pageText.findRev(str, subIndex, caseSensitive);

  if (textIndex == -1)
    return selection;

  // Because a single Hyperlink structure can possible store more then
  // one character we now have to search for the Indices in the
  // textBoxList Vector which corresponds to the found index in the
  // String pageText.  FIXME: It would be faster to search directly in
  // the textBoxList.
  int counter = 0;
  int startIndex = 0;
  while (counter < textIndex)
  {
    counter += textBoxList[startIndex].text.length();
    // If the string we searched for starts in the middle of some text element we better return a
    // selection that it somewhat bigger.
    if (counter > textIndex)
      break;

    startIndex++;

    // safety check
    if (startIndex >= (int)textBoxList.size())
      return selection;
  }

  // Search for the end index.
  // TODO: This algorithm is not entirely correct if str does not start exactly at the beginning of some text element.
  counter = 0;
  int endIndex = startIndex;
  while (counter < (int)str.length())
  {
    counter += textBoxList[endIndex].text.length();
    if (counter >= (int)str.length())
      break;

    endIndex++;

    // safety check
    if (endIndex >= (int)textBoxList.size())
      return selection;
  }

  // Set the selection
  selection.set(pageNr, startIndex, endIndex, str);
  return selection;
}

#include "renderedDocumentPage.moc"

