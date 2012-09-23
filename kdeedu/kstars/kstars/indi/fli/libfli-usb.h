/*

  Copyright (c) 2002 Finger Lakes Instrumentation (FLI), L.L.C.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

        Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

        Redistributions in binary form must reproduce the above
        copyright notice, this list of conditions and the following
        disclaimer in the documentation and/or other materials
        provided with the distribution.

        Neither the name of Finger Lakes Instrumentation (FLI), LLC
        nor the names of its contributors may be used to endorse or
        promote products derived from this software without specific
        prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

  ======================================================================

  Finger Lakes Instrumentation, L.L.C. (FLI)
  web: http://www.fli-cam.com
  email: support@fli-cam.com

*/

#ifndef _LIBFLI_USB_H_
#define _LIBFLI_USB_H_

#define FLI_CMD_ENDPOINT 2

#if defined(__linux__)
#define unix_bulkwrite linux_bulkwrite
#define unix_bulkread linux_bulkread

#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__)
#define unix_bulkwrite bsd_bulkwrite
#define unix_bulkread bsd_bulkread

#else
#define unix_bulkwrite null_bulkwrite
#define unix_bulkread null_bulkread

#warning "using null I/O operations!"
#endif

long unix_bulkwrite(flidev_t dev, void *buf, long *wlen);
long unix_bulkread(flidev_t dev, void *buf, long *rlen);
long unix_usbio(flidev_t dev, void *buf, long *wlen, long *rlen);
long unix_usbverifydescriptor(flidev_t dev, fli_unixio_t *io);

#endif /* _LIBFLI_USB_H_ */
