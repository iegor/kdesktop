/***************************************************************************
                               domtreeview.h
                             -------------------

    copyright            : (C) 2001 - The Kafka Team/Andreas Schlapbach
                           (C) 2005 - Leo Savernik
    email                : kde-kafka@master.kde.org
                           schlpbch@iam.unibe.ch
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* $Id: domtreeview.h 391867 2005-02-21 19:19:48Z savernik $ */

#ifndef DOMTREEVIEW_H
#define DOMTREEVIEW_H

#include <qptrdict.h>
#include <qptrlist.h>
#include <dom/css_stylesheet.h>
#include <dom/css_rule.h>
#include <dom/dom_node.h>

#include "domtreeviewbase.h"

namespace DOM {
  class Element;
  class CharacterData;
}

class DOMListViewItem;
class DOMTreeWindow;

class QListViewItem;

class KPushButton;
class KEdFind;
class KHTMLPart;

class DOMTreeView : public DOMTreeViewBase
{
    Q_OBJECT

    public:
	DOMTreeView(QWidget *parent, const char* name, bool allowSaving = true);
	~DOMTreeView();

        KHTMLPart *htmlPart() const { return part; }
        void setHtmlPart(KHTMLPart *);

	/** returns the main window */
	DOMTreeWindow *mainWindow() const;
    
    protected:
    /*
	void saveTreeAsHTML(const DOM::Node &pNode);
    */
        virtual bool eventFilter(QObject *o, QEvent *e);

    signals:
        /** emitted when the part has been changed. */
	void htmlPartChanged(KHTMLPart *part);
    
    public slots:
        void refresh();
	void increaseExpansionDepth();
	void decreaseExpansionDepth();
	void setMessage(const QString &msg);
	void hideMessageLine();

	void moveToParent();
	
	void activateNode(const DOM::Node &node);
	void deleteNodes();

	/**
	 * Takes measures to disconnect this instance from the current html
	 * part as long as it is active.
	 */
	void disconnectFromActivePart();

	/**
	 * Takes measures to disconnect this instance from the current html
	 * part when it is being torn down.
	 */
	void disconnectFromTornDownPart();

	/**
	 * Connects to the current html part.
	 */
	void connectToPart();
	
	void slotFindClicked();
	void slotAddElementDlg();
	void slotAddTextDlg();

    protected slots:
        void slotShowNode(const DOM::Node &pNode);
        void slotShowTree(const DOM::Node &pNode);
	void slotItemClicked(QListViewItem *);
	void slotRefreshNode(const DOM::Node &pNode);
	void slotMovedItems(QPtrList<QListViewItem> &items, QPtrList<QListViewItem> &afterFirst, QPtrList<QListViewItem> &afterNow);
	void slotPrepareMove();
	void slotSearch();

	// void slotSaveClicked();

	void slotPureToggled(bool);
        void slotShowAttributesToggled(bool);
	void slotHighlightHTMLToggled(bool);
	
	void showDOMTreeContextMenu(QListViewItem *, const QPoint &, int);

	void slotSetHtmlPartDelayed();
	void slotRestoreScrollOffset();

    private:
	QPtrDict<DOMListViewItem> m_itemdict;
	DOM::Node m_document;

	uint m_expansionDepth, m_maxDepth;
	bool m_bPure, m_bShowAttributes, m_bHighlightHTML;

    private:
	void showRecursive(const DOM::Node &pNode, const DOM::Node &node,
			   uint depth);

	// void saveRecursive(const DOM::Node &node, int ident);

	void searchRecursive(DOMListViewItem *cur_item,
			     const QString &searchText,
			     bool caseSensitive);

        void adjustDepth();
	void adjustDepthRecursively(QListViewItem *cur_item,  uint currDepth);
	void highlightHTML(DOMListViewItem *cur_item,
			   const QString &nodeName);

	void addElement(const DOM::Node &node, DOMListViewItem *cur_item,
			bool isLast);
        void updateIncrDecreaseButton();

    private:
	KEdFind* m_findDialog;

	KHTMLPart *part;
	QTextStream* m_textStream;

	const KListView* m_rootListView;

	KPushButton* m_saveButton;
	QObject *focused_child;
	DOM::Node current_node;
	DOM::CSSStyleSheet stylesheet;
	DOM::CSSRule active_node_rule;
	
	bool _refreshed;
	int scroll_ofs_x, scroll_ofs_y;


    // == DOM Node Info panel ======================================

    public:
        // Keep in sync with the widget stack children
        enum InfoPanel { ElementPanel, CDataPanel, EmptyPanel };
    
    public slots:
        void initializeOptionsFromNode(const DOM::Node &);
	void initializeOptionsFromListItem(QListViewItem *);

	void copyAttributes();
	void cutAttributes();
	void pasteAttributes();
	void deleteAttributes();
	
    private:
        void initDOMNodeInfo();

        void initializeOptionsFromElement(const DOM::Element &);
        void initializeOptionsFromCData(const DOM::CharacterData &);

    private slots:
        void slotItemRenamed(QListViewItem *, const QString &str, int col);
	void slotEditAttribute(QListViewItem *, const QPoint &, int col);
	void slotApplyContent();

	void showInfoPanelContextMenu(QListViewItem *, const QPoint &, int);
	
    private:
        DOM::Node infoNode;	// node these infos apply to

    // == End DOM Node Info panel ==================================
    
};

#endif
