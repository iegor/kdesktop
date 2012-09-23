/*
 * googlefetcherdialog.h
 *
 * Copyright (C) 2004 Nathan Toone <nathan@toonetown.com>
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

#ifndef GOOGLEFETCHERDIALOG_H
#define GOOGLEFETCHERDIALOG_H

#include <kiconview.h>
#include <kio/job.h>

#include "googlefetcher.h"

class KURL;

class GoogleFetcherDialog : public KDialogBase
{
    Q_OBJECT

public:
    GoogleFetcherDialog(const QString &name,
                        const GoogleImageList &urlList,
                        const QString &artist,
                        const QString &album,
                        QWidget *parent = 0);

    virtual ~GoogleFetcherDialog();

    QPixmap result() const { return m_pixmap; }
    bool takeIt() const { return m_takeIt; }
    bool newSearch() const { return m_newSearch; }

    void setLayout();
    void setImageList(const GoogleImageList &urlList);

public slots:
    int exec();
    void refreshScreen(GoogleImageList &list);

signals:
    void sizeChanged(GoogleFetcher::ImageSize);

protected slots:
    void slotOk();
    void slotCancel();
    void slotUser1();
    void imgSizeChanged(int index);

private:
    QPixmap fetchedImage(uint index) const;
    QPixmap pixmapFromURL(const KURL &url) const;

    QPixmap m_pixmap;
    GoogleImageList m_imageList;
    KIconView *m_iconWidget;
    bool m_takeIt;
    bool m_newSearch;
    QString m_artist, m_album;
};

namespace KIO
{
    class TransferJob;
}

class CoverIconViewItem : public QObject, public KIconViewItem
{
    Q_OBJECT

public:
    CoverIconViewItem(QIconView *parent, const GoogleImage &image);
    ~CoverIconViewItem();

private slots:
    void imageData(KIO::Job *job, const QByteArray &data);
    void imageResult(KIO::Job* job);

private:
    QByteArray m_buffer;
    QGuardedPtr<KIO::TransferJob> m_job;
};

#endif /* GOOGLEFETCHERDIALOG_H */
