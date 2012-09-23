// Copyright (c) 2000-2001 Charles Samuels <charles@kde.org>
// Copyright (c) 2000-2001 Neil Stevens <multivac@fcmail.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef PLUGINMODULE_H
#define PLUGINMODULE_H

#include "noatun/pref.h"
#include "noatun/pluginloader.h"

#include <klistview.h>
#include <qframe.h>
#include <qstringlist.h>

class KTabCtl;

class PluginListItem : public QCheckListItem
{
public:
	PluginListItem(const bool _exclusive, bool _checked, const NoatunLibraryInfo &_info, QListView *_parent);
	const NoatunLibraryInfo &info() const { return mInfo; }

	// This will toggle the state without "emitting" the stateChange
	void setChecked(bool);

protected:	
	virtual void stateChange(bool);
	virtual void paintCell(QPainter *, const QColorGroup &, int, int, int); 
private:
	NoatunLibraryInfo mInfo;
	bool silentStateChange;
	bool exclusive;
};

class PluginListView : public KListView
{
Q_OBJECT

friend class PluginListItem;

public:
	PluginListView(QWidget *_parent = 0, const char *_name = 0);
	PluginListView(unsigned _min, QWidget *_parent = 0, const char *_name = 0);
	PluginListView(unsigned _min, unsigned _max, QWidget *_parent = 0, const char *_name = 0);

	virtual void clear();

signals:
	void stateChange(PluginListItem *, bool);

private:
	void stateChanged(PluginListItem *, bool);
	
	bool hasMaximum;
	unsigned max, min;
	unsigned count;
};

class Plugins : public CModule
{
Q_OBJECT
public:
	Plugins(QObject *_parent = 0);
	virtual void save();
	virtual void reopen();

protected:
	virtual void showEvent(QShowEvent *);

private slots:
	void stateChange(PluginListItem *, bool);

private:
	void addPlugin(const NoatunLibraryInfo &);
	void removePlugin(const NoatunLibraryInfo &);
	PluginListItem *findItem(const NoatunLibraryInfo &) const;

	QStringList mAdded, mDeleted;
	PluginListView *interfaceList, *playlistList, *otherList, *visList;

	bool shown;
};

#endif
