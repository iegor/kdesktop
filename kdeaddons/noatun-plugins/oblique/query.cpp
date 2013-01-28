// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "query.h"
#include "file.h"

#include <iostream>

#include <klocale.h>

#include <qdom.h>
#include <qfile.h>

QueryGroup::QueryGroup()
{
	mFirstChild=0;
	mNextSibling=0;

	mFuzzyness = Case | Spaces | Articles;
	mOptions = AutoHide;
}

QueryGroup::QueryGroup(const QueryGroup &copy)
{
	mFirstChild=0;
	mNextSibling=0;

	operator=(copy);

}

QueryGroup &QueryGroup::operator =(const QueryGroup &copy)
{
	mFuzzyness = copy.mFuzzyness;
	mOptions = copy.mOptions;
	mPropertyName = copy.mPropertyName;
	mPresentation = copy.mPresentation;
	mValue = copy.mValue;
	return *this;
}

QueryGroup::~QueryGroup()
{
	delete mFirstChild;
	delete mNextSibling;
}

void QueryGroup::insertAfter(QueryGroup *insert)
{
	QueryGroup *oldAfter = mNextSibling;
	insert->setNextSibling(oldAfter);
	setNextSibling(insert);
}

void QueryGroup::insertUnder(QueryGroup *insert)
{
	QueryGroup *oldUnder = mFirstChild;
	insert->setNextSibling(oldUnder);
	setFirstChild(insert);

}

void QueryGroup::move(Query *query, QueryGroup *under, QueryGroup *after)
{
	query->dump();

	query->take(this);
	if (after) after->insertAfter(this);
	else if (under) under->insertUnder(this);
	else query->insertFirst(this);

	query->dump();
}


QueryGroup *QueryGroup::previous(Query *query)
{
	QueryGroup *f = query->firstChild();
	if (f == this) return 0;

	return previous(f);
}

QueryGroup *QueryGroup::previous(QueryGroup *startWith)
{
	QueryGroup *current = startWith;
	QueryGroup *after = 0;

	while (current)
	{
		after = current->nextSibling();
		if (after == this)
			return current;

		if (QueryGroup *child = current->firstChild())
		{
			if (child == this)
				return current;
			child = previous(child);
			if (child) return child;
		}
		current = after;
	}
	return 0;
}



QueryGroup *QueryGroup::lastChild()
{
	QueryGroup *first = mFirstChild;
	if (!first) return 0;
	while (first->nextSibling())
		first = first->nextSibling();
	return first;
}


bool QueryGroup::fuzzyness(Fuzzyness f) const
{
	return mFuzzyness & f;
}

bool QueryGroup::option(Option option) const
{
	return mOptions & option;
}

void QueryGroup::setOption(Option option, bool on)
{
	if (on)
		mOptions |= option;
	else
		mOptions &= ~option;
}

bool QueryGroup::matches(const File &file) const
{
	QString prop = file.property(propertyName());

	prop = prop.simplifyWhiteSpace();
	if (prop.isNull()) prop = "";

	QRegExp re(value());
	return re.search(prop) != -1;
}



QString QueryGroup::presentation(const File &file) const
{
	// "$(property)"
	QString format=presentation();

	QRegExp find("(?:(?:\\\\\\\\))*\\$\\((.*)");

	int start=0;
	while (start != -1)
	{
		start = find.search(format, start);
		if (start == -1) break;

		// test if there's an odd amount of backslashes
		if (start>0 && format[start-1]=='\\')
		{
			// yes, so half the amount of backslashes

			// count how many there are first
			QRegExp counter("([\\\\]+)");
			counter.search(format, start-1);
			uint len=counter.cap(1).length()-1;

			// and half them, and remove one more
			format.replace(start-1, len/2+1, "");
			start=start-1+len/2+find.cap(1).length()+3;
			continue;
		}

		// now replace the backslashes with half as many

		if (format[start]=='\\')
		{
			// count how many there are first
			QRegExp counter("([\\\\]+)");
			counter.search(format, start);
			uint len=counter.cap(1).length();

			// and half them
			format.replace(start, len/2, "");
			start=start+len/2;
		}

		// "sth"foo"sth"
		QString cont(find.cap(1));
		QString prefix,suffix,propname;
		unsigned int i=0;
		if (cont[i] == '"')
		{
			i++;
			for (; i < cont.length(); i++)
			{
				if (cont[i] != '"')
					prefix += cont[i];
				else
					break;
			}
			i++;
		}


		for (; i < cont.length(); ++i)
		{
			if (cont[i]!='"' && cont[i]!=')')
				propname += cont[i];
			else
				break;
		}

		if (cont[i] == '"')
		{
			i++;
			for (; i < cont.length(); i++)
			{
				if (cont[i] != '"')
					suffix += cont[i];
				else
					break;
			}
			i++;
		}
		i++;


		QString propval = file.property(propname);

// the following code won't be enabled until the presentation is reloaded
// at the best times
/*		if (propname == "length")
		{
			int len = propval.toInt();
			if ( len < 0 ) // no file loaded
				propval = "--:--";

			int secs = length()/1000; // convert milliseconds -> seconds
			int seconds = secs % 60;
			propval.sprintf("%.2d:%.2d", ((secs-seconds)/60), seconds);
		}
*/

		if (propval.length())
		{
			propval = prefix+propval+suffix;
			format.replace(start, i+2, propval);
			start += propval.length();
		}
		else
		{
			format.replace(start, i+2, "");
		}
	}
	return format;
}


Query::Query()
{
	mGroupFirst=0;
}

Query::~Query()
{
	delete mGroupFirst;
}

Query::Query(const Query &copy)
{
	mGroupFirst = 0;
	operator=(copy);
}

Query &Query::operator =(const Query &copy)
{
	if (&copy == this) return *this;
	delete mGroupFirst;
	mGroupFirst=0;
	if (const QueryGroup *parent = copy.firstChild())
	{
		mGroupFirst = new QueryGroup(*parent);
		deepCopy(parent->firstChild(), mGroupFirst);
	}
	return *this;
}

QueryGroup *Query::firstChild()
{
	return mGroupFirst;
}

const QueryGroup *Query::firstChild() const
{
	return mGroupFirst;
}

void Query::setFirstChild(QueryGroup *g)
{
	mGroupFirst = g;
}

void Query::insertFirst(QueryGroup *g)
{
	g->setNextSibling(mGroupFirst);
	mGroupFirst = g;
}

void Query::clear()
{
	delete mGroupFirst;
	mGroupFirst=0;
}

QString Query::load(const QString &filename)
{
	QFile file(filename);
	unless (file.open(IO_ReadOnly)) return QString::null;

	QDomDocument doc;
	doc.setContent(&file);
	return load(doc.documentElement());
}

QString Query::load(QDomElement element)
{
	clear();

	if (element.tagName().lower() == "obliqueschema")
	{
		QDomNode node = element.firstChild();

		while (!node.isNull())
		{
			QDomElement e = node.toElement();
			if (e.tagName().lower() == "group")
				loadGroup(e);
			node = node.nextSibling();
		}
	}
	else
	{
		return QString::null;
	}

	// for internationalization:
	// Add these if you create new schemas and release them with Oblique
	(void)I18N_NOOP("Standard");

	QString title = element.attribute("title");
	if (element.hasAttribute("standard"))
		title = i18n(title.utf8());
	return title;
}

void Query::save(const QString &name, QDomElement &element)
{
	element.setTagName("ObliqueSchema");
	element.setAttribute("version", "1.0");
	element.setAttribute("title", name);
	for (QueryGroup *g = firstChild(); g; g = g->nextSibling())
		saveGroup(element, g);
}

void Query::save(const QString &name, const QString &filename)
{
	QFile file(filename);
	unless (file.open(IO_Truncate|IO_ReadWrite ))
		return;
	QDomDocument doc("ObliqueSchema");
	doc.setContent(QString("<!DOCTYPE ObliqueSchema><ObliqueSchema/>"));
	QDomElement e = doc.documentElement();
	save(name, e);

	QTextStream ts(&file);
	ts.setEncoding(QTextStream::UnicodeUTF8);
	// scourge elimination
	QString data = doc.toString();
	QString old = data;
	while (data.replace(QRegExp("([\n\r]+)(\t*) "), "\\1\\2\t") != old)
	{
		old = data;
	}
	ts << data;
}


void Query::take(QueryGroup *item)
{
	QueryGroup *previous = item->previous(this);

	if (!previous)
	{
		mGroupFirst = item->nextSibling();
		item->setNextSibling(0);
		return;
	}

	if (previous->nextSibling() == item)
	{
		previous->setNextSibling(item->nextSibling());
		item->setNextSibling(0);
	}
	else if (previous->firstChild() == item)
	{
		previous->setFirstChild(item->nextSibling());
		item->setNextSibling(0);
	}
}

static void dump(QueryGroup *item, int depth)
{
	if (!item) return;

	do
	{
		for (int d = 0; d < depth; d++)
			std::cerr << "    ";
		std::cerr << "prop: " << item->propertyName().utf8() << " pres: "
			<< item->presentation().utf8() << std::endl;
		dump(item->firstChild(), depth+1);

	} while ((item = item->nextSibling()));

}

void Query::dump()
{
	::dump(firstChild(), 0);
}




void Query::loadGroup(QDomElement element, QueryGroup *parent)
{
	QDomNode node = element.firstChild();

	QueryGroup *group = new QueryGroup;
	if (parent)
	{
		if (QueryGroup *last = parent->lastChild())
			last->setNextSibling(group);
		else
			parent->setFirstChild(group);
	}
	else
	{
		mGroupFirst = group;
	}

	while (!node.isNull())
	{
		QDomElement e = node.toElement();
		if (e.tagName().lower() == "group")
		{
			loadGroup(e, group);
		}
		else if (e.tagName().lower() == "property")
		{
			group->setPropertyName(e.text());
		}
		else if (e.tagName().lower() == "value")
		{
			group->setValue(QRegExp(e.text()));
		}
		else if (e.tagName().lower() == "presentation")
		{
			group->setPresentation(e.text());
		}
		else if (e.tagName().lower() == "options")
		{
			QDomNode node = e.firstChild();
			while (!node.isNull())
			{
				QDomElement e = node.toElement();

				if (e.tagName().lower() == "disabled")
					group->setOption(QueryGroup::Disabled, true);
				else if (e.tagName().lower() == "unique") // backwards compat (for now)
					group->setOption(QueryGroup::Playable, true);
				else if (e.tagName().lower() == "playable")
					group->setOption(QueryGroup::Playable, true);
				else if (e.tagName().lower() == "childrenvisible")
					group->setOption(QueryGroup::ChildrenVisible, true);
				else if (e.tagName().lower() == "autoopen")
					group->setOption(QueryGroup::AutoOpen, true);

				node = node.nextSibling();
			}

		}
		node = node.nextSibling();
	}
}

void Query::saveGroup(QDomElement &parent, QueryGroup *group)
{
	QDomDocument doc = parent.ownerDocument();
	QDomElement element = doc.createElement("group");
	parent.appendChild(element);

	QDomElement childe;
	QDomText childtext;
	{
		childe = doc.createElement("property");
		element.appendChild(childe);
		childtext = doc.createTextNode(group->propertyName());
		childe.appendChild(childtext);
	}
	{
		childe = doc.createElement("value");
		element.appendChild(childe);
		childtext = doc.createTextNode(group->value().pattern());
		childe.appendChild(childtext);
	}
	{
		childe = doc.createElement("presentation");
		element.appendChild(childe);
		childtext = doc.createTextNode(group->presentation());
		childe.appendChild(childtext);
	}
	{
		childe = doc.createElement("options");
		element.appendChild(childe);
		if (group->option(QueryGroup::Disabled))
			childe.appendChild(doc.createElement("disabled"));
		if (group->option(QueryGroup::Playable))
			childe.appendChild(doc.createElement("playable"));
		if (group->option(QueryGroup::ChildrenVisible))
			childe.appendChild(doc.createElement("childrenvisible"));
		if (group->option(QueryGroup::AutoOpen))
			childe.appendChild(doc.createElement("autoopen"));
	}

	for (QueryGroup *c = group->firstChild(); c; c = c->nextSibling())
	{
		saveGroup(element, c);
	}
}

void Query::deepCopy(const QueryGroup *from, QueryGroup *toParent)
{
	if (!from) return;
	QueryGroup *last=0;

	while (from)
	{
		QueryGroup *copy = new QueryGroup(*from);
		if (last)
		{
			last->setNextSibling(copy);
			last = copy;
		}
		else
		{
			toParent->setFirstChild(copy);
			last = copy;
		}
		deepCopy(from->firstChild(), last);
		from = from->nextSibling();
	}
}

