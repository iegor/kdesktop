/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2004-2006                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

/*  This code generated by:
 *      Author : thomas
 *      Date   : Mon Jun 23 2003
 */

// own header
#include "javacodegenerationpolicy.h"
// qt/kde includes
#include <kconfig.h>
// app includes
#include "javacodegenerationpolicypage.h"
#include "javacodegenerator.h"
#include "../uml.h"

const bool JavaCodeGenerationPolicy::DEFAULT_AUTO_GEN_ATTRIB_ACCESSORS = true;
const bool JavaCodeGenerationPolicy::DEFAULT_AUTO_GEN_ASSOC_ACCESSORS = true;

// Constructors/Destructors
/*

JavaCodeGenerationPolicy::JavaCodeGenerationPolicy(CodeGenerationPolicy *defaults)
        : CodeGenerationPolicy(defaults)
{
    init();
    setDefaults(defaults,false);
}
 */

JavaCodeGenerationPolicy::JavaCodeGenerationPolicy(KConfig *config)
  //      : CodeGenerationPolicy(config)
{
    init();
    setDefaults(config,false);
}

JavaCodeGenerationPolicy::~JavaCodeGenerationPolicy ( ) { }

//
// Methods
//

// Accessor methods
//

// Public attribute accessor methods
//

/**
 * Set the value of m_autoGenerateAttribAccessors
 * @param new_var the new value
 */
void JavaCodeGenerationPolicy::setAutoGenerateAttribAccessors( bool var ) {
    m_autoGenerateAttribAccessors = var;
    m_commonPolicy->emitModifiedCodeContentSig();
}

/**
 * Set the value of m_autoGenerateAssocAccessors
 * @param new_var the new value
 */
void JavaCodeGenerationPolicy::setAutoGenerateAssocAccessors( bool var ) {
    m_autoGenerateAssocAccessors = var;
    m_commonPolicy->emitModifiedCodeContentSig();
}

/**
 * Get the value of m_autoGenerateAttribAccessors
 * @return the value of m_autoGenerateAttribAccessors
 */
bool JavaCodeGenerationPolicy::getAutoGenerateAttribAccessors( ){
    return m_autoGenerateAttribAccessors;
}

/**
 * Get the value of m_autoGenerateAssocAccessors
 * @return the value of m_autoGenerateAssocAccessors
 */
bool JavaCodeGenerationPolicy::getAutoGenerateAssocAccessors( ){
    return m_autoGenerateAssocAccessors;
}

// Other methods
//

void JavaCodeGenerationPolicy::writeConfig ( KConfig * config )
{

    // write ONLY the Java specific stuff
    config->setGroup("Java Code Generation");

    config->writeEntry("autoGenAccessors",getAutoGenerateAttribAccessors());
    config->writeEntry("autoGenAssocAccessors",getAutoGenerateAssocAccessors());

    CodeGenerator *codegen = UMLApp::app()->getGenerator();
    JavaCodeGenerator *javacodegen = dynamic_cast<JavaCodeGenerator*>(codegen);
    if (javacodegen)
        config->writeEntry("buildANTDocument", javacodegen->getCreateANTBuildFile());

}

void JavaCodeGenerationPolicy::setDefaults ( CodeGenPolicyExt * clone, bool emitUpdateSignal )
{

    JavaCodeGenerationPolicy * jclone;
    if (!clone)
        return;

    // NOW block signals for java param setting
    blockSignals(true); // we need to do this because otherwise most of these
    // settors below will each send the modifiedCodeContent() signal
    // needlessly (we can just make one call at the end).


    // now do java-specific stuff IF our clone is also a JavaCodeGenerationPolicy object
    if((jclone = dynamic_cast<JavaCodeGenerationPolicy*>(clone)))
    {
        setAutoGenerateAttribAccessors(jclone->getAutoGenerateAttribAccessors());
        setAutoGenerateAssocAccessors(jclone->getAutoGenerateAssocAccessors());
    }

    blockSignals(false); // "as you were citizen"

    if(emitUpdateSignal)
        m_commonPolicy->emitModifiedCodeContentSig();

}

void JavaCodeGenerationPolicy::setDefaults( KConfig * config, bool emitUpdateSignal )
{

    if(!config)
        return;

    // call method at the common policy to init default stuff
    m_commonPolicy->setDefaults(config, false);

    // NOW block signals (because call to super-class method will leave value at "true")
    blockSignals(true); // we need to do this because otherwise most of these
    // settors below will each send the modifiedCodeContent() signal
    // needlessly (we can just make one call at the end).

    // now do java specific stuff
    config -> setGroup("Java Code Generation");

    setAutoGenerateAttribAccessors(config->readBoolEntry("autoGenAccessors",DEFAULT_AUTO_GEN_ATTRIB_ACCESSORS));
    setAutoGenerateAssocAccessors(config->readBoolEntry("autoGenAssocAccessors",DEFAULT_AUTO_GEN_ASSOC_ACCESSORS));

    CodeGenerator *codegen = UMLApp::app()->getGenerator();
    JavaCodeGenerator *javacodegen = dynamic_cast<JavaCodeGenerator*>(codegen);
    if (javacodegen) {
        bool mkant = config->readBoolEntry("buildANTDocument", JavaCodeGenerator::DEFAULT_BUILD_ANT_DOC);
        javacodegen->setCreateANTBuildFile(mkant);
    }

    blockSignals(false); // "as you were citizen"

    if(emitUpdateSignal)
        m_commonPolicy->emitModifiedCodeContentSig();
}


/**
 * Create a new dialog interface for this object.
 * @return dialog object
 */
CodeGenerationPolicyPage * JavaCodeGenerationPolicy::createPage ( QWidget *parent, const char *name ) {
    return new JavaCodeGenerationPolicyPage ( parent, name, this );
}

void JavaCodeGenerationPolicy::init() {
    m_commonPolicy = UMLApp::app()->getCommonPolicy();
    m_autoGenerateAttribAccessors = DEFAULT_AUTO_GEN_ATTRIB_ACCESSORS;
    m_autoGenerateAssocAccessors = DEFAULT_AUTO_GEN_ASSOC_ACCESSORS;
}


#include "javacodegenerationpolicy.moc"
