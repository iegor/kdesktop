/*
 * playlistimport.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <qdom.h>
#include <qregexp.h>
#include <qxml.h>

#include <kdebug.h>
#include <kio/netaccess.h>

#include "playlistimport.h"

class MyXMLParser : public QXmlDefaultHandler
{

public:
	QValueList<MRL> mrls;
	bool isKaffeinePlaylist;


	MyXMLParser() : isKaffeinePlaylist(false)
	{}

	bool startElement(const QString&, const QString&,
	                  const QString &qname, const QXmlAttributes &att)
	{

		if (qname == "playlist")
			if (att.value("client") == "kaffeine")
			{
				isKaffeinePlaylist = true;
				return true;
			}
			else
			{
				return false;
			}

		if (qname != "entry") return true;

		QStringList subs = QStringList();
		int currentSub = -1;

		if ((!att.value("subs").isNull()) && (!att.value("subs").isEmpty()))
			subs =  QStringList::split("&",att.value("subs"),false);
		if ((!att.value("subs").isNull()) && (!att.value("subs").isEmpty()))
		{
			bool ok;
			currentSub = att.value("currentSub").toInt(&ok);
			if (!ok)
				currentSub = -1;
		}

		// kdDebug() << "PlaylistImport: kaffeine import url: " << att.value("url") << endl;
		mrls.append(MRL(att.value("url"), att.value("title"), PlaylistImport::stringToTime(att.value("length")),
		                att.value("mime"), att.value("artist"), att.value("album"), att.value("track"),
		                att.value("year"), att.value("genre"), QString::null, subs, currentSub));
		return true;
	}

};

bool PlaylistImport::kaffeine(const QString& playlist, QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: kaffeine: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;

	QXmlInputSource source(&file);
	QXmlSimpleReader reader;

	MyXMLParser parser;
	reader.setContentHandler(&parser);
	reader.parse(source);
	file.close();

	if (!parser.isKaffeinePlaylist)
	{
		return false;
	}
	else
	{
		QValueList<MRL>::ConstIterator end(parser.mrls.end());
		for (QValueList<MRL>::ConstIterator it = parser.mrls.begin(); it != end; ++it)
			mrls.append(*it);
		return true;
	}
}

class NoatunXMLParser : public QXmlDefaultHandler
{

public:
	QValueList<MRL> mrls;
	bool isNoatunPlaylist;

	NoatunXMLParser(): isNoatunPlaylist(false)
	{}

	bool startElement(const QString&, const QString &,
	                  const QString &qname, const QXmlAttributes &att)
	{

		if (qname == "playlist")
			if (att.value("client") == "noatun")
			{
				isNoatunPlaylist = true;
				return true;
			}
			else
			{
				return false;
			}

		if (qname != "item") return true;

		QString title = att.value("title");

		if (title.isNull())
			title = att.value("url");

		bool ok;
		QTime length;
		int time = att.value("length").toInt(&ok);
		if ((ok) && (time > 0))
		{
			length = QTime().addMSecs(time);
		}

		kdDebug() << "PlaylistImport: noatun import url: " << att.value("url") << endl;
		mrls.append(MRL(att.value("url"), title, length, QString::null, att.value("author"),
		                att.value("album"), att.value("track")));

		return true;
	}

};

bool PlaylistImport::noatun(const QString& playlist, QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: noatun: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;

	QXmlInputSource source(&file);
	QXmlSimpleReader reader;

	NoatunXMLParser parser;
	reader.setContentHandler(&parser);
	reader.parse(source);
	file.close();

	if (!parser.isNoatunPlaylist)
	{
		return false;
	}
	else
	{
		QValueList<MRL>::ConstIterator end(parser.mrls.end());
		for (QValueList<MRL>::ConstIterator it = parser.mrls.begin(); it != end; ++it)
			mrls.append(*it);
		return true;
	}
}

bool PlaylistImport::m3u(const QString& playlist , QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: m3u: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;
	QTextStream stream(&file);

	//  if (stream.readLine().upper() != "#EXTM3U") return false;
	QString url;
	int time;
	bool ok;
	QTime length;
	QString title;
	KURL kUrl;
	bool foundAnyUrl = false;
	KURL plurl(playlist);
	plurl.setFileName ("");

	while (!stream.eof())
	{
		url        = stream.readLine();
		time       = 0;
		title      = QString::null;
		length     = QTime();
		if (url.left(1) == "#")
		{
			if (url.left(7).upper() == "#EXTINF")
			{
				url  = url.remove(0,8);
				time = url.section(",", 0,0).toInt(&ok);
				if ((ok) && (time > 0))
				{
					length = QTime().addSecs(time);
				}

				title = url.section(",",1,1);
				url = stream.readLine();
			}
			else
			{
				continue;
			}
		}
		url.replace ('\\', '/'); /* for windows styled urls */
		kUrl = KURL (plurl, url); /* maybe a relative url */
		if (kUrl.isValid())
		{
			kdDebug() << "PlaylistImport: m3u import url: " << kUrl.prettyURL() << endl;

			MRL mrl;
			if (kUrl.isLocalFile())
				mrl.setURL(kUrl.path());
			else
				mrl.setURL(kUrl.prettyURL());
			if (title.isNull())
				title = kUrl.fileName();
			mrl.setTitle(title);
			mrl.setLength(length);

			mrls.append(mrl);

			foundAnyUrl = true;
		}
		else
			kdDebug() << "PlaylistImport: M3U: Not valid: " << kUrl.prettyURL() << endl;
	}
	file.close();
	if (foundAnyUrl)
		return true;
	else
		return false;
}

bool PlaylistImport::pls(const QString& playlist, QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: pls: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;

	QTextStream stream(&file);

	//if (stream.readLine().upper() != "[PLAYLIST]") return false;

	// Better Handling of pls playlists - Taken from amaroK - amarok.kde.org
	// Counted number of "File#=" lines.
	uint entryCnt = 0;
	// Value of the "NumberOfEntries=#" line.
	uint numberOfEntries = 0;
	bool havePlaylistSection = false;
	QString tmp;
	QStringList lines;

	// set Regexp keywords, Be case insensitive
	const QRegExp regExp_NumberOfEntries("^NumberOfEntries\\s*=\\s*\\d+$", false);
	const QRegExp regExp_File("^File\\d+\\s*=", false);
	const QRegExp regExp_Title("^Title\\d+\\s*=", false);
	const QRegExp regExp_Length("^Length\\d+\\s*=\\s*-?\\d+$", false);
	const QRegExp regExp_Version("^Version\\s*=\\s*\\d+$", false);
	const QString section_playlist("[playlist]");

	/* Preprocess the input data.
	* Read the lines into a buffer; Cleanup the line strings;
	* Count the entries manually and read "NumberOfEntries".
	*/
	while (!stream.atEnd()) {
		tmp = stream.readLine();
		tmp = tmp.stripWhiteSpace();
		if (tmp.isEmpty())
			continue;
		lines.append(tmp);

		if (tmp == section_playlist) {
			havePlaylistSection = true;
			continue;
		}

		if (tmp.contains(regExp_File)) {
			entryCnt++;
			continue;
		}

		if (tmp.contains(regExp_NumberOfEntries)) {
			numberOfEntries = tmp.section('=', -1).stripWhiteSpace().toUInt();
			continue;
		}
	}

	file.close();

	if (numberOfEntries != entryCnt) {
		kdError() << "PlaylistImport: Invalid \"NumberOfEntries\" value in .pls playlist.  "
			<< "NumberOfEntries=" << numberOfEntries << "  counted="
			<< entryCnt << endl;
		/* Corrupt file. The "NumberOfEntries" value is
		 * not correct. Fix it by setting it to the manually
		 * counted number and go on parsing.
		 */
		numberOfEntries = entryCnt;
	}
	// If we have no Entries, return
	if (!numberOfEntries)
		return true;

	int time;
	bool ok;
	uint index;
	bool inPlaylistSection = false;
	QString* titles = new QString[entryCnt];
	QString* files = new QString[entryCnt];
	QTime* length = new QTime[entryCnt];

	QStringList::const_iterator i = lines.begin(), end = lines.end();
	for ( ; i != end; ++i) {
		if (!inPlaylistSection && havePlaylistSection) {
			/* The playlist begins with the "[playlist]" tag.
			 * Skip everything before this.
			 */
			if ((*i) == section_playlist)
				inPlaylistSection = true;
			continue;
		}
		if ((*i).contains(regExp_File)) {
			// Have a "File#=XYZ" line.
			index = extractIndex(*i);
			if (index > numberOfEntries || index == 0)
				continue;
			files[index-1] = (*i).section('=', 1).stripWhiteSpace();
			continue;
		}
		if ((*i).contains(regExp_Title)) {
			// Have a "Title#=XYZ" line.
			index = extractIndex(*i);
			if (index > numberOfEntries || index == 0)
				continue;
			titles[index-1] = (*i).section('=', 1).stripWhiteSpace();
			continue;
		}
		if ((*i).contains(regExp_Length)) {
			// Have a "Length#=XYZ" line.
			index = extractIndex(*i);
			if (index > numberOfEntries || index == 0)
				continue;
			tmp = (*i).section('=', 1).stripWhiteSpace();
			time = tmp.toInt(&ok);
			if ( !(ok) || !(time > 0) ) continue;
			length[index-1] = QTime().addSecs(time);
				continue;
		}
		if ((*i).contains(regExp_NumberOfEntries)) {
			// Have the "NumberOfEntries=#" line.
			continue;
		}
		if ((*i).contains(regExp_Version)) {
			// Have the "Version=#" line.
			tmp = (*i).section('=', 1).stripWhiteSpace();
			// We only support Version=2
			if (tmp.toUInt(&ok) != 2)
			kdWarning() << "PlaylistImport: pls: Unsupported version." << endl;
			continue;
		}
		kdWarning() << "PlaylistImport: pls: Unrecognized line: \"" << *i << "\"" << endl;
	}

	for (uint i=0; i<entryCnt; i++)
	{
		if (files[i].isNull()) continue;
		kdDebug() << "PlaylistImport: pls import url: " << files[i] << endl;
		if (titles[i].isNull())
			titles[i] = files[i];
		mrls.append(MRL(files[i], titles[i], length[i]));
	}

	delete [] titles;
	delete [] files;
	delete [] length;

	return true;
}

// Helper for pls import
uint PlaylistImport::extractIndex( const QString &str )
{
	/* Extract the index number out of a .pls line.
	 * Example:
	 *   extractIndex("File2=foobar") == 2
	 */
	bool ok = false;
	unsigned int ret;
	QString tmp(str.section('=', 0, 0));
	tmp.remove(QRegExp("^\\D*"));
	ret = tmp.stripWhiteSpace().toUInt(&ok);
	if (!ok)
		kdError() << "PlaylistImport: pls: Corrupt pls file, Error extracting index." << endl;
	return ret;
}

bool PlaylistImport::ram(const MRL& playlist, QValueList<MRL>& mrls, QWidget* parent)
{
	Q_ULONG result;
	char buf[10];

	kdDebug() << "PlaylistImport: ram: " << playlist.url() << endl;
	memset( buf, 0, 10 );

	if (playlist.kurl().isLocalFile()) {
		QFile file(playlist.kurl().path());
		if (!file.open(IO_ReadOnly)) {
			kdError() << "PlaylistImport: Can't open " << playlist.url() << endl;
			return false;
		}
		result = file.readBlock(buf, 4);
		file.close();
		if (result != 4) {
			kdError() << "PlaylistImport: Can't read " << playlist.url() << endl;
			return false;
		}
		if (buf[0]=='.' && buf[1]=='R' && buf[2]=='M' && buf[3]=='F') {
			kdDebug() << "PlaylistImport: Seems to be a real media file" << endl;
			return false;
		}
	}
	else if (!playlist.kurl().protocol().startsWith("http")) {
		kdError() << "PlaylistImport: ram: Download via " << playlist.kurl().protocol() << " protocol not supported." << endl;
		return false;
	}

	kdDebug() << "PlaylistImport: Seems to be a ram playlist!" << endl;

	QString localFile, url;
	if (KIO::NetAccess::mimetype(playlist.kurl(), parent) == "application/vnd.rn-realmedia") {
		kdDebug() << "PlaylistImport: Seems to be a real media file" << endl;
		return false;
	}

	if (KIO::NetAccess::download(playlist.kurl(), localFile, parent))
	{
		QFile plFile(localFile);
		if (!plFile.open(IO_ReadOnly)) return false;
		QTextStream stream( &plFile );

		while (!stream.eof())
		{
			url = stream.readLine();

			if (url[0] == '#') continue; /* ignore comments */
			if (url == "--stop--") break; /* stop line */

			if ((url.left(7) == "rtsp://") || (url.left(6) == "pnm://") || (url.left(7) == "http://"))
			{
				kdDebug() << "PlaylistImport: ram import url: " << url << endl;
				mrls.append(MRL(url, url));
			}
		}
	}
	else {
		kdError() << "PlaylistImport: " << KIO::NetAccess::lastErrorString() << endl;
                return false;
        }

	return true;
}

/**********************************************************************************
 *                             load asx playlist                                  *
 *   spec: http://msdn.microsoft.com/library/en-us/wmplay/mmp_sdk/asxelement.asp  *
 **********************************************************************************/

bool PlaylistImport::asx(const QString& playlist, QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: asx: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;

	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
	{
		kdError() << "PlaylistImport: XML parse error: " << errorMsg
		          << " (line: " << errorLine << ", column: " << errorColumn << ")" << endl;
		return false;
	}

	QDomElement root = doc.documentElement();

	QString url;
	QString title;
	QString author;
	QTime length;
	QString duration;

	if (root.nodeName().lower() != "asx") return false;

	QDomNode node = root.firstChild();
	QDomNode subNode;
	QDomElement element;

	while (!node.isNull())
	{
		url = QString::null;
		title = QString::null;
		author = QString::null;
		length = QTime();
		if (node.nodeName().lower() == "entry")
		{
			subNode = node.firstChild();
			while (!subNode.isNull())
			{
				if ((subNode.nodeName().lower() == "ref") && (subNode.isElement()) && (url.isNull()))
				{
					element = subNode.toElement();
					if (element.hasAttribute("href"))
						url = element.attribute("href");
					if (element.hasAttribute("HREF"))
						url = element.attribute("HREF");
					if (element.hasAttribute("Href"))
						url = element.attribute("Href");
					if (element.hasAttribute("HRef"))
						url = element.attribute("HRef");

				}

				if ((subNode.nodeName().lower() == "duration") && (subNode.isElement()))
				{
					duration = QString::null;
					element = subNode.toElement();
					if (element.hasAttribute("value"))
						duration = element.attribute("value");
					if (element.hasAttribute("Value"))
						duration = element.attribute("Value");
					if (element.hasAttribute("VALUE"))
						duration = element.attribute("VALUE");

					if (!duration.isNull())
						length = PlaylistImport::stringToTime(duration);
				}

				if ((subNode.nodeName().lower() == "title") && (subNode.isElement()))
				{
					title = subNode.toElement().text();
				}
				if ((subNode.nodeName().lower() == "author") && (subNode.isElement()))
				{
					author = subNode.toElement().text();
				}

				/* possible nodes we ignore: ABSTRACT, BANNER, BASE, COPYRIGHT, ENDMARKER, MOREINFO, PARAM,
				 * PREVIEWDURATION, STARTMARKER, STARTTIME
				 */
				subNode = subNode.nextSibling();
			}

			if (!url.isNull())
			{
				if (title.isNull())
					title = url;
				kdDebug() << "PlaylistImport: asx import url: " << url << endl;
				mrls.append(MRL(url, title, length, QString::null, author));
			}
		}
		node = node.nextSibling();
	}

	file.close();

	return true;
}

/****************************************************************
 * Full SMIL support seems to be impossible at the moment...    *
 * spec: http://www.w3.org/TR/REC-smil/                         *
 ****************************************************************/

bool PlaylistImport::smil(const QString& playlist, const MRL& baseMRL, QValueList<MRL>& mrls)
{
	kdDebug() << "PlaylistImport: smil: " << playlist << endl;
	QFile file(playlist);
	if (!file.open(IO_ReadOnly)) return false;

	QDomDocument doc;
	doc.setContent(&file);
	QDomElement root = doc.documentElement();

	if (root.nodeName().lower() != "smil") return false;

	bool anyURL = false;
	KURL kurl;
	QString url;
	QDomNodeList nodeList;
	QDomNode node;
	QDomElement element;

	//video sources...
	nodeList = doc.elementsByTagName("video");
	kdDebug() << "PlaylistImport: smil: " << nodeList.count() << " 'video' tags found" << endl;
	for (uint i = 0; i < nodeList.count(); i++)
	{
		node = nodeList.item(i);
		url = QString::null;
		if ((node.nodeName().lower() == "video") && (node.isElement()))
		{
			element = node.toElement();
			if (element.hasAttribute("src"))
				url = element.attribute("src");
			if (element.hasAttribute("Src"))
				url = element.attribute("Src");
			if (element.hasAttribute("SRC"))
				url = element.attribute("SRC");
		}
		if (!url.isNull())
		{
			kurl = KURL(baseMRL.kurl(), url);
			kdDebug() << "PlaylistImport: smil: found video source: " << kurl.url() << endl;
			mrls.append(kurl);
			anyURL = true;
		}
	}

	//audio sources...
	nodeList = doc.elementsByTagName("audio");
	kdDebug() << "PlaylistImport: smil: " << nodeList.count() << " 'audio' tags found" << endl;
	for (uint i = 0; i < nodeList.count(); i++)
	{
		node = nodeList.item(i);
		url = QString::null;
		if ((node.nodeName().lower() == "audio") && (node.isElement()))
		{
			element = node.toElement();
			if (element.hasAttribute("src"))
				url = element.attribute("src");
			if (element.hasAttribute("Src"))
				url = element.attribute("Src");
			if (element.hasAttribute("SRC"))
				url = element.attribute("SRC");
		}
		if (!url.isNull())
		{
			kurl = KURL(baseMRL.kurl(), url);
			kdDebug() << "PlaylistImport: smil: found audio source: " << kurl.url() << endl;
			mrls.append(kurl);
			anyURL = true;
		}
	}

	file.close();
	return anyURL;
}

QTime PlaylistImport::stringToTime(const QString& timeString)
{
	int sec = 0;
	bool ok = false;
	QStringList tokens = QStringList::split(':',timeString);

	sec += tokens[0].toInt(&ok)*3600; //hours
	sec += tokens[1].toInt(&ok)*60; //minutes
	sec += tokens[2].toInt(&ok); //secs

	if (ok)
		return QTime().addSecs(sec);
	else
		return QTime();
}
