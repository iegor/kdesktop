#ifndef BOOST_THROW_EXCEPTION_HPP_INCLUDED
#define BOOST_THROW_EXCEPTION_HPP_INCLUDED


//
//  boost/throw_exception.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  http://www.boost.org/libs/utility/throw_exception.html
//

# include <exception>

namespace myboost
{

template<class E> void throw_exception(E const & e)
{
    throw e;
}

} // namespace myboost

#endif // #ifndef BOOST_THROW_EXCEPTION_HPP_INCLUDED
