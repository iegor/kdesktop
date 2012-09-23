//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#ifndef PMERRORDIALOG_H
#define PMERRORDIALOG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kdialogbase.h>
#include <qvaluelist.h>
#include <qptrdict.h>
#include <qptrlist.h>

class KConfig;
class QTextEdit;
class PMObject;

#include "pmmessage.h"

/**
 * Dialog that is shown if some errors or warnings occurred during
 * parsing or execution of commands.
 */
class PMErrorDialog : public KDialogBase
{
   Q_OBJECT
public:
   /**
    * Creates a modal PMErrorDialog with parent and name.
    *
    * messages is the message list. If the list contains a message of type
    * FatalError, the 'Proceed" button will not be shown.
    *
    * PMErrorDialog::exec( ) returns QDialog::Accepted if the command
    * should be continued.
    */
   PMErrorDialog( const PMMessageList& messages, int errorFlags,
                  QWidget* parent = 0, const char* name = 0 );

   static void saveConfig( KConfig* cfg );
   static void restoreConfig( KConfig* cfg );

protected:
   virtual void resizeEvent( QResizeEvent* ev );
   
private:
   void displayMessages( );
   
   static QSize s_size;
   QTextEdit* m_pTextView;
   QPtrDict< QPtrList<PMMessage> > m_messageDict;
   QPtrList<PMMessage> m_messages;
};

#endif
