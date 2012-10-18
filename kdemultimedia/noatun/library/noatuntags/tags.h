#ifndef __NOATUN_TAGS___TACOS_ARE_YUMMY
#define __NOATUN_TAGS___TACOS_ARE_YUMMY

#include <noatun/playlist.h>

class TagsGetter;

class Tags
{
	friend class TagsGetter;
public:
	/**
	 * priority is how early this comes
	 * 0 means "normal"
	 * anything larger than zero means that this should come later,
	 * negative numbers mean it should come first
	 * 
	 * I'm talking about the order which you're being processed
	 **/
	Tags(int priority=0);
	virtual ~Tags();

	/**
	 * this will be called occasionally
	 * with an item you should fill up
	 **/
	virtual bool update(PlaylistItem &item)=0;

private:
	static TagsGetter *getter;
	int mPriority;
};

#endif

