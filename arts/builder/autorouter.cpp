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

#include "autorouter.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arts/debug.h>

#include <kdebug.h>

#ifndef HAVE_LIBPTHREAD
#define pthread_create(a, b, c, d) ;
#define pthread_join(a, b) ;
#define pthread_mutex_lock(a) ;
#define pthread_mutex_unlock(a) ;
#define pthread_mutex_init(a, b) ;
#define pthread_attr_init(a) ;
#endif

using namespace std;

const int COST_INIT = 200000;
const int PENALTY_OTHERCONNECTION = 100;
const int PENALTY_CORNER = 100;
const int IDLE_SLEEP_MILLISECONDS = 40;

void *startroute(void *where)
{
	((AutoRouter *)where)->thread_command_loop();
	// just to prevent the compiler warning
	return 0;
}

bool ARCommand::isDestructive()
{
	return false;
}

AutoRouter::AutoRouter(int width, int height)
	: width(width), height(height)
{
#ifdef HAVE_LIBPTHREAD
	pthread_attr_t attrs;

	pthread_attr_init(&attrs);
	pthread_mutex_init(&mutex_sync, 0);
	pthread_mutex_init(&mutex_queue, 0);
#endif

	// allocate memory for fields..
	field = new Field*[width];
	completeField = new Field*[width];

	for(int i = 0; i < width; i++)
	{
		field[i] = new Field[height];
		completeField[i] = new Field[height];
	}

	// ..and clear the freshly allocated memory
	thread_clear();
	thread_sync();

	newOwner = 1;

	// init those two arrays - they should be constant somehow..
	directionMask[DIR_DOWN] = ~(left | right);
	directionMask[DIR_UP] = ~(left | right);
	directionMask[DIR_LEFT] = ~(up | down);
	directionMask[DIR_RIGHT] = ~(up | down);

	ownerIndex[DIR_DOWN] = OWNER_UD;
	ownerIndex[DIR_UP] = OWNER_UD;
	ownerIndex[DIR_LEFT] = OWNER_LR;
	ownerIndex[DIR_RIGHT] = OWNER_LR;

	initPseudoRandom();
	
	thread_terminate_now = false; 		// not yet
#ifdef HAVE_LIBPTHREAD
	// setup thread and run thread_command_loop() in it
	pthread_create(&route_thread, &attrs, startroute, this);
#endif

	arts_debug("AR UP...");
}

void AutoRouter::initPseudoRandom()
{
	// init pseudoRandomDir array with random directions
	
	int remainingDirs[4];	
	for(int i = 0; i < PRSIZE; i++)
	{
		if((i & 3) == 0)
		{
			int j = 0;
			for(int dir = DIR_UP; dir <= DIR_RIGHT; dir++)
				remainingDirs[j++] = dir;
		}

		int rnd;
		do rnd = rand()&3;
		while(remainingDirs[rnd] == DIR_NONE);

		pseudoRandomDir[i] = remainingDirs[rnd];
		
		remainingDirs[rnd] = DIR_NONE;
	}
	nextPR = 0;
}

AutoRouter::~AutoRouter()
{
	// tell the thread to shut down
	pthread_mutex_lock(&mutex_queue);
	thread_terminate_now = true;
	pthread_mutex_unlock(&mutex_queue);
	
	// terminate thread
	void *rc;
	pthread_join(route_thread, &rc);

	// clean up
	for(int i = 0; i < width; i++)
	{
		delete[] field[i];
		delete[] completeField[i];
	}
	delete[] completeField;
	delete[] field;
	
	arts_debug("AR DOWN...");
}

void AutoRouter::enqueue(ARCommand *command)
{
#ifdef HAVE_LIBPTHREAD
	// threaded execution, locking the queue
	pthread_mutex_lock(&mutex_queue);
	
	if(command->isDestructive())
	{
		// ok, then we can kill the whole list, since this will clear
		// the whole results anyway

		command_queue.setAutoDelete(true);
		command_queue.clear();
		command_queue.setAutoDelete(false);
	}
	command_queue.append(command);
	
	pthread_mutex_unlock(&mutex_queue);
#else
	// immediate execution
	command->execute(this);
	delete command;
#endif
}

void AutoRouter::thread_command_loop()
{
	while(1)
	{
		ARCommand *command = 0;

		// pop one command from queue if possible..
		pthread_mutex_lock(&mutex_queue);
		if(!command_queue.isEmpty())
		{
			command = command_queue.first();
			command_queue.remove(unsigned(0));
		}
		else
		{
			if (thread_terminate_now) {
				pthread_mutex_unlock(&mutex_queue);
				return;
			}
		}
		pthread_mutex_unlock(&mutex_queue);

		// ..and execute command if we got one
		if(command)
		{
			command->execute(this);
			delete command;
		}
		else // no more commands->wait for some milliseconds
			usleep(1000 * IDLE_SLEEP_MILLISECONDS);
		// TODO: use pthread_cond_wait here instead of half-busy-waiting?
	}
}

// ------------------------- getXXX() handling -------------------------

long AutoRouter::get(int x, int y)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	pthread_mutex_lock(&mutex_sync);
	long result = completeField[x][y].data;
	pthread_mutex_unlock(&mutex_sync);
	
	return(result);
}

void AutoRouter::getowners(int x, int y, long& ud_owner, long& lr_owner)
{
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	pthread_mutex_lock(&mutex_sync);
	ud_owner = completeField[x][y].owner[OWNER_UD];
	lr_owner = completeField[x][y].owner[OWNER_LR];
	pthread_mutex_unlock(&mutex_sync);
}

// ------------------------- sync() handling -------------------------

void AutoRouter::sync()
{
	enqueue(new ARSyncCommand());
}

void ARSyncCommand::execute(AutoRouter *router)
{
	router->thread_sync();
}

void AutoRouter::thread_sync()
{
	int i;
	pthread_mutex_lock(&mutex_sync);
	
	for(i = 0; i < width; i++)
		memcpy(completeField[i], field[i], sizeof(Field)*height);
	_needRedraw = true;
	
	pthread_mutex_unlock(&mutex_sync);
}

bool AutoRouter::needRedraw()
{
	bool result;

	pthread_mutex_lock(&mutex_sync);
	result = _needRedraw;
	_needRedraw = false;
#ifdef AR_DEBUG
	if(result) arts_debug("NEED REDRAW NOW!");
#endif
	pthread_mutex_unlock(&mutex_sync);

	return result;
}

// ------------------------- clear() handling -------------------------

void AutoRouter::clear()
{
	enqueue(new ARClearCommand());
}

bool ARClearCommand::isDestructive()
{
	return true;
}

void ARClearCommand::execute(AutoRouter *router)
{
	router->thread_clear();
}

void AutoRouter::thread_clear()
{
	arts_debug("clear()ing now...");
	int x, y;

	for(x = 0; x < width; x++)
		for(y = 0; y < height; y++)
		{
			field[x][y].data = none;
			field[x][y].penalty = 0;
			field[x][y].owner[0] = -1;
			field[x][y].owner[1] = -1;
		}
}

// ------------------------- set() command handling -------------------------

void AutoRouter::set(int x1, int y1, int x2, int y2, long lt)
{
	enqueue(new ARSetCommand(x1, y1, x2, y2, lt));
}

ARSetCommand::ARSetCommand(int x1, int y1, int x2, int y2, long lt)
	: _x1(x1), _y1(y1), _x2(x2), _y2(y2), _lt(lt)
{
}

void ARSetCommand::execute(AutoRouter *router)
{
	router->thread_set(_x1, _y1, _x2, _y2, _lt);
}

void AutoRouter::thread_set(int x1, int y1, int x2, int y2, long lt)
{
	for(int x = x1; x <= x2; x++)
	{
		for(int y = y1; y <= y2; y++)
		{
			assert(x >= 0 && x < width);
			assert(y >= 0 && y < height);

			if(lt & solid)
			{
				if((y - 1) >= 0)
					field[x][y - 1].penalty += 5;

				if((y - 2) >= 0)
					field[x][y - 2].penalty += 2;

				if((y + 1) < height)
					field[x][y + 1].penalty += 5;

				if((y + 2) < height)
					field[x][y + 2].penalty += 2;
			}
			
			field[x][y].data = lt;
			field[x][y].owner[0] = 0;
			field[x][y].owner[1] = 0;		// don't pass
		}
	}
}


long AutoRouter::connect(int x1, int y1, int x2, int y2, long owner)
{
	if(owner == 0)
		owner = newOwner++;

	enqueue(new ARConnectCommand(x1, y1, x2, y2, owner));

	return owner;
}

ARConnectCommand::ARConnectCommand(int x1, int y1, int x2, int y2, long owner)
{
	_x1 = x1; _y1 = y1; _x2 = x2; _y2 = y2; _owner = owner;
}

void ARConnectCommand::execute(AutoRouter *router)
{
	router->thread_connect(_x1, _y1, _x2, _y2, _owner);
}

void AutoRouter::thread_connect(int x1, int y1, int x2, int y2, long owner)
{
	currentOwner = owner;

#ifdef AR_DEBUG
	arts_debug("-field[x1][y1].owner[0..1] = %ld, %ld",field[x1][y1].owner[0],
													field[x1][y1].owner[1]);
	arts_debug("-field[x2][y2].owner[0..1] = %ld, %ld", field[x2][y2].owner[0],
													field[x2][y2].owner[1]);
#endif

	// clear data(source) & data(dest) first and restore later, since
	// they might be solid
	long sourceFieldData = field[x1][y1].data; field[x1][y1].data = none;
	long destFieldData   = field[x2][y2].data; field[x2][y2].data = none;

	for(int x = 0; x < width; x++)
		for(int y = 0; y < height; y++)
			field[x][y].minCost = COST_INIT;

#ifdef AR_DEBUG
	arts_debug("autorouter: trying to connect %d, %d with %d, %d (owner %ld)",
			   x1, y1, x2, y2, owner);
#endif
	nextPR = 0;

	bestGoalPath.cost = COST_INIT;

	int activelist = 0;
	numQueuedPaths = 0;

	PathInfo path;
	path.x1 = x1;
	path.x2 = x2;
	path.y1 = y1;
	path.y2 = y2;
	path.cost = 0;
	path.depth = 0;
	queuePath(path);

	while(numQueuedPaths)
	{
		while(!pathlist[activelist].size())
			activelist++;
		PathQueue& activePathList =
			pathlist[activelist];

		assert(activePathList.size());
		examinePath(activePathList.first());
		activePathList.pop_front();
		numQueuedPaths--;
	}

	field[x1][y1].data = sourceFieldData;
	field[x2][y2].data = destFieldData;

	if(bestGoalPath.cost != COST_INIT)
	{
		//arts_debug("bestGoalPath.history for this connection is %s", bestGoalPath.history.data());
		//arts_debug("minCost for that was %d", gms);

		const char *walk = bestGoalPath.history.ascii();

		int x = x1;
		int y = y1;

		while(*walk)
		{
			field[x][y].owner[ownerIndex[*walk]] = currentOwner;
			switch(*walk)
			{
			case DIR_DOWN:
				field[x][y++].data |= down;
				field[x][y].data |= up;
				break;
			case DIR_UP:
				field[x][y--].data |= up;
				field[x][y].data |= down;
				break;
			case DIR_LEFT:
				field[x--][y].data |= left;
				field[x][y].data |= right;
				break;
			case DIR_RIGHT:
				field[x++][y].data |= right;
				field[x][y].data |= left;
				break;
			}
			field[x][y].owner[ownerIndex[*walk]] = currentOwner;
			walk++;
		}
	}
	else
	{
#ifdef AR_DEBUG
		arts_debug("!! sorry, this connection is impossible !!");
#endif
	}
}

void AutoRouter::queuePath(const PathInfo &path)
{
	PathInfo newPath = path;

	int targetlist = newPath.cost/5;
	if(targetlist > 1023)
		targetlist = 1023;

	pathlist[targetlist].append(newPath);
	qHeapSort(pathlist[targetlist]);

	numQueuedPaths++;
}

void AutoRouter::examinePath(const PathInfo &path)
{
	const char *walk = path.history.ascii();

// check if we can go here:

	if(path.x1 < 0 || path.x1 >= width)
		return;
	if(path.y1 < 0 || path.y1 >= width)
		return;

	int currentFieldCost = path.cost;
	
	if(path.depth > 0)
	{
		// going over a field where already connections are is bad
		if(field[path.x1][path.y1].data != 0)
			currentFieldCost += PENALTY_OTHERCONNECTION;

		if(directionMask[walk[path.depth - 1]] & field[path.x1][path.y1].data)
		{
			// someone already uses that field... we can continue
			// only if the connection has the same sourceport

			long fieldowner = field[path.x1][path.y1].owner[ownerIndex[walk[path.depth - 1]]];

			if(fieldowner != -1) // used?
			{
				if(fieldowner != currentOwner)
					return;
				
				// oops, the connections are from the same owner -> no
				// penalty needed!
				currentFieldCost -= PENALTY_OTHERCONNECTION;
			}
		}
	}

	// add cityblock distance to costs
	currentFieldCost += abs(path.x1 - path.x2) + abs(path.y1 - path.y2);
	
	// add field penalty to costs
	currentFieldCost += field[path.x1][path.y1].penalty;

	// add corner penalty to costs if path had corner here
	if(path.depth > 2)
		if(walk[path.depth - 2] != walk[path.depth - 1])
			currentFieldCost += PENALTY_CORNER;
	
	if(currentFieldCost > bestGoalPath.cost)
		return;

	// better path found?
	if(currentFieldCost < field[path.x1][path.y1].minCost)
	{
		field[path.x1][path.y1].minCost = currentFieldCost;
	
		// check if we are where we wanted to be:
		if(path.x1 == path.x2 && path.y1 == path.y2) {
			// goal! success! :-)
			
			if(path.cost < bestGoalPath.cost)
			{
				bestGoalPath = path; // best solution until now
				bestGoalPath.cost = currentFieldCost;
			}
			
			return;
		}

		// not at the goal yet, search next place to go; take some
		// pseudo random direction order (this method improves search
		// speed)

		PathInfo newPath = path;
		newPath.depth++;
		newPath.cost = currentFieldCost;
		
		for(int i = 0; i < 4; i++)
		{
			if(nextPR >= PRSIZE)
				nextPR = 0;
		
			switch(pseudoRandomDir[nextPR++])
			{
			case DIR_LEFT:
				newPath.x1--;
				newPath.history = path.history + (char)DIR_LEFT;
				queuePath(newPath);
				newPath.x1++;
				break;
				
			case DIR_RIGHT:
				newPath.x1++;
				newPath.history = path.history + (char)DIR_RIGHT;
				queuePath(newPath);
				newPath.x1--;
				break;
				
			case DIR_UP:
				newPath.y1--;
				newPath.history = path.history + (char)DIR_UP;
				queuePath(newPath);
				newPath.y1++;
				break;
				
			case DIR_DOWN:
				newPath.y1++;
				newPath.history = path.history + (char)DIR_DOWN;
				queuePath(newPath);
				newPath.y1--;
				break;
			}
		}
	}
}
