#ifndef BOOST_DETAIL_LWM_PTHREADS_HPP_INCLUDED
#define BOOST_DETAIL_LWM_PTHREADS_HPP_INCLUDED

//
//  boost/detail/lwm_pthreads.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#include <pthread.h>

namespace myboost
{

namespace detail
{

class lightweight_mutex
{
private:

    pthread_mutex_t m_;

    lightweight_mutex(lightweight_mutex const &);
    lightweight_mutex & operator=(lightweight_mutex const &);

public:

    lightweight_mutex()
    {
        pthread_mutex_init(&m_, 0);
    }

    ~lightweight_mutex()
    {
        pthread_mutex_destroy(&m_);
    }

    class scoped_lock;
    friend class scoped_lock;

    class scoped_lock
    {
    private:

        pthread_mutex_t & m_;

        scoped_lock(scoped_lock const &);
        scoped_lock & operator=(scoped_lock const &);

    public:

        scoped_lock(lightweight_mutex & m): m_(m.m_)
        {
            pthread_mutex_lock(&m_);
        }

        ~scoped_lock()
        {
            pthread_mutex_unlock(&m_);
        }
    };
};

} // namespace detail

} // namespace myboost

#endif // #ifndef BOOST_DETAIL_LWM_PTHREADS_HPP_INCLUDED
