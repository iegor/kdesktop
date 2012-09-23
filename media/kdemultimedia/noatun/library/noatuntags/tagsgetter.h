#ifndef TAGSGET_H
#define TAGSGET_H

#include <qobject.h>
#include <cmodule.h>
#include <qvaluelist.h>
#include <qptrlist.h>
#include "tags.h"

class TagsGetter : public QObject, public PlaylistNotifier
{
Q_OBJECT
public:
	TagsGetter();
	void associate(Tags *t);
	// if returns true, I'm deleted
	bool unassociate(Tags *t);

	int interval() const;
	bool loadAuto() const;

public: //playlistnotifier
	virtual void added(PlaylistItem &);
	virtual void removed(PlaylistItem &);

protected:
	void timerEvent(QTimerEvent *);
	
private slots:
	// select the songs that need updating
	void getSongs();

	void newSong();

public slots:
	void setInterval(int ms);
	void setLoadAuto(bool);

private:
	void sortPriority();

private:
	QPtrList<Tags> tags;
	QValueList<PlaylistItem> items;
};

class Control : public CModule
{
Q_OBJECT
public:
	Control(TagsGetter* parent);
};


#endif

