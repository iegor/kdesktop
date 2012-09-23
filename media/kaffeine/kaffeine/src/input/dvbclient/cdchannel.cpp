/*
 * cdchannel.cpp
 *
 * Copyright (C) 2005 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "cdchannel.h"



CdChannel::CdChannel()
{
	name = "";
	vpid = apid = 0;
	ac3 = 0;
	subpid = page = id = 0;
	type = 0;
	lang = "";
}



CdChannel::CdChannel( const QString &n, int vp, int ap, int ac, int spid, int pg, int an, int tp, const QString &lg )
{
	name = n;
	vpid = vp;
	apid = ap;
	ac3 = ac;
	subpid = spid;
	page = pg;
	id = an;
	type = tp;
	lang = lg;
}



CdChannel::~CdChannel()
{
}
