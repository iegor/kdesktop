#include <noatun/playlist.h>
#include <noatun/app.h>
#include <noatun/player.h>
#include <noatun/downloader.h>
#include <noatun/pluginloader.h>

#include <kcmdlineargs.h>
#include <kfile.h>
#include <kmimetype.h>
#include <qregexp.h>
#include <assert.h>
#include <qmetaobject.h>
#include <kmedia2.h>
#include <vector>

PlaylistItemData::PlaylistItemData()
{
	mRefs=0;
}

PlaylistItemData::~PlaylistItemData()
{
}

QCString PlaylistItemData::mimetype() const
{
	if (isProperty("mimetype"))
		return property("mimetype").latin1();
	KMimeType::Ptr mimetype = KMimeType::findByURL(url());

	return mimetype->name().latin1();
}

QCString PlaylistItemData::playObject() const
{
	if (isProperty("playObject"))
		return property("playObject").latin1();

	std::string objectType;

	Arts::TraderQuery query;
	query.supports("Interface","Arts::PlayObject");
	query.supports("MimeType", std::string(mimetype()));

	std::vector<Arts::TraderOffer> *offers = query.query();
	if (!offers) return "";

	if(!offers->empty())
		objectType = offers->front().interfaceName();

	delete offers;

	return QCString(objectType.c_str());
}

QString PlaylistItemData::title() const
{
	if (isProperty("realtitle"))
		return property("realtitle");


	// "$(property)"
	QString format=napp->titleFormat();

	QRegExp find("(?:(?:\\\\\\\\))*\\$\\((.*)");

	int start=0;
	while (start != -1)
	{
		start = find.search(format, start);
		if (start == -1) break;

		// test if there's an odd amount of backslashes
		if (start>0 && format[start-1]=='\\')
		{
			// yes, so half the amount of backslashes

			// count how many there are first
			QRegExp counter("([\\\\]+)");
			counter.search(format, start-1);
			uint len=counter.cap(1).length()-1;

			// and half them, and remove one more
			format.replace(start-1, len/2+1, "");
			start=start-1+len/2+find.cap(1).length()+3;
			continue;
		}

		// now replace the backslashes with half as many

		if (format[start]=='\\')
		{
			// count how many there are first
			QRegExp counter("([\\\\]+)");
			counter.search(format, start);
			uint len=counter.cap(1).length();

			// and half them
			format.replace(start, len/2, "");
			start=start+len/2;
		}

		// "sth"foo"sth"
		QString cont(find.cap(1));
		QString prefix,suffix,propname;
		unsigned int i=0;
		if (cont[i] == '"')
		{
			i++;
			for (; i < cont.length(); i++)
			{
				if (cont[i] != '"')
					prefix += cont[i];
				else
					break;
			}
			i++;
		}


		for (; i < cont.length(); ++i)
		{
			if (cont[i]!='"' && cont[i]!=')')
				propname += cont[i];
			else
				break;
		}

		if (cont[i] == '"')
		{
			i++;
			for (; i < cont.length(); i++)
			{
				if (cont[i] != '"')
					suffix += cont[i];
				else
					break;
			}
			i++;
		}
		i++;


		QString propval = property(propname);
		if (propname == "title" && !propval.length())
		{
			propval = url().filename();
		}
		else if (propname == "comment")
		{
			// comments can contain newlines
			// these are not wanted in a formatted title
			propval.replace('\n', ' ');
		}

		if (propval.length())
		{
			propval = prefix+propval+suffix;
			format.replace(start, i+2, propval);
			start += propval.length();
		}
		else
		{
			format.replace(start, i+2, "");
		}
	}
	return format;

}

int PlaylistItemData::length() const
{
	return property("length", "-1").toInt();
}

void PlaylistItemData::setLength(int ms)
{
	setProperty("length", QString::number(ms));
}

PlaylistItem::PlaylistItem(const PlaylistItem &source)
{
	mData=source.mData;
	addRef();
}

PlaylistItem::PlaylistItem(PlaylistItemData *source)
{
	mData=source;
	addRef();
}

PlaylistItem::~PlaylistItem()
{
	removeRef();
}

PlaylistItem &PlaylistItem::operator =(const PlaylistItem &source)
{
	if (source)
		source.addRef();
	removeRef();
	mData=source.mData;
	return *this;
}

PlaylistItem &PlaylistItem::operator =(PlaylistItemData *source)
{
	if (source)
		source->addRef();
	removeRef();
	mData=source;
	return *this;
}

const PlaylistItem &PlaylistItem::operator =(const PlaylistItem &source) const
{
	if (source)
		source.addRef();
	removeRef();
	mData=source.mData;
	return *this;
}

const PlaylistItem &PlaylistItem::operator =(const PlaylistItemData *source) const
{
	if (source)
		const_cast<PlaylistItemData*>(source)->addRef();
	removeRef();
	mData=const_cast<PlaylistItemData*>(source);
	return *this;
}

// reference counting
void PlaylistItem::removeRef() const
{
	if (mData)
	{
		mData->removeRef();
	}
}

void PlaylistItem::addRef() const
{
	if (mData)
		mData->addRef();
}

QString PlaylistItemData::lengthString() const
{
	if ( length() == -1 ) // no file loaded
		return QString("--:--");

	int secs = length()/1000; // convert milliseconds -> seconds
	int seconds = secs % 60;
	return QString().sprintf("%.2d:%.2d", ((secs-seconds)/60), seconds);
}


bool PlaylistItemData::operator == (const PlaylistItemData &d) const
{
	return &d==this;
}

bool PlaylistItemData::operator != (const PlaylistItemData &d) const
{
	return ! (*this==d);
}












Playlist::Playlist(QObject *parent, const char *name) : QObject(parent, name)
{
	napp->player()->connect(this, SIGNAL(playCurrent()), SLOT(playCurrent()));
	napp->player()->connect(this, SIGNAL(listHidden()), SIGNAL(playlistHidden()));
	napp->player()->connect(this, SIGNAL(listShown()), SIGNAL(playlistShown()));

}

Playlist::~Playlist()
{

}

void Playlist::toggleList()
{
	if (listVisible())
		hideList();
	else
		showList();
}

int Playlist::handleArguments()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	int i;
	bool play= napp->autoPlay();

	for (i=0; i < args->count(); i++)
	{
		KURL u(args->url(i));
		if (u.isValid())
			addFile(u, play);
		play=false;
	}
	args->clear();
	return i;
}

QValueList<PlaylistItem> Playlist::select(
		const QStringList &keys, const QStringList &values,
		int limit, bool exact, bool caseSensitive
	)
{
	QValueList<PlaylistItem> list;
	QString k;
	QString v;
	QStringList::ConstIterator key, val;
	for (PlaylistItem i(getFirst()); i && limit; i=getAfter(i))
	{
		for (key = keys.begin(); key != keys.end() && limit ; ++key)
		{
			k=*key;
			v=i.property(k);

			for (val = values.begin(); val != values.end() && limit; ++val)
			{
				if ((*val).length()==0 && v.length()==0)
				{
					list.append(i);
					limit--;
					goto nextSong;
				}
				else if (exact)
				{
					if (
							(!caseSensitive && (*val).lower()==v.lower())
							||
							(caseSensitive && (*val)==v)
						)

					{
						list.append(i);
						limit--;
						goto nextSong;
					}
				}
				else
				{
					if ((*val).find(v, 0, caseSensitive)!=-1)
					{
						list.append(i);
						limit--;
						goto nextSong;
					}
				}
			}
		}
	nextSong:
		;
	}
	return list;
}

QValueList<PlaylistItem> Playlist::select(
		const QString &key, const QString &value,
		int limit, bool exact, bool caseSensitive
	)
{
	QStringList keys(key);
	QStringList values(value);
	return select(keys, values, limit, exact, caseSensitive);
}

void PlaylistItemData::added()
{
	PlaylistItem item(this);
	for (
			PlaylistNotifier *i=napp->player()->mNotifiers.first();
			i; i=napp->player()->mNotifiers.next())
	{
		i->added(item);
	}
}

void PlaylistItemData::removed()
{
	PlaylistItem item(this);
	for (
			PlaylistNotifier *i=napp->player()->mNotifiers.first();
			i; i=napp->player()->mNotifiers.next())
	{
		i->removed(item);
	}
}

void PlaylistItemData::modified()
{
	PlaylistItem item(this);
	for (
			PlaylistNotifier *i=napp->player()->mNotifiers.first();
			i; i=napp->player()->mNotifiers.next())
	{
		i->modified(item);
	}
}


PlaylistItem Playlist::nextSection()
{
	return next();
}

PlaylistItem Playlist::previousSection()
{
	return previous();
}


PlaylistNotifier::PlaylistNotifier()
{
	napp->player()->mNotifiers.append(this);
}

PlaylistNotifier::~PlaylistNotifier()
{
	napp->player()->mNotifiers.removeRef(this);
}



#include "playlist.moc"

