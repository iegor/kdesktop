/*

    Copyright (C) 2001-2002 Stefan Westerfeld
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

#include <math.h>

#include <arts/debug.h>
#include <arts/fft.h>
#include <arts/stdsynthmodule.h>
#include <arts/connect.h>
#include "artsmoduleseffects.h"

#include <stdio.h>
#include <stdlib.h>

#include <kglobal.h>
#include <klocale.h>

using namespace std;
using namespace Arts;

static inline bool odd(int x) { return ((x & 1) == 1); }

/* returns a blackman window: x is supposed to be in the interval [0..1] */
static inline float blackmanWindow(float x)
{
	if(x < 0) return 0;
	if(x > 1) return 0;
	return 0.42-0.5*cos(M_PI*x*2)+0.08*cos(4*M_PI*x);
}

void firapprox(double *filter, int filtersize, vector<GraphPoint>& points)
{
	assert((filtersize >= 3) && odd(filtersize));

	int fft_size = 8;
	while(fft_size/2 < filtersize)
		fft_size *= 2;

	vector<GraphPoint>::iterator pi = points.begin();
	float lfreq=-2, lval=1.0, rfreq=-1, rval=1.0;

	float *re = (float*) malloc(fft_size * sizeof(float));
	for(int i=0;i<fft_size/2;i++)
	{
		float freq = float(i)/float(fft_size/2);

		while(freq > rfreq && pi != points.end())
		{
			lfreq = rfreq; rfreq = pi->x;
			lval = rval; rval = pi->y;
			pi++;
		}
		float pos = (freq-lfreq)/(rfreq-lfreq);
		float val = lval*(1.0-pos) + rval*pos;

		//printf("%f %f\n",freq,val);
		re[i] = re[fft_size-1-i] = val;
	}

	float *filter_re = (float*) malloc(fft_size * sizeof(float));
	float *filter_im = (float*) malloc(fft_size * sizeof(float));
	arts_fft_float (fft_size, 1, re, 0, filter_re, filter_im);

	for(int i=0;i<filtersize;i++)
	{
		filter[i] = filter_re[(i+fft_size-filtersize/2) & (fft_size-1)]
				  *	blackmanWindow(float(i+1)/float(filtersize+1));
	}
        free(re);
        free(filter_re);
        free(filter_im);
}

namespace Arts {

class Synth_STEREO_FIR_EQUALIZER_impl
	: virtual public Synth_STEREO_FIR_EQUALIZER_skel,
	  virtual public StdSynthModule
{
	vector<GraphPoint> _frequencies;
	long _taps;
	unsigned long bpos;
	double filter[256];
	float lbuffer[256];
	float rbuffer[256];

public:
	Synth_STEREO_FIR_EQUALIZER_impl()
	{
		_frequencies.push_back(GraphPoint(0.0,1.0));
		_frequencies.push_back(GraphPoint(1.0,1.0));
		_taps = 3;
		for(bpos = 0; bpos < 256; bpos++)
			lbuffer[bpos] = rbuffer[bpos] = 0.0;

		calcFilter();
	}
	vector<GraphPoint> *frequencies() {
		return new vector<GraphPoint>(_frequencies);
	}
	void frequencies(const vector<GraphPoint>& newFrequencies)
	{
		_frequencies = newFrequencies;

		calcFilter();
	}
	long taps()
	{
		return _taps;
	}
	void taps(long newTaps)
	{
		arts_return_if_fail(newTaps >= 3 && newTaps <= 255);

		if(!odd(newTaps))
			newTaps++;
		_taps = newTaps;

		calcFilter();
	}
	void calcFilter()
	{
		firapprox(filter, _taps, _frequencies);
	}
	void calculateBlock(unsigned long samples)
	{
		for(unsigned i=0;i<samples;i++)
		{
			double lval = 0.0;
			double rval = 0.0;
			lbuffer[bpos & 255] = inleft[i];
			rbuffer[bpos & 255] = inright[i];

			for(int j=0;j<_taps;j++)
			{
				lval += lbuffer[(bpos-j) & 255] * filter[j];
				rval += rbuffer[(bpos-j) & 255] * filter[j];
			}
			outleft[i] = lval;
			outright[i] = rval;
			bpos++;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_STEREO_FIR_EQUALIZER_impl);

class StereoFirEqualizerGuiFactory_impl : public StereoFirEqualizerGuiFactory_skel
{
public:
	Widget createGui(Object equalizer);
};

REGISTER_IMPLEMENTATION(StereoFirEqualizerGuiFactory_impl);

}

Widget StereoFirEqualizerGuiFactory_impl::createGui(Object object)
{
	KGlobal::locale()->insertCatalogue( "artsmodules" );
	arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

	Synth_STEREO_FIR_EQUALIZER equalizer = DynamicCast(object);
	arts_return_val_if_fail(!equalizer.isNull(), Arts::Widget::null());

	VBox vbox;
	vbox.show();

	Graph g;
	g.parent(vbox);
	g.width(400);
	g.height(230);
	g.caption(i18n("a graph").utf8().data());
	g.minx(0.0);
	g.maxx(1.0);
	g.miny(0.0);
	g.maxy(1.0);
	g.show();

	GraphLine gline;
	gline.graph(g);
	vector<GraphPoint> *points = equalizer.frequencies();
	gline.points(*points);
	delete points;
	gline.color("red");
	gline.editable(true);

	connect(gline,"points_changed", equalizer, "frequencies");
	g._addChild(gline,"gline");

	SpinBox spinbox;
	spinbox.caption(i18n("channels").utf8().data());
	spinbox.min(3); spinbox.max(255);
	spinbox.value(equalizer.taps());
	spinbox.parent(vbox);
	spinbox.show();
	connect(spinbox,"value_changed", equalizer, "taps");
	vbox._addChild(spinbox,"spinbox");
	vbox._addChild(g,"g");

	return vbox;
}
