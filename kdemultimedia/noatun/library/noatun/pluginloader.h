#ifndef PLUGIN_LOADER_H
#define PLUGIN_LOADER_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <noatun/app.h>

#include <klibloader.h>
#include <qdict.h>
#include <kdemacros.h>

struct NoatunLibraryInfo
{
	QString specfile;
	QString filename;
	QString author;
	QString license;
	QString type;
	QString site;
	QString email;
	QString name;
	QString comment;
	QStringList require;
};

bool operator ==(const NoatunLibraryInfo &, const NoatunLibraryInfo &);

class Playlist;
class Visualization;
class Plugin;

/**
 * Used for loading plugins at runtime
 **/
class KDE_EXPORT LibraryLoader
{
	friend class Plugin;
	struct PluginLibrary
	{
		Plugin *plugin;
		KLibrary *library;
	};

public:
	LibraryLoader();
	~LibraryLoader();

	QValueList<NoatunLibraryInfo> available() const;
	QValueList<NoatunLibraryInfo> loaded() const;
	QValueList<NoatunLibraryInfo> loadedByType(const QString &type) const;

	/**
	 * loads all the enabled plugins
	 **/
	bool loadAll(void);
	bool loadAll(const QStringList &);

	bool isLoaded(const QString &spec) const;
	void add(const QString &spec);
	void setModules(const QStringList &mods);
	/**
	 * unload the plugin specified by spec
	 **/
	bool remove(const QString &spec);
	/**
	 * Same as the above, but does not call kapp->exit() even
	 * when the last userinterface plugin is removed. Necessary
	 * during session management (see marquis plugin).
	 * ### BIC: merge with above with terminateOnLastUI = true
	 */
	bool remove(const QString &spec, bool terminateOnLastUI);
	/**
	 * unload the plugin that is referenced by @par plugin
	 **/
	bool remove(const LibraryLoader::PluginLibrary *plugin);
	bool remove(const Plugin *plugin);

	Playlist *playlist() const;

	/**
	 * This is needed for the Plugin-List-View
	 * to see what plugins are required to show
	 * (when required by another noatun-plugin)
	 **/
	NoatunLibraryInfo getInfo(const QString &spec) const;
	QPtrList<Plugin> plugins() const;

private:
	bool loadSO(const QString &spec);
	void removeNow(const QString &spec);

private:
	QDict<LibraryLoader::PluginLibrary> mLibHash;
	Playlist *mPlaylist;
};

#endif

