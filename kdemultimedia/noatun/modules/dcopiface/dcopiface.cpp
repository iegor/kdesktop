#include "dcopiface.h"

#include <noatun/player.h>
#include <noatun/app.h>
#include <noatunarts/noatunarts.h>
#include <noatun/engine.h>

#include <dcopclient.h>

extern "C"
{
	KDE_EXPORT NIF *create_plugin()
	{
		return new NIF();
	}
}


NIF::NIF() : Plugin(), DCOPObject("Noatun")
{
	mLastVolume = 0;
//	connect(napp->player(), SIGNAL(newSong()), SLOT(newSongPlaying()));
}

NIF::~NIF()
{
	kapp->dcopClient()->emitDCOPSignal("exiting()", QByteArray());
}

void NIF::toggleListView()
{
	napp->player()->toggleListView();
}

void NIF::handleButtons()
{
	napp->player()->handleButtons();
}

void NIF::removeCurrent()
{
	napp->player()->removeCurrent();
}

void NIF::back()
{
	napp->player()->back();
}

void NIF::stop()
{
	napp->player()->stop();
}

void NIF::play()
{
	napp->player()->play();
}

void NIF::playpause()
{
	napp->player()->playpause();
}

void NIF::forward()
{
	napp->player()->forward();
}

void NIF::skipTo(int msec)
{
	napp->player()->skipTo(msec);
}

void NIF::loop()
{
	napp->player()->loop();
}

void NIF::setVolume(int i)
{
	napp->player()->setVolume(i);
}

int NIF::volume()
{
	return napp->player()->volume();
}

void NIF::volumeUp()
{
	napp->player()->setVolume(napp->player()->volume() + 5);
}

void NIF::volumeDown()
{
	napp->player()->setVolume(napp->player()->volume() - 5);
}

void NIF::toggleMute()
{
	int currVol = napp->player()->volume();
	if (currVol == 0)
	{
		napp->player()->setVolume(mLastVolume);
	}
	else
	{
		mLastVolume = currVol;
		napp->player()->setVolume(0);
	}
}

int NIF::length() // returns -1 if there's no playobject
{
	return napp->player()->getLength();
}

int NIF::position() // returns -1 if there's no playobject
{
	return napp->player()->getTime();
}

int NIF::state()
{
	if (napp->player()->isPlaying())
		return 2;
	if (napp->player()->isPaused())
		return 1;

	return 0; // default to stopped
}

QString NIF::lengthString()
{
	return napp->player()->current() ? napp->player()->current().lengthString() : "";
}

QString NIF::timeString()
{
	return napp->player()->lengthString();
}

QString NIF::title()
{
	return napp->player()->current() ? napp->player()->current().title() : "";
}

void NIF::setCurrentProperty(const QString &key, const QString &value)
{
	if (!napp->player()->current()) return;

	napp->player()->current().setProperty(key, value);
}

QString NIF::currentProperty(const QString &key)
{
	if (!napp->player()->current()) return "";

	return napp->player()->current().property(key);
}

void NIF::clearCurrentProperty(const QString &key)
{
	if (!napp->player()->current()) return;

	return napp->player()->current().clearProperty(key);
}


QCString NIF::visStack()
{
	return napp->player()->engine()->visualizationStack()->toString().c_str();
}

QCString NIF::session()
{
	return napp->player()->engine()->session()->toString().c_str();
}

// adds one file to the playlist
void NIF::addFile(const QString& f, bool autoplay)
{
	napp->player()->openFile(f, false, autoplay);
}

// Adds a bunch of files to the playlist
void NIF::addFile(const QStringList &f, bool autoplay)
{
	for (QStringList::ConstIterator it = f.begin(); it != f.end(); ++it )
		napp->player()->openFile(*it, false, autoplay);
}

void NIF::loadPlugin(const QString &spec)
{
	napp->libraryLoader()->add(spec);
}

QStringList NIF::availablePlugins() {
	QStringList available_spec_files;
        QValueList<NoatunLibraryInfo> available;

	available = napp->libraryLoader()->available();

	QValueList<NoatunLibraryInfo>::iterator it;
	for (it = available.begin();it != available.end();it++) {
		available_spec_files += (*it).specfile;
	}

	return available_spec_files;
}

QStringList NIF::loadedPlugins() {
	QStringList loaded_spec_files;
        QValueList<NoatunLibraryInfo> loaded;

	loaded = napp->libraryLoader()->loaded();

	QValueList<NoatunLibraryInfo>::iterator it;
	for (it = loaded.begin();it != loaded.end();it++) {
		loaded_spec_files += (*it).specfile;
	}

	return loaded_spec_files;
}

bool NIF::unloadPlugin(const QString &spec)
{
	return napp->libraryLoader()->remove(spec);
}

QStringList NIF::mimeTypes()
{
	return napp->mimeTypes();
}

QCString NIF::version()
{
	return napp->version();
}

void NIF::newSongPlaying()
{
	kapp->dcopClient()->emitDCOPSignal("newFile()", QByteArray());
}

void NIF::clear()
{
	napp->playlist()->clear();
}
