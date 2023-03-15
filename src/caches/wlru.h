#pragma once 
#include <stdlib.h>
#include <unordered_map>
#include <list>
#include <vector>
#include "wcache_node.h"
namespace zkv {
template <typename KeyType, typename ValueType>
class Wlru {
public:
    using Node = WCacheNode<KeyType, ValueType>;
    explicit Wlru(size_t capacity)
    : capacity_(capacity), hashmap_(capacity)
	{}
    bool get(const Node& node, Node* curNode) {
     
        auto res = hashmap_.find(node.hash);
		if (res == hashmap_.end())
		{
			return false;
		}
		auto pos = res->second;
		*curNode = *(*pos);

		lruList_.erase(pos);
		lruList_.push_front(curNode);

		// res->second = lruList_.begin();
        hashmap_[curNode->hash] = lruList_.begin();
		return true;
    }
    bool put(Node* node, Node* delNode) {
        bool flag = false; 
        delNode = nullptr;
        if (lruList_.size() == capacity_)
        {
            delNode = lruList_.back();
            lruList_.pop_back();
            hashmap_.erase(delNode->hash);
            flag = true;
        }
        lruList_.push_front(node);
        // hashmap_.insert(std::make_pair<uint32_t, typename std::list<Node>::iterator>(node->hash, lruList_.begin()));
        hashmap_[node->hash] = lruList_.begin();
        return flag;
    }


    size_t capacity() const
    {
        return capacity_;
    }

	size_t size() const 
	{
		return lruList_.size();
	}
private:
    size_t capacity_;

	std::unordered_map<uint32_t, typename std::list<Node*>::iterator> hashmap_;
    std::list<Node*> lruList_;
};
}
