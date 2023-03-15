#pragma once
#include <shared_mutex>
#include "BloomFilter.hpp"
#include "CountMinSketch.hpp"
#include "SegmentLRUCache.hpp"

enum CACHE_ZONE
{
	SEGMENT_LRU,
	WINDOWS_LRU
};

template<typename V>
class WindowsTinyLFU
{
public:
	typedef class LRUCache<V> LRUCache_;
	typedef class LRUNode<V> LRUNode_;
	typedef class SegmentLRUCache<V> SegmentLRUCache_;

	explicit WindowsTinyLFU(size_t capacity)
		: _wlru(std::ceil(capacity * 0.01))
		, _slru(std::ceil(0.2 * (capacity * (1 - 0.01))), std::ceil((1 - 0.2) * (capacity * (1 - 0.01))))
		, _bloomFilter(capacity, BLOOM_FALSE_POSITIVE_RATE / 100)
		, _cmSketch(capacity)
		, _dataMap(capacity)
		, _totalVisit(0)
		, _threshold(100)
	{}

	std::pair<V, bool> Get(const std::string& key)
	{
		uint32_t keyHash = Hash(key.c_str(), key.size(), KEY_HASH_SEED);
		uint32_t conflictHash = Hash(key.c_str(), key.size(), CONFLICT_HASH_SEED);

		std::shared_lock<std::shared_mutex> rLock(_rwMutex);
		return get(keyHash, conflictHash);
	}

	std::pair<V, bool> Del(const std::string& key)
	{
		uint32_t keyHash = Hash(key.c_str(), key.size(), KEY_HASH_SEED);
		uint32_t conflictHash = Hash(key.c_str(), key.size(), CONFLICT_HASH_SEED);

		std::unique_lock<std::shared_mutex> wLock(_rwMutex);	
		return del(keyHash, conflictHash);
	}

	bool Put(const std::string& key, const V& value)
	{
		uint32_t keyHash = Hash(key.c_str(), key.size(), KEY_HASH_SEED);
		uint32_t conflictHash = Hash(key.c_str(), key.size(), CONFLICT_HASH_SEED);

		std::unique_lock<std::shared_mutex> wLock(_rwMutex);
		return put(keyHash, value, conflictHash);
	}

	static unsigned int Hash(const void* key, int len, int seed)
	{
		const unsigned int m = 0x5bd1e995;
		const int r = 24;
		unsigned int h = seed ^ len;

		const unsigned char* data = (const unsigned char*)key;
		while (len >= 4)
		{
			unsigned int k = *(unsigned int*)data;
			k *= m;
			k ^= k >> r;
			k *= m;
			h *= m;
			h ^= k;
			data += 4;
			len -= 4;
		}

		switch (len)
		{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0];
			h *= m;
		};

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;
		return h;
	}


private:
	LRUCache_ _wlru;
	SegmentLRUCache_ _slru;
	BloomFilter _bloomFilter;
	CountMinSketch _cmSketch;
	std::shared_mutex _rwMutex;
	std::unordered_map<int, LRUNode_> _dataMap;	//���ڼ�¼�������ڵ�����
	size_t _totalVisit;
	size_t _threshold;	//���ʻ���

	static const int BLOOM_FALSE_POSITIVE_RATE = 1;		
	static const int KEY_HASH_SEED = 0xbc9f1d34;
	static const int CONFLICT_HASH_SEED = 0x9ae16a3b;


	std::pair<V, bool> get(uint32_t keyHash, uint32_t conflictHash)
	{

		++_totalVisit;
		if (_totalVisit >= _threshold)
		{
			_cmSketch.Reset();
			_bloomFilter.clear();
			_totalVisit = 0;
		}

		_cmSketch.Increment(keyHash);


		auto res = _dataMap.find(keyHash);
		if (res == _dataMap.end())
		{
			return std::make_pair(V(), false);
		}

		LRUNode node = res->second;
		V value;

		if (node._conflict != conflictHash)
		{
			return std::make_pair (V(), false);
		}

		if (node._flag == WINDOWS_LRU)
		{
			_wlru.get(node);
		}
		else
		{
			_slru.get(node);
		}
		return std::make_pair(node._value, true);
	}

	std::pair<V, bool> del(uint32_t keyHash, uint32_t conflictHash)
	{
		auto res = _dataMap.find(keyHash);
		if (res == _dataMap.end())
		{
			return std::make_pair(V(), false);
		}

		LRUNode node = res->second;

		if (node._conflict != conflictHash)
		{
			return std::make_pair(V(), false);
		}
		
		_dataMap.erase(keyHash);
		return std::make_pair(node._value, true);
	}

	bool put(uint32_t keyHash, const V& value, uint32_t conflictHash)
	{

		LRUNode newNode(keyHash, value, conflictHash, WINDOWS_LRU);

		std::pair<LRUNode, bool> res = _wlru.put(newNode);
		if (res.second == false)
		{
			_dataMap[keyHash] = newNode;
			return true;
		}

		if (_dataMap[res.first._key]._flag == WINDOWS_LRU)
		{
			_dataMap.erase(res.first._key);
		}
		


		newNode._flag = SEGMENT_LRU;

		std::pair<LRUNode, bool> victimRes = _slru.victim();

		if (victimRes.second == false)
		{
			_dataMap[keyHash] = newNode;
			_slru.put(newNode);
			return true;
		}


		if (!_bloomFilter.allow(keyHash))
		{
			return false;
		}


		int victimCount = _cmSketch.getCountMin(victimRes.first._key);
		int newNodeCount = _cmSketch.getCountMin(newNode._key);


		if (newNodeCount < victimCount)
		{
			return false;
		}
		_dataMap[keyHash] = newNode;
		_slru.put(newNode);


		if (_dataMap[res.first._key]._flag == SEGMENT_LRU)
		{
			_dataMap.erase(victimRes.first._key);
		}

		return true;
	}
};