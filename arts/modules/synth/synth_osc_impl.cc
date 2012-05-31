    /*

    Copyright (C) 2002 Stefan Westerfeld
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

#include "artsmodulessynth.h"
#include "debug.h"
#include "stdsynthmodule.h"
#include <gsl/gsloscillator.h>
#include <gsl/gslsignal.h>
#include <string.h>

#include <math.h>

using namespace Arts;

namespace Arts {

static double arts_gsl_window_osc(double x)
{
	const double FILTER_H = 22000.0;
	const double FILTER_L = 19000.0;
	double f = 22050.0 * fabs(x), fact;

	if(f > FILTER_H)
		fact = 0.0;
	else if (f < FILTER_L)
		fact = 1.0;
	else
		fact = cos(M_PI/2.0*((FILTER_L-f)/(FILTER_H-FILTER_L)));

	return fact;
}

class Synth_OSC_impl :public Synth_OSC_skel, public StdSynthModule
{
private:
	GslOscConfig cfg;
	GslOscData osc;
	SynthOscWaveForm _waveForm;

	bool infrequency_connected;
	bool modulation_connected;
	bool insync_connected;
	bool outvalue_connected;
	bool outsync_connected;

	void updateConnected()
	{
		infrequency_connected = inputConnectionCount("infrequency");
		modulation_connected = inputConnectionCount("modulation");
		insync_connected = inputConnectionCount("insync");
		outvalue_connected = outputConnectionCount("outvalue");
		outsync_connected = outputConnectionCount("outsync");
	}
public:
	Synth_OSC_impl() {
		_waveForm = soWaveTriangle;

		memset(&cfg, 0, sizeof(GslOscConfig));
		memset(&osc, 0, sizeof(GslOscData));

		cfg.table = 0;
		cfg.exponential_fm = 0;
		cfg.fm_strength = 0;
		cfg.self_fm_strength = 0;
		cfg.cfreq = 440;
		cfg.fine_tune = 0;
		cfg.pulse_width = 0.5;
		cfg.pulse_mod_strength = 0;

		waveForm(soWaveSine);
	};
	void apply()
	{
		gsl_osc_config(&osc, &cfg);
	}
	void streamInit()
	{
		updateConnected();
	}
	void calculateBlock(unsigned long samples)
	{
		if(connectionCountChanged())
			updateConnected();

		arts_debug("gop tab%p samples%ld f%p m%p is%p ov%p os%p\n",
					cfg.table, samples, infrequency_connected?infrequency:0,
					         modulation_connected?modulation:0,
									   insync_connected?insync:0,
									   outvalue_connected?outvalue:0,
									   outsync_connected?outsync:0);

		gsl_osc_process(&osc, samples, infrequency_connected?infrequency:0,
				                       modulation_connected?modulation:0,
									   insync_connected?insync:0,
									   outvalue_connected?outvalue:0,
									   outsync_connected?outsync:0);
	}
	SynthOscWaveForm waveForm()
	{
		return _waveForm;
	}
	void waveForm(SynthOscWaveForm wf)
	{
		if(wf != _waveForm)
		{
			if(cfg.table)
				gsl_osc_table_free(cfg.table);

			float freqs[100];
			int n_freqs = 0;

			freqs[n_freqs] = 20;
			while (freqs[n_freqs] < 22000)
			{
				freqs[n_freqs + 1] = freqs[n_freqs] * M_SQRT2;
				n_freqs++;
			}
			arts_debug("Synth_OSC::waveForm: n_freqs = %d", n_freqs);
			cfg.table = gsl_osc_table_create(samplingRateFloat, GslOscWaveForm(wf + 1), arts_gsl_window_osc, n_freqs, freqs);
			_waveForm = wf;
			apply();
			waveForm_changed(wf);
		}
	}
	bool fmExponential()
	{
		return cfg.exponential_fm;
	}
	void fmExponential(bool newFm)
	{
		bool oldFm = fmExponential();

		if(newFm != oldFm)
		{
			cfg.exponential_fm = newFm;
			apply();
			fmExponential_changed(newFm);
		}
	}
	float fmStrength()
	{
		return cfg.fm_strength;
	}
	void fmStrength(float f)
	{
		if(cfg.fm_strength != f)
		{
			cfg.fm_strength = f;
			apply();
			fmStrength_changed(f);
		}
	}
	float fmSelfStrength()
	{
		return cfg.self_fm_strength;
	}
	void fmSelfStrength(float f)
	{
		if(cfg.self_fm_strength != f)
		{
			cfg.self_fm_strength = f;
			apply();
			fmSelfStrength_changed(f);
		}
	}
	float phase()
	{
		return cfg.phase;
	}
	void phase(float p)
	{
		if(cfg.phase != p)
		{
			cfg.phase = p;
			apply();
			phase_changed(p);
		}
	}
	float frequency()
	{
		return cfg.cfreq;
	}
	void frequency(float f)
	{
		if(cfg.cfreq != f)
		{
			cfg.cfreq = f;
			apply();
			frequency_changed(f);
		}
	}
	long fineTune()
	{
		return cfg.fine_tune;
	}
	void fineTune(long f)
	{
		if(cfg.fine_tune != f)
		{
			cfg.fine_tune = f;
			apply();
			fineTune_changed(f);
		}
	}
	float pulseWidth()
	{
		return cfg.pulse_width;
	}
	void pulseWidth(float pw)
	{
		if(cfg.pulse_width != pw)
		{
			cfg.pulse_width = pw;
			apply();
			pulseWidth_changed(pw);
		}
	}
	float pulseModStrength()
	{
		return cfg.pulse_mod_strength;
	}
	void pulseModStrength(float pms)
	{
		if(cfg.pulse_mod_strength != pms)
		{
			cfg.pulse_mod_strength = pms;
			apply();
			pulseModStrength_changed(pms);
		}
	}
};
REGISTER_IMPLEMENTATION(Synth_OSC_impl);

}
