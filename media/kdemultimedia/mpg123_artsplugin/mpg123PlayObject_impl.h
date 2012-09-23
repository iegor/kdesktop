#ifndef MPG123PLAYER_IMPL_H
#define MPG123PLAYER_IMPL_H "$Id: mpg123PlayObject_impl.h 134186 2002-01-27 15:38:22Z bero $"

using namespace std;

#if (defined(__GNU_LIBRARY__) && defined(_SEM_SEMUN_UNDEFINED)) || defined(__osf__) || defined(__sun__)
/* union semun is defined by including <sys/sem.h> */
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

#define RESYNC_FRAMES 3

extern "C" {
	#include "mpg123/mpg123.h"
	#include "dxhead.h"

	// These are provided to make mpg123 happy
	int audio_get_formats(struct audio_info_struct *ai);
	char *equalfile; 
	struct parameter param;
	char *prgName;
	char *prgVersion;
	struct audio_info_struct ai;

	// We abuse these internal mpg123 objects
	extern double compute_bpf(struct frame *fr);
	extern double compute_tpf(struct frame *fr);
	extern void set_pointer(int, long);
	extern void audio_capabilities(struct audio_info_struct *);	
	extern unsigned char *pcm_sample;
	extern int pcm_point;
	extern int audiobufsize;
}

#define BACKBUFSIZ 8132

struct id3tag {
	char tag[3];
	char title[30];
	char artist[30];
	char album[30];
	char year[4];
	char comment[30];
	unsigned char genre;
};

namespace Arts {

class mpg123PlayObject_impl
	: public mpg123PlayObject_skel, public StdSynthModule
{
 public:
	mpg123PlayObject_impl();
	~mpg123PlayObject_impl();
	bool loadMedia(const string &filename);
	string description();
	poTime currentTime();
	poTime overallTime();
	poCapabilities capabilities();
	string mediaName();
	poState state();
	void play();
	void halt();
	void seek(const class poTime &t);
	void pause();
	void streamInit();
	void streamStart();
	void calculateBlock(unsigned long samples);
	void streamEnd();

 protected:
	// These are to enable seeking
	static void stream_jump_to_frame(struct frame *fr, int frame);
	static int calc_numframes(struct frame *);

	// This is to enable playing, what else?
	static void set_synth_functions(struct frame *fr);
	void initialise_decoder();
	int play_frame(int init); 

	inline float conv_16le_float(short x)
	{ return static_cast<float>(x) / 32768.0; }

	int streaming, /*padded, cnt, junk_size, */ sample_freq /*frame_size*/;
	char mpeg_name[FILENAME_MAX+1];
	struct mpstr *mp;
	static int decoder_init;

	int buf_pos; // loops around the circular buffer
	poState mState;
	struct buf_t{
		float left[BACKBUFSIZ];
		float right[BACKBUFSIZ];
		unsigned long len; // total frames
		unsigned long pos; // last decoded frame
		double tpf; // time per frame, seconds
	} *shm_buf;
	int shm_id, child_pid;
	int buflen_sem;
	unsigned char *pcm_buf;
};

};

#endif
