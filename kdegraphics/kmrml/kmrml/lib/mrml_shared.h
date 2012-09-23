/* This file is part of the KDE project
   Copyright (C) 2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>

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

#ifndef SHARED_H
#define SHARED_H

// maybe use mrml_const.h from libMRML, unfortunately not installed
// by gift 0.1.6pre2

#include <qshared.h>
#include <qstring.h>

class MrmlShared
{
public:
// attribute/element names for mrml
    static void ref();
    static bool deref();

    static const QString& sessionId()           { return *m_sessionId; }
    static const QString& transactionId()       { return *m_transactionId; }
    static const QString& algorithm()           { return *m_algorithm; }
    static const QString& algorithmId()         { return *m_algorithmId; }
    static const QString& algorithmName()       { return *m_algorithmName; }
    static const QString& algorithmList()       { return *m_algorithmList; }
    static const QString& algorithmType()       { return *m_algorithmType; }
    static const QString& collectionId()        { return *m_collectionId; }
    static const QString& collectionList()      { return *m_collectionList; }
    static const QString& collection()          { return *m_collection; }
    static const QString& collectionName()      { return *m_collectionName; }
    static const QString& queryParadigm()       { return *m_queryParadigm; }
    static const QString& queryParadigmList()   { return *m_queryParadigmList; }
    static const QString& configureSession()    { return *m_configureSession; }

    // property sheet stuff
    static const QString& propertySheet()       { return *m_propertySheet; }
    static const QString& propertySheetId()     { return *m_propertySheetId; }
    static const QString& propertySheetType()   { return *m_propertySheetType; }
    static const QString& sendName()            { return *m_sendName; }
    static const QString& sendType()            { return *m_sendType; }
    static const QString& sendValue()           { return *m_sendValue; }
    static const QString& maxSubsetSize()       { return *m_maxSubsetSize; }
    static const QString& minSubsetSize()       { return *m_minSubsetSize; }
    static const QString& caption()             { return *m_caption; }
    static const QString& from()                { return *m_from; }
    static const QString& to()                  { return *m_to; }
    static const QString& step()                { return *m_step; }
    static const QString& sendBooleanInverted() { return *m_sendBooleanInverted; }

    static const QString& multiSet()            { return *m_multiSet; }
    static const QString& subset()              { return *m_subset; }
    static const QString& setElement()          { return *m_setElement; }
    static const QString& boolean()             { return *m_boolean; }
    static const QString& numeric()             { return *m_numeric; }
    static const QString& textual()             { return *m_textual; }
    static const QString& panel()               { return *m_panel; }
    static const QString& clone()               { return *m_clone; }
    static const QString& reference()           { return *m_reference; }

    static const QString& element()             { return *m_element; }
    static const QString& attribute()           { return *m_attribute; }
    static const QString& attributeName()       { return *m_attributeName; }
    static const QString& attributeValue()      { return *m_attributeValue; }
    static const QString& children()            { return *m_children; }
    static const QString& none()                { return *m_none; }

    static const QString& visibility()          { return *m_visibility; }
    static const QString& visible()             { return *m_visible; }
    static const QString& invisible()           { return *m_invisible; }
    static const QString& popup()               { return *m_popup; }
//     static const QString& ()                 { return *m_; }

    // meta-data
    static const QString& mrml_data()           { return *m_mrml_data; }

    // kio_mrml tasks
    static const QString& kio_task()            { return *m_kio_task; }
    static const QString& kio_initialize()      { return *m_kio_initialize; }
    static const QString& kio_startQuery()      { return *m_kio_startQuery; }


private:
    static const QString * m_sessionId;
    static const QString * m_transactionId;
    static const QString * m_algorithm;
    static const QString * m_algorithmId;
    static const QString * m_algorithmName;
    static const QString * m_algorithmList;
    static const QString * m_algorithmType;
    static const QString * m_collectionId;
    static const QString * m_collectionList;
    static const QString * m_collection;
    static const QString * m_collectionName;
    static const QString * m_queryParadigm;
    static const QString * m_queryParadigmList;
    static const QString * m_configureSession;

    // property sheet stuff
    static const QString * m_propertySheet;
    static const QString * m_propertySheetId;
    static const QString * m_propertySheetType;
    static const QString * m_sendName;
    static const QString * m_sendType;
    static const QString * m_sendValue;
    static const QString * m_maxSubsetSize;
    static const QString * m_minSubsetSize;
    static const QString * m_caption;
    static const QString * m_from;
    static const QString * m_to;
    static const QString * m_step;
    static const QString * m_sendBooleanInverted;

    static const QString * m_multiSet;
    static const QString * m_subset;
    static const QString * m_setElement;
    static const QString * m_boolean;
    static const QString * m_numeric;
    static const QString * m_textual;
    static const QString * m_panel;
    static const QString * m_clone;
    static const QString * m_reference;

    static const QString * m_element;
    static const QString * m_attribute;
    static const QString * m_attributeName;
    static const QString * m_attributeValue;
    static const QString * m_children;
    static const QString * m_none;

    static const QString * m_visibility;
    static const QString * m_visible;
    static const QString * m_invisible;
    static const QString * m_popup;
//     static const QString * m_;

    // meta-data
    static const QString * m_mrml_data;

    // kio_mrml tasks
    static const QString * m_kio_task;
    static const QString * m_kio_initialize;
    static const QString * m_kio_startQuery;

private:
    static void cleanup();
    static void init();

    static int s_references;
};

#endif // SHARED_H
