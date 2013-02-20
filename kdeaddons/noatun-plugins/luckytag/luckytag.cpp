// Copyright (c) 2001 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <noatun/app.h>
#include <noatun/playlist.h>
#include <noatun/player.h>
#include "luckytag.h"
#include "parsedmp3filename.h"

extern "C" Plugin *create_plugin()
{
	return new LuckyTagger;
}

LuckyTagger::LuckyTagger(void) : Tags(-10)
{
}

bool LuckyTagger::update(PlaylistItem &item)
{
	ParsedMP3FileName parsed(item.file());

	if(parsed.title().length())
		item.setProperty("title", parsed.title());
	if(parsed.artist().length())
		item.setProperty("author", parsed.artist());
	if(parsed.album().length())
		item.setProperty("album", parsed.album());
	if(parsed.comment().length())
		item.setProperty("comment", parsed.comment());
	return true;
}

