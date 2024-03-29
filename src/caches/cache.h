#pragma once
#include <stdint.h>

#include <functional>
#include <string>
#include <memory>
#include <vector>

#include "lru.h"
namespace zkv {
    template <typename KeyType, typename ValueType>
class Cache {
public:
    Cache() = default;
    virtual ~Cache() = default;
    virtual const char* Name() const = 0;
    virtual void Insert(const KeyType& key, ValueType* value,
                        uint32_t ttl = 0) = 0;
    virtual CacheNode<KeyType, ValueType>* Get(const KeyType& key) = 0;
    virtual void Release(CacheNode<KeyType, ValueType>* node) = 0;
    virtual void Prune() = 0;
    virtual void Erase(const KeyType& key) = 0;
    virtual void RegistCleanHandle(
        std::function<void(const KeyType& key, ValueType* value)> destructor) = 0;
};


template <typename KeyType, typename ValueType>
class ShardCache final : public Cache<KeyType, ValueType> {
public:
    ShardCache(uint32_t capacity) {
        // 提前分配好内存
        cache_impl_.resize(kShardNum);
        for (int32_t index = 0; index < kShardNum; ++index) {
            cache_impl_[index] =
                std::make_shared<LruCachePolicy<KeyType, ValueType, MutexLock>>(capacity);
        }
    }
    /*
    meta control block：ref_cnt，weak_cnt等等
    data block:具体数据指针
    */
    ~ShardCache() = default;
    const char* Name() const {
        return "shard.cache";
    }
    void Insert(const KeyType& key, ValueType* value, uint32_t ttl = 0) {
        uint64_t shard_num = std::hash<KeyType>{}(key) % kShardNum;
        cache_impl_[shard_num]->Insert(key, value, ttl);
    }
    CacheNode<KeyType, ValueType>* Get(const KeyType& key) {
        uint64_t shard_num = std::hash<KeyType>{}(key) % kShardNum;
        return cache_impl_[shard_num]->Get(key);
    }
    void Release(CacheNode<KeyType, ValueType>* node) {
        uint64_t shard_num = std::hash<KeyType>{}(node->key) % kShardNum;
        return cache_impl_[shard_num]->Release(node);
    }
    void Prune() {
        for (int32_t index = 0; index < kShardNum; ++index) {
            cache_impl_[index]->Prune();
        }
    }
    void Erase(const KeyType& key) {
        uint64_t shard_num = std::hash<KeyType>{}(key) % kShardNum;
        return cache_impl_[shard_num]->Erase(key);
    }
    void RegistCleanHandle(
    std::function<void(const KeyType& key, ValueType* value)> destructor) {
        for (int32_t index = 0; index < kShardNum; ++index) {
            cache_impl_[index]->RegistCleanHandle(destructor);
        }
    }

private:
    // 默认分为5个shard
    static constexpr uint8_t kShardNum = 4;
    // 采用impl的机制来进行实现
    std::vector<std::shared_ptr<CachePolicy<KeyType, ValueType>>> cache_impl_;
};











// template <typename KeyType, typename ValueType>
// class WTinyLFU final : public Cache<KeyType, ValueType> {
// public:
//     WTinyLFU() {

//     }
//     ~WTinyLFU() = default;
//     const char* Name() {
//         return "WTinyLFU";
//     }
//     void Insert(const KeyType& key, ValueType* value,
//                         uint32_t ttl = 0) {

//     }
//     CacheNode<KeyType, ValueType>* Get(const KeyType& key)  {

//     }
//     void Release(CacheNode<KeyType, ValueType>* node) {
        
//     }
//     void Prune() {

//     }
//     void Erase(const KeyType& key) {

//     }
//     void RegistCleanHandle(
//         std::function<void(const KeyType& key, ValueType* value)> destructor) {

//     }
// private:
//     std::shared_ptr<CachePolicy<KeyType, ValueType>> cache_impl_;
// };
}