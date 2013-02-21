// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef _kdat_h_
#define _kdat_h_

// KDat program version.
#define KDAT_VERSION "2.0.1"

// Magic string.
#define KDAT_MAGIC "KDatMAGIC"
#define KDAT_MAGIC_LENGTH 9

// Tape header version numbers.
#define KDAT_TAPE_HEADER_VERSION_1_0 1
#define KDAT_TAPE_HEADER_VERSION KDAT_TAPE_HEADER_VERSION_1_0

// Index file version numbers.
#define KDAT_INDEX_FILE_VERSION_1_0 4
#define KDAT_INDEX_FILE_VERSION KDAT_INDEX_FILE_VERSION_1_0

// Constants for tape file format.
#define MAX_TAPE_NAME_LEN    4096
#define MAX_ARCHIVE_NAME_LEN 4096
#define MAX_NUM_ARCHIVES     4096

#endif
