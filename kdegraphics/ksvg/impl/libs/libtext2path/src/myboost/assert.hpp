//
//  boost/assert.hpp - BOOST_ASSERT(expr)
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/utility/assert.html for documentation.
//

#ifndef ASSERT_HPP
#define ASSERT_HPP

#undef BOOST_ASSERT

# include <assert.h>
# define BOOST_ASSERT(expr) assert(expr)

#endif
