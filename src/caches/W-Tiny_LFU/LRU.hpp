#pragma once
#include <unordered_map>
#include <list>

enum SEGMENT_ZONE
{
	PROBATION,
	PROTECTION
};

template<typename T>
struct LRUNode
{
public:
	uint32_t _key;
	T _value;
	uint32_t _conflict;	
	bool _flag;
	explicit LRUNode(uint32_t key = -1, const T& value = T(), uint32_t conflict = 0, bool flag = PROBATION)
		: _key(key)
		, _value(value)
		, _conflict(conflict)
		, _flag(flag)
	{}

};

template<typename T>
class LRUCache
{
public:
	typedef LRUNode<T> LRUNode_;

	explicit LRUCache(size_t capacity)
		: _capacity(capacity)
		, _hashmap(capacity)
	{}

	std::pair<LRUNode_, bool> get(const LRUNode_& node)
	{
		auto res = _hashmap.find(node._key);
		if (res == _hashmap.end())
		{
			return std::make_pair(LRUNode(), false);
		}

		typename std::list<LRUNode_>::iterator pos = res->second;
		LRUNode_ curNode = *pos;

		_lrulist.erase(pos);
		_lrulist.push_front(curNode);

		res->second = _lrulist.begin();

		return std::make_pair(curNode, true);
	}

	std::pair<LRUNode, bool> put(const LRUNode& node)
	{
		bool flag = false; 
		LRUNode delNode;


		if (_lrulist.size() == _capacity)
		{
			delNode = _lrulist.back();
			_lrulist.pop_back();
			_hashmap.erase(delNode._key);

			flag = true;
		}

		_lrulist.push_front(node);
		_hashmap.insert(make_pair (node._key, _lrulist.begin()));
		return std::make_pair(delNode, flag);
	}

	size_t capacity() const
	{
		return _capacity;
	}

	size_t size() const 
	{
		return _lrulist.size();
	}

private:
	size_t _capacity;

	std::unordered_map<int, typename std::list<LRUNode>::iterator> _hashmap;

	std::list<LRUNode> _lrulist;
};