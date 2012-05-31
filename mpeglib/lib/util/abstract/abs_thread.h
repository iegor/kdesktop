/*
  abstraction for threads
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#ifndef __ABS_THREAD_H
#define __ABS_THREAD_H


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
   This passed alle pthread_xxx calls to this interface, thus
   it can be easier replaced with other thread "layers"

   All posix pthread calls are conveterd to abs_thread.
*/

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
}

#define _ABS_BUSY EBUSY

#ifndef SDL_WRAPPER
// definitions for direct pthread support
#include <pthread.h>

typedef pthread_mutex_t abs_thread_mutex_t;
typedef pthread_cond_t abs_thread_cond_t;
typedef pthread_t abs_thread_t;



#define abs_thread_cond_init(cond) pthread_cond_init(cond,NULL)
#define abs_thread_cond_destroy(cond) pthread_cond_destroy(cond)
#define abs_thread_cond_signal(cond) pthread_cond_signal(cond)
#define abs_thread_cond_wait(cond,mutex) pthread_cond_wait(cond,mutex)

// CREATE / JOIN THREAD

#define abs_thread_create(thread,func,arg) pthread_create(thread,NULL,func,arg)
#define abs_thread_join(th,thread_return) pthread_join(th,thread_return)

// MUTEX FUNCTIONS

#define abs_thread_mutex_lock(mutex) pthread_mutex_lock(mutex)
#define abs_thread_mutex_unlock(mutex) pthread_mutex_unlock(mutex)
#define abs_thread_mutex_init(mutex) pthread_mutex_init(mutex,NULL)
#define abs_thread_mutex_destroy(mutex) pthread_mutex_destroy(mutex)

#endif
// not SDL_WRAPPER

#ifdef SDL_WRAPPER


// SDL SUPPORT DISABLED 

#if defined WIN32
  #include <SDL_thread.h>
  #include <SDL_mutex.h>
#else
  #include <SDL/SDL_thread.h>
  #include <SDL/SDL_mutex.h>
#endif


typedef SDL_mutex* abs_thread_mutex_t;
typedef SDL_cond*  abs_thread_cond_t;
typedef SDL_Thread* abs_thread_t;

// SIGNAL FUNCTIONS
// note we have _no_ cond attribut (not needed)
int abs_thread_cond_init(abs_thread_cond_t* cond);
int abs_thread_cond_destroy(abs_thread_cond_t *cond);

int abs_thread_cond_signal(abs_thread_cond_t* cond);

int abs_thread_cond_wait(abs_thread_cond_t* cond,
			 abs_thread_mutex_t *mutex);
// CREATE / JOIN THREAD
// Note: we have thread attribute
int abs_thread_create(abs_thread_t* thread,
		      void * (*start_routine)(void *), void * arg);

int abs_thread_join(abs_thread_t th, 
		    void **thread_return);


// MUTEX FUNCTIONS

int abs_thread_mutex_lock(abs_thread_mutex_t *mutex);
int abs_thread_mutex_trylock(abs_thread_mutex_t *mutex);
int abs_thread_mutex_unlock(abs_thread_mutex_t *mutex);
// not attribute!
int abs_thread_mutex_init(abs_thread_mutex_t   *mutex);

int abs_thread_mutex_destroy(abs_thread_mutex_t *mutex);



#endif
//SDL_WRAPPER



#endif


