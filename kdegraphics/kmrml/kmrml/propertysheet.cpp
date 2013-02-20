/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "propertysheet.h"

#include "mrml_elements.h"
#include "mrml_shared.h"

#include <knuminput.h>
#include <qwidget.h>

using namespace KMrml;

template class QValueList<QDomElement>;

PropertySheet::PropertySheet()
{
    init();
}

PropertySheet::PropertySheet( const QDomElement& elem )
{
    init();

    initFromDOM( elem );
}

PropertySheet::PropertySheet( const PropertySheet& ps )
{
    *this = ps;
}

PropertySheet& PropertySheet::operator= ( const PropertySheet& ps )
{
    if ( &ps == this )
        return *this;

    m_visibility = ps.m_visibility;
    m_type       = ps.m_type;
    m_caption    = ps.m_caption;
    m_id         = ps.m_id;

    m_sendType = ps.m_sendType;
    m_sendName = ps.m_sendName;
    m_sendValue = ps.m_sendValue;

    m_minRange = ps.m_minRange;
    m_maxRange = ps.m_maxRange;
    m_stepSize = ps.m_stepSize;

    m_minSubsetSize = ps.m_minSubsetSize;
    m_maxSubsetSize = ps.m_maxSubsetSize;

    // deep copy of m_subSheets
    QPtrListIterator<PropertySheet> it( ps.m_subSheets );
    for ( ; it.current(); ++it )
        m_subSheets.append( new PropertySheet( *it.current() ) );

    return *this;
}

void PropertySheet::init()
{
    m_subSheets.setAutoDelete( true );
    m_visibility = Visible;
}

void PropertySheet::initFromDOM( const QDomElement& elem )
{
    m_subSheets.clear();

    m_visibility = getVisibility( elem.attribute( MrmlShared::visibility() ));
    m_type = getType( elem.attribute( MrmlShared::propertySheetType() ) );
    m_caption = elem.attribute( MrmlShared::caption() );
    m_id = elem.attribute( MrmlShared::propertySheetId() );
    m_sendType = getSendType( elem.attribute( MrmlShared::sendType() ));
    m_sendName = elem.attribute( MrmlShared::sendName() );
    m_sendValue = elem.attribute( MrmlShared::sendValue() );
    m_minRange = toInt( elem.attribute( MrmlShared::from() ));
    m_maxRange = toInt( elem.attribute( MrmlShared::to() ));
    m_stepSize = toInt( elem.attribute( MrmlShared::step() ));

    m_minSubsetSize = toInt( elem.attribute( MrmlShared::minSubsetSize() ));
    m_maxSubsetSize = toInt( elem.attribute( MrmlShared::maxSubsetSize() ));

    QValueList<QDomElement> children =
        KMrml::directChildElements( elem, MrmlShared::propertySheet() );
    QValueListConstIterator<QDomElement> it = children.begin();
    for ( ; it != children.end(); ++it )
        m_subSheets.append( new PropertySheet( *it ) );
}

QWidget * PropertySheet::createWidget( QWidget */*parent*/, const char */*name*/ )
{
    QWidget *w = 0L;

    switch ( m_type )
    {
        case Numeric:
        {
//            KIntNumInput *input = new KIntNumInput();
            break;
        }

        case Subset:
        {
            if ( m_minSubsetSize == 1 && m_maxSubsetSize == 1 )
            {

            }

            break;
        }

        default:
            qDebug("** can't create widget for type: %i", m_type);
    }

    return w;
}


//
// static methods
//
PropertySheet::Visibility PropertySheet::getVisibility( const QString& value )
{
    Visibility vis;

    if ( value == MrmlShared::invisible() )
        vis = Invisible;
    else if ( value == MrmlShared::popup() )
        vis = Popup;
    else
        vis = Visible; // default value

    return vis;
}

PropertySheet::Type PropertySheet::getType( const QString& value )
{
    Type type = (Type) 0;

    if ( value == MrmlShared::multiSet() )
        type = MultiSet;
    else if ( value == MrmlShared::subset() )
        type = Subset;
    else if ( value == MrmlShared::setElement() )
        type = SetElement;
    else if ( value == MrmlShared::boolean() )
        type = Boolean;
    else if ( value == MrmlShared::numeric() )
        type = Numeric;
    else if ( value == MrmlShared::textual() )
        type = Textual;
    else if ( value == MrmlShared::panel() )
        type = Panel;
    else if ( value == MrmlShared::clone() )
        type = Clone;
    else if ( value == MrmlShared::reference() )
        type = Reference;

    return type;
}

PropertySheet::SendType PropertySheet::getSendType( const QString& value )
{
    SendType type = (SendType) 0;

    if ( value == MrmlShared::element() )
        type = Element;
    else if ( value == MrmlShared::attribute() )
        type = Attribute;
    else if ( value == MrmlShared::attributeName() )
        type = AttributeName;
    else if ( value == MrmlShared::attributeValue() )
        type = AttributeValue;
    else if ( value == MrmlShared::children() )
        type = Children;
    else if ( value == MrmlShared::none() )
        type = None;

    return type;
}

int PropertySheet::toInt( const QString& value, int defaultValue )
{
    bool ok = false;
    int res = value.toInt( &ok );
    return ok ? res : defaultValue;
}
