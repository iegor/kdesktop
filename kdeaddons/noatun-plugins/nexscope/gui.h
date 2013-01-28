#ifndef GUI_H
#define GUI_H

#include <kmainwindow.h>
#include <klistview.h>
#include <qsplitter.h>
#include <qdict.h>
#include <dcopobject.h>
#include <kurl.h>

class Renderer;

class Control : public KMainWindow
{
Q_OBJECT
	
public:
	Control();

	void addCreator(const QString &title);

public slots:
	void save(const KURL &file);
	void save();
	void saveAs();
	
	void open();
	void open(const KURL &file);
	
protected slots:
	void dropEvent(QDropEvent *e, QListViewItem *parent, QListViewItem *pafter);
	void currentChanged(QListViewItem *item);

private:
	QWidget *mRight;
	KListView *mTree, *mCreatorsList;
	QWidget *mConfigurator;
	
	KURL mCurrentURL;
};

class RendererListView : public KListView
{
Q_OBJECT
public:
	RendererListView(QWidget *p);
	
protected:
	virtual bool acceptDrag(QDropEvent *event) const;
	virtual QDragObject *dragObject() const;
};


#endif
