#ifndef NOATUNPLAYLISTSAVER_H
#define NOATUNPLAYLISTSAVER_H

#include <kurl.h>
#include <qmap.h>
#include <noatun/playlist.h>

class Structure;

/**
 * a simple one-group implementatio of
 * the standard XML playlist format:
 *
 * http://noatun.kde.org/formats/xmlplaylist.phtml
 *
 * A custom implementation like yours should have a
 * different "client" attribute for the playlist tag
 **/
class PlaylistSaver
{
	friend class NoatunXMLStructure;
	friend class MSASXStructure;
public:
	enum Options
	{
		XMLPlaylist=1,
		M3U=2,
		PLS=4,
		EXTM3U=8,
		ASX=16
	};

	PlaylistSaver();
	virtual ~PlaylistSaver();

	bool save(const KURL &file, int options=0);
	bool load(const KURL &file, int options=0);

	/**
	 * guess the list's content between M3U or
	 * PLS, and give it to you
	 *
	 * this is also the way to make icecast work.
	 * you can also pass true icecast urls
	 * here and it'l do its magic.
	 *
	 * This calls readItem
	 **/
	bool metalist(const KURL &url);

	/**
	 * unused, for future expansion, do not use
	 * @internal
	 **/
	virtual void setGroup(const QString &);

protected:
	/**
	 * read the item, and add it to your list
	 * Given is a list of properties which coincide
	 * with the standard noatun ones
	 **/
	virtual void readItem(const QMap<QString,QString> &properties) = 0;

	/**
	 * add this item to the XML file
	 * or a null item if we're at the end
	 **/
	virtual PlaylistItem writeItem() = 0;

	/**
	 * unused, for future expansion
	 * @internal
	 **/
	virtual void changeGroup(const QString &) {}

	/**
	 * this is called when you should either
	 * clear your list, or start writing from the start of the list
	 *
	 * You usually don't need to implement this, since it'l always
	 * be called immediately after load() or save()
	 **/
	virtual void reset() {}

private:
	bool loadXML(const KURL &file, int x=0);
	bool saveXML(const KURL &file, int x=0);

	bool loadM3U(const KURL &file, int x=0);
	bool saveM3U(const KURL &file, int x=0);

	bool loadPLS(const KURL &file, int x=0);
	bool savePLS(const KURL &file, int x=0);

private:
	class Private;
	PlaylistSaver::Private *d; // unused

};

#endif
