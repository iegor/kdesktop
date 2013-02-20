/*
    Copyright (C) 2003 Nikolas Zimmermann <wildfox@kde.org>
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

#ifndef T2P_CACHE_H
#define T2P_CACHE_H

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "myboost/shared_ptr.hpp"

namespace T2P
{
	class CacheElement
	{
	public:
		CacheElement(std::string key) : m_key(key), m_usage(0) { }
		~CacheElement() { }

		std::string key() const { return m_key; }
		int usage() const { return m_usage; }

		void incUsage() { m_usage++; }

	private:
		std::string m_key;
		int m_usage;
	};
	
	template<typename T>
	class Cache
	{
	public:
		typedef myboost::shared_ptr<T> SharedT;

		Cache(int maxSize = 10) : m_size(0), m_maxSize(maxSize) { }
		~Cache() { clear(); }

		// Number of elements in cache
		int size() const { return m_size; }
		
		// Maximum number of elements in cache
		int maxSize() const { return m_maxSize; } 
		
		// Add new entry
		void insert(const std::string &key, SharedT &value)
		{
			// TODO: Add real LRU logic, now i will just delete the less
			// used item when inserting a new item would exceed maxSize
			if(m_size == m_maxSize)
			{
				// Find less used item
				typename std::map<SharedT, CacheElement *>::const_iterator it = m_cacheMap.begin();
				int usage = (*it).second->usage(); std::string keyUsage = (*it).second->key();
				for(it++; it != m_cacheMap.end(); ++it)
				{
					int curUsage = (*it).second->usage();
					if(curUsage < usage)
					{
						usage = curUsage;
						keyUsage = (*it).second->key();
					}						
				}

//				std::cout << "[CACHE] Removing less used element with key: " << keyUsage << " (Used " << usage << " times!)" << std::endl;
				remove(keyUsage);
			}
			
			m_size++;
			m_entries.push_back(value);
			m_cacheMap[value] = new CacheElement(key);
		}
		
		// Remove single entry
		void remove(const std::string &key)
		{
			for(typename std::vector<SharedT>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			{
				SharedT cur = *it;
				if(m_cacheMap[cur]->key() == key)
				{
					m_size--;

					typename std::map<SharedT, CacheElement *>::iterator cacheElement = m_cacheMap.find(cur);
					m_cacheMap.erase(cacheElement);
					delete cacheElement->second;
					m_entries.erase(it); // Only valid to write because we don't go on here!
					break;				 //	Otherwhise the iterators _may_ be invalid!
				}
			}
		}

		// Remove all entries
		void clear()
		{
			m_size = 0;
			m_entries.clear();
			m_cacheMap.clear();
		}
		
		// Lookup entry
		SharedT find(const std::string &key)
		{
			for(typename std::vector<SharedT>::const_iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			{
				SharedT cur = *it;
				if(m_cacheMap[cur]->key() == key)
				{
					m_cacheMap[cur]->incUsage();
					return cur;
				}
			}

			return SharedT();
		}

		void dump()
		{
			std::cout << "[CACHE] " << size() << " of " << maxSize() << " Objects in the cache." << std::endl;

			for(typename std::vector<SharedT>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			{
				SharedT cur = *it;
				std::cout << "[CACHE] - " << m_cacheMap[cur]->key() << " -> " << cur << " (Usage: " << m_cacheMap[cur]->usage() << ")" << std::endl;
			}
		}

	private:
		std::vector<SharedT> m_entries;
		std::map<SharedT, CacheElement *> m_cacheMap;
		int m_size, m_maxSize;
	};
}

#endif

// vim:ts=4:noet
