// Copyright (c) 2001 Frerich Raabe <raabe@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "parsedmp3filename.h"

#include <qstring.h>
#include <qstringlist.h>

#include <kurl.h>

ParsedMP3FileName::ParsedMP3FileName(const QString &path)
{
	KURL url = path;
	
	QString fileName = url.fileName(false);
	m_directories = QStringList::split("/", url.directory());
	
	if (fileName.startsWith("(") && fileName.contains(")"))
	{
		m_artist = fileName.mid(1, fileName.find(")") - 1);
		m_title = fileName.right(fileName.length() - fileName.find(")") - 1);
		validateArtist();
	}
	else if (fileName.startsWith("[") && fileName.contains("]"))
	{
		m_artist = fileName.mid(1, fileName.find("]") - 1);
		m_title = fileName.right(fileName.length() - fileName.find("]") - 1);
		validateArtist();
	}
	else if (fileName.contains("-"))
	{
		m_artist = fileName.left(fileName.find("-") - 1);
		m_title = fileName.right(fileName.length() - fileName.find("-") - 1);
		validateArtist();
	}
	else
	{
		m_title = fileName;
		m_album = m_directories.last();
		m_artist = m_directories[m_directories.count() - 2];
	}
		
	if (m_title.contains("(") && m_title.find(")", m_title.find("(")))
	{
		unsigned int start = m_title.find("(");
		unsigned int end = m_title.find(")");
		m_comment = m_title.mid(start + 1, end - start - 1);
		m_title.truncate(start);
	}

	m_title = beautifyString(m_title);
	if (m_title.right(4) == ".mp3")
		m_title.truncate(m_title.length() - 4);
	m_artist = beautifyString(m_artist);
	m_album = beautifyString(m_album);
	m_comment = beautifyString(m_comment);
}

void ParsedMP3FileName::validateArtist()
{
	bool couldConvert;
	if (m_artist.toUShort(&couldConvert) < 30 && couldConvert)
		m_artist = m_directories[m_directories.count() - 2];
	m_album = m_directories.last();
}

QString ParsedMP3FileName::beautifyString(const QString &s)
{
	QString temp = s.lower().simplifyWhiteSpace();
	
	temp[0] = temp[0].upper();
	
	unsigned int numSpaces = temp.contains(" ");
	unsigned int spacePos = 0;
	while (numSpaces > 0) {
		spacePos = temp.find(" ", spacePos == 0? 0 : spacePos + 1);
		temp[spacePos + 1] = temp[spacePos + 1].upper();
		numSpaces--;
	}
	
	return temp;
}
