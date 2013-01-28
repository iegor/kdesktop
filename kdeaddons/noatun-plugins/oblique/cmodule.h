// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef CMODULE_H
#define CMODULE_H

#include <noatun/pref.h>
#include "query.h"

class KListView;
class KLineEdit;
class QDialog;
class Oblique;
class QCheckBox;
class QComboBox;
class QPushButton;
class QTabWidget;

class SchemaConfig : public QWidget
{
Q_OBJECT
	Oblique *mOblique;

	KListView *mSchemaTree;
	KLineEdit *mPropertyEdit, *mValueEdit, *mPresentationEdit;
	QComboBox *mSchemaList;

	QCheckBox *mOptionPlayable, *mOptionChildrenVisible, *mOptionAutoOpen;

	QPushButton *mAdd, *mRemove, *mCopy;

	QPushButton *mAddSibling, *mAddChild, *mRemoveSelf;

	QDialog *mRegexpEditor;
	bool mIgnore;

	struct QueryItem
	{
		Query query;
		QString title;
		bool changed;
	};

	QMap<QString, QueryItem> mQueries;

public:
	SchemaConfig(QWidget *parent, Oblique *oblique);

	Oblique *oblique() { return mOblique; }

	void reopen();
	void save();

	static QString nameToFilename(const QString &name);
	static QString filenameToName(const QString &filename);

public slots:
	void newSchema();
	void copySchema();
	void removeSchema();

	void selectSchema(const QString &title);

	void addSibling();
	void addChild();
	void removeSelf();

	void setCurrentModified();

private slots:
	void editValueRegexp();

	void setCurrent(QListViewItem *_item);
	void updateCurrent();

	void move(QListViewItem *item, QListViewItem *afterFirst, QListViewItem *afterNow);

private:
	QueryItem *currentQuery();
};

class SliceListItem;

class SliceConfig : public QWidget
{
Q_OBJECT
	Oblique *mOblique;
	KListView *mSliceList;
	QPushButton *mAdd, *mRemove;
	QValueList<SliceListItem*> mAddedItems;
	QValueList<Slice*> mRemovedItems;

public:
	SliceConfig(QWidget *parent, Oblique *oblique);
	Oblique *oblique() { return mOblique; }

	void reopen();
	void save();


	SliceListItem *currentItem();

private slots:
	void addSibling();
	void removeSelf();
};

class Configure : public CModule
{
Q_OBJECT
	QTabWidget *tabs;
	SchemaConfig *schema;
	SliceConfig *slice;

public:
	Configure(Oblique *parent);

	virtual void reopen();
	virtual void save();
};


#endif
