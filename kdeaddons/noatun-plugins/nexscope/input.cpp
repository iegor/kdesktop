
#include "nex.h"
#include "convolve.h"

#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <noatun/plugin.h>

Input::Input()
{
	memset(haystack, 0, sizeof(float)*512);
	state=new convolve_state;
	
	float d, *costab, *sintab;
	long ul, ndiv2 = 512 / 2;

	for (costab = fhtTab, sintab = fhtTab + 512 / 2 + 1, ul = 0; ul < 512; ul++)
	{ 
		d = PI * ul / ndiv2;
		*costab = *sintab = ::cos(d);

		costab += 2, sintab += 2;
		if (sintab > fhtTab + 512 * 2)
			sintab = fhtTab + 1;
	}

	notifier=0;
	mScope=0;
	
	connect();
}

Input::~Input()
{
	delete mScope;
	std::cerr<< "Scope gone"<< std::endl;
	delete notifier;
//	delete [] audio[2];
//	delete [] audio[3];
//	delete [] audio[4];
//	delete [] audio[5];
}

void Input::setConvolve(bool on)
{
	mConvolve=on;
}

void Input::connect()
{
	do 
	{
		delete notifier;
		delete mScope;
		ok=true;
		mScope=new StereoScope(10, Visualization::noatunPid());
		mScope->setSamples(samples>(512+256) ? samples : samples+256);

		notifier=new QObject;
		new BoolNotifier(&ok, new ExitNotifier(Visualization::noatunPid(), notifier), notifier);
	} while (!mScope->connected());
}

// [0] pcm left
// [1] pcm right
// [2] pcm center
// [3] FFT (FHT) left
// [4] FFT right
// [5] FFT center

void Input::getAudio(float **audio)
{
	static bool first=true;
	if (first)
	{
		audio[2]=new float[samples];
		audio[3]=new float[fhtsamples];
		audio[4]=new float[fhtsamples];
		audio[5]=new float[fhtsamples];
		first=false;
	}
		
	if (!ok || !mScope->connected())
	{
		std::cerr << "reconnect" <<std::endl;
		connect();
		return;
//		delete mScope;
//		::sleep(7);
//	    mScope=new StereoScope(10, Visualization::noatunPid());
//	    mScope->setSamples(samples);
	}
	std::vector<float> *left, *right;
	mScope->scopeData(left, right);
	
	register float *inleft=&*left->begin();
	register float *inright=&*right->begin();

	
	int offset=0;
	if (mConvolve)
	{ // find the offset
		for (register int i=0; i<512+256; ++i)
			temp[i]=inleft[i]+inright[i];
		offset=::convolve_match(haystack, temp, state);
		if (offset==-1) offset=0;
		inleft+=offset;
		inright+=offset;

		for (register int i=0; i<512; ++i)
		{
			haystack[i]*=.5;
			haystack[i]+=temp[i+offset];
		}
	}
	
	memcpy(outleft, inleft, samples*sizeof(float));
	memcpy(outright, inright, samples*sizeof(float));
		
	audio[0]=outleft;
	audio[1]=outright;
	
	// center channel
	for (int i=0; i<samples; ++i)
		audio[2][i]=(audio[0][i]+audio[1][i])*.5;

	// perform the FFT (FHT in this case)
	memcpy(audio[3], audio[0], fhtsamples);
	memcpy(audio[4], audio[1], fhtsamples);

	fht(audio[3]);
	fht(audio[4]);
	
	for (int i=0; i<fhtsamples; ++i)
	{
		audio[5][i]=(audio[3][i]+audio[4][i])*.5;
	}

	
	delete left;
	delete right;
}

void Input::fht(float *p)
{
	long i;
	float *q;
	transform(p, fhtsamples, 0);

	*p = (*p * *p), *p += *p, p++;

	for (i = 1, q = p + fhtsamples - 2; i < (fhtsamples / 2); i++, --q)
		*p++ = (*p * *p) + (*q * *q);

	for (long i = 0; i < (fhtsamples / 2); i++)
		*p++ *= .5;
}

void Input::transform(float *p, long n, long k)
{
	if (n == 8)
	{
		transform8(p + k);
		return;
	}

	long i, j, ndiv2 = n / 2;
	float a, *t1, *t2, *t3, *t4, *ptab, *pp;

	for (i = 0, t1 = fhtBuf, t2 = fhtBuf + ndiv2, pp = &p[k]; i < ndiv2; i++)
		*t1++ = *pp++, *t2++ = *pp++;

	memcpy(p + k, fhtBuf, sizeof(float) * n);

	transform(p, ndiv2, k);
	transform(p, ndiv2, k + ndiv2);

	j = 512 / ndiv2 - 1;
	t1 = fhtBuf;
	t2 = t1 + ndiv2;
	t3 = p + k + ndiv2;
	ptab = fhtTab;
	pp = p + k;

	a = *ptab++ * *t3++;
	a += *ptab * *pp;
	ptab += j;

	*t1++ = *pp + a;
	*t2++ = *pp++ - a;

	for (i = 1, t4 = p + k + n; i < ndiv2; i++, ptab += j)
	{
		a = *ptab++ * *t3++;
		a += *ptab * *--t4;

		*t1++ = *pp + a;
		*t2++ = *pp++ - a;
	}

	memcpy(p + k, fhtBuf, sizeof(float) * n);

}

void Input::transform8(float *p)
{
	float a, b, c, d, e, f, g, h,
	      b_f2, d_h2, sqrt2 = M_SQRT2,
	      a_c_eg, a_ce_g, ac_e_g, aceg,
	      b_df_h, bdfh;

	a = *p++, b = *p++, c = *p++, d = *p++;
	e = *p++, f = *p++, g = *p++, h = *p;
	b_f2 = (b - f) * sqrt2;
	d_h2 = (d - h) * sqrt2;

	a_c_eg = a - c - e + g;
	a_ce_g = a - c + e - g;
	ac_e_g = a + c - e - g;
	aceg = a + c + e + g;

	b_df_h = b - d + f - h;
	bdfh = b + d + f + h;

	*p = a_c_eg - d_h2;
	*--p = a_ce_g - b_df_h;
	*--p = ac_e_g - b_f2;
	*--p = aceg - bdfh;
	*--p = a_c_eg + d_h2;
	*--p = a_ce_g + b_df_h;
	*--p = ac_e_g + b_f2;
	*--p = aceg + bdfh;
}




