/*
   This file is part of KDE/aRts (Noatun) - xine integration
   Copyright (C) 2002-2003 Ewald Snel <ewald@rambo.its.tudelft.nl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
*/

#ifndef __XINEPLAYOBJECT_IMPL_H
#define __XINEPLAYOBJECT_IMPL_H

#include <string>
#include <pthread.h>
#include <stdsynthmodule.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <xine.h>

#include "audio_fifo_out.h"
#include "xinePlayObject.h"


using namespace std;
using Arts::poState;
using Arts::poTime;
using Arts::poCapabilities;

class xinePlayObject_impl : virtual public xinePlayObject_skel, public Arts::StdSynthModule
{
public:
    xinePlayObject_impl(bool audioOnly=false);
    virtual ~xinePlayObject_impl();

    bool loadMedia( const string &url );
    string description();
    poTime currentTime();
    poTime overallTime();
    poCapabilities capabilities();
    string mediaName();
    poState state();
    void play();
    void halt();
    void seek( const class poTime &t );
    void pause();
    void calculateBlock( unsigned long samples );

protected:
    void xineEvent( const xine_event_t &event );
    void clearWindow();
    void frameOutput( int &x, int &y,
		      int &width, int &height, double &ratio,
		      int displayWidth, int displayHeight,
		      double displayPixelAspect, bool dscb );
    void resizeNotify();
    void eventLoop();

    // C -> C++ wrapper for pthread API
    static inline void *pthread_start_routine( void *obj )
    {
	((xinePlayObject_impl *)obj)->eventLoop();
	pthread_exit( 0 );
    }

    // C -> C++ wrapper for xine API
    static inline void xine_handle_event( void *obj, const xine_event_t *event )
    {
	((xinePlayObject_impl *)obj)->xineEvent( *event );
    }

    // C -> C++ wrapper for xine API
    static inline void frame_output_cb( void *obj,
					int video_width, int video_height,
					double video_pixel_aspect,
					int *dest_x, int *dest_y,
					int *dest_width, int *dest_height,
					double *dest_pixel_aspect,
					int *win_x, int *win_y )
    {
	((xinePlayObject_impl *)obj)->frameOutput( *win_x, *win_y,
						   *dest_width, *dest_height,
						   *dest_pixel_aspect,
						   video_width, video_height,
						   video_pixel_aspect, false );

	*dest_x = 0;
	*dest_y = 0;
    }

    // C -> C++ wrapper for xine API
    static inline void dest_size_cb( void *obj,
				     int video_width, int video_height,
				     double video_pixel_aspect,
				     int *dest_width, int *dest_height,
				     double *dest_pixel_aspect )
    {
	int win_x, win_y;

	((xinePlayObject_impl *)obj)->frameOutput( win_x, win_y,
						   *dest_width, *dest_height,
						   *dest_pixel_aspect,
						   video_width, video_height,
						   video_pixel_aspect, true );
    }

private:
    double		 flpos;
    string		 mrl;

protected:
    pthread_mutex_t	 mutex;
    pthread_t		 thread;

    // xine data
    xine_t		*xine;
    xine_stream_t	*stream;
    xine_event_queue_t	*queue;
    xine_audio_port_t	*ao_port;
    xine_video_port_t	*vo_port;
    void		*ao_driver;
    x11_visual_t	 visual;
    xine_arts_audio	 audio;

    Display		*display;
    Window		 xcomWindow;
    Atom		 xcomAtomQuit;
    Atom		 xcomAtomResize;
    int			 screen;
    int			 width;
    int			 height;
    int			 dscbTimeOut;
    int			 shmCompletionType;

private:
    int			 streamLength;
    int			 streamPosition;
    bool		 audioOnly;
};

class xineAudioPlayObject_impl : virtual public xineAudioPlayObject_skel, public xinePlayObject_impl
{
public:
    xineAudioPlayObject_impl() : xinePlayObject_impl(true) {};
};

class xineVideoPlayObject_impl : virtual public xineVideoPlayObject_skel, public xinePlayObject_impl
{
public:
    xineVideoPlayObject_impl() : xinePlayObject_impl(false) {};
    long x11Snapshot();
    long x11WindowId();
    void x11WindowId( long window );
};


#endif
