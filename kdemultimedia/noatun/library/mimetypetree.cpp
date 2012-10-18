#include "mimetypetree.h"
#include <kmimetype.h>
#include <qdict.h>
#include <qheader.h>


MimeTypeTree::MimeTypeTree(QWidget *parent)
	: KListView(parent)
{
	KMimeType::List list=KMimeType::allMimeTypes();
	QDict<QListViewItem> map;
	setRootIsDecorated(true);

	addColumn("-");
	header()->hide();
    QValueListIterator<KMimeType::Ptr> i(list.begin());
	for (; i != list.end(); ++i)
	{
		QString mimetype = (*i)->name();
		int slash = mimetype.find("/");
		QString major = mimetype.left(slash);
		
		// hide all and inode majors
		if (major == "all" || major=="inode")
			continue;

		QString minor = mimetype.mid(slash+1);
		QListViewItem *majorItem=map[major];
		if (!majorItem)
		{
			majorItem=addMajor(major);
			map.insert(major, majorItem);
		}
		
		new QListViewItem(majorItem, minor);
	}
}

void MimeTypeTree::sel(QListViewItem *item)
{
	QListViewItem *p=item->parent();
	if (!p) return;
	QString major=p->text(0);
	QString minor=item->text(0);
	
	emit selected(major+'/'+minor);
}

QListViewItem* MimeTypeTree::addMajor(const QString &name)
{
	return new QListViewItem(this, name);
}


// GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666
// 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 
// GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666
// 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 
// GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666
// 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 
// GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666
// 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 666 GPL 

#include "mimetypetree.moc"
