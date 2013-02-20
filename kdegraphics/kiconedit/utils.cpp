/*
    KDE Icon Editor - a small graphics drawing program for the KDE

    Copyright (C) 1998 Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <kimageio.h>
#include <klocale.h>
#include <kdebug.h>

#include "utils.h"
#include "config.h"

imageFormats *formats = 0L;

void setupImageHandlers()
{
  if(formats != 0L)
    return;

  KImageIO::registerFormats();

  kdDebug(4640) << "Initializing formats" << endl;
  formats = new imageFormats;
  Q_CHECK_PTR(formats);
  formats->setAutoDelete(true);
  formats->append(new imageFormat("GIF", "GIF",  "gif"));
#ifdef HAVE_LIBJPEG
  formats->append(new imageFormat("JFIF", "JPEG", "jpg"));
#endif
  formats->append(new imageFormat("XPM", "XPM",  "xpm"));
  formats->append(new imageFormat("ICO", "Windows Icon File",  "ico"));
/*
#ifdef HAVE_LIBJPEG
  QImageIO::defineIOHandler("JFIF","^\377\330\377\340", 0, read_jpeg_jfif, NULL);
#endif
*/
}

// Simple copy operation on local files (isn't there something like this in the libs?)
bool copyFile(const QString &src, const QString &dest)
{
  QFile f_src(src);
  QFile f_dest(dest);
  QFileInfo fi(f_src);
  uint src_size = fi.size();
  kdDebug(4640) << "Size: " << src_size << endl;

  if ( f_src.open(IO_ReadOnly) )
  {    // file opened successfully
    if ( !f_dest.open(IO_WriteOnly) )
    {
      kdDebug(4640) << "copyFile - There was an error opening destination file: " << dest << endl;
      f_src.close();
      return false;
    }
    char *data = new char[src_size];
    if(f_src.readBlock(data, src_size) == -1)
    {
      kdDebug(4640) << "copyFile - There was an error reading source file: " << src << endl;
      f_src.close();
      f_dest.close();
      delete [] data;
      return false;
    }
    if(f_dest.writeBlock(data, src_size) == -1)
    {
      kdDebug(4640) << "copyFile - There was an error writing to destination file: " << dest << endl;
      f_src.close();
      f_dest.close();
      delete [] data;
      return false;
    }

    f_src.close();
    f_dest.close();
    delete [] data;
    return true;
  }
  kdDebug(4640) << "copyFile - There was an error opening source file: " << src << endl;
  return false;
}

bool removeFile(const QString &file)
{
  if(file.length() > 0 && QFile::exists(file))
  {
    QDir d;
    kdDebug(4640) << "Removing " << file << endl;
    if(!d.remove(file))
    {
      kdDebug(4640) << "removeFile - There was an error removing the file: " << file << endl;
      return false;
    }
    return true;
  }
  return false;
}

bool moveFile(const QString &src, const QString &dest)
{
  if(copyFile(src, dest))
    return removeFile(src);
  return false;
}

uint kdeColor(uint color)
{
  uint c = iconpalette[0]|OPAQUE_MASK;

  for(uint i = 0; i < 42; i++)
  {
    //kdDebug(4640) << "Color #" << i << " " << iconpalette[i] << endl;
    if( (iconpalette[i]|OPAQUE_MASK) - c < (iconpalette[i]|OPAQUE_MASK) - color)
      c = iconpalette[i]|OPAQUE_MASK;
  }
  //kdDebug(4640) << color << " -> " << c << endl;
  return c;
}
