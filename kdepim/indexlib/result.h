#ifndef LPC_RESULT_H1118420718_INCLUDE_GUARD_
#define LPC_RESULT_H1118420718_INCLUDE_GUARD_


/* This file is part of indexlib.
 * Copyright (C) 2005 Luís Pedro Coelho <luis@luispedro.org>
 *
 * Indexlib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation and available as file
 * GPL_V2 which is distributed along with indexlib.
 * 
 * Indexlib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA
 * 
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of this program with any edition of
 * the Qt library by Trolltech AS, Norway (or with modified versions
 * of Qt that use the same license as Qt), and distribute linked
 * combinations including the two.  You must obey the GNU General
 * Public License in all respects for all of the code used other than
 * Qt.  If you modify this file, you may extend this exception to
 * your version of the file, but you are not obligated to do so.  If
 * you do not wish to do so, delete this exception statement from
 * your version.
 */

#include "index.h"

namespace indexlib {
namespace detail {

struct simple_result : indexlib::result {
	public:
		simple_result( std::vector<unsigned> r ):res( r ) { }

		std::vector<unsigned> list() const { return res; }
		std::auto_ptr<result> search( const char* ) { return std::auto_ptr<result>( 0 ); }
	private:
		std::vector<unsigned> res;
};

struct empty_result : indexlib::result {
	public:
		std::vector<unsigned> list() const { return std::vector<unsigned>(); }
		std::auto_ptr<result> search( const char* ) { return std::auto_ptr<result>(); }
};
} //namespace detail
} //namespace indexlib


#endif /* LPC_RESULT_H1118420718_INCLUDE_GUARD_ */
