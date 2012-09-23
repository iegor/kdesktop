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

#include <qxml.h>
#include <qfile.h>
#include <qimage.h>
#include <qbuffer.h>

#include <kurl.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kfilterdev.h>
#include <kio/netaccess.h>

#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGImageElementImpl.h"

#include "KSVGLoader.moc"

using namespace KSVG;

KSVGLoader::KSVGLoader() : m_data(0)
{
	m_job = 0;
}

KSVGLoader::~KSVGLoader()
{
}

QString KSVGLoader::loadXML(::KURL url)
{
	QString tmpFile;
	if(KIO::NetAccess::download(url, tmpFile, 0))
	{
		QIODevice *dev = KFilterDev::deviceForFile(tmpFile, "application/x-gzip", true);
		QByteArray contents;
		if(dev->open(IO_ReadOnly))
			contents = dev->readAll();
		delete dev;
		KIO::NetAccess::removeTempFile(tmpFile);
		return QString(contents);
	}

	return QString::null;
}

void KSVGLoader::getSVGContent(::KURL url)
{
	if(!url.prettyURL().isEmpty())
	{
		if(m_job == 0)
			m_job = KIO::get(url, false, false);

		m_job->setAutoErrorHandlingEnabled(true);

		connect(m_job, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(slotData(KIO::Job *, const QByteArray &)));
		connect(m_job, SIGNAL(result(KIO::Job *)), this, SLOT(slotResult(KIO::Job *)));
	}
}

void KSVGLoader::newImageJob(SVGImageElementImpl *image, ::KURL baseURL)
{
	if(image && image->fileName().isEmpty())
	{
		kdDebug(26001) << "Image Element has no URL!" << endl;
		return;
	}

	ImageStreamMap *map = new ImageStreamMap();
	map->data = new QByteArray();
	map->imageElement = image;

	KIO::TransferJob *imageJob = KIO::get(::KURL(baseURL, map->imageElement->fileName()), false, false);
	connect(imageJob, SIGNAL(data(KIO::Job *, const QByteArray &)), this, SLOT(slotData(KIO::Job *, const QByteArray &)));
	connect(imageJob, SIGNAL(result(KIO::Job *)), this, SLOT(slotResult(KIO::Job *)));

	m_imageJobs.insert(imageJob, map);
}

void KSVGLoader::slotData(KIO::Job *job, const QByteArray &data)
{
	if(job == m_job)
	{
		QDataStream dataStream(m_data, IO_WriteOnly | IO_Append);
		dataStream.writeRawBytes(data.data(), data.size());
	}
	else
	{
		QMap<KIO::TransferJob *, ImageStreamMap *>::Iterator it;
		for(it = m_imageJobs.begin(); it != m_imageJobs.end(); ++it)
		{
			if(it.key() == job)
			{
				QDataStream dataStream(*(it.data())->data, IO_WriteOnly | IO_Append);
				dataStream.writeRawBytes(data.data(), data.size());
				break;
			}
		}
	}
}

void KSVGLoader::slotResult(KIO::Job *job)
{
	if(job == m_job)
	{
		if(m_job->error() == 0)
		{
			QString check = static_cast<KIO::TransferJob *>(job)->url().prettyURL();
			if(check.contains(".svgz") || check.contains(".svg.gz"))
			{
				// decode the gzipped svg and emit it
				QIODevice *dev = KFilterDev::device(new QBuffer(m_data), "application/x-gzip");
				dev->open(IO_ReadOnly);
				emit gotResult(dev);
			}
			else
			{
				m_job = 0;
				emit gotResult(new QBuffer(m_data));
				m_data.resize(0);
			}
		}
	}
	else if(m_postUrlData.job == job)
	{
		// Notify that we're done
		KJS::List callBackArgs;
		callBackArgs.append(*m_postUrlData.status);

		m_postUrlData.status->put(m_postUrlData.exec, KJS::Identifier("success"), KJS::Boolean(true));
		m_postUrlData.callBackFunction->call(m_postUrlData.exec, *m_postUrlData.callBackFunction, callBackArgs);
	}
	else
	{
		QMap<KIO::TransferJob *, ImageStreamMap *>::Iterator it;
		for(it = m_imageJobs.begin(); it != m_imageJobs.end(); ++it)
		{
			if(it.key() == job)
			{
				ImageStreamMap *streamMap = it.data();

				QBuffer buffer(*(streamMap->data));

				if(buffer.open(IO_ReadOnly))
				{
					const char *imageFormat = QImageIO::imageFormat(&buffer);

					if(imageFormat != 0)
					{
						QImageIO imageIO(&buffer, imageFormat);

						// Gamma correction
						imageIO.setGamma(1/0.45454); 

						if(imageIO.read())
						{
							QImage *image = new QImage(imageIO.image());
							image->detach();
							(streamMap->imageElement)->setImage(image);
						}
					}

					buffer.close();
				}

				(streamMap->data)->resize(0);

				m_imageJobs.remove(static_cast<KIO::TransferJob *>(job));

				emit imageReady(streamMap->imageElement);
				break;
			}
		}
	}
}

QString KSVGLoader::getUrl(::KURL url, bool local)
{
	// Security issue: Only retrieve http and https
	if(local || (!url.prettyURL().isEmpty()) && ((url.protocol() == "http") || (url.protocol() == "https")))
		return loadXML(url);

	return QString::null;
}

void KSVGLoader::postUrl(::KURL url, const QByteArray &data, const QString &mimeType,  KJS::ExecState *exec, KJS::Object &callBackFunction, KJS::Object &status)
{
	KIO::TransferJob *job = KIO::http_post(url, data, false);
	job->addMetaData("content-type", mimeType);

	m_postUrlData.job = job;
	m_postUrlData.exec = exec;
	m_postUrlData.status = &status;
	m_postUrlData.callBackFunction = &callBackFunction;

	connect(job, SIGNAL(result(KIO::Job *)), SLOT(slotResult(KIO::Job *)));
}

class CharacterDataSearcher : public QXmlDefaultHandler
{
public:
	CharacterDataSearcher(const QString &id) : m_id(id) { }

	virtual bool startDocument()
	{
		m_foundCount = 0;
		return true;
	}

	virtual bool startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &atts)
	{
		kdDebug(26001) << "CharacterDataSearcher::startElement, qName " << qName << endl;

		int pos = atts.index("id");
		if(pos > -1 && atts.value(pos) == m_id)
		{
			m_foundCount++;
			m_tagFound = qName;
		}

		return true;
	}

	virtual bool endElement(const QString &, const QString &, const QString &qName)
	{
		if(m_tagFound == qName && m_foundCount > 0)
		{
			m_foundCount--;
			if(m_foundCount == 0)
				return false; // done!
		}

		return true;
	}

	virtual bool characters(const QString &ch)
	{
		kdDebug(26001) << "CharacterDataSearcher::characters, read " << ch.latin1() << endl;

		if(m_tagFound != 0)
			m_result += ch;

		return true;
	}		

	QString result() { return m_result; }

private:
	QString m_id, m_result, m_tagFound;
	int m_foundCount;
};

QString KSVGLoader::getCharacterData(::KURL url, const QString &id)
{
	QXmlSimpleReader reader;

	CharacterDataSearcher searcher(id);
	reader.setContentHandler(&searcher);
	reader.setErrorHandler(&searcher);

	QString s = loadXML(url);

	QXmlInputSource source;
	source.setData(s);

	reader.parse(&source);

	return searcher.result();
}



class SVGFragmentSearcher : public QXmlDefaultHandler
{
public:
	SVGFragmentSearcher(SVGDocumentImpl *doc, const QString &id, ::KURL url) : m_id(id), m_url(url), m_doc(doc) { }

	virtual bool startDocument()
	{
		m_result = 0;
		m_currentNode = 0;
		
		return true;
	}

	virtual bool startElement(const QString &namespaceURI, const QString &, const QString &qName, const QXmlAttributes &attrs)
	{
		kdDebug(26001) << "SVGFragmentSearcher::startElement, namespaceURI " << namespaceURI << ", qName " << qName << endl;
		bool parse = m_result;
		if(!parse)
		{
			int pos = attrs.index("id");
			if(pos > -1 && attrs.value(pos) == m_id)
				parse = true;
		}

		if(parse)
		{
			DOM::Element impl = static_cast<DOM::Document *>(m_doc)->createElementNS(namespaceURI, qName);
			SVGElementImpl *newElement = SVGDocumentImpl::createElement(qName, impl, m_doc);
			newElement->setViewportElement(m_doc->rootElement());

			if(m_currentNode)
				m_currentNode->appendChild(*newElement);
			else
				m_result = newElement;

			QXmlAttributes newAttrs;

			for(int i = 0; i < attrs.count(); i++)
			{
				QString name = attrs.localName(i);
				QString value = attrs.value(i);

				if(name == "id")
				{
					value = "@fragment@" + m_url.prettyURL() + "@" + value;
					m_idMap[value] = newElement;
				}
				else
				if(name == "href")
				{
					value.stripWhiteSpace();

					if(value.startsWith("#"))
					{
						value.remove(0, 1);

						// Convert the id to its mangled version.
						QString id = "@fragment@" + m_url.prettyURL() + "@" + value;

						if(m_idMap.contains(id))
						{
							// This is a local reference to an element within the fragment.
							// Just convert the href.
							value = id;
						}
						else
						{
							// This is a local reference to an id outside of the fragment.
							// Change it into an absolute href.
							value = m_url.prettyURL() + "#" + value;
						}
					}
				}

				newAttrs.append(attrs.qName(i), attrs.uri(i), attrs.localName(i), value);
			}

			newElement->setAttributes(newAttrs);
			m_currentNode = newElement;
		}

		return true;
	}

	virtual bool endElement(const QString &, const QString &, const QString &)
	{
		if(m_result)
		{
			m_parentNode = m_currentNode->parentNode();
			
			if(m_parentNode.isNull())
				return false; // done!

			m_currentNode = &m_parentNode;
		}

		return true;
	}

	virtual bool characters(const QString &ch)
	{
		kdDebug(26001) << "SVGFragmentSearcher::characters, read " << ch.latin1() << endl;

		if(m_result)
		{
			SVGElementImpl *element = m_result->ownerDoc()->getElementFromHandle(m_currentNode->handle());
			if(element)
			{
				QString t = ch;

				SVGLangSpaceImpl *langSpace = dynamic_cast<SVGLangSpaceImpl *>(element);
				if(langSpace)
					t = langSpace->handleText(ch);

				if(!t.isEmpty())
				{
					DOM::Text impl = static_cast<DOM::Document *>(m_result->ownerDoc())->createTextNode(t);
					element->appendChild(impl);
				}
			}
		}

		return true;
	}

	SVGElementImpl *result() { return m_result; }

private:
	QString m_id;
	::KURL m_url;

	SVGDocumentImpl *m_doc;
	SVGElementImpl *m_result;

	DOM::Node *m_currentNode, m_parentNode;
	QMap<QString, SVGElementImpl *> m_idMap;
};

SVGElementImpl *KSVGLoader::getSVGFragment(::KURL url, SVGDocumentImpl *doc, const QString &id)
{
	QXmlSimpleReader reader;

	kdDebug(26001) << "getSVGFragment: " << url.prettyURL() << "#" << id << endl;
	SVGFragmentSearcher searcher(doc, id, url);
	reader.setContentHandler(&searcher);
	reader.setErrorHandler(&searcher);

	QString s = loadXML(url);

	QXmlInputSource source;
	source.setData(s);

	reader.parse(&source);

	return searcher.result();
}

// vim:ts=4:noet
