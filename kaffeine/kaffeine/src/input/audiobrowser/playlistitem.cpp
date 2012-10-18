/*
 * playlistitem.cpp - a self-displayable item for kaffeine's playlist
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

#include <qstringlist.h>

#include <kdebug.h>

#include "mrl.h"
#include "playlistitem.h"

PlaylistItem::PlaylistItem(KListView *list, KListViewItem *after, const QString& url, const QString& mime, const QString& title,
             const QString& length, const QString& artist, const QString& album, const QString& year, const QString& genre,
             const QString& track, const QStringList& subtitles, int currentSubtitle) :
             KListViewItem(list, after, mime, title, artist, album, track, length),
	     m_url(url), m_year(year), m_genre(genre), m_subtitles(subtitles), m_currentSubtitle(currentSubtitle), isCurrent(false)
{}

PlaylistItem::PlaylistItem(KListView* list, KListViewItem* after, const MRL& mrl) :
                           KListViewItem(list, after, mrl.mime(), mrl.title(), mrl.artist(), mrl.album(),
			   mrl.track(), mrl.length().toString("h:mm:ss")), m_url(mrl.url()),
			   m_year(mrl.year()), m_genre(mrl.genre()),
			   m_subtitles(mrl.subtitleFiles()), m_currentSubtitle(mrl.currentSubtitle()), isCurrent(false)
{}

PlaylistItem::~PlaylistItem() {}

MRL PlaylistItem::toMRL() const
{
  return MRL(url(), title(), stringToTime(length()), mime(), artist(), album(), track(), year(), genre(), QString::null, subtitles(), currentSubtitle());
}

const QString& PlaylistItem::url() const
{
  return m_url;
}

QString PlaylistItem::mime() const
{
  return this->text(MIME_COLUMN);
}

QString PlaylistItem::title() const
{
  return this->text(TITLE_COLUMN);
}

QString PlaylistItem::artist() const
{
  return this->text(ARTIST_COLUMN);
}

QString PlaylistItem::album() const
{
  return this->text(ALBUM_COLUMN);
}

QString PlaylistItem::track() const
{
  return this->text(TRACK_COLUMN);
}

QString PlaylistItem::year() const
{
  return m_year;
}

QString PlaylistItem::genre() const
{
  return m_genre;
}

QString PlaylistItem::length() const
{
  return this->text(LENGTH_COLUMN);
}

const QStringList& PlaylistItem::subtitles() const
{
  return m_subtitles;
}

int PlaylistItem::currentSubtitle() const
{
  return m_currentSubtitle;
}

void PlaylistItem::setUrl(const QString& url)
{
  m_url = url;
}

void PlaylistItem::setMime(const QString& mime)
{
  this->setText(MIME_COLUMN, mime);
}

void PlaylistItem::setTitle(const QString& title)
{
  this->setText(TITLE_COLUMN, title);
}

void PlaylistItem::setArtist(const QString& artist)
{
  this->setText(ARTIST_COLUMN, artist);
}

void PlaylistItem::setAlbum(const QString& album)
{
  this->setText(ALBUM_COLUMN, album);
}

void PlaylistItem::setTrack(const QString& track)
{
  this->setText(TRACK_COLUMN, track);
}

void PlaylistItem::setYear(const QString& year)
{
  m_year = year;
}

void PlaylistItem::setGenre(const QString& genre)
{
  m_genre = genre;
}

void PlaylistItem::setLength(const QString& length)
{
  this->setText(LENGTH_COLUMN, length);
}

void PlaylistItem::setSubtitles(const QStringList& subs)
{
  m_subtitles = subs;
}

void PlaylistItem::addSubtitle(const QString& sub)
{
  /* This will add subtitle & set it to the current one */
  for (uint i=0; i < m_subtitles.count(); i++) {
    if (m_subtitles[i] == sub) {
      m_currentSubtitle = i;
      return;
    }
  }
  m_subtitles.append(sub);
  m_currentSubtitle = m_subtitles.count()-1;
}

void PlaylistItem::setCurrentSubtitle(int sub)
{
  m_currentSubtitle = sub;
}

QTime PlaylistItem::stringToTime(const QString& timeString)
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

int PlaylistItem::compare(QListViewItem *i, int col, bool ascending ) const
{
   PlaylistItem* p = dynamic_cast<PlaylistItem*>(i);
   if (!p)
     return QListViewItem::compare(i, col, ascending);

   //if both strings are empty, sort by filename
   if (text(col).isEmpty() && i->text(col).isEmpty())
     return -url().compare(p->url());

   bool ok;
   int track1, track2 = 0;
   track1 = track().toInt(&ok);
   if (ok)
     track2 = p->track().toInt(&ok);

   if (col == TRACK_COLUMN)
   {
      if (ok)
        return (track2 - track1);
       else
        return QListViewItem::compare(i, col, ascending);
   }
   else
   {
      int result =  QListViewItem::compare(i, col, ascending);

      //if artists are equal, compare albums
      if ((col == ARTIST_COLUMN) && (result == 0))
      {
        result = QListViewItem::compare(i, ALBUM_COLUMN, ascending);
	if (!ascending)
	  result = -result;
      }

      //if strings are equal, sort by track number or filename
      if (result == 0)
      {
         int diff;
	 if (ok)
	   diff = track1 - track2;
	  else
           diff = url().compare(p->url());

	 if (ascending)
           return diff;
          else
           return -diff;
      }
      else
        return result;
   }
}

void PlaylistItem::setPlaying(bool playing)
{
	isCurrent = playing;
}

void PlaylistItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
	if (isCurrent)
	{
		QColorGroup colorGroup = cg;

		QColor base = colorGroup.base();
		QColor selection = colorGroup.highlight();

		int r = (base.red() + selection.red()) / 2;
		int b = (base.blue() + selection.blue()) / 2;
		int g = (base.green() + selection.green()) / 2;

		QColor c(r, g, b);

		colorGroup.setColor(QColorGroup::Base, c);
		QListViewItem::paintCell(p, colorGroup, column, width, align);
	}
	else
		return KListViewItem::paintCell(p, cg, column, width, align);
}
