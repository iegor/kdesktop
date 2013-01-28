/*
   Copyright (C) 2001 Malte Starostik <malte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// $Id: webarchivecreator.cpp 626814 2007-01-24 17:16:52Z mueller $

#include <time.h>

#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

#include <kapplication.h>
#include <khtml_part.h>

#include "webarchivecreator.moc"

extern "C"
{
	ThumbCreator * KDE_EXPORT new_creator()
	{
		return new WebArchiveCreator;
	}
}

WebArchiveCreator::WebArchiveCreator()
	: m_html(0)
{
}

WebArchiveCreator::~WebArchiveCreator()
{
	delete m_html;
}

bool WebArchiveCreator::create(const QString &path, int width, int height, QImage &img)
{
	if (!m_html)
	{
		m_html = new KHTMLPart;
		connect(m_html, SIGNAL(completed()), SLOT(slotCompleted()));
		m_html->setJScriptEnabled(false);
		m_html->setJavaEnabled(false);
		m_html->setPluginsEnabled(false);
	}
	KURL url;
	url.setProtocol( "tar" );
	url.setPath( path );
	url.addPath( "index.html" );
	m_html->openURL( url );
	m_completed = false;
	startTimer(5000);
	while (!m_completed)
		kapp->processOneEvent();
	killTimers();

	// render the HTML page on a bigger pixmap and use smoothScale,
	// looks better than directly scaling with the QPainter (malte)
	QPixmap pix;
	if (width > 400 || height > 600)
	{
		if (height * 3 > width * 4)
			pix.resize(width, width * 4 / 3);
		else
			pix.resize(height * 3 / 4, height);
	}
	else
		pix.resize(400, 600);
	// light-grey background, in case loadind the page failed
	pix.fill( QColor( 245, 245, 245 ) );

	int borderX = pix.width() / width,
	borderY = pix.height() / height;
	QRect rc(borderX, borderY, pix.width() - borderX * 2, pix.height() - borderY *
2);

	QPainter p;
	p.begin(&pix);
	m_html->paint(&p, rc);
	p.end();

	img = pix.convertToImage();
	return true;
}

void WebArchiveCreator::timerEvent(QTimerEvent *)
{
	m_html->closeURL();
	m_completed = true;
}

void WebArchiveCreator::slotCompleted()
{
	m_completed = true;
}

ThumbCreator::Flags WebArchiveCreator::flags() const
{
	return DrawFrame;
}

// vim: ts=4 sw=4 et
