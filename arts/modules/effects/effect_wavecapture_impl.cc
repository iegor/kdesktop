/*
	Copyright (C) 2001 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* $Id: effect_wavecapture_impl.cc 215419 2003-03-21 12:51:42Z akrille $ */

#include "artsmoduleseffects.h"
#include <stdsynthmodule.h>
#include <flowsystem.h>

using namespace std;
namespace Arts {

class Effect_WAVECAPTURE_impl : virtual public Effect_WAVECAPTURE_skel,
							  virtual public StdSynthModule
{
protected:
	Synth_CAPTURE_WAV _capture;

public:
	void streamStart();
	void streamEnd();
	string filename() { return _capture.filename(); }
	void filename( const string &newFilename ) { _capture.filename( newFilename ); }
};

void Effect_WAVECAPTURE_impl::streamStart()
{
	_capture.start();
	_node()->virtualize("inleft",_capture._node(),"left");
	_node()->virtualize("inright",_capture._node(),"right");
	_node()->virtualize("outleft",_node(),"inleft");
	_node()->virtualize("outright",_node(),"inright");
}

void Effect_WAVECAPTURE_impl::streamEnd()
{
	_node()->devirtualize("inleft",_capture._node(),"left");
	_node()->devirtualize("inright",_capture._node(),"right");
	_node()->devirtualize("outleft",_node(),"inleft");
	_node()->devirtualize("outright",_node(),"inright");
	_capture.stop();
}

REGISTER_IMPLEMENTATION(Effect_WAVECAPTURE_impl);

}

// vim:ts=4:sw=4
