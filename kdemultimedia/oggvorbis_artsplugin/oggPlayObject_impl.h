#ifndef OGGPLAYER_IMPL_H
#define OGGPLAYER_IMPL_H "$Id: oggPlayObject_impl.h 674869 2007-06-13 10:18:09Z hasso $"

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED) || defined(__DragonFly__)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val;                    /* value for SETVAL */
	struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array;  /* array for GETALL, SETALL */
	struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

int buf_pos;

namespace Arts {

class oggPlayObject_impl
	: public oggPlayObject_skel, public StdSynthModule
{
 public:
	oggPlayObject_impl();
	~oggPlayObject_impl();
	bool loadMedia(const std::string &filename);
	std::string description();
	poTime currentTime();
	poTime overallTime();
	poCapabilities capabilities();
	std::string mediaName();
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

	static const int BACKBUFSIZ=4096;
	OggVorbis_File vf;
	std::string currentFile;

	inline float conv_16le_float(short x)
		{ return static_cast<float>(x) / 32768.0; }

	poState mState;
	struct buf_t{
		float left[BACKBUFSIZ];
		float right[BACKBUFSIZ];
	} *shm_buf;
	int shm_id, child_pid;
	int buflen_sem;
};

};

#endif
