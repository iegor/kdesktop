/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2004-2007                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Wed Jun 18 2003
 */


// own header
#include "textblock.h"

// qt/kde includes
#include <qregexp.h>

// local includes
#include "codedocument.h"
#include "codegenerator.h"
#include "codegenerationpolicy.h"
#include "uml.h"

// Constructors/Destructors
//

TextBlock::TextBlock ( CodeDocument * parent, const QString & text )
        : QObject ( (QObject *)parent, "textBlock")
{
    initFields(parent);
    setText(text);
}

TextBlock::~TextBlock ( ) { }

//
// Methods
//


// Accessor methods
//


/**
 * Set the value of the  parent code document
 * @param new_var the new value of m_parentDocument
 */
void TextBlock::setParentDocument ( CodeDocument * new_var ) {
    m_parentDocument = new_var;
}

bool TextBlock::canDelete ( ) {
    return m_canDelete;
}

/**
 * Get the value of m_parentDocument
 * @return the value of m_parentDocument
 */
CodeDocument * TextBlock::getParentDocument ( ) {
    return m_parentDocument;
}

/**
 * Set the value of m_text
 * The actual text of this code block.
 * @param new_var the new value of m_text
 */
void TextBlock::setText ( const QString &new_var ) {
    m_text = new_var;
}

/**
 * Add text to this object.
 *
 */
void TextBlock::appendText ( const QString &new_text ) {
    m_text = m_text + new_text;
}

/**
 * Get the value of m_text
 * The actual text of this code block.
 * @return the value of m_text
 */
QString TextBlock::getText ( ) const {
    return m_text;
}

/**
 * Get the tag of this text block. This tag
 * may be used to find this text block in the code document
 * to which it belongs.
 */
QString TextBlock::getTag( ) const {
    return m_tag;
}

/**
 * Set the tag of this text block. This tag
 * may be used to find this text block in the code document
 * to which it belongs.
 */
void TextBlock::setTag ( const QString &value ) {
    m_tag = value;
}

/**
 * Set the value of m_writeOutText
 * Whether or not to include the text of this TextBlock into a file.
 * @param new_var the new value of m_writeOutText
 */
void TextBlock::setWriteOutText ( bool new_var ) {
    m_writeOutText = new_var;
}

/**
 * Get the value of m_writeOutText
 * Whether or not to include the text of this TextBlock into a file.
 * @return the value of m_writeOutText
 */
bool TextBlock::getWriteOutText ( ) {
    return m_writeOutText;
}

/** Set how many times to indent this text block.
 */
void TextBlock::setIndentationLevel ( int level ) {
    m_indentationLevel = level;
}

/** Get how many times to indent this text block.
 * The amount of each indenatation is determined from the parent
 * codedocument codegeneration policy.
 */
int TextBlock::getIndentationLevel ( ) {
    return m_indentationLevel;
}

QString TextBlock::getNewLineEndingChars ( ) {
    CodeGenerationPolicy * policy = UMLApp::app()->getCommonPolicy();
    return policy->getNewLineEndingChars();
}

QString TextBlock::getIndentation() {
    CodeGenerationPolicy * policy = UMLApp::app()->getCommonPolicy();
    return policy->getIndentation();
}

QString TextBlock::getIndentationString ( int level ) {
    if (!level)
        level = m_indentationLevel;
    QString indentAmount = getIndentation();
    QString indentation = "";
    for(int i=0; i<level; i++)
        indentation.append(indentAmount);
    return indentation;
}

// Other methods
//

/** Ush. These are terrifically bad and must one day go away.
 * Both methods indicate the range of lines in this textblock
 * which may be edited by the codeeditor (assuming that any are
 * actually editable). The default case is no lines are editable.
 * The line numbering starts with '0' and a '-1' means no line
 * qualifies.
 */
int TextBlock::firstEditableLine() { return 0; }
int TextBlock::lastEditableLine() { return 0; }

QString TextBlock::getNewEditorLine ( int amount ) {
    return getIndentationString(amount);
}

// will remove indenation from this text block.
QString TextBlock::unformatText ( const QString & text, const QString & indent )
{
    QString output = text;
    QString myIndent = indent;
    if(myIndent.isEmpty())
        myIndent = getIndentationString();

    if(!output.isEmpty())
        output.remove(QRegExp('^'+myIndent));

    return output;
}

void TextBlock::release () {
    this->disconnect();
    //this->deleteLater();
}

QString TextBlock::formatMultiLineText ( const QString &work, const QString &linePrefix,
        const QString& breakStr, bool addBreak, bool lastLineHasBreak ) {
    QString output = "";
    QString text = work;
    QString endLine = getNewLineEndingChars();
    int matches = text.contains(QRegExp(breakStr));
    if(matches)
    {
        // check that last part of string matches, if not, then
        // we have to tack on extra match
        if(!text.contains(QRegExp(breakStr+"\\$")))
            matches++;

        for(int i=0; i < matches; i++)
        {
            QString line = text.section(QRegExp(breakStr),i,i);
            output += linePrefix + line;
            if((i != matches-1) || lastLineHasBreak)
                output += endLine; // add break to line
        }
    } else {
        output = linePrefix + text;
        if(addBreak)
            output += breakStr;
    }

    return output;
}

void TextBlock::setAttributesOnNode ( QDomDocument & doc, QDomElement & blockElement)
{

    QString endLine = UMLApp::app()->getCommonPolicy()->getNewLineEndingChars();

    if (&doc != 0 ) {

        blockElement.setAttribute("tag",getTag());

        // only write these if different from defaults
        if(getIndentationLevel())
            blockElement.setAttribute("indentLevel",QString::number(getIndentationLevel()));
        if(!m_text.isEmpty())
            blockElement.setAttribute("text",encodeText(m_text,endLine));
        if(!getWriteOutText())
            blockElement.setAttribute("writeOutText",getWriteOutText()?"true":"false");
        if(!canDelete())
            blockElement.setAttribute("canDelete",canDelete()?"true":"false");

    }

}

void TextBlock::setAttributesFromObject(TextBlock * obj)
{

    // DONT set tag here.
    setIndentationLevel(obj->getIndentationLevel());
    setText(obj->getText());
    setWriteOutText(obj->getWriteOutText());
    m_canDelete = obj->canDelete();

}

void TextBlock::setAttributesFromNode (QDomElement & root ) {

    QString endLine = UMLApp::app()->getCommonPolicy()->getNewLineEndingChars();

    setIndentationLevel(root.attribute("indentLevel","0").toInt());
    setTag(root.attribute("tag",""));
    setText(decodeText(root.attribute("text",""),endLine));
    setWriteOutText(root.attribute("writeOutText","true") == "true" ? true : false);
    m_canDelete = root.attribute("canDelete","true") == "true" ? true : false;

}

// encode text for XML storage
// we simply convert all types of newLines to the "\n" or &#010;
// entity.
QString TextBlock::encodeText(const QString& text, const QString &endLine) {
    QString encoded = text;
    encoded.replace(QRegExp(endLine),"&#010;");
    return encoded;
}

// encode text for XML storage
// we simply convert all types of newLines to the "\n" or &#010;
// entity.
QString TextBlock::decodeText(const QString& text, const QString &endLine) {
    QString decoded = text;
    decoded.replace(QRegExp("&#010;"),endLine);
    return decoded;
}

/**
 * @return      QString
 */
QString TextBlock::toString ( )
{

    // simple output method
    if(m_writeOutText && !m_text.isEmpty())
    {
        QString endLine = UMLApp::app()->getCommonPolicy()->getNewLineEndingChars();
        return formatMultiLineText(m_text, getIndentationString(), endLine);
    } else
        return "";
}

void TextBlock::initFields ( CodeDocument * parent ) {
    m_canDelete = true;
    m_writeOutText = true;
    m_parentDocument = parent;
    m_text = "";
    m_tag = "";
    m_indentationLevel = 0;
}

#include "textblock.moc"
