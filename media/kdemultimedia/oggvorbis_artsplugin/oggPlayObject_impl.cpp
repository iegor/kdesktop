/* $Id: oggPlayObject_impl.cpp 185834 2002-10-27 01:10:28Z pfeiffer $ */

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

/////////////////////////////////////////////////////////////
// aRts interface

#include <stdsynthmodule.h>
#include "oggarts.h"
#include <convert.h>
#include <debug.h>


#include "oggPlayObject_impl.h"

using namespace Arts;

oggPlayObject_impl::oggPlayObject_impl()
{
	struct shmid_ds bleh;
	shm_id = shmget(IPC_PRIVATE, sizeof(*shm_buf), 0600);
	shm_buf = (struct buf_t *)shmat(shm_id, 0, 0);
 
	// mark it to be destroyed after the last detach
	shmctl(shm_id, IPC_RMID, &bleh);

	// sem0 has base
	// sem1 remaining space
	// sem2 seekTo
	buflen_sem = semget(IPC_PRIVATE, 4, 0600);
	child_pid = 0;
}

oggPlayObject_impl::~oggPlayObject_impl()
{
	halt();
	union semun foo;
	arts_debug("oggvorbis: removing IPC resources");
	semctl(buflen_sem, 0, IPC_RMID, foo);
}

bool oggPlayObject_impl::loadMedia(const std::string &filename)
{
	halt(); // stop playing any previous stream
	currentFile = filename;

	//throw off a process to handle the decoding
	//fill the buffers first...

	short decode_buf[BACKBUFSIZ];

	struct sembuf semoper;
	semoper.sem_flg = 0;

	buf_pos = 0;
	// setup the initial semaphore joy

	union semun foo;

	foo.val = 0;
	if (semctl(buflen_sem, 0, SETVAL, foo))
		arts_debug("oggvorbis: couldn't clear queue %d, %s", errno, strerror(errno)); // no data in the queue

	foo.val = 0;
	// seekTo is -1 (ie, no seek)
	if (semctl(buflen_sem, 2, SETVAL, foo))
		arts_debug("oggvorbis: couldn't clear seekTo, %s",strerror(errno));

	semctl(buflen_sem, 3, SETVAL, foo);
	arts_debug("oggvorbis: seekTo is %d", semctl(buflen_sem, 2, GETVAL, foo));

	// setup the starting free space in the buffer
	foo.val = BACKBUFSIZ;
	if (semctl(buflen_sem, 1, SETVAL, foo))
		arts_debug("oggvorbis: couldn't mark buffer empty");

	FILE *ogg_fp;
	ogg_fp = fopen(filename.c_str(), "r");
	int current_section=0;

	ov_open(ogg_fp, &vf, NULL, 0);
	if (!(child_pid = fork())) {
		arts_debug("oggvorbis: child process");
		do {
			// get more data

			int seekTo = semctl(buflen_sem, 2, GETVAL, foo);
			if (seekTo) {
				arts_debug("oggvorbis: seeking to %d", seekTo);
				// we have a seek command
				int ret = ov_time_seek(&vf, (double)seekTo-1);
				arts_debug("oggvorbis: ov_time_seek returned: %d\n", ret);
				foo.val=0;
				semctl(buflen_sem, 2, SETVAL, foo); // we've done it
			}
			foo.val=(long)ov_time_tell(&vf);
			if (foo.val == -1) foo.val=0;
			semctl(buflen_sem, 3, SETVAL, foo);

			int thisPass = ov_read(&vf, (char *)decode_buf, BACKBUFSIZ*sizeof(short), 0, 2, 1, &current_section);
			if (thisPass == 0) {
				// we're done, or we errored (in which case we're done)
				break;
			}
			thisPass = (thisPass / 4);


			semoper.sem_num = 1;
			semoper.sem_op = -thisPass;
			semop(buflen_sem, &semoper, 1);

			// block until there's enough space to stick in this frame
			int roomFor = semctl(buflen_sem, 1, GETVAL, foo);
			if (roomFor > BACKBUFSIZ) {
				arts_debug("oggvorbis: exit requested, bye!");
				// this can never go above BACKBUFSIZ in normal operation,
				// the consumer wants us to exit
				break;
			}

			for (int i=0 ; i <thisPass ; ++i, buf_pos = ((buf_pos + 1) % BACKBUFSIZ)) {
				shm_buf->left[buf_pos] = conv_16le_float(decode_buf[2*i]);
				shm_buf->right[buf_pos] = conv_16le_float(decode_buf[2*i+1]);
			}

			//arts_debug("oggvorbis: enqueued them");
			semoper.sem_num = 0;
			semoper.sem_op = thisPass;
			semop(buflen_sem, &semoper, 1); // mark the additional data now available

			//arts_debug("oggvorbis: calculated %d more samples",shm_buf->back$
		} while(1);

		//signal completion
		foo.val = 0;
		// no more data available
		semctl(buflen_sem, 0, SETVAL, foo);
		// and no room either (ie, none coming)
		semctl(buflen_sem, 1, SETVAL, foo);

		arts_debug("oggvorbis: decoder process exiting");
		exit(0);
	}
	return true;
}

std::string oggPlayObject_impl::description()
{
        return "ogg/vorbis artsplugin - this is my w00t!";
}

poTime oggPlayObject_impl::currentTime()
{
	poTime time;
	union semun foo;
	foo.val=0;
	time.seconds = semctl(buflen_sem, 3, GETVAL, foo);
	if (time.seconds == -1)
		time.seconds = 0; // Eek infinite loop time.
	time.ms=0;
	//arts_debug("oggvorbis: time is now %d\n", time.seconds);
	return time;
}

poTime oggPlayObject_impl::overallTime() {
	poTime time;
	time.seconds=(long)ov_time_total(&vf, -1);
	time.ms=0;
	return time;
}

poCapabilities oggPlayObject_impl::capabilities() {
	return static_cast<poCapabilities>(capPause|capSeek);
}

std::string oggPlayObject_impl::mediaName() {
	return currentFile;
}

poState oggPlayObject_impl::state() {
	return mState;
}

void oggPlayObject_impl::play() {
	arts_debug("oggvorbis: play");
	mState = posPlaying;
}

void oggPlayObject_impl::halt()
{
	mState = posIdle;
	union semun foo;

	if (child_pid) {
		arts_debug("oggvorbis: killing decoder process");
		foo.val = 2*BACKBUFSIZ;
		semctl(buflen_sem, 1, SETVAL, foo);
		waitpid(child_pid, NULL, 0);
		child_pid = 0;
	}
	// tell the producer to exit (would also result in us halting, if we weren't already)

	// mainly this is to ensure that the decoder wakes up to notice
}

void oggPlayObject_impl::seek(const class poTime &t)
{
	union semun foo;

	// this index is one-based so 0 can represent no seek
	foo.val = static_cast<int>(t.seconds);
	arts_debug("requesting seek to %d", foo.val);
	semctl(buflen_sem, 2, SETVAL, foo); // we've done it
}

void oggPlayObject_impl::pause()
{
	mState = posPaused;
}

void oggPlayObject_impl::streamInit()
{
	arts_debug("oggvorbis: streamInit");
}

void oggPlayObject_impl::streamStart()
{
	arts_debug("ogg_vorbis: streamStart");
}

void oggPlayObject_impl::calculateBlock(unsigned long samples)
{
	int samplesAvailable = 0;

	//arts_debug("oggvorbis: calculateBlock");

	if (mState == posPlaying) {
		//arts_debug("oggvorbis: calculateBlock, %d(%d) of %d samples in buffer",
		//shm_buf->buflen - bufpos, shm_buf->backbuflen,samples);

		struct sembuf bleh;

		bleh.sem_num = 0;
		bleh.sem_flg = IPC_NOWAIT;
		//arts_debug("oggvorbis: %d samples wanted", samplesAvailable);
		bleh.sem_op = -samples; // does the buffer have sufficient samples?
		if (semop(buflen_sem, &bleh, 1) == -1) {
			if (errno == EAGAIN) {
				union semun foo;
				arts_debug("oggvorbis: buffer underrun");
				samplesAvailable = semctl(buflen_sem, 0, GETVAL, foo);
				// no samples AND no room is the decoder's way of signalling completion
				if (semctl(buflen_sem, 1, GETVAL, foo) == 0) {
					halt();
					samplesAvailable = 0;
				}
			} else {
				// something awful has happened
				halt();
				samplesAvailable = 0;
			}
		} else {
			samplesAvailable = samples; // number of samples we pushed from buffers
			// used to calculate the number we should zero out for an underrun
		}
		bleh.sem_flg = 0; // back to normal now

		//arts_debug("oggvorbis: %d samples available",samplesAvailable);
		for (int i = 0; i < samplesAvailable;
			++i, buf_pos = ((buf_pos + 1) % BACKBUFSIZ)) {

			left[i] = shm_buf->left[buf_pos];
			right[i] = shm_buf->right[buf_pos];
		}

		bleh.sem_num = 1;
		bleh.sem_op = samplesAvailable;
		semop(buflen_sem, &bleh, 1); // mark the now-free space
	}
	// zero out any samples we didn't have enough to complete
	while(static_cast<unsigned long>(samplesAvailable) < samples) {
		left[samplesAvailable] = 0.0;
		right[samplesAvailable] = 0.0;
		samplesAvailable++;
	}
}

void oggPlayObject_impl::streamEnd()
{
        arts_debug("oggvorbis: streamEnd");
}

REGISTER_IMPLEMENTATION(oggPlayObject_impl);
