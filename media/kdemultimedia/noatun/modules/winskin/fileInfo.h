#ifndef _FILEINFO_H
#define _FILEINFO_H

#include <noatun/playlist.h>

class fileInfo {
  public:
    fileInfo(const PlaylistItem &);
    ~fileInfo();

    unsigned int bps();
    unsigned int KHz();
    unsigned int channelCount();

  private:
     int _KHz;
     int _bps;
     int _channelCount;
};

#endif
