/***************************************************************************
 *   Copyright (C) 2004 by Martin Koller                                   *
 *   m.koller@surfeu.at                                                    *
 *                                                                         *
 *   This function reads the content of a M$-Windoze .lnk file             *
 *   and returns data in the given structure.                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             *
 ***************************************************************************/

#include "read_lnk.h"
#include <stdio.h>
#include <kdebug.h>
#include <kio/netaccess.h>

//--------------------------------------------------------------------------------

//TODO: little/big endian problem ?
struct LNKHeader
{
   char magic[4];
   char GUID[16];
   Q_UINT32 flags;
   Q_UINT32 attributes;
   char time1[8];
   char time2[8];
   char time3[8];
   Q_UINT32 length;
   Q_UINT32 iconNum;
   Q_UINT32 showWnd;
   Q_UINT32 hotKey;
   char filler[8];
};

struct LNKFileLocation
{
  Q_UINT32 totalLen;
  Q_UINT32 ptr;
  Q_UINT32 flags;
  Q_UINT32 localVolume;
  Q_UINT32 basePath;
  Q_UINT32 netVolume;
  Q_UINT32 pathname;
};

//--------------------------------------------------------------------------------

bool readLNK(const KURL &url, LNKInfo &info)
{
  const char* lnkFile = 0;

  QString tempFile;
  if ( KIO::NetAccess::download(url, tempFile, 0) )
    lnkFile = tempFile.latin1();
  else
    return false;

  kdDebug(7034) << "opening:" << lnkFile << endl;
  FILE *fd = fopen(lnkFile, "rb");
  if ( !fd )
  {
    kdWarning(7034) << "could not open file " << lnkFile << endl;
    KIO::NetAccess::removeTempFile(tempFile);
    return false;
  }

  LNKHeader header;

  if ( fread(&header, sizeof(header), 1, fd) != 1 )
  {
    kdWarning(7034) << "wrong header size" << endl;
    fclose(fd);
    KIO::NetAccess::removeTempFile(tempFile);
    return false;
  }

  if ( memcmp(header.magic, "L\0\0\0", 4) != 0 )
  {
    kdWarning(7034) << "wrong magic in header" << endl;
    fclose(fd);
    KIO::NetAccess::removeTempFile(tempFile);
    return false;
  }

  if ( header.flags & 0x1 )  // the shell item id list is present
  {
    Q_UINT16 len;

    // skip that list
    if ( (fread(&len, sizeof(len), 1, fd) != 1) || (fseek(fd, len, SEEK_CUR) != 0) )
    {
      kdWarning(7034) << "could not read shell item id list" << endl;
      fclose(fd);
      KIO::NetAccess::removeTempFile(tempFile);
      return false;
    }
  }

  info.isDirectory = (header.attributes & 0x10);

  if ( ! info.isDirectory )  // not a directory
    info.fileSize = header.length;

  info.isFileOrDir = (header.flags & 0x2);  // points to file or directory

  if ( info.isFileOrDir )
  {
    LNKFileLocation loc;

    if ( fread(&loc, sizeof(loc), 1, fd) != 1 )
    {
      kdWarning(7034) << "could not read file localtion table" << endl;
      fclose(fd);
      KIO::NetAccess::removeTempFile(tempFile);
      return false;
    }

    // limit the following "new", because the size to allocate is in the file
    // which can easily be manipulted to contain a huge number and lead to a crash
    if ( (loc.totalLen <= sizeof(loc)) || (loc.totalLen > 4096) )  // 4096 is just an arbitrary number I think shall be enough
    {
      fclose(fd);
      KIO::NetAccess::removeTempFile(tempFile);
      return false;
    }

    size_t size = loc.totalLen - sizeof(loc);
    char *data = new char[size];
    char *start = data - sizeof(loc);

    if ( fread(data, size, 1, fd) != 1 )
    {
      kdWarning(7034) << "could not read pathes data" << endl;
      delete [] data;
      fclose(fd);
      KIO::NetAccess::removeTempFile(tempFile);
      return false;
    }

    info.isNetworkPath = !(loc.flags & 0x1);

    if ( !info.isNetworkPath )
    {
      info.volumeName = (start + loc.localVolume + 0x10);  // volume label

      info.path = QString::null;

      if ( *(start + loc.basePath) )
      {
        // Don't put any more than "X:" into info.driveName.
        info.driveName = *(start + loc.basePath);
        info.driveName += ':';

        // If we in fact do have more than just "X:", store any additional
        // path information separately in info.path.
        if ( *(start + loc.basePath + 1) == ':' &&
             *(start + loc.basePath + 2) != 0)
          info.path = (start + loc.basePath + 2);
      }

      if ( *(start + loc.pathname) != 0 )
      {
        if ( info.path.isNull() )
          info.path = (start + loc.pathname);
        else
          info.path = info.path + "\\" + (start + loc.pathname);
      }
    }
    else  // network path
    {
      info.path = QString("%1\\%2")
                          .arg(start + loc.netVolume + 0x14)  // network share name
                          .arg(start + loc.pathname);
    }

    delete [] data;
    data = 0;

    if ( header.flags & 0x4 )   // has description string
    {
      Q_UINT16 len;

      if ( fread(&len, sizeof(len), 1, fd) != 1 )
      {
        kdWarning(7034) << "could not read description string length" << endl;
        fclose(fd);
        KIO::NetAccess::removeTempFile(tempFile);
        return false;
      }

      data = new char[len+1];  // this can never be > 65K, so its OK to not check the size

      if ( fread(data, len, 1, fd) != 1 )
      {
        kdWarning(7034) << "could not read description string" << endl;
        delete [] data;
        fclose(fd);
        KIO::NetAccess::removeTempFile(tempFile);
        return false;
      }

      data[len] = 0;  // nullbyte seems to miss

      info.description = data;

      delete [] data;
    }
  }

  fclose(fd);
  KIO::NetAccess::removeTempFile(tempFile);

  return true;
}
