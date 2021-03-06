/*
 *  Copyright 1999-2002 David Faure <faure@kde.org>
 *
 *  Redistribution and use in source and binary forms, with or
 *    without modification, are permitted provided that the following
 *    conditions are met:
 *
 *    - Redistributions of source code must retain the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 *    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
 *    BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *    TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *    THE POSSIBILITY OF SUCH DAMAGE.
 *
 * (BSD License, from kdelibs/doc/common/bsd-license.html)
 */

#ifndef __DEFS_H
#define __DEFS_H

#include "includ.h"

/* Configuration file-read buffer sizes */
#define S_CFGLINE 80

   /* from answmach/io.c */
#define S_MESSG 120
#define S_COMMAND 100

#define S_INVITE_LINES 200

#define DISPLAYS_LIST_MAX 200
#define DISPLAY_MAX 50

/*
 * Invitation displayed by the answering machine when the caller connects.
 * Add \n at the end of any line you want it to display,
 *   and \ at the end of any line of the definition of INVITE_LINES
 * They are not necessarily the same ...
 *
 * ANNOUNCE* are the 3 lines displayed on the terminal / on 
 * the console, to announce the request.
 */

#define ANNOUNCE1 "Message from Talk_Daemon at %d:%02d ..."
#define ANNOUNCE2 "talk: connection requested by %s."
#define ANNOUNCE3 "talk: respond with: talk %s"
#define INVITE_LINES "Hello. You're connected to a talk answering \
machine.\nThe person you have paged isn't there at the moment.\n\
Please leave a message and quit normally when finished.\n"

/*Please check that INVITE_LINES is max S_INVITE_LINES chars*/

/* Default message if N.E.U (non existent user) called */
#define NEU_BANNER1 "The person you're asking to talk with is unknown at this host."
#define NEU_BANNER2 "You may have mistyped the name, or network address. Try again"
#define NEU_BANNER3 "or leave a message which will be sent to the system administrator."

/* return value from process_request : */
#define PROC_REQ_OK 0
#define PROC_REQ_ERR 1
#define PROC_REQ_FORWMACH 2
#define PROC_REQ_ANSWMACH 3
#define PROC_REQ_ANSWMACH_NOT_LOGGED 4
#define PROC_REQ_ANSWMACH_NOT_HERE 5

/* Min value to launch answer machine : */
#define PROC_REQ_MIN_A 3

#include "options.h"

#endif /* __DEFS_H */
