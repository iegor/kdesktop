/*
   Copyright (C) 2003 Antonio Larrosa <larrosa@kde.org>

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

#ifndef _ARCHIVEDIALOG_H_
#define _ARCHIVEDIALOG_H_

#include <dom/dom_core.h>
#include <dom/html_document.h>

#include <kdialogbase.h>
#include <ktempfile.h>
#include <kio/job.h>

#include <qstring.h>
#include <qmap.h>
#include <qvaluelist.h>

class QWidget;
class KHTMLPart;
class ArchiveViewBase;
class KURL;
class KTar;
class QTextStream;
class QListViewItem;

class ArchiveDialog : public KDialogBase
{
   Q_OBJECT
public:
   ArchiveDialog(QWidget *parent, const QString &targetFilename, KHTMLPart *part);
   ~ArchiveDialog();

   void archive();

public slots:
   void finishedDownloadingURL( KIO::Job *job );
   void setSavingState();
protected:
   void saveFile( const QString& fileName);
   void saveToArchive(QTextStream* _textStream);
   void saveArchiveRecursive(const DOM::Node &node, const KURL& baseURL,
                             QTextStream* _textStream, int ident);
   QString handleLink(const KURL& _url, const QString & _link);
   KURL getAbsoluteURL(const KURL& _url, const QString& _link);
   QString getUniqueFileName(const QString& fileName);
   QString stringToHTML(const QString& string);
   QString analyzeInternalCSS(const KURL& _url, const QString& string);
   void downloadNext();

   ArchiveViewBase *m_widget;
   QMap<QString, QString> m_downloadedURLDict;
   QMap<QString, QString> m_linkDict;
   KTar* m_tarBall;
   bool m_bPreserveWS;
   QListViewItem *m_currentLVI;
   unsigned int m_iterator;
   enum State { Retrieving=0, Downloading, Saving };
   State m_state;
   QValueList <KURL>m_urlsToDownload;
   KTempFile *m_tmpFile;
   KURL m_url;
   DOM::Document m_document;
   
};

#endif // _ARCHIVEDIALOG_H_
