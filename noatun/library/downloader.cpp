#include <noatun/downloader.h>
#include <noatun/app.h>
#include <assert.h>
#include <qfile.h>
#include <qtimer.h>
#include <kio/job.h>
#include <klocale.h>

DownloadItem::DownloadItem()
{

}

DownloadItem::~DownloadItem()
{
	dequeue();
}

bool DownloadItem::isDownloaded() const
{
	return true;
}

QString DownloadItem::localFilename() const
{
	return mLocalFilename;
}

void DownloadItem::setLocalFilename(const QString &filename)
{
	mLocalFilename=filename;
}

void DownloadItem::downloadFinished()
{
}

void DownloadItem::downloaded(int )
{
}

void DownloadItem::downloadTimeout()
{
}

bool DownloadItem::enqueue(const KURL &url)
{
	if (url.isLocalFile())
	{
		setLocalFilename(url.path());
		return false;
	}
	else
	{
		napp->downloader()->enqueue(this, url);
		return true;
	}
}

void DownloadItem::dequeue()
{
	napp->downloader()->dequeue(this);
}




Downloader::Downloader(QObject *parent)
	: QObject(parent), localfile(0), current(0), mJob(0), mTimeout(0)
{
	mStarted=false;
	mUnstartedQueue=new QPtrList<Downloader::QueueItem>;
}

Downloader::~Downloader()
{

}

void Downloader::start()
{
	assert(!mStarted);
	mStarted=true;
	if (current)
		emit enqueued(current->notifier, current->file);

	for (QPtrListIterator<Downloader::QueueItem> i(*mUnstartedQueue); i.current(); ++i)
	{
		(*i)->notifier->mLocalFilename = (*i)->local;
		mQueue.append(*i);
		emit enqueued((*i)->notifier, (*i)->file);
	}
	
	delete mUnstartedQueue;
	mUnstartedQueue=0;
	QTimer::singleShot(0, this, SLOT(getNext()));
}

static QString nonExistantFile(const QString &file)
{
	if (file.right(1)=="/") return i18n("Unknown");
	int i=0;
	QString f(file);
	while (QFile(f).exists())
	{
		i++;
		f=file;
		f.insert(f.findRev('.'), '_'+QString::number(i));
	}
	return f;
}

QString Downloader::enqueue(DownloadItem *notifier, const KURL &file)
{
	if (file.isLocalFile()) return 0;
	QueueItem *i=new QueueItem;
	i->notifier=notifier;
	i->file=file;
	
	if (!mStarted)
	{
		i->local=notifier->mLocalFilename;
		if (!notifier->localFilename().length())
		{
			i->local = 
				nonExistantFile(napp->saveDirectory()+'/'+file.fileName());
		}
		mUnstartedQueue->append(i);
		return i->local;
	}
	
	
	if (!notifier->localFilename().length())
	{
		notifier->mLocalFilename=
			i->local =
				nonExistantFile(napp->saveDirectory()+'/'+file.fileName());
	}
	else
	{
		i->local = notifier->mLocalFilename;
	}

	mQueue.append(i);
	QTimer::singleShot(0, this, SLOT(getNext()));
	emit enqueued(notifier, file);
	return i->local;
}

void Downloader::dequeue(DownloadItem *notifier)
{
	if (current && notifier==current->notifier)
	{
		mJob->kill();
		jobDone(mJob);
		return;
	}
	for (QPtrListIterator<Downloader::QueueItem> i(mQueue); i.current(); ++i)
	{
		if ((*i)->notifier==notifier)
		{
			mQueue.removeRef(*i);
			if (mStarted)
				emit dequeued(notifier);
			delete *i;
			return;
		}
	}
}

void Downloader::getNext()
{
	if (current) return;
	if (!mStarted) return;
	if (mQueue.isEmpty()) return;
	current=mQueue.take(0);

	// open the QFile
	localfile=new QFile(current->local);
	localfile->open(IO_ReadWrite | IO_Append);

	mJob= KIO::get(current->file, true, false);
	connect(mJob, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(data(KIO::Job*, const QByteArray&)));
	connect(mJob, SIGNAL(result(KIO::Job*)), SLOT(jobDone(KIO::Job*)));
	connect(mJob, SIGNAL(percent(KIO::Job*, unsigned long)), SLOT(percent(KIO::Job*, unsigned long)));

	if (mTimeout)
		delete mTimeout;
	mTimeout=new QTimer(this);
	mTimeout->start(30000, true);
	connect(mTimeout, SIGNAL(timeout()), SLOT(giveUpWithThisDownloadServerIsRunningNT()));
}

void Downloader::data(KIO::Job *, const QByteArray &data)
{
	localfile->writeBlock(data);
	localfile->flush();
	delete mTimeout;
	mTimeout=0;
}

void Downloader::jobDone(KIO::Job *)
{
	delete mTimeout;
	mTimeout=0;
	current->notifier->downloadFinished();
	if (mStarted)
		emit dequeued(current->notifier);
	delete current;
	current=0;
	mJob=0;
	getNext();
}

void Downloader::giveUpWithThisDownloadServerIsRunningNT()
{
	delete mTimeout;
	mTimeout=0;
	if (!current) return;
	DownloadItem *old=current->notifier;
	if (!old) return;	
	old->downloadTimeout();
	current=0;
	mJob=0;
	delete old;
	getNext();
}

void Downloader::percent( KIO::Job *, unsigned long percent)
{
	if (current && current->notifier)
		current->notifier->downloaded((int)percent);
}

#include "downloader.moc"

