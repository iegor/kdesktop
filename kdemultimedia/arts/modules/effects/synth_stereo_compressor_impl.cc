/*  This file is part of the KDE project
    Copyright (C) 2002 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

*/

#include "artsmoduleseffects.h"
#include <stdsynthmodule.h>
#include <flowsystem.h>
#include <debug.h>

using namespace Arts;

namespace Arts {

class Synth_STEREO_COMPRESSOR_impl : virtual public Synth_STEREO_COMPRESSOR_skel,
                                     virtual public StdSynthModule
{
public:
	Synth_STEREO_COMPRESSOR_impl()
	{
		attack( 10 );
		release( 10 );
		threshold( 1 );
		ratio( 0.8 );
		output( 1 );
		_thru = false;
		_run = false;
	}

	void streamStart()
	{
		_run = true;
		compleft.start();
		compright.start();
		if(!_thru)
			connectComp(true);
		else
			connectThru(true);
	}

	void streamEnd()
	{
		_run = false;
		connectComp(false);
		connectThru(false);
		compleft.stop();
		compright.stop();
	}

	float attack() { return compleft.attack(); };
	void attack( float f ) { compleft.attack(f); compright.attack(f); }
	float release() { return compleft.release(); };
	void release( float f ) { compleft.release(f); compright.release(f); }
	float threshold() { return compleft.threshold(); };
	void threshold( float f ) { compleft.threshold(f); compright.threshold(f); }
	float ratio() { return compleft.ratio(); };
	void ratio( float f ) { compleft.ratio(f); compright.ratio(f); }
	float output() { return compleft.output(); };
	void output( float f ) { compleft.output(f); compright.output(f); }

	bool thru() { return _thru; }
	void thru( bool f )
	{
		if( _thru != f )
		{
			if(!_thru)
				connectComp(false);
			else
				connectThru(false);
			_thru = f;
			if(!_thru)
				connectComp(true);
			else
				connectThru(true);
		}
	}

private:
	Synth_COMPRESSOR compleft, compright;
	bool _thru;
	bool _run;

	void connectComp( bool _connect )
	{
		if(_connect)
		{
			_node()->virtualize("inleft",compleft._node(),"invalue");
			_node()->virtualize("inright",compright._node(),"invalue");
			_node()->virtualize("outleft",compleft._node(),"outvalue");
			_node()->virtualize("outright",compright._node(),"outvalue");
		}
		else
		{
			_node()->devirtualize("inleft",compleft._node(),"invalue");
			_node()->devirtualize("inright",compright._node(),"invalue");
			_node()->devirtualize("outleft",compleft._node(),"outvalue");
			_node()->devirtualize("outright",compright._node(),"outvalue");
		}
	}

	void connectThru( bool _connect )
	{
		if(_connect)
		{
			_node()->virtualize("inleft",_node(),"outleft");
			_node()->virtualize("inright",_node(),"outright");
		}
		else
		{
			_node()->devirtualize("inleft",_node(),"outleft");
			_node()->devirtualize("inright",_node(),"outright");
		}
	}

};

// vim:sw=4:ts=4

REGISTER_IMPLEMENTATION(Synth_STEREO_COMPRESSOR_impl);

}
