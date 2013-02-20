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

#include "mrml_creator.h"

QDomElement MrmlCreator::createMrml( QDomDocument& doc,
                                     const QString& sessionId,
                                     const QString& transactionId )
{
    QDomElement mrml = doc.createElement( "mrml" );
    doc.appendChild( mrml );
    mrml.setAttribute( MrmlShared::sessionId(), sessionId );
    if ( !transactionId.isNull() )
        mrml.setAttribute( MrmlShared::transactionId(), transactionId );

    return mrml;
}

QDomElement MrmlCreator::configureSession( QDomElement& mrml,
                                           const KMrml::Algorithm& algo,
                                           const QString& sessionId )
{
    QDomDocument doc = mrml.ownerDocument();
    QDomElement config = doc.createElement( MrmlShared::configureSession() );
    mrml.appendChild( config );
    config.setAttribute( MrmlShared::sessionId(), sessionId );
    algo.toElement( config );

    return config;
}

QDomElement MrmlCreator::addQuery( QDomElement& mrml, int resultSize )
{
    QDomElement query = mrml.ownerDocument().createElement("query-step");
    mrml.appendChild( query );
    // query.setAttribute( "query-step-id", "5" ); // ###
    query.setAttribute( "result-size", QString::number( resultSize ));
    return query;
}

QDomElement MrmlCreator::addRelevanceList( QDomElement& query )
{
    QDomElement elem =
        query.ownerDocument().createElement("user-relevance-element-list");
    query.appendChild( elem );
    return elem;
}

/**
 * Creates a <user-relevance-element> with the given attributes set.
 */
void MrmlCreator::createRelevanceElement( QDomDocument& doc,
                                          QDomElement& parent,
                                          const QString& url,
                                          Relevance relevance )
{
    QDomElement element = doc.createElement( "user-relevance-element" );
    element.setAttribute( "image-location", url );
    element.setAttribute( "user-relevance", QString::number( relevance ) );
    parent.appendChild( element );
}
