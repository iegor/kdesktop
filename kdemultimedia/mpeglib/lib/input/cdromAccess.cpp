/*
 * system dependent wrapper for access to cdrom
 * Copyright (C) 1999  Martin Vogt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation.
 *
 *  For more information look at the file COPYRIGHT in this package
 *
 * $Id: cdromAccess.cpp 470242 2005-10-13 12:30:28Z mueller $
 *
 */

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif



#ifdef OS_AIX 
  #include "cdromAccess_Empty.cpp"
#endif

#ifdef OS_Linux
  #include <sys/types.h>
  #include "cdromAccess_Linux.cpp"
#endif

#ifdef OS_BSD
  #include "cdromAccess_Empty.cpp"
#endif

#if defined(OS_IRIX) || defined(OS_IRIX64)
  #include "cdromAccess_Empty.cpp"
#endif

#ifdef OS_HPUX
  #include "cdromAccess_Empty.cpp"
#endif

#ifdef OS_SunOS
  #include "cdromAccess_Empty.cpp"
#endif

#ifdef __BEOS__
  #include "cdromAccess_Empty.cpp"
#endif

#ifdef WIN32
  #include "cdromAccess_Empty.cpp"
#endif


