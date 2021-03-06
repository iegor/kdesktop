
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Wed Jun 18 2003
 */



#ifndef CODEBLOCKWITHCOMMENTS_H
#define CODEBLOCKWITHCOMMENTS_H


#include "codeblock.h"
#include "codecomment.h"

class HierarchicalCodeBlock;

/**
  * class CodeBlockWithComments
  * A very common type of text block in any type of code.
  */

class CodeBlockWithComments : public CodeBlock
{
    Q_OBJECT
public:

    // Constructors/Destructors
    //


    /**
     * Basic Constructor
     */
    explicit CodeBlockWithComments ( CodeDocument * doc , const QString & body = "", const QString & comment = "");
    explicit CodeBlockWithComments ( HierarchicalCodeBlock * hblock, const QString & body = "", const QString & comment = "" );

    /**
     * Empty Destructor
     */
    virtual ~CodeBlockWithComments ( );


    /**
     * Set the Comment object
     */
    void setComment ( CodeComment * object );

    /**
     * Get the Comment object
     */
    CodeComment * getComment ( );

    /**
     * @return  QString
     */
    virtual QString toString ( );

    /** A utility method that causes the comment and body of the code block
     * to have the same indentation level.
     */
    void setOverallIndentationLevel ( int level );

    /**
     * Save the XMI representation of this object
     */
    virtual void saveToXMI ( QDomDocument & doc, QDomElement & root );

    /**
     * load params from the appropriate XMI element node.
     */
    virtual void loadFromXMI ( QDomElement & root );

    /** set the class attributes from a passed object
     */
    virtual void setAttributesFromObject (TextBlock * obj);

protected:

    /** set attributes of the node that represents this class
     * in the XMI document.
     */
    virtual void setAttributesOnNode (QDomDocument & doc, QDomElement & blockElement);

    /** set the class attributes of this object from
    * the passed element node.
    */
    virtual void setAttributesFromNode ( QDomElement & element);

private:

    CodeComment * m_comment;

    void initFields(CodeDocument *parent, const QString& comment);

};

#endif // CODEBLOCKWITHCOMMENTS_H
