/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1999 by Red Hat Software
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* modifications (c) 1999 Peter Putzer */

#ifndef H_LEVELDB
#define H_LEVELDB


#ifdef __cplusplus

extern "C" {
#endif

#include <glob.h>

struct service {
    char * name;
    int levels, kPriority, sPriority;
    char * desc;
};

int parseLevels(const char * str, int emptyOk);

/* returns 0 on success, 1 if the service is not chkconfig-able, -1 if an
   I/O error occurs (in which case errno can be checked) */
int readServiceInfo(const char* RUNLEVELS, const char* name, struct service * service);
int currentRunlevel(void);
int isOn(const char* RUNLEVELS, const char* name, int where);
int isConfigured(const char* RUNLEVELS, const char* name, int level);
int doSetService(const char* RUNLEVELS, struct service s, int level, int on);
int findServiceEntries(const char* RUNLEVELS, const char* name, int level, glob_t * globresptr);

#ifdef __cplusplus
}
#endif

#endif
