#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "db/skiplist.h"
#include "memory/area.h"
#include "db/comparator.h"
namespace zkv {


template <typename _KeyType, typename _KeyComparator, typename _Allocator>
class MemTable {
public:
    using Table = SkipList<_KeyType, _KeyComparator, _Allocator>;
    

private:
    Table table_;
};



}