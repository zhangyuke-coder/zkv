#pragma once 
#include <stdlib.h>
#include <unordered_map>
#include <list>
#include <vector>
#include "wcache_node.h"
namespace zkv {
enum SEGMENT_ZONE
{
	PROBATION,
	PROTECTION
};
template <typename KeyType, typename ValueType>
class Wslru {
public:
    using Node = WCacheNode<KeyType, ValueType>;
	explicit Wslru(size_t probationCapacity, size_t protectionCapacity)
		: probationCapacity_(probationCapacity)
		, protectionCapacity_(protectionCapacity)
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

        newNode->stage_ = PROBATION;
        if(probationList_.size() < probationCapacity_ || size() < probationCapacity_ + protectionCapacity_) {
            probationList_.push_front(newNode);
            hashmap_[newNode->hash] = probationList_.begin();
            return false;
        }

        delNode = probationList_.back();
        hashmap_.erase(delNode->hash);
        probationList_.pop_back();

        probationList_.push_front(newNode);
        hashmap_[newNode->hash] = probationList_.begin();
		return true;
    } 
    bool get(Node& node, Node* curNode) {

        auto res = hashmap_.find(node.hash);

		if (res == hashmap_.end())
		{
            curNode = nullptr;
            return false;
		}


		auto pos = res->second;

		if (node.stage_ == PROTECTION)
		{
            protectionList_.erase(pos);

            *curNode = node;
			protectionList_.push_front(curNode);
            
			res->second = protectionList_.begin();
			
            return true;
		}

		if (protectionList_.size() < protectionCapacity_)
		{
			node.stage_ = PROTECTION;
			probationList_.erase(pos);
            *curNode = node;
			protectionList_.push_front(curNode);
			res->second = protectionList_.begin();
            
			return true;
		}

		auto backNode = protectionList_.back();

		std::swap(backNode->stage_, node.stage_);

		probationList_.erase(hashmap_[node.hash]);
		protectionList_.erase(hashmap_[backNode->hash]);
        *curNode = node;
		probationList_.push_front(backNode);
		protectionList_.push_front(curNode);

		hashmap_[backNode->hash] = probationList_.begin();
		hashmap_[node.hash] = protectionList_.begin();


		return true;
    }
    bool victim(Node* curNode) {

		if (probationCapacity_ + protectionCapacity_ > size())
		{
			return false;
		}
        *curNode = *(probationList_.back());
		return true;
    }
private:
    size_t probationCapacity_;
    size_t protectionCapacity_;

    std::unordered_map<uint32_t, typename std::list<Node*>::iterator> hashmap_;

    std::list<Node*> probationList_;
    std::list<Node*> protectionList_;
};

}