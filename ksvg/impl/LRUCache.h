/*
    Copyright (C) 2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <qvaluelist.h>

namespace KSVG
{

// A value-based LRU cache with a maximum total cost constraint, but with the exception that the
// most recently added item is kept in the cache even if its cost exceeds the maximum total cost.
template<class keyType, class valueType>
class MinOneLRUCache
{
public:
	MinOneLRUCache(int maxTotalCost = 0) : m_maxTotalCost(maxTotalCost), m_totalCost(0) {}
	virtual ~MinOneLRUCache() {}

	void insert(const keyType& key, const valueType& value, int cost);
	bool find(const keyType& key, valueType& result);

	void setMaxTotalCost(int maxTotalCost);
	int maxTotalCost() const { return m_maxTotalCost; }
	int totalCost() const { return m_totalCost; }

	void clear();

protected:
	class CacheItem
	{
	public:
		CacheItem() : m_cost(0) {}
		CacheItem(const keyType& key, const valueType& value, int cost) : m_key(key), m_value(value), m_cost(cost) {}

		const keyType& key() const { return m_key; }
		const valueType& value() const { return m_value; }
		int cost() const { return m_cost; }

	private:
		keyType m_key;
		valueType m_value;
		int m_cost;
	};

	typedef QValueList<CacheItem> CacheItemList;

	typename CacheItemList::iterator find(const keyType& key);
	void enforceCostConstraint();

	CacheItemList m_items;
	int m_maxTotalCost;
	int m_totalCost;
};

template<class keyType, class valueType>
void MinOneLRUCache<keyType, valueType>::insert(const keyType& key, const valueType& value, int cost)
{
	typename CacheItemList::iterator it = find(key);

	if(it != m_items.end())
	{
		// Replace the existing item.
		m_totalCost -= (*it).cost();
		m_items.erase(it);
	}

	// We always hold the most recently added item in the cache, even if it exceeds
	// the maximum total cost.
	m_items.push_front(CacheItem(key, value, cost));
	m_totalCost += cost;
	enforceCostConstraint();
}

template<class keyType, class valueType>
bool MinOneLRUCache<keyType, valueType>::find(const keyType& key, valueType& result)
{
	bool foundKey = false;
	typename CacheItemList::iterator it = find(key);

	if(it != m_items.end())
	{
		CacheItem item = *it;
		result = item.value();

		if(it != m_items.begin())
		{
			// This is now the most recently used item.
			m_items.erase(it);
			m_items.push_front(item);
		}

		foundKey = true;
	}

	return foundKey;
}

template<class keyType, class valueType>
typename MinOneLRUCache<keyType, valueType>::CacheItemList::iterator MinOneLRUCache<keyType, valueType>::find(const keyType& key)
{
	typename CacheItemList::iterator it;

	for(it = m_items.begin(); it != m_items.end(); it++)
	{
		if((*it).key() == key)
			break;
	}

	return it;
}

template<class keyType, class valueType>
void MinOneLRUCache<keyType, valueType>::enforceCostConstraint()
{
	if(m_totalCost > m_maxTotalCost && m_items.size() > 1)
	{
		typename CacheItemList::iterator it = m_items.begin();
		m_totalCost = (*it).cost();
		++it;

		while(it != m_items.end() && m_totalCost + (*it).cost() <= m_maxTotalCost)
		{
			m_totalCost += (*it).cost();
			++it;
		}

		// Remove the remainder
		while(it != m_items.end())
			it = m_items.erase(it);
	}
}

template<class keyType, class valueType>
void MinOneLRUCache<keyType, valueType>::setMaxTotalCost(int maxTotalCost)
{
	m_maxTotalCost = maxTotalCost;
	enforceCostConstraint();
}

template<class keyType, class valueType>
void MinOneLRUCache<keyType, valueType>::clear()
{
	m_items.clear();
	m_totalCost = 0;
}

}

#endif

