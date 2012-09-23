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

#ifndef KSVGLoader_H
#define KSVGLoader_H

#include <qobject.h>

class KURL;

namespace KIO
{
	class Job;
	class TransferJob;
}

namespace KJS
{
	class Object;
	class ExecState;
}

namespace KSVG
{

struct ImageStreamMap;

typedef struct
{
	KIO::Job *job;
	KJS::ExecState *exec;
	KJS::Object *callBackFunction, *status;
} PostUrlData;
	
class SVGImageElementImpl;
class SVGElementImpl;
class SVGDocumentImpl;
class KSVGLoader : public QObject
{
Q_OBJECT
public:
	KSVGLoader();
	~KSVGLoader();

	void getSVGContent(::KURL url);
	void newImageJob(SVGImageElementImpl *impl, ::KURL url);

	static QString getUrl(::KURL url, bool local = false);
	void postUrl(::KURL url, const QByteArray &data, const QString &mimeType, KJS::ExecState *exec, KJS::Object &callBackFunction, KJS::Object &status);
	static QString getCharacterData(::KURL url, const QString &id);
	static SVGElementImpl *getSVGFragment(::KURL, SVGDocumentImpl *doc, const QString &id);

signals:
	void gotResult(QIODevice *);
	void imageReady(SVGImageElementImpl *);

private slots:
	void slotData(KIO::Job *, const QByteArray &);
	void slotResult(KIO::Job *);

private:
	static QString loadXML(::KURL);

	PostUrlData m_postUrlData;

	QByteArray m_data;
	KIO::TransferJob *m_job;
	QMap<KIO::TransferJob *, ImageStreamMap *> m_imageJobs;
};

}

#endif

// vim:ts=4:noet
