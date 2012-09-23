/***************************************************************************
                          kgroupeduca.cpp  -  description
                             -------------------
    begin                : Thu Sep 7 2000
    copyright            : (C) 2000 by Javier Campos Morales
    email                : javi@asyris.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgroupeduca.h"
#include "kgroupeduca.moc"
#include "kcheckeduca.h"
#include "kradioeduca.h"

#include <klocale.h>

KGroupEduca::KGroupEduca(QWidget *parent, const char *name ) : QVButtonGroup(i18n("Answers"), parent, name)
{
    _sv = new QScrollView(this);
    _sv->setVScrollBarMode(QScrollView::Auto);
    _sv->setHScrollBarMode(QScrollView::Auto);
    _sv->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    _vbox2 = new QVBox( _sv->viewport() );
    _vbox2->setSpacing( 6 );
    _vbox2->setMargin( 11 );
    _sv->viewport()->setBackgroundMode( _vbox2->backgroundMode() );
    _sv->setStaticBackground(true);
    _sv->addChild( _vbox2 );
    _typeMode = Radio;
}

KGroupEduca::~KGroupEduca(){
}

/** Insert a check or radio button */
void KGroupEduca::insertAnswer( const QString& text)
{
    QButton *answer = 0;

    switch( _typeMode )
    {
    case Radio:
        answer = new KRadioEduca( _vbox2 );
        break;
    case Check:
        answer = new KCheckEduca( _vbox2 );
        break;
    }
    answer->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0 ) );
    answer->setText( text );
    answer->show();
    insert(answer);
}

/** Set type */
void KGroupEduca::setType(ButtonType type)
{
    _typeMode = type;
}

/** Clear all kradio or kcheck answers */
void KGroupEduca::clearAnswers()
{
    unsigned int maxButton = count();
    for( unsigned int i=0 ; i<maxButton ; ++i )
    {
        QButton *tmpButton;
        if( (tmpButton = find(i)) )
            remove(tmpButton);
        delete tmpButton;
    }

}

/** Return if is checked radio or check buttons */
bool KGroupEduca::isChecked(int id)
{
    switch( _typeMode )
    {
    case Radio:
    {
        KRadioEduca *tmpRadioButton = (KRadioEduca*) find(id);
        if(tmpRadioButton != 0) return tmpRadioButton->isChecked();
    }
    break;
    case Check:
    {
        KCheckEduca *tmpCheckButton = (KCheckEduca*) find(id);
        if(tmpCheckButton != 0) return tmpCheckButton->isChecked();
    }
    break;
    }
    return 0;
}
