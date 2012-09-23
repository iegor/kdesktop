
#include "metatag.h"
#include "edit.h"

#include <string.h>

#include <noatun/app.h>
#include <noatun/stdaction.h>

#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qregexp.h>

#include <klocale.h>
#include <kaction.h>
#include <kglobal.h>
#include <klineedit.h>
#include <kconfig.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kfilemetainfo.h>

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new MetaTagLoader;
	}
}

MetaTagLoader::MetaTagLoader():Plugin()
{
	mAction = new KAction(i18n("&Tag Editor..."), "edit", 0, this, SLOT(editTag()), this, "edittag");
	napp->pluginActionMenu()->insert(mAction);
}

MetaTagLoader::~MetaTagLoader()
{
	napp->pluginActionMenu()->remove(mAction);
}

void MetaTagLoader::editTag()
{
	PlaylistItem i = napp->player()->current();

	if(!i)
		return;

	Editor *e = new Editor();
	e->open(i);
	e->show();

	connect(e, SIGNAL(saved(PlaylistItem &)),
		SLOT(update(PlaylistItem &)));
}

bool MetaTagLoader::update(PlaylistItem & item)
{
	KFileMetaInfo file_info(item.file(),item.mimetype());

	// Ack, no file info :(
	if ( !file_info.isValid() )
		return false;

	if(item.length() == -1) // no value set, set almost correct id3tag time
	{
		KFileMetaInfoItem length_item = file_info.item("Length");
		if(length_item.isValid())
		{
			int numVal = length_item.value().toInt();
			if (numVal != 0)
				item.setLength(numVal * 1000);
		}
	}

	// Yes, this is icky. It maps KFileMetaInfo property names to Noatun's
	setProperty(file_info, item, "Title", "title");
	setProperty(file_info, item, "Artist", "author");
	setProperty(file_info, item, "Album", "album");
	setProperty(file_info, item, "Genre", "genre");
	setProperty(file_info, item, "Tracknumber", "track");
	setProperty(file_info, item, "Date", "date");
	setProperty(file_info, item, "Comment", "comment");
	setProperty(file_info, item, "Location", "location");
	setProperty(file_info, item, "Organization", "organization");

	// Now map the audio properties over
	setProperty(file_info, item, "Bitrate", "bitrate");
	setProperty(file_info, item, "Sample Rate", "samplerate");
	setProperty(file_info, item, "Channels", "channels");

	return true;
}

bool MetaTagLoader::setProperty(KFileMetaInfo &info, PlaylistItem &item, const QString &key, const QString &property)
{
	KFileMetaInfoItem info_item = info.item(key);

	if ( info_item.isValid() )
	{
		if (!info_item.value().toString().stripWhiteSpace().isEmpty())
		{
			// The item is valid and non-empty, add it
			item.setProperty(property, info_item.value().toString());
		}
		else
		{
			// If the info_item is valid, but empty.
			// This means we know for a fact that this
			// property has no value. Blow it away.
			item.clearProperty(property);
		}
		return true;
	}

	// The item isn't valid, so we don't know that it has
	// no value. Don't remove the property, so we can work
	// well with other tag readers, like Lucky
	return false;
}

#include "metatag.moc"
