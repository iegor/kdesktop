#include "player.h"

#include <noatun/playlist.h>
#include "engine.h"
#include "app.h"
#include "titleproxy.h"

#include <klibloader.h>
#include <knotifyclient.h>
#include <klocale.h>
#include <qfile.h>

enum ArtsPOS { posIdle=0, posPlaying, posPaused };



Player::Player(QObject *parent) : QObject(parent, "Player"),
	position(-1), mLoopStyle(None), firstTimeout(true)
{
	mEngine=new Engine;
	connect(&filePos, SIGNAL(timeout()), SLOT(posTimeout()));
	connect(mEngine, SIGNAL(aboutToPlay()), this, SLOT(aboutToPlay()));
	connect(mEngine,
		SIGNAL(receivedStreamMeta(const QString &, const QString &,
			const QString &, const QString &,
			const QString &, const QString &)),
		this, SLOT(
			slotUpdateStreamMeta(const QString &, const QString &,
				const QString &, const QString &,
				const QString &, const QString &))
	);
	connect(mEngine, SIGNAL(playingFailed()), this, SLOT(forward()));

	handleButtons();
}

Player::~Player()
{
	delete mEngine;
}

bool Player::isPlaying()
{
	return mEngine->state()==posPlaying;
}

bool Player::isPaused()
{
	return mEngine->state()==posPaused;
}

bool Player::isStopped()
{
	return mEngine->state()==posIdle;
}

void Player::toggleListView()
{
	napp->playlist()->toggleList();
}

void Player::handleButtons()
{
	switch (mEngine->state())
	{
	case (posPlaying):
		emit playing();
		break;
	case (posPaused):
		emit paused();
		break;
	case (posIdle):
		emit stopped();
	}
}

void Player::back()
{
	if (napp->playlist()->previous())
	{
		stop();
		play();
	}
}

void Player::stop()
{
	filePos.stop();
	position=0;
	mEngine->stop();
	emit stopped();
	mCurrent=0;
}

void Player::play()
{
	napp->processEvents();
	bool work=false;
	firstTimeout=true;

	if (mEngine->state()==posPlaying) // do nothing if already playing
		return;

	if (mEngine->state()==posPaused)
	{
		work=mEngine->play();
	}
	else
	{
		stop();
		mCurrent = napp->playlist()->current();
		if (!mCurrent)
		{
			work=false;
		}
		else
		{
			mEngine->blockSignals(true);
			work=mEngine->open(mCurrent);
			mEngine->blockSignals(false);
		}
	}

	if (!work)
	{
		forward(false);
	}
	else
	{
		filePos.start(500);
		emit changed();
		mEngine->play();
	}

	handleButtons();
}

void Player::play(const PlaylistItem &item)
{
	napp->playlist()->setCurrent(item);
}

void Player::playpause()
{
	if (mEngine->state()==posPlaying)
	{
		filePos.stop();
		mEngine->pause();
//		emit paused(); NOT necessary because emitted in handleButtons()  (mETz)
		handleButtons();
	}
	else
		play();
}

void Player::forward(bool allowLoop)
{
	stop();
	if (napp->playlist()->next())
		play();
	else if (allowLoop && napp->loopList())
		if (napp->playlist()->reset(), napp->playlist()->current())
			play();
}

void Player::skipTo(int msec) // skip to a certain time in the track
{
	if( (current()) && (msec>=0) )
	{
		mEngine->seek(msec);
		position = mEngine->position(); // make sure position is recent
		emit timeout(); // update the UI
		emit skipped(msec);
		emit skipped();
	}
}

void Player::playCurrent()
{
	if (!mEngine->initialized()) return;
	stop();
	mCurrent=0;
	if (napp->playlist()->current())
		play();
}

void Player::newCurrent()
{
	// the second half of the following
	if (!napp->playlist() || !mEngine->initialized())
		return; // no playlist, or squelch playing as an optimization
	if ((mEngine->state()!=posPlaying) && napp->autoPlay())
		playCurrent();
}

void Player::posTimeout()
{
	if (mEngine->state()==posIdle)
	{
		stop();
		handleButtons();
		// If you're supposed to loop the song, don't go next
		// otherwise, do go next
		if (loopStyle()==Song || napp->playlist()->next())
			play();
		else if (loopStyle()==Playlist)
		{
			napp->playlist()->reset();
			play();
		}
		else if (napp->loopList())
			napp->playlist()->reset();

		return;
	}
	position = mEngine->position();

	if (current())
	{
		current().setLength(mEngine->length());
		if (current().length() && firstTimeout)
		{
			int minutes = (int) ( current().length() / 60 );
			int seconds = current().length() - minutes * 60;
			emit newSongLen ( minutes, seconds );
			firstTimeout = false;
			emit newSong();
		}
	}

	emit timeout();
}

QString Player::lengthString(int _position)
{
	if (!current())
		return QString("--:--/--:--");

	QString posString;
	QString lenString;
	int secs, seconds, minutes;

	if (_position < 0)
		_position = position;

	if (_position<0)
	{
		posString="--:--/";
	}
	else
	{ // get the position
		bool remain = napp->displayRemaining() && current();
		if (remain && current().length()<0)
		{
			remain = false;
		}

		if (remain)
		{
			_position = current().length() - _position;
		}

		secs = _position / 1000; // convert milliseconds -> seconds
		seconds = secs % 60;
		minutes = (secs - seconds) / 60;

		// the string has to look like '00:00/00:00'
		posString.sprintf("%.2d:%.2d/", minutes, seconds);
		if (remain) posString.prepend('-');
	}

	if (!current() || current().length()<0)
	{
		posString+="--:--";
	}
	else
	{ // get the length
		secs = current().length() / 1000; // convert milliseconds -> seconds
		seconds = secs % 60;
		minutes = (secs - seconds) / 60;

		// the string has to look like '00:00/00:00'
		lenString.sprintf("%.2d:%.2d", minutes, seconds);
		posString += lenString;
	}

	return posString;
}

int Player::getLength()
{
	if (!current())
		return -1;
	return current().length(); // return track-length in milliseconds
}

void Player::openFile(const KURL &f, bool purge, bool autoplay)
{
	if (purge)
		napp->playlist()->clear();
	napp->playlist()->addFile(f, autoplay);
}

void Player::openFile(const KURL::List &f, bool purge, bool autoplay)
{
	if (purge)
		napp->playlist()->clear();
	for (KURL::List::ConstIterator i(f.begin()); i != f.end(); ++i)
	{
		napp->playlist()->addFile(*i, autoplay);
		autoplay=false;
	}
}


void Player::loop()
{
	mLoopStyle++;
	if (mLoopStyle>Random)
		mLoopStyle=None;
	emit loopTypeChange(mLoopStyle);
}
void Player::loop(int i)
{
	mLoopStyle=i;
	emit loopTypeChange(mLoopStyle);
}

void Player::removeCurrent()
{
	if (napp->playlist()->current())
		napp->playlist()->current().remove();
}

void Player::setVolume(int v)
{
	if (v<0) v=0;
	if (v>100) v=100;
	mEngine->setVolume(v);
	emit timeout();
	emit volumeChanged(v);
}

int Player::volume() const
{
	return mEngine->volume();
}

void Player::aboutToPlay()
{
	emit aboutToOpen( mCurrent );
}

void Player::slotUpdateStreamMeta(
	const QString &streamName, const QString &streamGenre,
	const QString &streamUrl, const QString &streamBitrate,
	const QString &trackTitle, const QString &trackUrl)
{
	PlaylistItem currentItem = napp->playlist()->current();
	if(currentItem)
	{
		currentItem.setProperty("title", trackTitle);
		currentItem.setProperty("bitrate", streamBitrate);

		if(!streamName.isEmpty())
			currentItem.setProperty("author", streamName);
		if(!streamGenre.isEmpty())
			currentItem.setProperty("genre", streamGenre);
		if(!trackUrl.isEmpty())
			currentItem.setProperty("comment", trackUrl);
		else if(!streamUrl.isEmpty())
			currentItem.setProperty("comment", streamUrl);
		else
			currentItem.clearProperty("comment");
		emit changed();
	}
}

#include "player.moc"
