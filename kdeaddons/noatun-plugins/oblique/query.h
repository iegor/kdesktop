// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef QUERY_H
#define QUERY_H

#include "base.h"

#include <qregexp.h>
#include <qstring.h>

class Query;
class QDomElement;

class QueryGroup
{
	friend class Query;

	QueryGroup *mFirstChild;
	QueryGroup *mNextSibling;

	int mFuzzyness;
	int mOptions;

	QString mPropertyName;
	QString mPresentation;
	QRegExp mValue;

public:
	QueryGroup();
	QueryGroup(const QueryGroup &copy);
	QueryGroup &operator =(const QueryGroup &copy);


	/**
	 * delete my first child, and my next sibling
	 **/
	~QueryGroup();
	void setFirstChild(QueryGroup *g) { mFirstChild = g; }
	void setNextSibling(QueryGroup *g) { mNextSibling = g; }


	QueryGroup *firstChild() { return mFirstChild; }
	const QueryGroup *firstChild() const { return mFirstChild; }
	QueryGroup *lastChild();
	QueryGroup *nextSibling() { return mNextSibling; }
	const QueryGroup *nextSibling() const { return mNextSibling; }

	/**
	 * insert @p after as a sibling immediately after this
	 **/
	void insertAfter(QueryGroup *insert);

	/**
	 * insert @p immediately after this as a child
	 **/
	void insertUnder(QueryGroup *insert);

	/**
	 * Try get the "best fit" for the two parameters
	 **/
	void move(Query *query, QueryGroup *under, QueryGroup *after);

	QString propertyName() const { return mPropertyName; }
	QRegExp value() const { return mValue; }
	QString presentation() const { return mPresentation; }

	void setPropertyName(const QString &v) { mPropertyName = v; }
	void setPresentation(const QString &v) { mPresentation = v; }
	void setValue(const QRegExp &v) { mValue = v; }

	enum Fuzzyness
	{
		Case = 1<<0, Spaces = 1<<1, Articles = 1<<2, Symbols = 1<<3
	};

	bool fuzzyness(Fuzzyness f) const;

	enum Option
	{
		AutoHide = 1<<0, Disabled = 1<<1, Playable = 1<<2,
		ChildrenVisible = 1<<3, AutoOpen = 1<<4
	};

	bool option(Option option) const;
	void setOption(Option option, bool on);

	/**
	 * @return if I match @p file
	 **/
	bool matches(const File &file) const;

	QString presentation(const File &file) const;

private:
	/**
	 * apply all the "normalizing" transformations according
	 * to the fuzzyness
	 **/
	QString fuzzify(const QString &str) const;
	/**
	 * @returns the previous or parent of this item (slow)
	 **/
	QueryGroup *previous(Query *query);
	QueryGroup *previous(QueryGroup *startWith);

};




/**
 * a query is the tree structure that is shown to the user
 **/
class Query
{
	QueryGroup *mGroupFirst;
	QString mName;

public:
	Query();
	Query(const Query &copy);
	~Query();

	Query &operator =(const Query &copy);

	QueryGroup *firstChild();
	const QueryGroup *firstChild() const;

	void setFirstChild(QueryGroup *g);
	void insertFirst(QueryGroup *g);

	void clear();
	
	/**
	 * @returns the name to be used internally
	 **/
	QString name() const { return mName; }
	void setName(const QString &name) { mName = name; }

	/**
	 * @returns the name of the query
	 **/
	QString load(const QString &filename);
	void save(const QString &name, QDomElement &element);
	void save(const QString &name, const QString &filename);

	/**
	 * remove any trace of this from the tree, but don't actually delete it
	 **/
	void take(QueryGroup *item);

	void dump();
	
	/**
	 * @returns the name of this query as used internally by the db.
	 *
	 * Will give it a name in the db if necessary
	 **/
	QString dbname(Base *base);

private:
	void loadGroup(QDomElement element, QueryGroup *parent=0);
	void saveGroup(QDomElement &parent, QueryGroup *group);

	void deepCopy(const QueryGroup *from, QueryGroup *toParent);
	
	/**
	 * @returns the name of the query
	 **/
	QString load(QDomElement element);
};



#endif
