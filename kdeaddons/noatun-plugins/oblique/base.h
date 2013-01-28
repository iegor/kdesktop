// Copyright (c) 2003,2004 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.
#ifndef BASE_H
#define BASE_H

// ;)
#define unless(e) if(!(e))

#include <qstring.h>
#include <qobject.h>
#include <qptrlist.h>

class File;
class Slice;

typedef unsigned int FileId;

class Base : public QObject
{
Q_OBJECT

	struct Private;
	Private *d; // not for BC, but for compile times :)
	friend class Slice;
	friend class File;
	unsigned int mFormatVersion;

public:
	Base(const QString &file);
	~Base();

	File add(const QString &file);

	File find(FileId id);

	void clear();

	/**
	 * get the highest FileID
	 **/
	FileId high() const;

	/**
	 * @return first item after the given id (inclusive)
	 **/
	File first(FileId id=1);

	QString property(FileId id, const QString &property) const;
	void setProperty(FileId id, const QString &key, const QString &value);
	QStringList properties(FileId id) const;
	void clearProperty(FileId, const QString &key);

	/**
	 * same as File::remove
	 **/
	void remove(File f);

	/**
	 * change the id of a file
	 **/
	void move(FileId oldid, FileId newid);

	void dump();

	QPtrList<Slice> slices();
	Slice *addSlice(const QString &name);
	Slice *defaultSlice();
	Slice *sliceById(int id);

	unsigned int formatVersion() const { return mFormatVersion; }
	void resetFormatVersion();

public slots:
	void notifyChanged(const File &file);

signals:
	void added(File file);
	void removed(File file);
	void modified(File file);

	void addedTo(Slice *slice, File file);
	void removedFrom(Slice *slice, File file);

	/**
	 * emitted when something of the slices gets modified
	 * @ref Slice calls this itself via a friendship
	 **/
	void slicesModified();

private:
	void loadIntoCache(FileId id) const;

private: // friends for Slice
	void removeSlice(Slice *slice);

private:
	/**
	 * load the xml that lives at the head of the db and contains
	 * potentially lots of structured data
	 **/
	void loadMetaXML(const QString &xml);
	QString saveMetaXML();
};

#endif
