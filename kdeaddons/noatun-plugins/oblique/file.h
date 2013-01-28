// Copyright (c) 2003,2004 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef FILE_H
#define FILE_H

#include "base.h"
#include <kurl.h>


class Slice;
class Query;

/**
 * just an file from the list in the database
 **/
class File
{

	friend class Base;
	File(Base *base, FileId id);

	mutable Base *mBase;
	FileId mId;

public:
	/**
	 * create a copy of the reference
	 **/
	File(const File &ref);
	/**
	 * create a null reference
	 **/
	File();

	/**
	 * make me a copy of the reference
	 **/
	File &operator=(const File &ref);

	bool operator ==(const File &other) { return mId == other.mId; }

	inline operator bool() const { return mId; }

	QString file() const;
	KURL url() const;
	QString property(const QString &property) const;
	void setProperty(const QString &key, const QString &value);
	void clearProperty(const QString &key);
	QStringList properties() const;
	inline FileId id() const { return mId; }
	void setId(FileId id);

	inline Base *base() { return mBase; }
	inline const Base *base() const { return mBase; }
	
	/**
	 * when displaying me in @p query, place
	 * me immediately after @p after
	 **/
	void setPosition(Query *query, const File &after);
	
	/**
	 * when displaying @p query, set @p after according
	 * to what @ref setPosition was given.
	 *
	 * @return false if no position was set
	 **/
	bool getPosition(const Query *query, File *after) const;

	/**
	 * remove this file from the db, and emit Base::removed(File)
	 * the File objects don't change, but become invalid (careful!)
	 **/
	void remove();

	void addTo(Slice *slice);
	void removeFrom(Slice *slice);
	bool isIn(const Slice *slice) const;

	/**
	 * load the tag information into the DB
	 **/
	void makeCache();
};

class Slice
{
	int mId;
	QString mName;
	Base *mBase;

public:
	/**
	 * @internal
	 * create a slice based on its data
	 **/
	Slice(Base *base, int id, const QString &name);

	QString name() const;
	void setName(const QString &name);

	/**
	 * remove this slice from the list of slices
	 * and dereference all Files from this slice
	 **/
	void remove();

	int id() const { return mId; }
};




#endif
