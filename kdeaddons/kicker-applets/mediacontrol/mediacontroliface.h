/***************************************************************************
                      dcopinterface for mediacontrol
              mainly used to be informed about new settings
                             -------------------
    begin                : Mon Jan 15 21:09:00 MEZ 2001
    copyright            : (C) 2001 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MEDIACONTROLIFACE_H
#define MEDIACONTROLIFACE_H

#include <dcopobject.h>

class MediaControlIface : virtual public DCOPObject
{
	K_DCOP

	k_dcop:
		virtual void reparseConfig() = 0;
};

#endif // MEDIACONTROLIFACE_H
