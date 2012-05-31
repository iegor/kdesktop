/*  aKode: aRts InputStream

    Copyright (C) 2004 Allan Sandfeld Jensen <kde@carewolf.com>

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

#ifndef _AKODE_ARTS_INPUTSTREAM_H
#define _AKODE_ARTS_INPUTSTREAM_H

extern "C" {
#include <stdio.h>
#include <fcntl.h>
}

#include <arts/dispatcher.h>

#include <akode/file.h>
#include <akode/bytebuffer.h>

class Arts_InputStream : public aKode::File
{
protected:
    // m_instream is mutable because Arts::InputStream has not marked const functions
    mutable Arts::InputStream m_instream;
    aKode::ByteBuffer *m_buffer;
    bool m_open, m_seekable;
    long m_pos;
    long m_len;
public:
    Arts_InputStream(Arts::InputStream inputstream, aKode::ByteBuffer *buffer)
                    : File("arts_inputstream"),
                      m_instream(inputstream), m_buffer(buffer),
                      m_open(false), m_seekable(false),
                      m_pos(-1), m_len(-1)
    {
        m_instream.streamInit();
    };
    virtual ~Arts_InputStream() {
    }
    bool openRO() {
        m_open = true;
        m_pos = 0;
//        Arts::Dispatcher::lock();
        m_len = m_instream.size();
        m_seekable = m_instream.seekOk();
        m_instream.streamStart();
//        Arts::Dispatcher::unlock();
        return true;
    }
    void close() {
        m_open = false;
        m_instream.streamEnd();
    }
    long read(char* ptr, long num) {
        if (!m_open) return -1;
        if (num<=0) return 0;
        long n = m_buffer->read(ptr,num,true);
        m_pos += n;
        return n;
    }
    long write(const char*, long) {
        return -1;
    }
    bool seek(long to, int whence) {
        if(!m_open || !seekable()) return false;

        arts_debug("akode: InputStream seeking");

        long newpos = 0;
        switch (whence) {
            case SEEK_SET:
                newpos = to;
                break;
            case SEEK_CUR:
                newpos = m_pos + to;
                break;
            case SEEK_END:
                if (m_len < 0) return false;
                newpos = m_len + to;
                break;
            default:
                return false;
        }

        long s = m_instream.seek(newpos);
        if (s >= 0) {
            m_pos = s;
            m_buffer->flush();
            return true;
        }
        else
            return false;
    }

    long position() const {
        if(!m_open) return -1;
        return m_pos;
    }

    long length() const {
        if(!m_open) return -1;
        return m_len;
    }

    bool seekable() const {
        return m_seekable;
    }

    bool readable() const {
        return true;
    }

    bool writeable() const {
        return false;
    }
    // Arts-call needs to be protected since
    // the member-functions here are called by another thread
    bool eof() const {
        if(!m_open) return true;
        bool res = false;
        if (m_buffer->empty()) {
            Arts::Dispatcher::lock();
            res = m_instream.eof();
            Arts::Dispatcher::unlock();
        }
        return res;
    }

    bool error() const {
        return (!m_open);
    }

    void fadvise() {
        return;
    }
};

#endif
