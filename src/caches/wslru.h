#pragma once 
#include <stdlib.h>
#include <unordered_map>
#include <list>
#include <vector>
#include "wcache_node.h"
namespace zkv {
enum SEGMENT_ZONE
{
	PROBATION = 0,
	PROTECTION
};
template <typename KeyType, typename ValueType>
class Wslru {
public:
    using Node = WCacheNode<KeyType, ValueType>;
	explicit Wslru(size_t probationCapacity, size_t protectionCapacity)
		: probationCapacity_(probationCapacity)
		, probationCapacity_(protectionCapacity)
		, hashmap_(probationCapacity +  protectionCapacity)
	{}

    int size() const
    {
        return probationList_.size() + protectionList_.size();
    }
    size_t capacity() const
    {
        return probationCapacity_ + protectionCapacity_;
    }
    bool put(Node* newNode, Node* delNode) {

        newNode->stage_ = PROBATION
        if(probationList_.size() < probationCapacity_ || size() < probationCapacity_ + protectionCapacity_) {
            probationList_.push_front(newNode);
            hashmap[newNode->hash] = probationList_.begin();
            return false;
        }

        delNode = probationList_.back();
        hashmap_.erase(delNode->hash);
        probationList_.pop_back();

        probationList_.push_front(newNode);
        hashmap_[newNode->hash] = probationList_.begin();
		return true;
    } 
    bool get(const Node& node, Node* curNode) {

        auto res = hashmap_.find(node->hash);

		if (res == hashmap_.end())
		{
            curNode = nullptr;
            return false;
		}


		auto pos = res->second;

		if (node.stage_ == PROTECTION)
		{
            protectionList_.erase(pos);


			protectionList_.push_front(node);
            *curNode = *(*pos);
			res->second = protectionList_.begin();
			
            return true;
		}

		if (_protectionList.size() < _probationCapacity)
		{
			node.stage_ = PROTECTION;
			_probationList.erase(pos);

			_protectionList.push_front(node);
			res->second = _protectionList.begin();

			return std::make_pair(node, true);
		}

		LRUNode backNode = _protectionList.back();

		std::swap(backNode._flag, node._flag);

		_probationList.erase(_hashmap[node._key]);
		_protectionList.erase(_hashmap[backNode._key]);

		_probationList.push_front(backNode);
		_protectionList.push_front(node);

		_hashmap[backNode._key] = _probationList.begin();
		_hashmap[node._key] = _protectionList.begin();


		return std::make_pair(node, true);
    }
private:
    size_t probationCapacity_;
    size_t protectionCapacity_;

    std::unordered_map<int, typename std::list<Node>::iterator> hashmap_;

    std::list<Node*> probationList_;
    std::list<Node*> protectionList_;
};

}