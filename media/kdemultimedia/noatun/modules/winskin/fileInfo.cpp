#include <noatun/app.h>
#include <noatun/playlist.h>

#include <qstring.h>
#include <kfilemetainfo.h>

#include "fileInfo.h"

fileInfo::fileInfo(const PlaylistItem &item)
{
    QString prop;

    prop = item.property("bitrate");
    if (prop.isNull())
        _bps = 0;
    else
        _bps = prop.toInt();

    prop = item.property("samplerate");
    if (prop.isNull())
        _KHz = 44100;
    else
        _KHz = prop.toInt();

    prop = item.property("channels");
    if (prop.isNull())
        _channelCount = 2;
    else
        _channelCount = prop.toInt();
}

fileInfo::~fileInfo()
{
}

unsigned int fileInfo::bps()
{
    return _bps;
}

unsigned int fileInfo::KHz()
{
    return _KHz;
}

unsigned int fileInfo::channelCount()
{
    return _channelCount;
}

