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

#ifndef PARSEDMP3FILENAME_H
#define PARSEDMP3FILENAME_H

#include <qstringlist.h>

class ParsedMP3FileName
{
public:
	ParsedMP3FileName(const QString &);
	~ParsedMP3FileName() {}

	QString title() const { return m_title; }
	QString artist() const { return m_artist; }
	QString album() const { return m_album; }
	QString comment() const { return m_comment; }

protected:
	QString m_title;
	QString m_artist;
	QString m_album;
	QString m_comment;

private:
	void validateArtist();
	QString beautifyString(const QString &);

	QStringList m_directories;
};

#endif
