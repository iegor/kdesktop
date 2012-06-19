/*
    cwsearchwidget.cpp  -  Search Pane

    copyright   : (c) 2001 by Martijn Klingens
    email       : klingens@kde.org

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include <qpushbutton.h>
#include <klocale.h>
#include <kdialog.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <kcombobox.h>
#include <qlabel.h>

#include "cwsearchwidget.h"

using namespace KBugBusterMainWindow;

CWSearchWidget::CWSearchWidget( QWidget *parent , const char * name )
: CWSearchWidget_Base( parent, name )
{
    // Set fonts and margins
    CWSearchWidget_BaseLayout->setSpacing( KDialog::spacingHint() );
    CWSearchWidget_BaseLayout->setMargin( KDialog::marginHint() );

    QFont f = m_searchLabel->font();
    f.setBold( true );
    m_searchLabel->setFont( f );

    connect( m_searchDesc, SIGNAL( textChanged ( const QString & ) ),
             this, SLOT( textDescriptionChanged ( const QString & ) ) );

    connect( m_searchBugNumber, SIGNAL( textChanged ( const QString & ) ),
             this, SLOT( textNumberChanged ( const QString & ) ) );
             
    m_searchDescBtn->setEnabled( !m_searchDesc->text().isEmpty() );
    m_searchBugNumberBtn->setEnabled( !m_searchBugNumber->text().isEmpty() );

//    m_searchPackages->setCompletionMode( KGlobalSettings::CompletionAuto );
}

CWSearchWidget::~CWSearchWidget()
{
}

void CWSearchWidget::textDescriptionChanged ( const QString &_text )
{
    m_searchDescBtn->setEnabled( !_text.isEmpty() );
}

void CWSearchWidget::textNumberChanged ( const QString &_text )
{
    m_searchBugNumberBtn->setEnabled( !_text.isEmpty() );
}

#include "cwsearchwidget.moc"

/* vim: set et ts=4 sw=4 softtabstop=4: */

