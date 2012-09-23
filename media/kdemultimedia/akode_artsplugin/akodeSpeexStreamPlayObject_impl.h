/*  akodeSpeexStreamPlayObject

    Copyright (C) 2004 Allan Sandfeld Jensen <kde@carewolf.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef _AKODESPEEXSTREAMPLAYOBJECT_IMPL_H
#define _AKODESPEEXSTREAMPLAYOBJECT_IMPL_H

#include "akodePlayObject_impl.h"
#include "akodearts.h"

#include <akode/decoder.h>

class akodeSpeexStreamPlayObject_impl
    : virtual public akodeSpeexStreamPlayObject_skel, public akodePlayObject_impl
{
public:
	akodeSpeexStreamPlayObject_impl();
        bool loadSource();
protected:
        aKode::DecoderPlugin *speex_plugin;
};

#endif
