/***
 * Copyright (c) 2001 Charles Samuels <charles@kde.org>
 * Standard BSD License. Second version.
 * The added stipulation is that this cannot link
 * to GPL code.  Except in the explicit case
 * of Noatun linking to this, and to a GPL plugin,
 * where the GPL plugin does not use any code
 * in this class.  However, it may link directly
 * to the Qt Library, where Qt may be under any license.
 *
 * Debian, Gnome, and GNU must ALL DIE.
 * Especially GNU's stupid info pages.
 **/
#ifndef MIMETYPETREE_H
#define MIMETYPETREE_H

#include <klistview.h>

class MimeTypeTree : public KListView
{
Q_OBJECT
public:
	MimeTypeTree(QWidget *parent);

private:
	QListViewItem *addMajor(const QString &name);
private slots:
	void sel(QListViewItem *item);
	
signals:
	void selected(const QString &mimetype);
};

#endif

