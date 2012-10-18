


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef SDL_WRAPPER

  #include "audioIO_SDL.cpp"

//
// If native sound is defined compiled for that
//

#else


#ifdef OS_AIX 
  #include "audioIO_AIX.cpp"
#endif

#ifdef OS_Linux
  #include "audioIO_Linux.cpp"
#endif

#ifdef OS_BSD
  #include "audioIO_Linux.cpp"
#endif

#if defined(OS_IRIX) || defined(OS_IRIX64)
  #include "audioIO_IRIX.cpp"
#endif

#ifdef OS_HPUX
  #include "audioIO_HPUX.cpp"
#endif

#ifdef OS_SunOS
  #include "audioIO_SunOS.cpp"
#endif

#ifdef __BEOS__
  #include "audioIO_BeOS.cpp"
#endif


#endif
