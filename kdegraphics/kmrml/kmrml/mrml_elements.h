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

#ifndef MRML_ELEMENTS_H
#define MRML_ELEMENTS_H

#include <qdom.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include "mrml_shared.h"
#include "propertysheet.h"

#include <assert.h>

namespace KMrml
{
    class PropertySheet;

    class QueryParadigm
    {
    public:
        QueryParadigm() {}
        QueryParadigm( const QDomElement& elem );
        bool matches( const QueryParadigm& other ) const;
        QString type() const { return m_type; }

//         bool operator== ( const QueryParadigm& p1, const QueryParadigm& p2 )

    private:
        QString m_type;
        QMap<QString,QString> m_attributes;

        static bool equalMaps( const QMap<QString,QString>,
                               const QMap<QString,QString> );
    };

    class QueryParadigmList : protected QValueList<QueryParadigm>
    {
    public:
        typedef QValueListIterator<QueryParadigm> Iterator;
        typedef QValueListConstIterator<QueryParadigm> ConstIterator;

        void initFromDOM( const QDomElement& elem );
        bool matches( const QueryParadigmList& other ) const;
    };

    class MrmlElement
    {
    public:
        MrmlElement() {}
        MrmlElement( const QDomElement& elem );
        virtual ~MrmlElement() {}

        QString id() const      { return m_id;   }
        QString name() const    { return m_name; }
        QString attribute( const QString& name ) const { return m_attributes[ name ]; }
        QueryParadigmList paradigms() const { return m_paradigms; }


        QMapConstIterator<QString,QString> attributeIterator() const {
            return m_attributes.begin();
        }
        QMapConstIterator<QString,QString> end() const { return m_attributes.end(); }

        bool isValid() const { return !m_name.isNull() && !m_id.isNull(); }

    protected:
        QString m_id;
        QString m_name;
        QueryParadigmList m_paradigms;
        QMap<QString,QString> m_attributes;

        void setOtherAttributes( QDomElement& elem ) const;
    };

    class Algorithm : public MrmlElement
    {
    public:
        Algorithm() { m_collectionId = "adefault"; }
        Algorithm( const QDomElement& elem );
        QString type() const { return m_type; }

        QString collectionId() const
        {
            return m_collectionId;
        }
        void setCollectionId( const QString& id )
        {
            m_collectionId = id;
        }

        QDomElement toElement( QDomElement& parent ) const;
        const PropertySheet& propertySheet() const;

        static Algorithm defaultAlgorithm();

    private:
        QString m_type;
        PropertySheet m_propertySheet;

        QString m_collectionId;
    };

    class Collection : public MrmlElement
    {
    public:
        Collection() {}
        Collection( const QDomElement& elem );
    };

    template <class t> class MrmlElementList : public QValueList<t>
    {
    public:
        typedef QValueListIterator<t> Iterator;
        typedef QValueListConstIterator<t> ConstIterator;

        /**
         * Creates an invalid element.
         */
        MrmlElementList( const QString& tagName ) :
            QValueList<t>(),
            m_tagName( tagName ) {}
        MrmlElementList( const QDomElement& elem, const QString& tagName ) :
            QValueList<t>(),
            m_tagName( tagName )
        {
            initFromDOM( elem );
        }
        virtual ~MrmlElementList() {};

        void initFromDOM( const QDomElement& elem )
        {
            assert( !m_tagName.isEmpty() );

            QValueList<t>::clear();

            QDomNodeList list = elem.elementsByTagName( m_tagName );
            for ( uint i = 0; i < list.length(); i++ )
            {
                QDomElement elem = list.item( i ).toElement();
                t item( elem );
                if ( item.isValid() )
                    append( item );
            }
        }

        t findByName( const QString& name ) const
        {
            QValueListConstIterator<t> it = QValueList<t>::begin();
            for ( ; it != QValueList<t>::end(); ++it )
            {
                if ( (*it).name() == name )
                    return *it;
            }

            return t();
        }

        t findById( const QString& id ) const
        {
            QValueListConstIterator<t> it = QValueList<t>::begin();
            for ( ; it != QValueList<t>::end(); ++it )
            {
                if ( (*it).id() == id )
                    return *it;
            }

            return MrmlElement();
        }

        QStringList itemNames() const {
            QStringList list;
            QValueListConstIterator<t> it = QValueList<t>::begin();
            for ( ; it != QValueList<t>::end(); ++it )
                list.append( (*it).name() );

            return list;
        }

        void setItemName( const QString& tagName ) { m_tagName = tagName; }
        QString tagName() const { return m_tagName; }

    private:
        QString m_tagName;
        MrmlElementList();
    };

    class AlgorithmList : public MrmlElementList<Algorithm>
    {
    public:
        AlgorithmList() :
            MrmlElementList<Algorithm>( MrmlShared::algorithm() )
        {}

        AlgorithmList algorithmsForCollection( const Collection& coll ) const;
    };

    class CollectionList : public MrmlElementList<Collection>
    {
    public:
        CollectionList() :
            MrmlElementList<Collection>( MrmlShared::collection() )
        {}
    };


    QValueList<QDomElement> directChildElements( const QDomElement& parent,
                                                 const QString& tagName);
    QDomElement firstChildElement( const QDomElement& parent,
                                   const QString& tagName );


    QDataStream& operator<<( QDataStream& stream, const QueryParadigm& );
    QDataStream& operator>>( QDataStream& stream, QueryParadigm& );

    QDataStream& operator<<( QDataStream& stream, const QueryParadigmList& );
    QDataStream& operator>>( QDataStream& stream, QueryParadigmList& );

    QDataStream& operator<<( QDataStream& stream, const MrmlElement& );
    QDataStream& operator>>( QDataStream& stream, MrmlElement& );

    QDataStream& operator<<( QDataStream& stream, const Algorithm& );
    QDataStream& operator>>( QDataStream& stream, Algorithm& );

    QDataStream& operator<<( QDataStream& stream, const Collection& );
    QDataStream& operator>>( QDataStream& stream, Collection& );

    template <class t> QDataStream& operator<<( QDataStream&,
                                                const MrmlElementList<t>& );
    template <class t> QDataStream& operator>>( QDataStream&,
                                                MrmlElementList<t>& );

    QDataStream& operator<<( QDataStream&, const AlgorithmList& );
    QDataStream& operator>>( QDataStream&, AlgorithmList& );

}

#endif // MRML_ELEMENTS_H
