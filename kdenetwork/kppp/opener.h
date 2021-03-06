#ifndef _FILEOPENER_H_
#define _FILEOPENER_H_

#define DEVNULL "/dev/null"

// workaround for bug in glibc on RedHat 5.0 and Debian 2.1
#if defined(__GLIBC__) && (__GLIBC__ == 2 && __GLIBC_MINOR__ == 0 && defined(__linux__))
# define MY_SCM_RIGHTS 1
#else
# define MY_SCM_RIGHTS SCM_RIGHTS
#endif

// ### add by bhughes - FreeBSD defines 'BSD' in sys/param.h
#include <sys/param.h>

#if defined(BSD) || defined(__svr4__)
# define IOV_BASE_CAST (char *)
#else
# define IOV_BASE_CAST (void *)
#endif

const char *pppdPath();

class Opener {

public:
  Opener(int);
  ~Opener();

  enum { OpenDevice = 1,
         OpenLock, RemoveLock,
         OpenResolv,
         OpenSysLog,
         SetSecret, RemoveSecret,
         SetHostname,
         ExecPPPDaemon, KillPPPDaemon,
	 PPPDExitStatus,
         Stop };
  enum Auth { PAP = 1, CHAP };
  enum { MaxPathLen = 30, MaxStrLen = 40, MaxArgs = 100 };

private:
  enum { Original=0x100, New=0x200, Old=0x400 } Version;
  void mainLoop();
  int sendFD(int ttyfd, struct ResponseHeader *response);
  int sendResponse(struct ResponseHeader *response);
  const char *deviceByIndex(int idx);
  bool createAuthFile(Auth method, char *username, char *password);
  bool removeAuthFile(Auth method);
  const char* authFile(Auth method, int version = Original);
  bool execpppd(const char *arguments);
  bool killpppd()const;
  void parseargs(char* buf, char** args);

  int socket;
  int ttyfd;
  char lockfile[MaxPathLen+1];
};


struct RequestHeader {
  int	type;
  int	len;
  //  int   id;     // TODO: Use a transmission id and check whether
                    //       response matches request
};

struct ResponseHeader {
  int	status; /* 0 or errno */
  //  int   id;
};

struct OpenModemRequest {
  struct RequestHeader header;
  int    deviceNum;
};

struct RemoveLockRequest {
  struct RequestHeader header;
};

struct OpenLockRequest {
  struct RequestHeader header;
  int    deviceNum;
  int    flags;
};

struct OpenResolvRequest {
  struct RequestHeader header;
  int    flags;
};

struct OpenLogRequest {
  struct RequestHeader header;
};

struct SetSecretRequest {
  struct RequestHeader header;
  Opener::Auth method;   // PAP or CHAP
  char   username[Opener::MaxStrLen+1];
  char   password[Opener::MaxStrLen+1];
};

struct RemoveSecretRequest {
  struct RequestHeader header;
  Opener::Auth method;   // PAP or CHAP
};

struct SetHostnameRequest {
  struct RequestHeader header;
  char   name[Opener::MaxStrLen+1];
};

struct ExecDaemonRequest {
  struct RequestHeader header;
  char   arguments[MAX_CMDLEN+1];
};

struct KillDaemonRequest {
  struct RequestHeader header;
};

struct PPPDExitStatusRequest {
  struct RequestHeader header;
};

struct StopRequest {
  struct RequestHeader header;
};

union AllRequests {
  struct RequestHeader header;
  struct OpenModemRequest  modem;
  struct OpenLockRequest lock;
  struct RemoveLockRequest unlock;
  struct OpenResolvRequest resolv;
  struct SetSecretRequest secret;
  struct RemoveSecretRequest remove;
  struct SetHostnameRequest host;
  struct OpenLogRequest log;
  struct ExecDaemonRequest daemon;
  struct ExecDaemonRequest kill;
  struct PPPDExitStatusRequest status;
  struct StopRequest stop;
};

#endif
