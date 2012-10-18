    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef ARTS_SEQUENCEUTILS_H
#define ARTS_SEQUENCEUTILS_H

#include <vector>
#include <string>
#include <kdelibs_export.h>

KDE_EXPORT int parse_line(const char *in, char *& cmd, char *& param);

KDE_EXPORT void sqprintf(std::vector<std::string> *list, const char *fmt, ...)
#ifdef __GNUC__
    __attribute__ (( format (printf, 2, 3)))
#endif
;
KDE_EXPORT void addSubStringSeq(std::vector<std::string> *target, const std::vector<std::string> *source);
KDE_EXPORT void appendStringSeq(std::vector<std::string> *target, const std::vector<std::string> *source);
KDE_EXPORT int parse_line(const std::string& in, std::string& cmd, std::string& param);
KDE_EXPORT std::vector<std::string> *getSubStringSeq(const std::vector<std::string> *seq,unsigned long& i);

#endif /* ARTS_SEQUENCEUTILS_H */
