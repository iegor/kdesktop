
#ifndef __XINIT_H__
#define __XINIT_H__

#define __USE_X_SHAREDMEMORY__

#include <pthread.h>



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>



#ifdef  X11_SHARED_MEM
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif


#ifdef X11_XV
#include <X11/extensions/Xv.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/XShm.h>
#endif

#ifdef X11_XVIDMODE
#include <X11/extensions/xf86vmode.h>
#endif

#ifdef X11_DGA2
#include <X11/extensions/xf86dga.h>
#endif

#define ERR_XI_FAILURE        0xFF
#define ERR_XI_OK             0x00
#define ERR_XI_NOSHAREDMEMORY 0x01
#define ERR_XI_DISPLAY        0x02
#define ERR_XI_BADDEPTH       0x03
#define ERR_XI_WINDOW         0x04
#define ERR_XI_VIRTALLOC      0x05
#define ERR_XI_XIMAGE         0x06
#define ERR_XI_SHMALLOC       0x07
#define ERR_XI_SHMXIMAGE      0x08
#define ERR_XI_SHMSEGINFO     0x09
#define ERR_XI_SHMVIRTALLOC   0x0A
#define ERR_XI_SHMATTACH      0x0B




#define PIXEL unsigned long

extern const char *ERR_XI_STR[];

struct XWindow {

  Display *display;
  Window window;
  Screen *screenptr;
  int screennum;
  Visual *visual;
  GC gc;

  Colormap colormap;
  PIXEL *palette;
  int colorcells;
  
  int x;
  int y;
  int width;
  int height;
  int depth;
  int pixelsize;
  int screensize;
  int lOpen;

  // colorMask
  unsigned int redMask;
  unsigned int greenMask;
  unsigned int blueMask;

  // colortable for 8 bit colormap
  // (created with interference by the XServer)
  unsigned char pixel[256];

};


#endif /* __XINIT_H__ */
