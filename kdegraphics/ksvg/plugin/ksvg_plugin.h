/*
    Copyright (C) 2001-2003 KSVG Team
	This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KSVGPlugin_H
#define KSVGPlugin_H

#include <qobject.h>
#include <kparts/part.h>

namespace KSVG
{
	class SVGDocument;
	class SVGDocumentImpl;
	class SVGDescElementImpl;
	class SVGTitleElementImpl;
}

class KSVGPlugin : public KParts::ReadOnlyPart
{
Q_OBJECT
public:
	KSVGPlugin(QWidget *parentWidget, const char *wname, QObject *parent, const char *name, unsigned int width = 0, unsigned int height = 0);
	virtual ~KSVGPlugin();

	virtual bool openFile() { return false; }
	virtual bool openURL(const KURL &url);

	KSVG::SVGDocument document();
	KSVG::SVGDocumentImpl *docImpl();

	void reset();
	void setPanPoint(const QPoint &translate);
	void update();
	void setPopupActive(bool);

signals:
	void gotHyperlink(const QString &);
	void gotHyperlinkCoordinate(const QRect &);

public slots:
	void slotRedraw(const QRect &);

private slots:
	void browseURL(const QString &url);
	void slotStop();
	void slotViewSource();
	void slotViewMemory();
	void slotFontKerning();
	void slotProgressiveRendering();
	void slotRenderingBackend();
	void slotZoomIn();
	void slotZoomOut();
	void slotZoomReset();
	void slotAboutKSVG();
	void slotSaveToPNG();
  
	void slotParsingFinished(bool error, const QString &errorDesc);
	void slotRenderingFinished();
	void slotSetDescription(const QString &);
	void slotSetTitle(const QString &);
	void slotGotURL(const QString &);

private:
	class Private;
	Private *ksvgd;
};

#endif

// vim:ts=4:noet
