/*
 * $Id: mpg123PlayObject_impl.cpp 212185 2003-03-07 22:11:39Z waba $
 * Copyright (C) 2001 Kevin Puetz
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <config.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////
// aRts interface

#include <stdsynthmodule.h>
#include "mpg123arts.h"
#include <convert.h>
#include <debug.h>


#include "mpg123PlayObject_impl.h"

using namespace Arts;

int mpg123PlayObject_impl::decoder_init = 0;

// This is to minimize the hackery in mpg123
int audio_get_formats(struct audio_info_struct *)
{
	return AUDIO_FORMAT_SIGNED_16;
}

// This is purely convenience
void mpg123PlayObject_impl::set_synth_functions(struct frame *fr)
{
	typedef int (*func)(real *,int,unsigned char *,int *);
	typedef int (*func_mono)(real *,unsigned char *,int *);
	typedef void (*func_dct36)(real *,real *,real *,real *,real *);

	int ds = fr->down_sample;
	int p8=0;
	// it's as big as pcmdata (internal to mpg123 - where this size comes from I dunno
	// but it'll be big enough, since all data comes as memcpy's from there.

	static func funcs[][4] = { 
		{ synth_1to1,
		  synth_2to1,
		  synth_4to1,
		  synth_ntom } ,
		{ synth_1to1_8bit,
		  synth_2to1_8bit,
		  synth_4to1_8bit,
		  synth_ntom_8bit } 
#ifdef USE_3DNOW
		,{synth_1to1_3dnow,
		  synth_2to1,
		  synth_4to1,
		  synth_ntom }
#endif 
	};

	static func_mono funcs_mono[2][2][4] = {    
		{ { synth_1to1_mono2stereo ,
		    synth_2to1_mono2stereo ,
		    synth_4to1_mono2stereo ,
		    synth_ntom_mono2stereo } ,
		  { synth_1to1_8bit_mono2stereo ,
		    synth_2to1_8bit_mono2stereo ,
		    synth_4to1_8bit_mono2stereo ,
		    synth_ntom_8bit_mono2stereo } } ,
		{ { synth_1to1_mono ,
		    synth_2to1_mono ,
		    synth_4to1_mono ,
		    synth_ntom_mono } ,
		  { synth_1to1_8bit_mono ,
		    synth_2to1_8bit_mono ,
		    synth_4to1_8bit_mono ,
		    synth_ntom_8bit_mono } }
	};

#ifdef USE_3DNOW      
	static func_dct36 funcs_dct36[2] = {dct36 , dct36_3dnow};
#endif

	if (0) // ((ai.format & AUDIO_FORMAT_MASK) == AUDIO_FORMAT_8)
		p8 = 1;
	fr->synth = funcs[p8][ds];
	fr->synth_mono = funcs_mono[param.force_stereo?0:1][p8][ds];

#ifdef USE_3DNOW
	arts_debug("set_synth_functions: 3dnow?");
	/* check cpuflags bit 31 (3DNow!) and 23 (MMX) */
	if((param.stat_3dnow < 2) && 
		((param.stat_3dnow == 1) ||
		(getcpuflags() & 0x80800000) == 0x80800000)) 
	{
  		fr->synth = funcs[2][ds]; /* 3DNow! optimized synth_1to1() */
			fr->dct36 = funcs_dct36[1]; /* 3DNow! optimized dct36() */
	} else {
		fr->dct36 = funcs_dct36[0];
	}
#endif

	if (p8) 
	{
		make_conv16to8_table(-1); // FIX
	}
}

void mpg123PlayObject_impl::initialise_decoder()
{
	arts_debug("initializing decoder");
	set_synth_functions(&mp->fr);
	make_decode_tables(param.outscale);
	init_layer2();	// inits also shared tables with layer1
	init_layer3(mp->fr.down_sample);	// No down sample support (yet?)
}

int mpg123PlayObject_impl::play_frame(int init) 
{
	struct frame *fr = &mp->fr;
	int clip;
	long newrate;
	long old_rate,old_format,old_channels;

	if(fr->header_change || init) {

		if(fr->header_change > 1 || init) {
			old_rate = ai.rate;
			old_format = ai.format;
			old_channels = ai.channels;

			newrate = (long)param.pitch * (freqs[fr->sampling_frequency]>>(param.down_sample));
			if(param.verbose && param.pitch != 1.0)
				fprintf(stderr,"Pitching to %f => %ld Hz\n",param.pitch,newrate);   

			fr->down_sample = param.down_sample;

			ai.format = AUDIO_FORMAT_SIGNED_16;
			ai.rate = 44100;
			ai.channels = 2;

			/* check, whether the fitter setted our proposed rate */
			if(ai.rate != newrate) {
				arts_debug("resampling from %d to %d",newrate, ai.rate);
				if(ai.rate == (newrate>>1) )
					fr->down_sample++;
				else if(ai.rate == (newrate>>2) )
					fr->down_sample+=2;
				else {
					fr->down_sample = 3;
					fprintf(stderr,"Warning, flexible rate not heavily tested!\n");
				}
				if(fr->down_sample > 3)
					fr->down_sample = 3;
			}

			if(fr->down_sample > 3)
				fr->down_sample = 3;

			switch(fr->down_sample) {
				case 0:
				case 1:
				case 2:
					fr->down_sample_sblimit = SBLIMIT>>(fr->down_sample);
					break;
				case 3:
					{
						long n = (long)param.pitch * freqs[fr->sampling_frequency];
						long m = ai.rate;

						synth_ntom_set_step(n,m);

						if(n>m) {
							fr->down_sample_sblimit = SBLIMIT * m;
							fr->down_sample_sblimit /= n;
						}
						else {
							fr->down_sample_sblimit = SBLIMIT;
						}
					}
					break;
			}

			set_synth_functions(fr);
			//init_output(); XXX: eh?
			if(ai.rate != old_rate || ai.channels != old_channels ||
					ai.format != old_format || param.force_reopen) {
				if(param.force_mono < 0) {
					if(ai.channels == 1)
						fr->single = 3;
					else
						fr->single = -1;
				}
			}
			else
				fr->single = param.force_mono;

			param.force_stereo &= ~0x2;
			if(fr->single >= 0 && ai.channels == 2) {
				param.force_stereo |= 0x2;
			}

			set_synth_functions(fr);
			init_layer3(fr->down_sample_sblimit);
			//	reset_audio(); XXX: wha?
			if(param.verbose) {
				if(fr->down_sample == 3) {
					long n = (long)param.pitch * freqs[fr->sampling_frequency];
					long m = ai.rate;
					if(n > m) {
						fprintf(stderr,"Audio: %2.4f:1 conversion,",(float)n/(float)m);
					}
					else {
						fprintf(stderr,"Audio: 1:%2.4f conversion,",(float)m/(float)n);
					}
				}
				else {
					fprintf(stderr,"Audio: %ld:1 conversion,",(long)pow(2.0,fr->down_sample));
				}
				fprintf(stderr," rate: %ld, encoding: %s, channels: %d\n",ai.rate,audio_encoding_name(ai.format),ai.channels);
			}
		}
	}

	if (fr->error_protection) {
		/* skip crc, we are byte aligned here */
		getbyte(&bsi);
		getbyte(&bsi);
	}

	/* do the decoding */
	switch(fr->lay) {
		case 1:
			if( (clip=do_layer1(mp,fr,param.outmode,&ai)) < 0 )
				return 0;
			break;
		case 2:
			if( (clip=do_layer2(mp,fr,param.outmode,&ai)) < 0 )
				return 0;
			break;
		case 3:
			if( (clip=do_layer3(mp,fr,param.outmode,&ai)) < 0 )
				return 0;
			break;
		default:
			clip = 0;
	}

	if(clip > 0 && param.checkrange)
		fprintf(stderr,"%d samples clipped\n", clip);

	return pcm_point / 4;
}

mpg123PlayObject_impl::mpg123PlayObject_impl()
{
	pcm_buf = new unsigned char[16384*2+1024*2];
	mp = (struct mpstr *)malloc(sizeof(struct mpstr));
	memset(mp, 0, sizeof(struct mpstr));
	//memset(&mp->fr, 0, sizeof(struct frame)); XXX: why would I need to do this?

	prgName = strdup("arts/mpg123");
	prgVersion = strdup("$Revision: 212185 $");
	pcm_point = 0;
	pcm_sample=pcm_buf; // just point this to our internal buffer
	memset(&param, 0, sizeof(struct parameter));
	param.halfspeed = 0;
	param.outmode = DECODE_BUFFER+999;
	param.usebuffer = 0;
	param.down_sample = 0;
	param.force_stereo = 1; // XXX was 0
	param.force_mono = -1;
	param.pitch = 1.0;
	param.checkrange = 0;
	param.outscale = 32768;
	param.tryresync = 2;
	
	equalfile = NULL;
	struct shmid_ds bleh;
	shm_id = shmget(IPC_PRIVATE, sizeof(*shm_buf), 0600);
	shm_buf = (struct buf_t *)shmat(shm_id, 0, 0);
	// mark it to be destroyed after the last detach
	shmctl(shm_id, IPC_RMID, &bleh);
	// sem0 has base, sem1 remaining space,sem2 seekTo
	buflen_sem = semget(IPC_PRIVATE, 3, 0600);
	child_pid = 0;
}

mpg123PlayObject_impl::~mpg123PlayObject_impl()
{
	artsdebug("Destroying PlayObject");
	halt();
	union semun semdat;
	arts_debug("removing IPC resources");
	semctl(buflen_sem,0,IPC_RMID,semdat);
        // WABA: Don't remove the cast, it is needed on some platforms.
	shmdt((char *)shm_buf);
	delete pcm_buf;
}

bool mpg123PlayObject_impl::loadMedia(const string &filename)
{
//	string filename = "http://131.174.33.2:9024/";
	arts_debug("mpg123: loadMedia %s", filename.c_str());
	halt(); // stop playing any previous stream
	arts_debug("previous playback killed");
	struct sembuf semoper;
	union semun semdat;

	semoper.sem_flg = 0; // normal blocking semaphores

	semdat.val = 0;
	if(semctl(buflen_sem,0,SETVAL,semdat)) // no data in the queue 
		arts_debug("couldn't clear queue");
	semdat.val = 0;
	if(semctl(buflen_sem,2,SETVAL,semdat)) // seekTo is -1 (ie, no seek)
		arts_debug("couldn't clear seekTo");
	semdat.val = BACKBUFSIZ;
	if(semctl(buflen_sem,1,SETVAL,semdat)) // setup the starting free space
		arts_debug("couldn't mark buffer empty"); 

	buf_pos = 0;

	//throw off a process to handle the decoding
	if((child_pid = fork())) {
		return true; // all further setup happens in the child
	}
	arts_debug("child process");
	initialise_decoder();

	snprintf(param.filename, 250, filename.c_str());
	memset(&ai, 0, sizeof(struct audio_info_struct));
	mp->fr.sampling_frequency = 0;
	mp->fr.down_sample = 0;
	mp->fr.single = -1;
	mp->fr.down_sample_sblimit = SBLIMIT>>(mp->fr.down_sample);
	sample_freq = freqs[mp->fr.sampling_frequency]>>(param.down_sample);

	// audio_info_struct_init
	ai.rate = 44100;
	ai.gain = -1;
	ai.output = AUDIO_OUT_LINE_OUT;
	ai.device = 0;
	ai.channels = 2;
	ai.format = AUDIO_FORMAT_SIGNED_16;
	audio_capabilities(&ai);

	set_synth_functions(&mp->fr);

	if (rd)
		rd->close(rd);
	if (!open_stream(filename.c_str(), -1)) {
		printf("erorr opening stream\n");
		return 0;
	}

	mpeg_name[0] = 0;
	snprintf(mpeg_name, 1000, filename.c_str());	
	if (strstr(filename.c_str(), "http://") != NULL) {
		sprintf(mpeg_name, "ShoutCast from %s\n", filename.c_str());
		streaming = 1;
	}

	read_frame_init(&mp->fr);

	XHEADDATA xingHeader;

	shm_buf->pos = 0;

	read_frame(rd,&mp->fr); // read in a frame for the xing code to play with
	bool gotXing = false;
	if(!streaming) {
		gotXing = mpg123_stream_check_for_xing_header(&mp->fr,&xingHeader);
	if(gotXing) 
		shm_buf->len = xingHeader.frames;
	else // assume no VBR for non-Xing
		shm_buf->len = static_cast<unsigned long>(rd->filelen / compute_bpf(&mp->fr));
	} else {
		shm_buf->len = 1;
	}
	// can't calculate tpf until we reach a non-header frame

	int skipped = 0;
	if (sync_stream(rd, &mp->fr, 0xffff, &skipped) <= 0) {
		fprintf(stderr,"Can't find frame start");
		rd->close(rd);
		return 0;
	}


/*
	if (!mpeg_get_frame_info(filename.c_str())) {
		printf("mpeg_get_frame_info(%s) failed\n", filename.c_str());
		return 0;
	}
*/
	arts_debug("mpg123: loadMedia %s got %s", filename.c_str(), mpeg_name);
	
	short *decode_buf = reinterpret_cast<short *>(pcm_sample);
	bool init=true;
		do {
			// get more data

			int seekTo = semctl(buflen_sem, 2, GETVAL, semdat);
			if (seekTo) { // we need to seek, to sync back up
				unsigned long offset;
				arts_debug("seeking to %d\n", seekTo);
				if(gotXing && (xingHeader.flags & TOC_FLAG) && xingHeader.toc) // do we have a table of contents?
					offset = mpg123_seek_point(xingHeader.toc,rd->filelen,100 * (seekTo -1) / double(shm_buf->len)); // relative position in file
				else
					offset = static_cast<unsigned long>(rd->filelen * ((seekTo-1) / double(shm_buf->len))); // the crude ole' fashioned way
				rd->rewind(rd);
				lseek(rd->filept, offset, SEEK_SET);

				// now we need to sync back up :-)
				read_frame(rd,&mp->fr); 
				read_frame(rd,&mp->fr); 
			//	if (sync_stream(rd, &mp->fr, 0xffff, &skipped) <= 0) {
			//		arts_debug("Can't find frame start");
		//			rd->close(rd);
		//			break;
		//		}
				shm_buf->pos = seekTo; // assume we got the frame we were after? I don't have a better idea...
				semdat.val = 0;
				semctl(buflen_sem, 2, SETVAL, semdat); // we've done it
			}

			if (!read_frame(rd,&mp->fr)) {
				// mpg123 says we're done, or we errored (in which case we're done)
				arts_debug("out of frames, exiting");
				break;
			}

			if(init) // need to figure this one out with a real audio frame...
			{	
				arts_debug("samplerate: %d (%d)",mp->fr.sampling_frequency,freqs[mp->fr.sampling_frequency]>>(param.down_sample));
				shm_buf->tpf = compute_tpf(&mp->fr);
			}
			int thisPass = play_frame(init);
			if(init) // need to figure this one out with a real audio frame...
				arts_debug("samplerate: %d",mp->fr.sampling_frequency);
			init=false; // no longer init :-)

			semoper.sem_num = 1;
			semoper.sem_op = -thisPass; 
			semop(buflen_sem, &semoper, 1);

			// block until there's enough space to stick in this frame
			int roomFor = semctl(buflen_sem, 1, GETVAL, semdat);
			if (roomFor > BACKBUFSIZ) { 
				arts_debug("exit requested (%d slots available), bye!",roomFor);
				// this can never go above BACKBUFSIZ in normal operation, 
				// the consumer wants us to exit
				break;
			}

			//arts_debug("decoded %d frames (%d avail)",thisPass,roomFor);
				
			for(int i=0 ; i <thisPass ; 
			    ++i, buf_pos = ((buf_pos + 1) % BACKBUFSIZ)) {
				shm_buf->left[buf_pos] = conv_16le_float(decode_buf[2*i]);
				shm_buf->right[buf_pos] = conv_16le_float(decode_buf[2*i+1]);
			}
			shm_buf->pos++; // ran another frame through the mill
			pcm_point=0;
			//arts_debug("enqueued them");
			semoper.sem_num = 0;
			semoper.sem_op = thisPass; 
			semop(buflen_sem,&semoper,1); // mark the additional data now available

			//arts_debug("calculated %d more samples",shm_buf->backbuflen );
		} while(1);

		//signal completion
		semdat.val = 0;
		// no more data available
		semctl(buflen_sem, 0, SETVAL, semdat); 
		// and no room either (ie, none coming)
		semctl(buflen_sem, 1, SETVAL, semdat); 

		arts_debug("decoder process exiting");
		exit(0);
	return true;
}

string mpg123PlayObject_impl::description()
{
	return "mpg123 artsplug - w00t!";
}

//XXX
poTime mpg123PlayObject_impl::currentTime()
{
	return poTime(shm_buf->pos * shm_buf->tpf, 0, 0, "none");
}

//XXX
poTime mpg123PlayObject_impl::overallTime()
{
	return poTime(shm_buf->len * shm_buf->tpf, 0, 0, "none");
}

poCapabilities mpg123PlayObject_impl::capabilities()
{
	return static_cast<poCapabilities>(capPause | capSeek);
}

//YYY
string mpg123PlayObject_impl::mediaName()
{
	return param.filename;
}

poState mpg123PlayObject_impl::state()
{
	return mState;
}

void mpg123PlayObject_impl::play()
{
	arts_debug("mpg123: play");
	mState = posPlaying;
}

void mpg123PlayObject_impl::halt()
{
	mState = posIdle;
	union semun semdat;

	if (child_pid) {
		arts_debug("killing decoder process");
		semdat.val = 2*BACKBUFSIZ;
		semctl(buflen_sem, 1, SETVAL, semdat); 
		waitpid(child_pid, NULL, 0);
		child_pid = 0;
	}
	// tell the producer to exit (would also result in us halting, if we weren't already)

	// mainly this is to ensure that the decoder wakes up to notice
}

//XXX disabled for now
void mpg123PlayObject_impl::seek(const class poTime &t)
{
	union semun foo;

	// this index is one-based so 0 can represent no seek
	foo.val = static_cast<int>(t.seconds / shm_buf->tpf + 1);
	arts_debug("requesting seek to %d", foo.val);
	semctl(buflen_sem, 2, SETVAL, foo); // we've done it
}

/* Pause implemented on the streaming-side - decoding will simply block on it's own */
void mpg123PlayObject_impl::pause()
{
	mState = posPaused;
}

/*
 * SynthModule interface
 * - where is stop? initialize?
 */

void mpg123PlayObject_impl::streamInit()
{
	arts_debug("streamInit");
}

void mpg123PlayObject_impl::streamStart()
{
	arts_debug("streamStart");
}

void mpg123PlayObject_impl::calculateBlock(unsigned long samples)
{
	int samplesAvailable = 0;
	
	//arts_debug("calculateBlock");
	
	if (mState==posPlaying) {
		//arts_debug("calculateBlock, %d(%d) of %d samples in buffer", 
		//shm_buf->buflen - bufpos, shm_buf->backbuflen,samples);
			
		struct sembuf bleh;
				
		bleh.sem_num = 0;
		bleh.sem_flg = IPC_NOWAIT;
						
		//arts_debug("%d samples wanted", samplesAvailable);
		bleh.sem_op = -samples; // does the buffer have sufficient samples?
		if (semop(buflen_sem, &bleh, 1) == -1) {
			if (errno == EAGAIN) {
				union semun semdat;
				arts_debug("buffer underrun");
//				samplesAvailable = semctl(buflen_sem,0,GETVAL,semdat);
//				if (semctl(buflen_sem, 1, GETVAL, semdat) == 0) {
//					samplesAvailable = semctl(buflen_sem,0,GETVAL,semdat);
				if ((semctl(buflen_sem, 1, GETVAL, semdat) == 0) && (semctl(buflen_sem,0,GETVAL,semdat) == 0))
				{
					arts_debug("decoder requested exit");
					// no samples AND no room is the decoder's way of signalling completion
					halt();
		//		samplesAvailable = 0;
				}
					samplesAvailable = 0; // 
			} else {
				arts_debug("something awful happened to our semaphores...");
				// something awful has happened
				halt();
				samplesAvailable = 0;
			}
		} else {
			samplesAvailable = samples; // number of samples we pushed from buffers
			// used to calculate the number we should zero out for an underrun
		}
		bleh.sem_flg = 0; // back to normal now
		
		if(samplesAvailable) {
			//arts_debug("%d samples available",samplesAvailable);
			for (int i = 0; i < samplesAvailable; 
					++i, buf_pos = ((buf_pos + 1) % BACKBUFSIZ)) {

				left[i] = shm_buf->left[buf_pos];
				right[i] = shm_buf->right[buf_pos];
			}

			bleh.sem_num = 1;
			bleh.sem_op = samplesAvailable; 
			// 0 here CAN block, which is why this is in an if(samplesAvailable)
			semop(buflen_sem, &bleh, 1); // mark the now-free space
		}
	}
	// zero out any samples we didn't have enough to complete - no buzz of death!
	while(static_cast<unsigned long>(samplesAvailable) < samples) {
		left[samplesAvailable] = 0.0;
		right[samplesAvailable] = 0.0;
		samplesAvailable++;
	}
}

void mpg123PlayObject_impl::streamEnd()
{
	arts_debug("streamEnd");
}

REGISTER_IMPLEMENTATION(mpg123PlayObject_impl);

