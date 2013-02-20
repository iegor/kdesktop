// Copyright (C) 2000 Julien Carme
// Copyright (C) 2001 Charles Samuels <charles@kde.org>
// Copyright (C) 2001 Neil Stevens <neil@qualityassistant.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <kinstance.h>

extern "C"
{
#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_thread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "renderer.h"
#include "display.h"
}

// stuff in main.h
extern "C"
{
typedef struct t_config {
    int xres,yres,sres;
    int teff,tcol;
} t_config;

t_config config = {512, 352, 1, 100, 100};
//t_config config = {640, 480, 1, 100, 100};

bool finished;
SDL_Thread *thread;
SDL_mutex *synchro_mutex, *access_mutex;
short pcm_data[2][512];
SDL_Surface *screen;
}

int winID(void)
{
	SDL_SysWMinfo info;
	info.version.major = SDL_MAJOR_VERSION;
	info.subsystem = SDL_SYSWM_X11;

	SDL_GetWMInfo(&info);
	return info.info.x11.wmwindow;
}

int main(int, char **)
{
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);

	atexit(SDL_Quit);

	KInstance in("noatuntyler");

	finished = false;
	access_mutex = SDL_CreateMutex();

	init_renderer1();
	printf("%u\n", winID());
	fflush(stdout);
	init_renderer2();
	thread = SDL_CreateThread((int (*)(void *))renderer, NULL);

	fd_set set;
	struct timeval tv;
	while(check_finished() != 1)
	{
		FD_ZERO(&set);
		FD_SET(STDIN_FILENO, &set);
		tv.tv_sec = 0;
		tv.tv_usec = 250;
		if(0 < select(STDIN_FILENO + 1, &set, 0, 0, &tv))
		{
			SDL_mutexP(access_mutex);
			read(STDIN_FILENO, (void *)pcm_data, sizeof(short) * 2 * 512);
			SDL_mutexV(access_mutex);
		}
		usleep(5);
	}
	/*fprintf(stderr,"exiting main()\n");*/
	SDL_WaitThread(thread, NULL);
}
