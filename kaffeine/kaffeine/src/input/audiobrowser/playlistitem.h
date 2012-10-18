/*
 * playlistitem.h - a self-displayable item for kaffeine's playlist
 *
 * Copyright (C) 2004-2005 Giorgos Gousios
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

#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <klistview.h>

#define MIME_COLUMN 0
#define TITLE_COLUMN 1
#define ARTIST_COLUMN 2
#define ALBUM_COLUMN 3
#define TRACK_COLUMN 4
#define LENGTH_COLUMN 5

class QTime;
class QString;
class QStringList;
class MRL;

class PlaylistItem : public KListViewItem
{

public:
   PlaylistItem(KListView *list, KListViewItem *after, const QString& url, const QString& mime, const QString& title,
                const QString& length = QString::null, const QString& artist = QString::null, const QString& album = QString::null,
                const QString& track = QString::null, const QString& year = QString::null, const QString& genre = QString::null,
		const QStringList& subtitles = QStringList(), int currentSubtitle = -1);
   PlaylistItem(KListView* list, KListViewItem* after, const MRL&);
   virtual ~PlaylistItem();

   MRL toMRL() const;

   const QString& url() const;
   QString mime() const;
   QString title() const;
   QString artist() const;
   QString album() const;
   QString track() const;
   QString year() const;
   QString genre() const;
   QString length() const;
   const QStringList& subtitles() const;
   int currentSubtitle() const;

   void setUrl(const QString&);
   void setMime(const QString&);
   void setTitle(const QString&);
   void setArtist(const QString&);
   void setAlbum(const QString&);
   void setTrack(const QString&);
   void setYear(const QString&);
   void setGenre(const QString&);
   void setLength(const QString&);
   void setSubtitles(const QStringList&);
   void addSubtitle(const QString&);
   void setCurrentSubtitle(int);

   //reimplement to fix track order
   int compare( QListViewItem *i, int col, bool ascending ) const;

   void setPlaying(bool playing);
   virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);

private:
   static QTime stringToTime(const QString&);
   QString m_url;
   QString m_year;
   QString m_genre;
   QStringList m_subtitles;
   int m_currentSubtitle;
   bool isCurrent;
};

#endif /* PLAYLISTITEM_H */
