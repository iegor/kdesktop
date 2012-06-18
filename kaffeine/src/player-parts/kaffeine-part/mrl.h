/*
 * mrl.h
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

#ifndef MRL_H
#define MRL_H

#include <qdatetime.h>
#include <qstringlist.h>

#include <kurl.h>

/*
 * media resource locator
 * holds url and meta info
 */

class KDE_EXPORT MRL
{
public:
   MRL();
   /* don't use this constructor to prevent encoding problems */
   MRL(const KURL& url, const QString& title = QString::null, const QTime& length = QTime(), const QString& mime = QString::null,
       const QString& artist = QString::null, const QString& album = QString::null, const QString& track = QString::null,
       const QString& year = QString::null, const QString& genre = QString::null, const QString& comment = QString::null,
       const QStringList& subtitleFiles = QStringList(), const int currentSubtitle = -1);
   MRL(const QString& url, const QString& title = QString::null, const QTime& length = QTime(), const QString& mime = QString::null,
       const QString& artist = QString::null, const QString& album = QString::null, const QString& track = QString::null,
       const QString& year = QString::null, const QString& genre = QString::null, const QString& comment = QString::null,
       const QStringList& subtitleFiles = QStringList(), const int currentSubtitle = -1);
   virtual ~MRL();

   void reset() { m_url = QString::null; m_kurl = KURL(); }
   bool isEmpty() { return (m_url == QString::null); }

   const QString& url() const { return m_url; }
   const KURL& kurl() const { return m_kurl; }
   const QString& title() const { return m_title; }
   const QString& artist() const { return m_artist; }
   const QString& album() const { return m_album; }
   const QString& track() const { return m_track; }
   const QString& year() const { return m_year; }
   const QString& genre() const { return m_genre; }
   const QString& comment() const { return m_comment; }
   const QString& mime() const { return m_mime; }
   const QTime& length() const { return m_length; }
   const QStringList& subtitleFiles() const { return m_subtitleFiles; }
   int currentSubtitle() const { return m_currentSubtitle; }

   void setURL(const QString& url) { m_url = url; m_kurl = KURL(url); }
   void setTitle(const QString& title) { m_title = title; }
   void setArtist(const QString& artist) { m_artist = artist; }
   void setAlbum(const QString& album) { m_album = album; }
   void setTrack(const QString& track) { m_track = track; }
   void setYear(const QString& year) { m_year = year; }
   void setGenre(const QString& genre) { m_genre = genre; }
   void setComment(const QString& comment) { m_comment = comment; }
   void setMime(const QString& mime) { m_mime = mime; }
   void setLength(const QTime& length) { m_length = length; }
   void setSubtitleFiles(const QStringList& urls) { m_subtitleFiles = urls; }
   void addSubtitleFile(const QString& url) { m_subtitleFiles.append(url); }
   void setCurrentSubtitle(int sub) { m_currentSubtitle = sub; }

private:
   QString m_url;
   KURL m_kurl;
   QString m_title;
   QString m_artist;
   QString m_album;
   QString m_track;
   QString m_year;
   QString m_genre;
   QString m_comment;
   QString m_mime;
   QTime m_length;
   QStringList m_subtitleFiles;
   int m_currentSubtitle; /* -1 -> off */
};

#endif /* MRL_H */
