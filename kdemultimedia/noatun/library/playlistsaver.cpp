#include <noatun/playlistsaver.h>
#include <qdom.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <qtextstream.h>
#include <noatun/app.h>
#include "ksaver.h"
#include <ksimpleconfig.h>
#include <kmimetype.h>
#include <klocale.h>
#include <qregexp.h>
#include <qxml.h>
#include <kdebug.h>

PlaylistSaver::PlaylistSaver()
{
}

PlaylistSaver::~PlaylistSaver()
{
}

bool PlaylistSaver::save(const KURL &file, int opt)
{
//	kdDebug(66666) << k_funcinfo << "opt=" << opt << endl;

	if(file.isEmpty() || !file.isValid())
		return false;

	switch (opt)
	{
		default:
		case 0:
		case XMLPlaylist:
			return saveXML(file, opt);

		case M3U:
		case EXTM3U:
			return saveM3U(file, opt);

		case PLS:
			return savePLS(file, opt);

		case ASX:
			return false; // No, I won't code that! [mETz]
	}
}

bool PlaylistSaver::load(const KURL &file, int opt)
{
//	kdDebug(66666) << k_funcinfo << "opt=" << opt << endl;

	switch (opt)
	{
		default:
		case 0:
		case XMLPlaylist:
		case ASX:
			return loadXML(file, opt);

		case M3U:
		case EXTM3U:
			return loadM3U(file, opt);

		case PLS:
			return loadPLS(file, opt);
	}
}

bool PlaylistSaver::metalist(const KURL &url)
{
	kdDebug(66666) << k_funcinfo << "url=" << url.url() << endl;

	QString end=url.filename().right(3).lower();
	/*
	if (end=="mp3" || end=="ogg") // we want to download streams only
	{
		kdDebug(66666) << k_funcinfo << "I can only load playlists" << endl;
		return false;
	}
	*/

/*
.wax	audio/x-ms-wax		Metafiles that reference Windows Media files with the
								.asf, .wma or .wax file extensions.

.wvx	video/x-ms-wvx		Metafiles that reference Windows Media files with the
								.wma, .wmv, .wvx or .wax file extensions.

.asx	video/x-ms-asf		Metafiles that reference Windows Media files with the
								.wma, .wax, .wmv, .wvx, .asf, or .asx file extensions.
*/

	// it's actually a stream!
	if (end!="pls" &&
		end!="m3u" &&
		end!="wax" && // windows mediaplayer metafile
		end!="wvx" && // windows mediaplayer metafile
		end!="asx" && // windows mediaplayer metafile
		url.protocol().lower()=="http")
	{
		KMimeType::Ptr mimetype = KMimeType::findByURL(url);
		QString type=mimetype->name();

		if (type!="application/octet-stream")
			return false;

		QMap<QString,QString> map;
		map["playObject"]="Arts::StreamPlayObject";
		map["title"] = i18n("Stream from %1").arg(url.host());

		KURL u(url);
		if (!u.hasPath())
			u.setPath("/");

		map["stream_"] = map["url"] = u.url();

		reset();
		readItem(map);
		return true;
	}

	// it is a pls, m3u or ms-media-player file by now
	if(loadXML(url, XMLPlaylist))
		return true;

	if(loadXML(url,ASX))
		return true;

	if(loadPLS(url))
		return true;

	if(loadM3U(url))
		return true;

	return false;
}

bool PlaylistSaver::saveXML(const KURL &file, int )
{
	QString localFile;
	if (file.isLocalFile())
		localFile = QFile::encodeName(file.path());
	else
		localFile = napp->tempSaveName(file.path());

	// QDom is a pain :)
	QDomDocument doc("playlist");
	doc.setContent(QString("<!DOCTYPE XMLPlaylist><playlist version=\"1.0\" client=\"noatun\"/>"));

	QDomElement docElem=doc.documentElement();

	reset();
	PlaylistItem i;
	QStringList props;
	while ((i=writeItem()))
	{
		// write all properties
		props=i.properties();
		QDomElement elem=doc.createElement("item");
		for (QStringList::Iterator pi(props.begin()); pi!=props.end(); ++pi)
		{
			QString val=i.property(*pi);
			elem.setAttribute(*pi, val);

			if ((*pi)=="url")
			{
				KURL u(val);
				if (u.isLocalFile())
				{
					elem.setAttribute("local", u.path());
				}
			}
		}

		docElem.appendChild(elem);
		props.clear();
	}
	Noatun::KSaver saver(localFile);
	if (!saver.open())
		return false;
	saver.textStream().setEncoding(QTextStream::UnicodeUTF8);
	saver.textStream() << doc.toString();
	saver.close();

	return true;
}

class NoatunXMLStructure : public QXmlDefaultHandler
{
public:
	PlaylistSaver *saver;
	bool fresh;

	NoatunXMLStructure(PlaylistSaver *s)
		: saver(s), fresh(true)
	{
	}

	bool startElement(
			const QString&, const QString &,
			const QString &name, const QXmlAttributes &a
					 )
	{
		if (fresh)
		{
			if (name=="playlist")
			{
				fresh=false;
				return true;
			}
			else
			{
				return false;
			}
		}

		if (name != "item")
			return true;

		QMap<QString,QString> propMap;

		for (int i=0; i<a.count(); i++)
		{
			propMap[a.qName(i)] = a.value(i);
		}
		saver->readItem(propMap);

		return true;
	}
};


class MSASXStructure : public QXmlDefaultHandler
{
public:
	PlaylistSaver *saver;
	bool fresh;
	bool inEntry, inTitle;
	QMap<QString,QString> propMap;
	QString mAbsPath;

	MSASXStructure(PlaylistSaver *s, const QString &absPath)
		: saver(s), fresh(true), inEntry(false),
		inTitle(false), mAbsPath(absPath)
	{
		//kdDebug(66666) << k_funcinfo << endl;
	}

	bool startElement(const QString&, const QString &,
		const QString &name, const QXmlAttributes &a)
	{
		if (fresh)
		{
			if (name.lower()=="asx")
			{
 				//kdDebug(66666) << "found ASX format" << endl;
				fresh=false;
				return true;
			}
			else
			{
				kdDebug(66666) << "This is NOT an ASX style playlist!" << endl;
				return false;
			}
		}

		if (name.lower()=="entry")
		{
			if(inEntry) // WHOOPS, we are already in an entry, this should NEVER happen
			{
				kdDebug(66666) << "STOP, ENTRY INSIDE ENTRY!" << endl;
				return false;
			}
//			kdDebug(66666) << "<ENTRY> =====================" << endl;
			inEntry=true;
		}
		else
		{
			if (inEntry) // inside entry block
			{
				// known stuff inside an <entry> ... </entry> block:
				// <title>blah</title>
				// <param album="blub" />
				// <param artist="blah" />
				// <ref HREF="file:/something" />
				if(name.lower()=="ref")
				{
					for (int i=0; i<a.count(); i++)
					{
						if(a.qName(i).lower()=="href")
						{
							QString filename=a.value(i);
							if (filename.find(QRegExp("^[a-zA-Z0-9]+:/"))==0)
							{
								KURL url(filename);
								KMimeType::Ptr mimetype = KMimeType::findByURL(url);
								QString type=mimetype->name();
								if (type != "application/octet-stream")
								{
									propMap["url"]=filename;
								}
								else
								{
									propMap["playObject"]="SplayPlayObject";
									propMap["title"] = i18n("Stream from %1").arg(url.host());
									if (!url.hasPath())
										url.setPath("/");
									propMap["url"] = url.url();
									propMap["stream_"]=propMap["url"];
//									readItem(propMap);
//									continue;
								}
							}
							else
							{
								KURL u1;
								// we have to deal with a relative path
								if (filename.find('/'))
								{
									u1.setPath(mAbsPath); //FIXME: how to get the path in this place?
									u1.setFileName(filename);
								}
								else
								{
									u1.setPath(filename);
								}
								propMap["url"]=u1.url();
							}
//							kdDebug(66666) << "adding property url, value='" << propMap["url"] << "'" << endl;
						}
					}
				}
				else if(name.lower()=="param")
				{
					QString keyName="", keyValue="";

					for (int i=0; i<a.count(); i++)
					{
						if(a.value(i).lower()=="album")
							keyName="album";
						else if(a.value(i).lower()=="artist")
							keyName="author";
						else if(!keyName.isEmpty()) // successfully found a key, the next key=value pair has to be the value
						{
//							kdDebug(66666) << "keyName=" << keyName << ", next value is '" << a.value(i) << "'" << endl;
							keyValue=a.value(i);
						}
					}

					if (!keyName.isEmpty() && !keyValue.isEmpty())
					{
//						kdDebug(66666) << "adding property; key='" << keyName << "', value='" << keyValue << "'" << endl;
						propMap[keyName]=keyValue;
					}
				}
				else if(name.lower()=="title")
				{
					if(inTitle) // WHOOPS, we are already in an entry, this should NEVER happen
					{
						kdDebug(66666) << "STOP, TITLE INSIDE TITLE!" << endl;
						return false;
					}
//					kdDebug(66666) << "<TITLE> ======" << endl;
					inTitle=true;
				}
/*				else
				{
					kdDebug(66666) << "Unknown/unused element inside ENTRY block, NAME='" << name << "'" << endl;
					for (int i=0; i<a.count(); i++)
						kdDebug(66666) << " | " << a.qName(i) << " = '" << a.value(i) << "'" << endl;
				}*/
			}
/*			else
			{
				kdDebug(66666) << "Uninteresting element, NAME='" << name << "'" << endl;
				for (int i=0; i<a.count(); i++)
					kdDebug(66666) << "  | " << a.qName(i) << " = '" << a.value(i) << "'" << endl;
			}*/
		}

		return true;
	}

	bool endElement(const QString&,const QString&, const QString& name)
	{
//		kdDebug(66666) << k_funcinfo << "name='" << name << "'" << endl;
		if (name.lower()=="entry")
		{
			if(inEntry)
			{
/*				kdDebug(66666) << "</ENTRY> =====================" << endl;
				for (QMap<QString,QString>::ConstIterator it=propMap.begin(); it!=propMap.end(); ++it )
					kdDebug(66666) << "key='" << it.key() << "', val='" << it.data() << "'" << endl;
*/
				saver->readItem(propMap);
				propMap.clear();
				inEntry=false;
			}
			else // found </entry> without a start
			{
				kdDebug(66666) << "STOP, </ENTRY> without a start" << endl;
				return false;
			}
		}
		else if (name.lower()=="title")
		{
			if(inTitle && inEntry)
			{
//				kdDebug(66666) << "</TITLE> ======" << endl;
				inTitle=false;
			}
			else if (inTitle) // found </title> without a start or not inside an <entry> ... </entry>
			{
				kdDebug(66666) << "STOP, </TITLE> without a start" << endl;
				return false;
			}
		}
		return true;
	}

	bool characters(const QString &ch)
	{
		if(inTitle)
		{
			if (!ch.isEmpty())
			{
				propMap["title"]=ch;
//				kdDebug(66666) << "adding property; key='title', value='" << ch << "'" << endl;
			}
		}
		return true;
	}
};


bool PlaylistSaver::loadXML(const KURL &url, int opt)
{
	kdDebug(66666) << k_funcinfo <<
		"file='" << url.url() << "', opt=" << opt << endl;

	QString dest;
	if(KIO::NetAccess::download(url, dest, 0L))
	{
		QFile file(dest);
		if (!file.open(IO_ReadOnly))
			return false;

		reset();

		// QXml is horribly documented
		QXmlInputSource source(&file);
		QXmlSimpleReader reader;

		if (opt == ASX ||
			url.path().right(4).lower()==".wax" ||
			url.path().right(4).lower()==".asx" ||
			url.path().right(4).lower()==".wvx")
		{
			MSASXStructure ASXparser(this, url.path(0));
			reader.setContentHandler(&ASXparser);
			reader.parse(source);
			return !ASXparser.fresh;
		}
		else
		{
			NoatunXMLStructure parser(this);
			reader.setContentHandler(&parser);
			reader.parse(source);
			return !parser.fresh;
		}
	} // END download()

	return false;
}

bool PlaylistSaver::loadM3U(const KURL &file, int /*opt*/)
{
	kdDebug(66666) << k_funcinfo << "file='" << file.path() << endl;

	QString localFile;
	if(!KIO::NetAccess::download(file, localFile, 0L))
		return false;

	// if it's a PLS, transfer control, again (KIO bug?)
#if 0
	{
		KSimpleConfig list(local, true);
		list.setGroup("playlist");

		// some stupid Windows lusers like to be case insensitive
		QStringList groups=list.groupList().grep(QRegExp("^playlist$", false));
		if (groups.count())
		{
			KURL l;
			l.setPath(local);
			return loadPLS(l);
		}
	}
#endif

	QFile saver(localFile);
	saver.open(IO_ReadOnly);
	QTextStream t(&saver);

	bool isExt = false; // flag telling if we load an EXTM3U file
	QString filename;
	QString extinf;
	QMap<QString,QString> prop;
	reset();

	while (!t.eof())
	{
		if (isExt)
		{
			extinf = t.readLine();
			if (!extinf.startsWith("#EXTINF:"))
			{
//				kdDebug(66666) << "EXTM3U extinf line != extinf, assuming it's a filename." << endl;
				filename = extinf;
				extinf="";
			}
			else
			{
				filename = t.readLine(); // read in second line containing the filename
			}
			//kdDebug(66666) << "EXTM3U filename = '" << filename << "'" << endl;
		}
		else // old style m3u
		{
			filename = t.readLine();
		}

		if (filename == "#EXTM3U") // on first line
		{
//			kdDebug(66666) << "FOUND '#EXTM3U' @ " << saver.at() << "." << endl;
			isExt=true;
			continue; // skip parsing the first (i.e. this) line
		}

		if (filename.isEmpty())
			continue;

		if (filename.find(QRegExp("^[a-zA-Z0-9]+:/"))==0)
		{
			//kdDebug(66666) << k_funcinfo << "url filename = " << filename << endl;

			KURL protourl(filename);
			KMimeType::Ptr mimetype = KMimeType::findByURL(protourl);

			if (mimetype->name() != "application/octet-stream")
			{
				prop["url"] = filename;
			}
			else
			{
				prop["playObject"]="SplayPlayObject";
				// Default title, might be overwritten by #EXTINF later
				prop["title"] = i18n("Stream from %1").arg(protourl.host());

				if (!protourl.hasPath())
					protourl.setPath("/");

				prop["url"] = protourl.url();
				prop["stream_"] = prop["url"];
			}
		}
		else // filename that is not of URL style (i.e. NOT "proto:/path/somefile")
		{
			KURL u1;
			// we have to deal with a relative path
			if (filename.find('/'))
			{
				u1.setPath(file.path(0));
				u1.setFileName(filename);
			}
			else
			{
				u1.setPath(filename);
			}
			prop["url"] = u1.url();
		}

		// parse line of the following format:
		//#EXTINF:length,displayed_title
		if (isExt)
		{
			extinf.remove(0,8); // remove "#EXTINF:"
			//kdDebug(66666) << "EXTM3U extinf = '" << extinf << "'" << endl;
			int timeTitleSep = extinf.find(',', 0);

			int length = (extinf.left(timeTitleSep)).toInt();
			if (length>0)
				prop["length"]=QString::number(length*1000);

			QString displayTitle=extinf.mid(timeTitleSep+1);
			if (!displayTitle.isEmpty())
			{
				int artistTitleSep = displayTitle.find(" - ",0);
				if (artistTitleSep == -1) // no "artist - title" like format, just set it as title
				{
					prop["title"] = displayTitle;
				}
				else
				{
					prop["author"] = displayTitle.left(artistTitleSep);
					prop["title"] = displayTitle.mid(artistTitleSep+3);
				}
				/*kdDebug(66666) << "EXTM3U author/artist='" << prop["author"] <<
					"', title='" << prop["title"] << "'" << endl;*/
			} // END !displayTitle.isEmpty()
		} // END if(isExt)

//		kdDebug(66666) << k_funcinfo << "adding file '" << prop["url"] << "' to playlist" << endl;
		readItem(prop);
		prop.clear();
	} // END while()

	KIO::NetAccess::removeTempFile(localFile);

//	kdDebug(66666) << k_funcinfo << "END" << endl;
	return true;
}

bool PlaylistSaver::saveM3U(const KURL &file, int opt)
{
//	kdDebug(66666) << k_funcinfo << "file='" << file.path() << "', opt=" << opt << endl;

	bool isExt=(opt==EXTM3U); // easier ;)

	QString local(napp->tempSaveName(file.path()));
	QFile saver(local);
	saver.open(IO_ReadWrite | IO_Truncate);
	QTextStream t(&saver);

	reset();
	PlaylistItem i;
	QStringList props;

	// this is more code but otoh faster than checking for isExt inside the loop
	if(isExt)
	{
		t << "#EXTM3U" << '\n';

		while ((i=writeItem()))
		{
			int length = static_cast<int>(((i.property("length")).toInt())/1000);
			if(length==0) length=-1; // special value in an EXTM3U file, means "unknown"
			KURL u(i.property("url"));
			QString title;

			// if a playlistitem is without a tag or ONLY title is set
			if((i.property("author").isEmpty() && i.property("title").isEmpty())
			|| (i.property("author").isEmpty() && !i.property("title").isEmpty()) )
				title = u.filename().left(u.filename().length()-4);
			else
				title = i.property("author") + " - " + i.property("title");

//			kdDebug(66666) << "#EXTINF:"<< QString::number(length) << "," << title << endl;
			t << "#EXTINF:"<< QString::number(length) << "," << title << '\n';

			if (u.isLocalFile())
				t << u.path() << '\n';
			else
				t << u.url() << '\n';
		}
	}
	else
	{
		while ((i=writeItem()))
		{
			KURL u(i.property("url"));
			if (u.isLocalFile())
				t << u.path() << '\n';
			else
				t << u.url() << '\n';
		}
	}

	saver.close();
	KIO::NetAccess::upload(local, file, 0L);
	saver.remove();
	return true;
}

static QString findNoCase(const QMap<QString,QString> &map, const QString &key)
{
	for (QMap<QString,QString>::ConstIterator i=map.begin(); i!=map.end(); ++i)
	{
		if (i.key().lower() == key.lower())
			return i.data();
	}
	return 0;
}

bool PlaylistSaver::loadPLS(const KURL &file, int /*opt*/)
{
	kdDebug(66666) << k_funcinfo << "file='" << file.path() << endl;

	QString localFile;
	if(!KIO::NetAccess::download(file, localFile, 0L))
		return false;

	QFile checkFile(localFile);
	checkFile.open(IO_ReadOnly);
	QTextStream t(&checkFile);
	QString firstLine = t.readLine();
	if(firstLine.lower() != "[playlist]")
	{
		kdDebug(66666) << k_funcinfo << "PLS didn't start with '[playlist]', aborting" << endl;
		return false;
	}


	KSimpleConfig list(localFile, true);
	//list.setGroup("playlist");

	// some stupid Windows lusers like to be case insensitive
	QStringList groups = list.groupList().grep(QRegExp("^playlist$", false));
	/*
	if (!groups.count()) // didn't find "[playlist]", it's not a .pls file
		return false;
	*/

	QMap<QString,QString> group = list.entryMap(groups[0]);

	QString numOfEntries = findNoCase(group, "numberofentries");
	if(numOfEntries.isEmpty())
		return false;

	reset();

	unsigned int nEntries = numOfEntries.toInt();
	for(unsigned int entry = 1; entry <= nEntries; ++entry )
	{
		QString str;
		str.sprintf("file%d", entry);
		QString cast = findNoCase(group, str.utf8());
		str.sprintf("title%d", entry);
		QString title = findNoCase(group, str.utf8());

		// assume that everything in a pls is a streamable file
		QMap<QString,QString> map;

		KURL url(cast);
		if (!url.hasPath())
			url.setPath("/");

		map["playObject"]="SplayPlayObject";
		if (title.isEmpty())
			map["title"] = i18n("Stream from %1 (port: %2)").arg( url.host() ).arg( url.port() );
		else
			map["title"] = i18n("Stream from %1, (ip: %2, port: %3)").arg( title ).arg( url.host() ).arg(url.port() );

		map["url"] = map["stream_"]= url.url();

		readItem(map);
	}
	return true;
}

bool PlaylistSaver::savePLS(const KURL &, int)
{
	return false;
}

void PlaylistSaver::setGroup(const QString &)
{
}


