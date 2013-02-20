/***************************************************************************
                          kpybrowser.h  -  description
                             -------------------
    begin                : Fri Aug 24 15:11:58 MST 2001
    copyright            : (C) 2001 by Christian Bird
    email                : chrisb@lineo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KPYBROWSER_H
#define KPYBROWSER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kapplication.h>
#include <klistview.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qtooltip.h>
#include <qdict.h>
#include "pybrowsenode.h"


/** KPyBrowser is the base class of the project */
class KPyBrowser : public KListView
{
  Q_OBJECT
  public:
    /** construtor */
    KPyBrowser(QWidget* parent=0, const char *name=0);
    /** destructor */
    ~KPyBrowser();
    void parseText(QString &pytext);

        //used by KPBToolTip to dynamically create the needed tooltip
        void tip (const QPoint &p, QRect &r, QString &str);

	private:
		PyBrowseNode *class_root, *function_root;

        //create a mapping of names to nodes
        QDict<PyBrowseNode> node_dict;

	public slots:
		void nodeSelected(QListViewItem *node);
	signals:
		void selected(QString, int);
    private:
        class KPBToolTip : public QToolTip
        {
            public:
                KPBToolTip(QWidget *parent);
            protected:
                void maybeTip( const QPoint & );
        };
        KPBToolTip* tooltip;
};

#endif
