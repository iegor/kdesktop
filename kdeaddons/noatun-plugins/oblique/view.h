// Copyright (c) 2003,2004 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef VIEW_H
#define VIEW_H

#include <kmainwindow.h>
#include <kaction.h>
#include <klineedit.h>

#include "tree.h"

class SchemaListAction;
class TabWidget;

class View : public KMainWindow
{
Q_OBJECT
	Oblique *mOblique;
	Tree *mTree;
	QValueList<Tree*> mTrees;
	TabWidget *mTabs;
	KAction *mRemoveTabAction;
	SchemaListAction *mSchemaListAction;

public:
	View(Oblique *oblique);
	~View();

	Tree *tree() { return mTree; }

	Oblique *oblique() { return mOblique; }

public slots:
	void addFiles();
	void addDirectory();
	void addTab();
	void removeTab();

signals:
	void listHidden();
	void listShown();

private slots:
	void configureToolBars();
	void newToolBarConfig();
	void jumpTextChanged(const QString &text);
	void use(Slice*);
	void currentTabChanged(QWidget *);
	void setSchema(const QString &file);

protected:
	virtual void showEvent(QShowEvent *) { emit listShown(); }
	virtual void hideEvent(QHideEvent *) { emit listHidden(); }
	void closeEvent(QCloseEvent*);
};


class LineEditAction : public KWidgetAction
{
Q_OBJECT
public:
	LineEditAction(const QString &text, const QObject *reciever, const char *slot, KActionCollection *parent, const char *name);

	KLineEdit *widget() { return static_cast<KLineEdit*>(KWidgetAction::widget()); }
	KLineEdit *lineEdit() { return widget(); }
};


#endif

