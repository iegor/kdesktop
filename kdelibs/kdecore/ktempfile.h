/*
   This file is part of the KDE libraries
   Copyright (c) 1999 Waldo Bastian <bastian@kde.org>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _KTEMPFILE_H_
#define _KTEMPFILE_H_

#include <qstring.h>
#include <stdio.h>
#include <errno.h>
#include "kdelibs_export.h"

class QFile;
class QTextStream;
class QDataStream;
class KSaveFile;
class KTempFilePrivate;

/**
 * The KTempFile class creates and opens a unique file for temporary use.
 *
 * This is especially useful if you need to create a file in a world
 * writable directory like /tmp without being vulnerable to so called
 * symlink attacks.
 *
 * KDE applications, however, shouldn't create files in /tmp in the first 
 * place but use the "tmp" resource instead. The standard KTempFile 
 * constructor will do that by default.
 *
 * To create a temporary file that starts with a certain name
 * in the "tmp" resource, one should use:
 * KTempFile(locateLocal("tmp", prefix), extension);
 *
 * KTempFile does not create any missing directories, but locateLocal() does.
 *
 * See also KStandardDirs
 *
 * @author Waldo Bastian <bastian@kde.org>
 */
class KDECORE_EXPORT KTempFile
{
   friend class KSaveFile;
public:
   /**
    * Creates a temporary file with the name:
    *  \<filePrefix>\<six letters>\<fileExtension>
    *
    * The default @p filePrefix is "$KDEHOME/tmp-$HOST/appname/"
    * The default @p fileExtension is ".tmp"
    * @param filePrefix the prefix of the file name, or QString::null
    *        for the default value
    * @param fileExtension the extension of the prefix, or QString::null for the
    *        default value
    * @param mode the file permissions
    **/
   KTempFile(QString filePrefix=QString::null,
             QString fileExtension=QString::null,
             int mode = 0600 );


   /**
    * The destructor closes the file.
    * If autoDelete is enabled the file gets unlinked as well.
    **/
   ~KTempFile();

   /**
    * Turn automatic deletion on or off.
    * Automatic deletion is off by default.
    * @param autoDelete true to turn automatic deletion on
    **/
   void setAutoDelete(bool autoDelete) { bAutoDelete = autoDelete; }

   /**
    * Returns the status of the file based on errno. (see errno.h)
    * 0 means OK.
    *
    * You should check the status after object creation to check
    * whether a file could be created in the first place.
    *
    * You may check the status after closing the file to verify that
    * the file has indeed been written correctly.
    * @return the errno status, 0 means ok
    **/
   int status() const;

   /**
    * Returns the full path and name of the file.
    *
    * Note that in most circumstances the file needs to be closed 
    * before you use it by name. 
    *
    * In particular, if another process or software part needs to write data 
    * to the file based on the filename, the file should be closed before doing 
    * so. Otherwise the act of closing the file later on may cause the file to 
    * get truncated to a zero-size, resulting in an unexpected loss of the data.
    *
    * In some cases there is only interest in the filename itself but where the
    * actual presence of a file with such name is a problem. In that case the
    * file should first be both closed and unlinked. Such usage is not recommended
    * since it may lead to the kind of symlink vulnerabilities that the KTempFile 
    * design attempts to prevent.
    *
    * @return The name of the file, or QString::null if opening the
    *         file has failed or the file has been unlinked already.
    **/
   QString name() const;
   
   /**
    * An integer file descriptor open for writing to the file 
    * @return The file descriptor, or a negative number if opening
    *         the file failed
    **/
   int handle() const;
   
   /**
    * Returns the FILE* of the temporary file.
    * @return FILE* stream open for writing to the file, or 0
    *         if opening the file failed
    **/
   FILE *fstream();

   /**
    * Returns the QTextStream for writing.
    * @return QTextStream open for writing to the file, or 0
    *         if opening the file failed
    **/
   QTextStream *textStream();

   /**
    * Returns a QDataStream for writing.
    * @return QDataStream open for writing to the file, or 0 
    *         if opening the file failed
    **/
   QDataStream *dataStream();

   /**
    * Returns a QFile.
    * @return A QFile open for writing to the file, or 0 if 
    *         opening the file failed.
    **/
   QFile *file();

   /**
    * Unlinks the file from the directory. The file is
    * deleted once the last reader/writer closes it.
    **/
   void unlink();   

   /**
    * Flushes file to disk (fsync).
    *
    * If you want to be as sure as possible that the file data has
    * actually been physically stored on disk you need to call sync().
    * 
    * See status() for details about errors.
    * @return true if successful, or false if an error has occurred.
    * @since 3.3
    **/
   bool sync();

   /**
    * Closes the file.
    * 
    * See status() for details about errors.
    * @return true if successful, or false if an error has occurred.
    **/
   bool close();

protected:
   /**
    * Constructor used by KSaveFile
    **/
   KTempFile(bool);

   /**
    * @internal
    * Create function used internally by KTempFile and KSaveFile
    **/
   bool create(const QString &filePrefix, 
               const QString &fileExtension, int mode);

   void setError(int error) { mError = error; }
private:
   int mError;
   QString mTmpName;
   int mFd;
   FILE *mStream;
   QFile *mFile;
   QTextStream *mTextStream;
   QDataStream *mDataStream;
   bool bOpen;
   bool bAutoDelete;

   KTempFilePrivate *d;
};

#endif
