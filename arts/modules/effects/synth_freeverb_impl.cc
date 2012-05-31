    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

#include "freeverb/revmodel.hpp"
#include "artsmoduleseffects.h"
#include "stdsynthmodule.h"

using namespace Arts;

class Synth_FREEVERB_impl : virtual public Synth_FREEVERB_skel,
							virtual public StdSynthModule
{
	revmodel *model;
public:
	float	roomsize()				{ return model->getroomsize(); }
	void	roomsize(float newval)	{ return model->setroomsize(newval); }

	float	damp()					{ return model->getdamp(); }
	void	damp(float newval)		{ return model->setdamp(newval); }

	float	wet()					{ return model->getwet(); }
	void	wet(float newval)		{ return model->setwet(newval); }

	float	dry()					{ return model->getdry(); }
	void	dry(float newval)		{ return model->setdry(newval); }

	float	width()					{ return model->getwidth(); }
	void	width(float newval)		{ return model->setwidth(newval); }

	float	mode()					{ return model->getmode(); }
	void	mode(float newval)		{ return model->setmode(newval); }

	void streamInit()
	{
		/* prevent full buffers to be carried over stop-start sequence */
		model->mute();
	}

	void calculateBlock(unsigned long samples)
	{
		model->processreplace(inleft, inright, outleft, outright, samples,1);
		// don't add the original signal - that's what the "dry" argument is for
		//for(unsigned long i = 0;i < samples; i++)
		//{
			//outleft[i] += inleft[i];
			//outright[i] += inright[i];
		//}
	}
	Synth_FREEVERB_impl()
	{
		// "revmodel" object size is far too big, vtable
		// can't handle it
		model=new revmodel;
		// set dry to 1 so it at first sounds like it always did before
		// ok, since scaledry = 2 calling dry( 0.5 ) has the desired
		// effect
		dry( 0.5f );
	}
	~Synth_FREEVERB_impl()
	{
		delete model;
	}
};

REGISTER_IMPLEMENTATION(Synth_FREEVERB_impl);
