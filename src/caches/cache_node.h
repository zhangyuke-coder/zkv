#pragma once 
#include <stdint.h>
namespace zkv {
template <typename KeyType, typename ValueType>
struct CacheNode {
    KeyType key;
    ValueType* value;
    uint32_t refs = 0;
    uint32_t hash = 0;
    bool in_cache = false;
    uint64_t last_access_time = 0;
    uint64_t ttl = 0;
}; 
}