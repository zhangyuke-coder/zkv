#pragma once
#include "LRU.hpp"

template<typename T>
class SegmentLRUCache
{
public:
	typedef LRUNode<T> LRUNode;

	explicit SegmentLRUCache(size_t probationCapacity, size_t protectionCapacity)
		: _probationCapacity(probationCapacity)
		, _protectionCapacity(protectionCapacity)
		, _hashmap(probationCapacity +  protectionCapacity)
	{}

	std::pair<LRUNode, bool> get(LRUNode& node)
	{

		auto res = _hashmap.find(node._key);
		if (res == _hashmap.end())
		{
			return std::make_pair(LRUNode(), false);
		}

		typename std::list<LRUNode>::iterator pos = res->second;

		if (node._flag == PROTECTION)
		{
			_protectionList.erase(pos);

			_protectionList.push_front(node);
			res->second = _protectionList.begin();
			
			return std::make_pair(node, true);
		}


		if (_protectionList.size() < _probationCapacity)
		{
			node._flag = PROTECTION;
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

	std::pair<LRUNode, bool> put(LRUNode& newNode)
	{

		newNode._flag = PROBATION;
		LRUNode delNode;


		if (_probationList.size() < _probationCapacity || size() < _probationCapacity + _protectionCapacity)
		{
			_probationList.push_front(newNode);
			_hashmap.insert(make_pair(newNode._key, _probationList.begin()));

			return std::make_pair(delNode, false);
		}


		delNode = _probationList.back();

		_hashmap.erase(delNode._key);
		_probationList.pop_back();

		_probationList.push_front(newNode);
		_hashmap.insert(make_pair(newNode._key, _probationList.begin()));

		return std::make_pair(delNode, true);
	}

	std::pair<LRUNode, bool> victim()
	{

		if (_probationCapacity + _protectionCapacity > size())
		{
			return std::make_pair(LRUNode(), false);
		}


		return std::make_pair(_probationList.back(), true);
	}

	int size() const
	{
		return _protectionList.size() + _probationList.size();
	}

private:
	size_t _probationCapacity;
	size_t _protectionCapacity;

	std::unordered_map<int, typename std::list<LRUNode>::iterator> _hashmap;

	std::list<LRUNode> _probationList;
	std::list<LRUNode> _protectionList;
};