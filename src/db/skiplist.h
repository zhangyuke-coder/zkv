#pragma once
#include <stdint.h>
#include <atomic>
namespace zkv {
struct SkipListOption {
    static constexpr int32_t kMaxHeight = 20;
    static constexpr uint32_t kBranching = 4;
};

template <typename _KeyType, typename _KeyComparator, typename _Allocator>
class SkipList final {
    struct Node;
public:
    SkipList(_KeyComparator comparator);
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;
private:
    _KeyComparator comparator_;
    _Allocator arena_;
    Node* head_ = nullptr;
};


}