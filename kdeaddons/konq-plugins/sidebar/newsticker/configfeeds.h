// -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 8; -*-

/***************************************************************************
    configfeeds.h
    Copyright: Marcus Camen  <mcamen@mcamen.de>
    Copyright: Nathan Toone  <nathan@toonetown.com>
***************************************************************************/

/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef _konq_sidebar_news_configfeedsh_
#define _konq_sidebar_news_configfeedsh_

#include "configfeedsbase.h"


//
//
// ATM this is just a noop wrapper
//
//


namespace KSB_News {

  class ConfigFeeds : public ConfigFeedsBase {

    Q_OBJECT

  public:
    ConfigFeeds(QWidget* parent,  const char* name = 0);

  };

} // namespace KSB_News


#endif
