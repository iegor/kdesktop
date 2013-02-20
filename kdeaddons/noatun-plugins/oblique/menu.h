// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef MENU_H
#define MENU_H

#include <kpopupmenu.h>
#include <kpropertiesdialog.h>
#include <kaction.h>

#include "file.h"
#include "oblique.h"

/**
 * a context menu for a item in the list
 **/
class FileMenu : public KPopupMenu
{
Q_OBJECT
	QValueList<File> mFiles;

public:
	FileMenu(QWidget *parent, Oblique *oblique, File file);
	/**
	 * @recursively uses everything under @p files
	 **/
	FileMenu(QWidget *parent, Oblique *oblique, TreeItem *files);

private slots:
	void removeFromList();
	void properties();
	void toggleInSlice(Slice *slice);
};

class SliceListAction : public KActionMenu
{
Q_OBJECT
	QMap<int, Slice*> mIndexToSlices;
	QValueList<File> mFiles;
	Oblique *mOblique;

public:
	SliceListAction(
			const QString &text, Oblique *oblique,
			QObject *reciever, const char *slot,
			const QValueList<File> &files = QValueList<File>(), QObject *parent=0, const char *name=0
		);

signals:
	void activated(Slice *slice);

private slots:
	void hit(int index);
	void slicesModified();
};


class SchemaListAction : public KActionMenu
{
Q_OBJECT
	QMap<int, QString> mIndexToSchemas;
	Tree *mTree;

public:
	SchemaListAction(
			const QString &text,
			QObject *reciever, const char *slot,
			QObject *parent, const char *name
		);

	void setTree(Tree *tree) { mTree = tree; }

signals:
	void activated(const QString &);

private slots:
	void hit(int index);
	void prepare();
};


class ObliquePropertiesDialog : public KPropertiesDialog
{
Q_OBJECT
	QValueList<File> mFiles;

public:
	ObliquePropertiesDialog(const QValueList<File> &files, QWidget *parent);

private:
	static KFileItemList makeItems(const QValueList<File> &files);

private slots:
	void modified();
};


#endif

