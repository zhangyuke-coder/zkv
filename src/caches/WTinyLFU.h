#pragma once
#include <functional>
#include <list>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <memory>
#include "utils/hash_util.h"
#include "utils/mutex.h"
#include "utils/util.h"
#include "cache_node.h"
#include "cache_policy.h"
#include "count_min_sketch.h"
// #include "wlru.h"
// #include "wslru.h"

namespace zkv {
class Wlru;
class Wslru;
class CountMinSketch;
template <typename KeyType, typename ValueType, typename LockType = NullLock>
class WTinyLFUCachePolicy final : public CachePolicy<KeyType, ValueType> {
public:
    WTinyLFUCachePolicy(uint32_t capacity) : capacity_(capacity) {}
    virtual ~CachePolicy() = default;

    virtual void Insert(const KeyType& key, ValueType* value,
                    uint32_t ttl = 0) = 0;
    virtual CacheNode<KeyType, ValueType>* Get(const KeyType& key) = 0;
    virtual void Release(CacheNode<KeyType, ValueType>* node) = 0;
    virtual void Prune() = 0;
    virtual void Erase(const KeyType& key) = 0;
    virtual void RegistCleanHandle(
        std::function<void(const KeyType& key, ValueType* value)> destructor) = 0;

private:
    
	static const int BLOOM_FALSE_POSITIVE_RATE = 1;		
	static const int KEY_HASH_SEED = 0xbc9f1d34;
	static const int CONFLICT_HASH_SEED = 0x9ae16a3b;
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
    const uint32_t capacity_;
    std::shared_ptr<typename Wlru<KeyType, ValueType>> windowLRU_;
    std::shared_ptr<Wslru> segmentedLRU_;

    std::shared_ptr<CountMinSketch> c_;
    std::unordered_map<int, WCacheNode> datamap_;
};
}