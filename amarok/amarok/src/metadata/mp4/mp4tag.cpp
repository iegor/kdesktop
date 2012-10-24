
/***************************************************************************
copyright            : (C) 2005 by Andy Leadbetter
email                : andrew.leadbetter@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,            *
 *   MA  02110-1301  USA                                                   *
 ***************************************************************************/

#include "mp4tag.h"
#include <tag.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

using namespace TagLib;

MP4::Tag::Tag()
: TagLib::Tag::Tag()
, m_tags(NULL) {
	m_tags = (MP4Tags*)MP4TagsAlloc();
}

MP4::Tag::~Tag() {
	MP4TagsFree(m_tags);
	m_tags = NULL;
}

bool MP4::Tag::isEmpty() const {
    return  m_tags == NULL;
}

void MP4::Tag::duplicate(const Tag *source, Tag *target, bool overwrite) {
    // Duplicate standard information
    Tag::duplicate(source, target, overwrite);

//     if (overwrite || target->compilation() == Undefined &&
// 				source->compilation() != Undefined)
//         target->setCompilation(source->compilation());
// 
//     if (overwrite || target->cover().size() == 0)
//         target->setCover(source->cover());
}

void MP4::Tag::readTags( MP4FileHandle mp4file ) {
	if(m_tags != NULL) {
		MP4TagsFetch(tags, mp4file);
	}
}

void MP4::Tag::writeTags( MP4FileHandle mp4file ) {
	if(m_tags != NULL) {
		MP4TagsStore(tags, mp4file);
	}
}
