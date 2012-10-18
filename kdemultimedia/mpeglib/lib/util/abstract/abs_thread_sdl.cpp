/*
  abstraction for threads
  Copyright (C) 2000 Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include "abs_thread.h"


// START SDL


#ifdef SDL_WRAPPER



int abs_thread_cond_init(abs_thread_cond_t* cond) {
  *cond=SDL_CreateCond();
  return (*cond != NULL);
}

int abs_thread_cond_destroy(abs_thread_cond_t *cond) {
  SDL_DestroyCond(*cond);
  return true;
}


int abs_thread_cond_signal(abs_thread_cond_t* cond) {
  
  return SDL_CondSignal(*cond);
}


int abs_thread_cond_wait(abs_thread_cond_t* cond,
			 abs_thread_mutex_t* mutex) {
  SDL_CondWait(*cond,*mutex);
  return true;
}


// CREATE / JOIN THREAD
int abs_thread_create(abs_thread_t* thread,
		      void * (*start_routine)(void *), void * arg) {
  int (*func)(void *);
  func=(int (*)(void *))start_routine;
  *thread=SDL_CreateThread(func,arg);
  return (*thread != NULL);
}

int abs_thread_join(abs_thread_t th, 
		    void **thread_return) {
  SDL_WaitThread(th,(int*)*thread_return);
  return true;
}


// MUTEX FUNCTIONS

int abs_thread_mutex_lock(abs_thread_mutex_t *mutex) {
  return SDL_LockMutex(*mutex);
}


int abs_thread_mutex_unlock(abs_thread_mutex_t *mutex) {
  return SDL_UnlockMutex(*mutex);
}


int abs_thread_mutex_init(abs_thread_mutex_t   *mutex) {
  *mutex=SDL_CreateMutex();
  return true;
}


int abs_thread_mutex_destroy(abs_thread_mutex_t *mutex) {
  SDL_DestroyMutex(*mutex);
  return true;
}



#endif
