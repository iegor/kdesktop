/* This file is part of the KDE project

Copyright (C) 2001, 2003 Lukas Tinkl <lukas@kde.org>
Andreas Schlapbach <schlpbch@iam.unibe.ch>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License version 2 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/

#ifndef kimgalleryplugin_h
#define kimgalleryplugin_h

#include <kparts/plugin.h>
#include <klibloader.h>
#include <konq_dirpart.h>
#include <kio/jobclasses.h>

class QProgressDialog;
class KURL;
class KIGPDialog;

typedef QMap<QString,QString> CommentMap;

class KImGalleryPlugin : public KParts::Plugin
{
  Q_OBJECT
    public:
  KImGalleryPlugin( QObject* parent, const char* name,
	            const QStringList & );
  ~KImGalleryPlugin() {}

  public slots:
    void slotExecute();
  void slotCancelled();

 private:
  bool m_cancelled;
  bool m_recurseSubDirectories;
  bool m_copyFiles;
  bool m_useCommentFile;

  int m_imgWidth;
  int m_imgHeight;
  int m_imagesPerRow;

  QProgressDialog *m_progressDlg;

  KonqDirPart* m_part;

  KIGPDialog *m_configDlg;

  CommentMap* m_commentMap;

  bool createDirectory(QDir thumb_dir, QString imgGalleryDir, QString dirName);

  void createHead(QTextStream& stream);
  void createCSSSection(QTextStream& stream);
  void createBody(QTextStream& stream, const QString& sourceDirName, const QStringList& subDirList, const QDir& imageDir, const KURL& url, const QString& imageFormat);

  bool createThumb( const QString& imgName, const QString& sourceDirName, const QString& imgGalleryDir, const QString& imageFormat);

  bool createHtml( const KURL& url, const QString& sourceDirName, int recursionLevel, const QString& imageFormat);
  void deleteCancelledGallery( const KURL& url, const QString& sourceDirName, int recursionLevel, const QString& imageFormat);
  void loadCommentFile();

  static QString extension(const QString& imageFormat);
};

#endif
