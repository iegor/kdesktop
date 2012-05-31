//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001 by Andreas Zehender
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

#ifndef PMPOVRAYOUTPUTWIDGET_H
#define PMPOVRAYOUTPUTWIDGET_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <kdialog.h>
class QTextEdit;
class KConfig;

/**
 * Widget to display the povray text output
 */
class PMPovrayOutputWidget : public KDialog
{
   Q_OBJECT
public:
   /**
    * Standard constructor
    */
   PMPovrayOutputWidget( QWidget* parent = 0, const char* name = 0 );
   /**
    * Destructor
    */
   ~PMPovrayOutputWidget( );

   static void saveConfig( KConfig* cfg );
   static void restoreConfig( KConfig* cfg );

public slots:
   /**
    * Clears the text
    */
   void slotClear( );
   /**
    * Adds the text to the output
    */
   void slotText( const QString& output );

protected slots:
   /**
    * Called when the close button is clicked
    */
   void slotClose( );

protected:
   virtual void resizeEvent( QResizeEvent* ev );

private:
   QTextEdit* m_pTextView;
   int m_startOfLastLine;
   QString m_output;
   static QSize s_size;
};


#endif
