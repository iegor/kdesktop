//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2002 by Andreas Zehender
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


#ifndef PMACTIONS_H
#define PMACTIONS_H

#include <qguardedptr.h>
#include <kaction.h>

class QComboBox;
class QSpinBox;
class QLabel;
class QToolButton;

/**
 * Combobox action for the toolbar.
 *
 * Copied from konq_actions.h, author: Simon Hausmann <hausmann@kde.org>
 */
class PMComboAction : public KAction
{
   Q_OBJECT
public:
   PMComboAction( const QString& text, int accel, const QObject* receiver, const char* member, QObject* parent, const char* name );
   ~PMComboAction( );
   
   virtual int plug( QWidget* w, int index = -1 );
   
   virtual void unplug( QWidget* w );
   
   QGuardedPtr<QComboBox> combo( ) { return m_combo; }

   void setMaximumWidth( int w ) { m_maxWidth = w; }
   void setMinimumWidth( int w ) { m_minWidth = w; }
   
signals:
   void plugged( );
   
private:
   QGuardedPtr<QComboBox> m_combo;
   const QObject* m_receiver;
   const char* m_member;
   int m_minWidth, m_maxWidth;
};

/**
 * Label action for the toolbar.
 *
 * Copied from konq_actions.h, author: Simon Hausmann <hausmann@kde.org>
 */
class PMLabelAction : public KAction
{
   Q_OBJECT
public:
   PMLabelAction( const QString &text, QObject *parent = 0, const char *name = 0 );
   
   virtual int plug( QWidget *widget, int index = -1 );
   virtual void unplug( QWidget *widget );
   QToolButton* button( ) { return m_button; }
   
private:
   QToolButton* m_button;
};

/**
 * Spinbox action for the toolbar.
 */
class PMSpinBoxAction : public KAction
{
   Q_OBJECT
public:
   PMSpinBoxAction( const QString& text, int accel, const QObject* receiver, const char* member, QObject* parent, const char* name );
   ~PMSpinBoxAction( );
   
   virtual int plug( QWidget* w, int index = -1 );
   virtual void unplug( QWidget* w );
   
   QGuardedPtr<QSpinBox> spinBox( ) { return m_spinBox; }

signals:
   void plugged( );
   
private:
   QGuardedPtr<QSpinBox> m_spinBox;
   const QObject* m_receiver;
   const char* m_member;
};


#endif
