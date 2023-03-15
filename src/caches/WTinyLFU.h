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
#include "wlru.h"
#include "wslru.h"

namespace zkv {
class CountMinSketch;
template <typename KeyType, typename ValueType, typename LockType = NullLock>
class WTinyLFUCachePolicy final : public CachePolicy<KeyType, ValueType> {
public:
	using LRUCache = Wlru<KeyType, ValueType>;
	using SegmentLRUCache = Wslru<KeyType, ValueType>;
	using Node = WCacheNode<KeyType, ValueType>;
public:
    WTinyLFUCachePolicy(uint32_t capacity) : capacity_(capacity) {}
    ~WTinyLFUCachePolicy() = default;

    void Insert(const KeyType& key, ValueType* value,
                    uint32_t ttl = 0) {
		LockType lock_type;
        ScopedLockImpl<LockType> lock_guard(lock_type);
        CacheNode<KeyType, ValueType>* new_node =
            new CacheNode<KeyType, ValueType>();
        new_node->hash = std::hash<KeyType>{}(key);
        new_node->key = key;
        new_node->value = value;
        new_node->in_cache = true;
        new_node->refs = 1;
        new_node->ttl = ttl;
        if (ttl > 0) {
            new_node->last_access_time = util::GetCurrentTime();
        }
		bool flag;
		Node* delnode;
		flag = windowLRU_->put(new_node, delnode);
		if(!flag) {
			datamap_[new_node->hash] = new_node;
			return true;
		} 
		Node victim;
		flag = segmentedLRU_->victim(&victim);
		if(!flag) {
			segmentedLRU_->put(delnode);
			return true;
		}

		if(!) {
			return true;
		}
		auto vcount = c_->Estimate(victim.hash);
		auto ocount = c_->Estimate(delnode->hash);
		if(ocount < vcount) {
			return true;
		}
		segmentedLRU_->put(delnode, nullptr);
		return true;
	}
    CacheNode<KeyType, ValueType>* Get(const KeyType& key) {
		LockType lock_type;
        ScopedLockImpl<LockType> lock_guard(lock_type);
		++totalVisit_;
		if (totalVisit_ >= threshold_)
		{
			c_.Reset();
			_bloomFilter.clear();
			totalVisit_ = 0;
		}

		//��cmSketch�Է��ʼ���
		auto keyHash = std::hash<KeyType>{}(key)
		c_.Increment(keyHash);

		//���Ȳ���map�����map��û���ҵ���˵��������
		auto res = datamap_.find(keyHash);
		if (res == datamap_.end())
		{
			return nullptr;
		}

		Node* node = res->second;
		if (node._flag == WINDOWS_LRU)
		{
			_wlru.get(node);
		}
		else
		{
			_slru.get(node);
		}
		return node->value;
	}
    void Release(CacheNode<KeyType, ValueType>* node);
    void Prune() = 0;
    void Erase(const KeyType& key) = 0;
    void RegistCleanHandle(
        std::function<void(const KeyType& key, ValueType* value)> destructor);


private:
    const uint32_t capacity_;
    std::shared_ptr<LRUCache> windowLRU_;
    std::shared_ptr<SegmentLRUCache> segmentedLRU_;

    std::shared_ptr<CountMinSketch> c_;
    std::unordered_map<int, Node*> datamap_;
	size_t totalVisit_;
	size_t threshold_;	
};
}