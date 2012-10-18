#ifndef PLAYER_H
#define PLAYER_H

#include <qobject.h>
#include <qtimer.h>
#include <kurl.h>
#include <noatun/playlist.h>
#include <kdemacros.h>
class Engine;
class Playlist;
class KLibrary;

/**
 * This class has slots for all the common media player buttons
 * The slots are called, and it queries the Playlist for the appropriate
 * file.
 *
 * @short Noatun player backend
 * @author Charles Samuels
 * @version 2.4
 **/
class KDE_EXPORT Player : public QObject
{
Q_OBJECT
friend class Effects;
friend class PlaylistItemData;
friend class PlaylistNotifier;

public:
	/**
	 * "None":     Plays the playlist entries sequentially until the
	 *             end of the playlist.
	 * "Song":     Repeats the current playlist entry indefinitely.
	 * "Playlist": Plays the playlist entries sequentially, until
	 *             end of the playlist. Noatun will then restart
	 *             playback at the first song.
	 * "Random":   Plays the entries of the playlist in a random,
	 *             non-repeating order. Playback will continue
	 *             indefinitely.
	 **/
	enum LoopType { None=0, Song, Playlist, Random };

public:
	Player(QObject *parent=0);
	~Player();

	/**
	 * @return a string with the time that can be used in the UI:
	 * CC:CC/LL:LL (mm:ss)
	 **/
	QString lengthString(int _position=-1);
	/**
	 * @return LoopType enum
	 **/
	int loopStyle() const { return mLoopStyle; }
	/**
	 * @return the volume from 0-100
	 * use volume() instead
	 **/
	int volume() const;
	/**
	 * @return the position in milliseconds
	 **/
	int getTime() const { return position; }
	/**
	 * @return the track-length in milliseconds
	 **/
	int getLength();
	/**
	 * @return true if we're playing
	 **/
	bool isPlaying();
	/**
	 * @return true if paused
	 **/
	bool isPaused();
	/**
	 * @return true if stopped
	 **/
	bool isStopped();

	/**
	 * get the current playlist
	 * this may be null
	 * And this may not be necessarily an item allocated
	 * by playlist()
	 **/
	PlaylistItem current() const { return mCurrent;} // TODO: uninline

	/**
	 * loads a file and optionally plays it
	 * @param file the file to load
	 * @param purge true to clear the playlist on open
	 * @param autoplay start playing that file after loading it
	 **/
	void openFile(const KURL &file, bool purge=true, bool autoplay=false);

	/**
	 * loads all given files
	 * @param files list of files to load
	 * @param purge true to clear the playlist on open
	 * @param autoplay if true, play the first added item
	 **/
	void openFile(const KURL::List &files, bool purge=true, bool autoplay=false);

	Engine *engine() const { return mEngine; }

public slots:
	/**
	 * show or hide the playlist
	 **/
	void toggleListView();
	/**
	 * force the playing/paused/stopped/playlist shown signals to
	 * be sent out, also, you can send this if you want to
	 * make all the UIs re-display the current item
	 **/
	void handleButtons();
	/**
	 * remove current from playlist
	 **/
	void removeCurrent();

	/**
	 * go back a track
	 **/
	void back();
	/**
	 * stop playing
	 **/
	void stop();
	/**
	 * start playing
	 **/
	void play();

	/**
	 * play the given file
	 **/
	void play(const PlaylistItem &item);
	/**
	 * start playing, or pause if we're currently playing
	 **/
	void playpause();
	/**
	 * go forward a track
	 **/
	void forward(bool allowLoop = true);

	/**
	 * skip to the position, unit is milliseconds
	 **/
	void skipTo(int msec);

	/**
	 * goes to the next type of looping
	 **/
	void loop();

	/**
	 * set the type of looping
	 **/
	void loop(int i);

	void setVolume(int);

public slots:
	/**
	 * @internal
	 * Play the current file
	 **/
	void playCurrent();
	/**
	 * @internal
	 * load the current file
	 **/
	void newCurrent();

private slots:
	void posTimeout();
	void aboutToPlay();
	void slotUpdateStreamMeta(
		const QString &streamName, const QString &streamGenre,
		const QString &streamUrl, const QString &streamBitrate,
		const QString &trackTitle, const QString &trackUrl
	);

signals:
	/**
	 * Tells you to update the seekbar, volume
	 * and title.
	 **/
	void timeout();

	void stopped();

	void playing();

	void paused();

	/**
	 * when the type of looping is
	 * changed
	 **/
	void loopTypeChange(int t);

	/**
	 * the playlist is made visible
	 **/
	void playlistShown();

	/**
	 * the playlist is hidden
	 **/
	void playlistHidden();

	/**
	 * called at the same time as newSong, but
	 * maybe easier to work with
	 **/
	void newSongLen(int mins, int sec);

	/**
	 * when a new song is currently playing
	 **/
	void newSong();

	/**
	 * Called when a new song is about to be played, but
	 * hasn't started.  player->current() is the
	 * next song
	 **/
	void changed();

	/**
	 * called when we're about to load item, but it hasn't been yet
	 *
	 * this is used for implementing new protocols
	 **/
	void aboutToOpen(PlaylistItem item);

	void volumeChanged(int);

	/**
	 * this signal is emitted when the user (or a plugin) seeks 
	 * the song with @sa skipTo
	 **/
	void skipped();

	/**
	 * this signal is emitted when the user (or a plugin) seeks 
	 * the song with @sa skipTo
	 *
	 * @param msec is the position in the song in milliseconds 
	 **/
	void skipped(int msec);

private:
	Engine *mEngine;
	QTimer filePos;
	int position;
	int mLoopStyle;
	bool firstTimeout;
	PlaylistItem mCurrent; // TODO eliminate
	QPtrList<PlaylistNotifier> mNotifiers;
};


#endif

