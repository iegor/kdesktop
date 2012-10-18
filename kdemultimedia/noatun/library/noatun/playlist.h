#ifndef NOATUNPLAYLIST_H
#define NOATUNPLAYLIST_H

#include <qobject.h>
#include <kurl.h>
#include <qdict.h>
#include <qstringlist.h>
#include <cassert>
#include <kdemacros.h>

class PlaylistItem;

/**
 * If you're not coding a playlist, ignore this class.
 *
 * The backend.  Since PlaylistItemData is refcounted,
 * this contains the data, the PlaylistItem is the "reference"
 * <pre>
 * PlaylistItem m=new PlaylistItemData;
 * </pre>
 * Of course, you're supposed to inherit from PlaylistItemData
 * in your Playlist, since there are pure virtuals.
 *
 * You can create these objects on demand.
 *
 * @short Playlist item data
 * @author Charles Samuels
 * @version 2.3
 **/
class KDE_EXPORT PlaylistItemData
{
public:
	PlaylistItemData();
	virtual ~PlaylistItemData();

	/**
	 * Noatun asks your playlist for properties.  It is your
	 * responsiblity to store the information.  But usually a QMap<QString,QString>
	 * is enough.
	 *
	 * If you return the default value, the default should not
	 * be written.
	 *
	 * This returns the property, or def if such a property doesn't exist
	 **/
	virtual QString property(const QString &key, const QString &def=0) const=0;

	/**
	 * This sets the property with the given key and value.
	 *
	 * Important: If you use a QMap, you'll have to remove the current
	 * item before adding a new one
	 **/
	virtual void setProperty(const QString &key, const QString &property)=0;

	/**
	 * remove the item with given key
	 **/
	virtual void clearProperty(const QString &key)=0;

	/**
	 * return a list of property keys
	 **/
	virtual QStringList properties() const=0;

	/**
	 * return whether if the given key exists
	 **/
	virtual bool isProperty(const QString &key) const=0;

	/**
	 * return the title of the song. By default, this will
	 * use the following by default, in order of priority
	 *
	 * property("realtitle")
	 * property("title")
	 * url().filename()
	 *
	 * you shouldn't need to override this.
	 **/
	virtual QString title() const;

	/**
	 * the true filename of the song, remote or local
	 **/
	virtual KURL url() const { return KURL(property("url")); }
	/**
	 * set the true filename of the song, remote or local
	 **/
	virtual void setUrl(const KURL &url) { setProperty("url", url.url()); }

	/**
	 * first, this checks for the property "mimetype", else
	 * it'l ask KMimeType based on file()
	 **/
	virtual QCString mimetype() const;

	/**
	 * first, checks for the property "playObject", else,
	 * it'l ask aRts
	 **/
	virtual QCString playObject() const;

	/**
	 * return the filename to send to the playobject
	 **/
	virtual QString file() const { return url().path(); }

	/**
	 * what's the length of the song, in milliseconds?
	 **/
	virtual int length() const;

	/**
	 * sets the length of the song, in milliseconds
	 **/
	virtual void setLength(int ms);

	/**
	 * returns a friendly representation of the length
	 * of this file
	 **/
	QString lengthString() const;

	/**
	 * compare yourself with the given PlaylistItemData
	 * This is implemented in the slow fashion of
	 * comparing all the properties.  You may
	 * have a much faster way of implementing this
	 * if this==&d, this will not be called, normally
	 **/
	virtual bool operator == (const PlaylistItemData &d) const;

	/**
	 * this is implemented as !(*this==d), you may have a
	 * faster way to implement this
	 **/
	virtual bool operator != (const PlaylistItemData &d) const;

	/**
	 * remove this item from the list
	 **/
	virtual void remove() = 0;


	/**
	 * Playlists should not download files if this is true
	 **/
	bool streamable() const  { return isProperty("stream_"); }

public:
	/**
	 * Call this when you want to signal
	 * the given item has been added to the list
	 **/
	void added();

	/**
	 * Your playlist must call this when the file
	 * is removed from the playlist
	 **/
	void removed();

	/**
	 * Your playlist must call this when the file
	 * is modified
	 **/
	void modified();


public: // reference counting
	/**
	 * Have the reference counter never delete this
	 *
	 * This is useful for when you want to keep all
	 * your items hanging around
	 **/
	void addRef() { mRefs++; }
	void removeRef()
	{
		mRefs--;
		if (!mRefs)
			delete this;
	}

private:
	mutable int mRefs;
};


/**
 * a reference to a PlaylistItem(Data)
 *
 * All methods here should have the same behavior
 * as they do for PlaylistItemData
 *
 * If you're a playlist, you should inherit
 * from PlaylistItemData
 *
 * It's client code's responsibility to ensure that
 * PlaylistItem is not null by using either the boolean
 * conversion or isNull()
 *
 * @short Playlist items
 * @author Charles Samuels
 * @version 2.3
 **/
class KDE_EXPORT PlaylistItem
{
public:
	PlaylistItem(const PlaylistItem &source);
	PlaylistItem(PlaylistItemData *source);
	PlaylistItem() : mData(0) {}
	~PlaylistItem();

	PlaylistItem &operator =(const PlaylistItem &source);
	PlaylistItem &operator =(PlaylistItemData *source);

	PlaylistItemData *data() { return mData; }
	const PlaylistItemData *data() const { return mData; }

	const PlaylistItem &operator =(const PlaylistItem &source) const;
	const PlaylistItem &operator =(const PlaylistItemData *source) const;

	operator bool() const { return (bool)mData; }
	bool isNull() const { return !(bool)mData; }

	bool operator ==(const PlaylistItem &i) const
	{
		if (data()==i.data()) return true;
		if (!data() || !i.data()) return false;
		return *i.data()==*data();
	}
	bool operator ==(const PlaylistItemData *i) const
	{
		if (data()==i) return true;
		if (!data() || !i) return false;
		return *i==*data();
	}

	bool operator !=(const PlaylistItem &i) const
		{ return ! (*this==i); }
	bool operator !=(const PlaylistItemData *i) const
		{ return ! (*this->data()==*i); }

	QString property(const QString &key, const QString &def=0) const
	{
		assert(mData);
		return mData->property(key, def);
	}

	void setProperty(const QString &key, const QString &property)
	{
		assert(mData);
		const_cast<PlaylistItemData*>(mData)->setProperty(key, property);
	}

	void clearProperty(const QString &key)
	{
		assert(mData);
		const_cast<PlaylistItemData*>(mData)->clearProperty(key);
	}

	QStringList properties() const
	{
		assert(mData);
		return mData->properties();
	}

	bool isProperty(const QString &key) const
	{
		assert(mData);
		return mData->isProperty(key);
	}

	KURL url() const { assert(mData); return mData->url(); }
	void setUrl(const KURL &url)
	{
		assert(mData);
		const_cast<PlaylistItemData*>(mData)->setUrl(url);
	}

	QCString mimetype() const { assert(mData); return mData->mimetype(); }
	QCString playObject() const { assert(mData); return mData->playObject(); }
	QString file() const { assert(mData); return mData->file(); }

	QString title() const
	{
		assert(mData);
		return mData->title();
	}

	int length() const
	{
		assert(mData);
		return mData->length();
	}

	void setLength(int ms) const
	{
		assert(mData);
		mData->setLength(ms);
	}

	QString lengthString() const { assert(mData); return mData->lengthString(); }

	void remove() { assert(mData); mData->remove(); }

	bool streamable() const  { assert(mData); return mData->streamable(); }

private:
	// reference counting
	void removeRef() const;
	void addRef() const; // requires mData already has item

private:
	mutable PlaylistItemData *mData;
	void *_bc1, *_bc2;
};

/**
 * The playlist, which you derive from when creating
 * your own playlist.
 *
 * Do not, under any circumstances, call a Playlist method
 * when you can call a Player method, unless, of course, you
 * ARE the playlist.
 **/
class Playlist : public QObject
{
Q_OBJECT
	friend class PlaylistItemData;
public:
	Playlist(QObject *parent, const char *name);
	/**
	 * on playlist unload, your playlist must
	 * have current()==0 and emit playCurrent
	 **/
	virtual ~Playlist();

	/**
	 * go to the front
	 **/
	virtual void reset()=0;

	/**
	 * empty the list
	 **/
	virtual void clear()=0;

	/**
	 * add a file
	 */
	virtual void addFile(const KURL&, bool play=false)=0;

	/**
	 * cycle forward, return that
	 **/
	virtual PlaylistItem next()=0;

	/**
	 * cycle to next section, return that
	 * defaults to return next()
	 */
	virtual PlaylistItem nextSection();

	/**
	 * cycle back, return that
	 **/
	virtual PlaylistItem previous()=0;

	/**
	 * cycle to previous section, return that
	 * defaults to return previous()
	 */
	virtual PlaylistItem previousSection();

	/**
	 * current item
	 **/
	virtual PlaylistItem current()=0;
	/**
	 * set the current item
	 **/
	virtual void setCurrent(const PlaylistItem &)=0;

	/**
	 * get the first item
	 **/
	virtual PlaylistItem getFirst() const =0;

	/**
	 * get the item after item, note that getFirst and getAfter do not
	 * have to follow play order since they are used solely to iterate
	 * over the entire collection in some order. Duplicating the play
	 * order (by looking into the future) is not necessary.
	 **/
	virtual PlaylistItem getAfter(const PlaylistItem &item) const =0;

	/**
	 * is the view visible?
	 **/

	virtual bool listVisible() const =0;

	/**
	 * do the KCmdLineArgs stuff
	 **/
	int handleArguments();

	/**
	 * return a list of songs in which at least one
	 * of the keys matches at least one of the values
	 *
	 * the default implementation will call getFirst()
	 * and getAfter() which could be potentially slow,
	 * depending how your playlist is designed.  So
	 * you're free to reimplement this if you could
	 * do better
	 *
	 * A value of "" is equal to an unset value
	 *
	 * limit is the maximum amount of items to return,
	 * or -1 if you want as many as possible
	 *
	 * if exact is true, a match is only made if
	 * the string is identical to a value.  if false
	 * a match is made if the string contains a value
	 *
	 * caseSensitive, if false, means that the given
	 * values are compared case insensitively to
	 * to the items in the playlist.  The keys
	 * are always compared with case sensitivity
	 **/
	virtual QValueList<PlaylistItem> select(
			const QStringList &keys, const QStringList &values,
			int limit=-1, bool exact=false, bool caseSensitive=false
		);

	/**
	 * The default implementation will just call
	 * the above select.  Of course, you're free to implement
	 * both of these (with different mechanisms if need be)
	 * for speed
	 **/
	virtual QValueList<PlaylistItem> select(
			const QString &key, const QString &value,
			int limit=-1, bool exact=false, bool caseSensitive=false
		);
	/**
	 * exactly the same as the above, except converts
	 * the const char* to QString (utf8)
	 **/
	inline QValueList<PlaylistItem> select(
			const char *key, const char *value,
			int limit=-1, bool exact=false, bool caseSensitive=false
		)
	{
		return select(
				QString(key), QString(value),
				limit, exact, caseSensitive
			);
	}

public slots:
	/**
	 * show the list!
	 **/
	virtual void showList()=0;
	/**
	 * hide it
	 **/
	virtual void hideList()=0;
	/**
	 * toggle visibility
	 **/
	virtual void toggleList();

signals:
	/**
	 * when you want the engine to reload current()
	 * This is how your playlist forces noatun to
	 * play a new song
	 **/
	void playCurrent();

	/**
	 * when the list is hidden
	 **/
	void listHidden();

	/**
	 * when the list is shown
	 **/
	void listShown();
};

/**
 * this class's methods will be called whenever
 * something happens to the playlist or its
 * items.
 *
 * If the playlist plugin changes, you don't have to do
 * anything.
 **/
class PlaylistNotifier
{
public:
	PlaylistNotifier();
	virtual ~PlaylistNotifier();

	/**
	 * a new item is added to the list
	 **/
	virtual void added(PlaylistItem &) {}

	/**
	 * an item is removed from the list
	 **/
	virtual void removed(PlaylistItem &) {}

	/**
	 * this item was modified (via a changed
	 * or added property
	 **/
	virtual void modified(PlaylistItem &) {}
};


#endif

