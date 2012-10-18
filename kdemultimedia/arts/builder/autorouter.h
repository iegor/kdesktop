/*

	Copyright (C) 1998 Stefan Westerfeld <stefan@space.twc.de>,
				  2002 Hans Meine <hans_meine@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef __AUTOROUTER_H_
#define __AUTOROUTER_H_

// If you get into trouble with threading (random crashes), you can configure
// things with --disable-threading, which should fix everything by not using
// threads
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBPTHREAD
#include <pthread.h>
#endif

#include <qptrlist.h>
#include <qvaluelist.h>

class PathInfo
{
public:
	int x1, x2, y1, y2, cost, depth;
	QString history;
	int operator<(const PathInfo& x) const { return cost < x.cost; }
	int operator==(const PathInfo& x) const { return cost == x.cost; }
};

typedef QValueList<PathInfo> PathQueue;

class ARCommand;

/**
 * The AutoRouter uses threads, commands are passed as objects via
 * AutoRouter::enqueue() to the routing thread.
 */
class AutoRouter
{
public:
	enum { none=0,up=1,down=2,left=4,right=8,head=16,tail=32,solid=64 };
	enum Direction { DIR_NONE=0, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

protected:
	int width, height;

	enum OwnerType { OWNER_UD=0, OWNER_LR=1 };
	OwnerType ownerIndex[DIR_RIGHT + 1]; // index is of type Direction
	long directionMask[DIR_RIGHT + 1];   // index is of type Direction

	struct Field
	{
		long data;
		long minCost;
		long penalty;
		long owner[2];
	} **field, **completeField;

	long newOwner; // next free owner ID
	long currentOwner;

	bool _needRedraw;

	PathInfo bestGoalPath;
	PathQueue pathlist[1024];
	int numQueuedPaths;

	// pseudo random table for fast "random" decisions
	enum { PRSIZE = 16 };
	long pseudoRandomDir[PRSIZE];
	int nextPR;
	void initPseudoRandom();
	
/****** thread stuff *****/
#ifdef HAVE_LIBPTHREAD
	pthread_mutex_t mutex_sync;
	pthread_mutex_t mutex_queue;

	pthread_t		route_thread;
#endif
	QPtrList<ARCommand> command_queue;

	bool			thread_terminate_now;
/*************************/

	void queuePath(const PathInfo &path);
	void examinePath(const PathInfo &path);

public:
	AutoRouter(int width, int height);
	~AutoRouter();

	// queries _needRedraw flag and deletes it
	// (assuming that the client is smart and redraws when getting true ;)
	bool needRedraw();

	long get(int x, int y);
	void getowners(int x, int y, long& ud_owner, long& lr_owner);

	void enqueue(ARCommand *command);

	// marks the entire field as unused
	void clear();
	// sets the
	void set(int x1, int y1, int x2, int y2, long lt);
	long connect(int x1, int y1, int x2, int y2, long owner);
	// 
	void sync();

	// the following functions are not for direct use; they're used
	// for threading only
	void thread_clear();
	void thread_set(int x1, int y1, int x2, int y2, long lt);
	void thread_connect(int x1, int y1, int x2, int y2, long owner);
	void thread_sync();

	void thread_command_loop();
};

/**
 * The ARCommand classes are used to communicate with the routing
 * thread, see AutoRouter::enqueue()
 */
class ARCommand
{
public:
	virtual void execute(AutoRouter *autorouter) = 0;
	// if a command is destructive (default: false), the command queue
	// will be emptied before queuing this one, assuming it'll destroy
	// results of all other commands anyway.
	virtual bool isDestructive();
};

class ARClearCommand :public ARCommand
{
public:
	void execute(AutoRouter *autorouter);
	bool isDestructive();
};

class ARSyncCommand :public ARCommand
{
public:
	void execute(AutoRouter *autorouter);
};

class ARConnectCommand :public ARCommand
{
	int _x1, _y1, _x2, _y2;
	long _owner;
public:
	ARConnectCommand(int x1, int y1, int x2, int y2, long owner);
	void execute(AutoRouter *autorouter);
};

class ARSetCommand :public ARCommand
{
private:
	int _x1, _y1, _x2, _y2;
	long _lt;
public:
	ARSetCommand(int x1, int y1, int x2, int y2, long lt);
	void execute(AutoRouter *autorouter);
};

#endif
