#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H

#include <kurl.h>
#include <qobject.h>
#include <qptrlist.h>

class QFile;
class QTimer;
class Downloader;


namespace KIO
{
	class TransferJob;
	class Job;
}

/**
 * Item to download, usually queued up in Downloader
 **/
class DownloadItem
{
	friend class Downloader;
public:
	DownloadItem();
	virtual ~DownloadItem();

	bool isDownloaded() const;

	/**
	 * @return the local filename this item will be saved to
	 **/
	QString localFilename() const;

	virtual void setLocalFilename(const QString &filename);

	/**
	 * Called if this item has been fully downloaded
	 **/
	virtual void downloadFinished();
	/**
	 * Called at regular intervals while downloading this item
	 **/
	virtual void downloaded(int percent);
	/**
	 * Called when downloading this item timed out
	 **/
	virtual void downloadTimeout();
	/**
	 * @return true if the download was scheduled, false if the file is local
	 **/
	bool enqueue(const KURL &url);
	/**
	 * Remove this item from Downloader queue
	 **/
	void dequeue();

private:
	QString mLocalFilename;
};

/**
 * download playlistitems, in a queue based fasion
 **/
class Downloader : public QObject
{
Q_OBJECT
	struct QueueItem
	{
		DownloadItem *notifier;
		KURL file;
		QString local;
	};

public:
	Downloader(QObject *parent=0);
	virtual ~Downloader();

public slots:
	QString enqueue(DownloadItem *notifier, const KURL &file);
	void dequeue(DownloadItem *notifier);

	/**
	 * @internal
	 **/
	void start();

signals:
	/**
	 * Emitted when a new DownloadItem was added to the queue
	 * @p notifier is the added item
	 **/
	void enqueued(DownloadItem *notifier, const KURL &file);
	/**
	 * Emitted when a DownloadItem was removed from the queue
	 * @p notifier is the removed item
	 **/
	void dequeued(DownloadItem *notifier);

private slots:
	void getNext();

	void data( KIO::Job *, const QByteArray &data);
	void percent( KIO::Job *, unsigned long percent);
	void jobDone( KIO::Job *);
	void giveUpWithThisDownloadServerIsRunningNT();

private:
	QPtrList<Downloader::QueueItem> mQueue;
	QPtrList<Downloader::QueueItem> *mUnstartedQueue;
	QFile *localfile;
	Downloader::QueueItem *current;
	KIO::TransferJob *mJob;
	QTimer *mTimeout;
	bool mStarted;
};

#endif

