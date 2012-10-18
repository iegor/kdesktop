#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <noatun/playlist.h>
#include <noatun/plugin.h>

/*
class PlaylistItem
{
	PlaylistItem(const KURL &u=0);
	virtual ~PlaylistItem();

	QString title() const;
	virtual void setTitle(const QString &t);

	KURL url() const;
	virtual void setUrl(const KURL &u);

	int length() const;
	virtual void setLength(int l);
};
*/
class SafeListViewItem;
class View;
class List;
class QListViewItem;

class SplitPlaylist : public Playlist, public Plugin
{
Q_OBJECT
friend class SafeListViewItem;
friend class List;
public:
	SplitPlaylist();
	~SplitPlaylist();

	/**
	 * go to the front
	 **/
	virtual void reset();

	virtual void clear();
	virtual void addFile(const KURL&, bool play=false);
	/**
	 * Cycle everthing through forward
	 **/
	virtual PlaylistItem next();
	PlaylistItem next(bool play);
	/**
	 * return the one that might/should be playing now
	 **/
	virtual PlaylistItem current();
	/**
	 * Cycle through backwards
	 **/
	virtual PlaylistItem previous();

	virtual PlaylistItem getFirst() const;
	virtual PlaylistItem getAfter(const PlaylistItem &item) const;

	virtual bool listVisible() const;
	virtual void init();

	virtual Playlist *playlist()
		{ return this; }

	static SplitPlaylist *SPL() { return Self; }
	inline bool exiting() const { return mExiting; }
public slots:
	virtual void showList();
	virtual void hideList();
	virtual void remove(const PlaylistItem&);
	virtual void sort();


public slots:
	void setCurrent(const PlaylistItem &, bool emitC);
	void setCurrent(const PlaylistItem &);

	void listItemSelected(QListViewItem*);

	void randomize();

private:
	PlaylistItem currentItem, randomPrevious;

signals:
	void play(PlaylistItem*);

private:
	bool mExiting; // HACK HACK HACK HACK!!!
	View *view;
//	QRect currentRect;
	static SplitPlaylist *Self;
};


#endif
