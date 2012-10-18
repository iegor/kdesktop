#ifndef DCOPIFACE_H
#define DCOPIFACE_H

#include <noatun/player.h>
#include <noatun/plugin.h>

#include <dcopobject.h>
#include <kdemacros.h>

class KDE_EXPORT NIF : public Plugin, public DCOPObject
{
K_DCOP

public:
	NIF();
	~NIF();

private slots:
	void newSongPlaying();

k_dcop:
	void toggleListView();
	void handleButtons();
	void removeCurrent();

	void back();
	void stop();
	void play();
	void playpause();
	void forward();
	void skipTo(int);
	void loop();

	void setVolume(int);
	int volume();
	void volumeUp();
	void volumeDown();
	void toggleMute();

	/**
	 * length in milliseconds
	 **/
	int length();
	/**
	 * position in milliseconds
	 **/
	int position();

	/**
	 * 0 stopped
	 * 1 paused
	 * 2 playing
	 **/
	int state();

	QString lengthString();
	QString timeString();

	QString title();

	/**
	 * set a property for the current song
	 **/
	void setCurrentProperty(const QString &key, const QString &value);
	/**
	 * get a property from the current song
	 **/
	QString currentProperty(const QString &key);
	/**
	 * clear a property from the current song
	 **/
	void clearCurrentProperty(const QString &key);

	QCString visStack();
	QCString session();

	void addFile(const QString& f, bool autoplay);
	void addFile(const QStringList &f, bool autoplay);

	void loadPlugin(const QString &specFile);
	QStringList availablePlugins();
	QStringList loadedPlugins();
	bool unloadPlugin(const QString &specFile);

	QStringList mimeTypes();
	QCString version();

	/**
	 * clear the playlist
	 **/
	void clear();
private:
	int mLastVolume; // remember volume for mute/unmute

#ifdef DOCUMENTATION_BLEH_BLEH_DONT_TRY_COMPILING_THIS
signals:
	void exiting();
	void newFile();

#endif
};

#endif

