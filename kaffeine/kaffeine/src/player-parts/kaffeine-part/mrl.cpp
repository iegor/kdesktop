/*
 * mrl.cpp
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

#include "mrl.h"

MRL::MRL()
{
  reset();
}

MRL::MRL(const KURL& url, const QString& title, const QTime& length, const QString& mime,
     const QString& artist, const QString& album, const QString& track,
     const QString& year, const QString& genre, const QString& comment,
     const QStringList& subtitleFiles, const int currentSubtitle) :
     m_url(url.prettyURL()), m_kurl(url), m_title(title), m_artist(artist), m_album(album), m_track(track), m_year(year), m_genre(genre), m_comment(comment),
     m_mime(mime), m_length(length), m_subtitleFiles(subtitleFiles), m_currentSubtitle(currentSubtitle)
{}

MRL::MRL(const QString& url, const QString& title, const QTime& length, const QString& mime,
     const QString& artist, const QString& album, const QString& track,
     const QString& year, const QString& genre, const QString& comment,
     const QStringList& subtitleFiles, const int currentSubtitle) :
     m_url(url), m_kurl(KURL::fromPathOrURL(url)), m_title(title), m_artist(artist),
     m_album(album), m_track(track), m_year(year), m_genre(genre), m_comment(comment),
     m_mime(mime), m_length(length), m_subtitleFiles(subtitleFiles), m_currentSubtitle(currentSubtitle)
{}

MRL::~MRL()
{
}
