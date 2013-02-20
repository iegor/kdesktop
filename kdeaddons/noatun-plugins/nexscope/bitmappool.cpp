#include "nex.h"

struct BitmapPool::PoolItem
{
	PoolItem() : used(false) {}
	
	bool used;
	Bitmap bitmap;
};

BitmapPool::BitmapPool()
{

}

BitmapPool::~BitmapPool()
{

}
	
Bitmap *BitmapPool::get(bool clear)
{
	mMutex.lock();
	BitmapPool::PoolItem *p=0;
	for (QPtrListIterator<BitmapPool::PoolItem> i(mBitmaps); i.current(); ++i)
	{
		if (!(*i)->used)
			p=*i;
	}
	if (!p)
	{
		p=new BitmapPool::PoolItem;
		p->bitmap.resize(width, height);
	}
	
	p->used=true;
	
	if (clear) p->bitmap.clear();
	
	mMutex.unlock();
	return &(p->bitmap);
}

void BitmapPool::release(Bitmap *bitmap)
{
	mMutex.lock();
	for (QPtrListIterator<BitmapPool::PoolItem> i(mBitmaps); i.current(); ++i)
	{
		if (&((*i)->bitmap)==bitmap)
			(*i)->used=false;
	}
	mMutex.unlock();
}

