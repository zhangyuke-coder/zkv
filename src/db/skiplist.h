#pragma once
#include <stdint.h>
#include <atomic>
#include "../utils/random_util.h"
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
    void Insert(const _KeyType& key) {
        Node* prev[SkipListOption::kMaxHeight] = {nullptr};
        Node* node = FindGreaterOrEqual(key, prev);
        if(node != nullptr) {
            if(Equal(node->key, key)) {
                printf("key:%s has existed \n", key);
                return;
            }
        }

        int32_t new_level = RandomHeight();
        int32_t cur_max_level = GetMaxHeight();
        if(new_level > cur_max_level) {
            for(int32_t index = cur_max_level; index < new_level; ++index) {
                prev[index] = head_;
            }
            cur_height_.store(new_level, std::memory_order_relaxed);
        }
        Node* new_node = NewNode(key, new_level);
        for (int32_t index = 0; index < new_level; ++index) {
            new_node->NoBarrier_SetNext(index, prev[index]->NoBarrier_Next(index));
            prev[index]->NoBarrier_SetNext(index, new_node);
        }

    }
    bool Contains(const _KeyType& key) {
        Node* node = FindGreaterOrEqual(key, nullptr);
        return node != nullptr && Equal(key, node->key);
    }

    bool Equal(const _KeyType& a, const _KeyType& b) {
        return comparator_.Compare(a, b) == 0;
    }
private:
    Node* NewNode(const _KeyType& key, int32_t height) {

    }
    int32_t RandomHeight();
    int32_t GetMaxHeight() {
        return cur_height_.load(std::memory_order_relaxed);
    }
    bool KeyIsAfterNode(const _KeyType& key, Node* n) {
        return (n != nullptr && comparator_.Compare(n->key, key) < 0); 
    }
    Node* FindGreaterOrEqual(const _KeyType& key, Node** prev) {
        Node* cur = head_;
        int32_t level = GetMaxHeight() - 1;
        while(true) {
            Node* next = cur->Next(level);
            if(KeyIsAfterNode(key, next)) {
                cur = next;
            } else {
                if(prev != nullptr) {
                    prev[level] = cur;
                } 
                if(level == 0) {
                    return next;
                } 
                level--;
            }
        }
    }
private:
    _KeyComparator comparator_;
    _Allocator arena_;
    std::atomic<int32_t> cur_height_;
    Node* head_ = nullptr;
    RandomUtil rnd_;
};

template <typename _KeyType, typename _KeyComparator, typename _Allocator>
struct SkipList<_KeyType, _KeyComparator, _Allocator>::Node {
    explicit Node(const _KeyType& k) : key(k) {} 
    const _KeyType key;
    Node* Next(int32_t n) {
        return next_[n].load(std::memory_order_acquire);
    }
    void SetNext(int n, Node* x) {  
        assert(n >= 0);
        // Use a 'release store' so that anybody who reads through this
        // pointer observes a fully initialized version of the inserted node.
        next_[n].store(x, std::memory_order_release);
    }
    Node* NoBarrier_Next(int n) {
        return next_[n].load(std::memory_order_relaxed);
    }
    void NoBarrier_SetNext(int n, Node* x) {
        next_[n].store(x, std::memory_order_relaxed);
    }

private:
    std::atomic<Node*> next_[1];
};


template <typename _KeyType, typename _Comparator, typename _Allocator>
typename SkipList<_KeyType, _Comparator, _Allocator>::Node*
SkipList<_KeyType, _Comparator, _Allocator>::NewNode(const _KeyType& key,
                                                     int32_t height) {
    char* node_memory = (char*)arena_.Allocate(
        sizeof(Node) + sizeof(std::atomic<Node*>) * (height - 1)
    );
    return new (node_memory) Node(key);
}

template <typename _KeyType, typename _Comparator, typename _Allocator>
int32_t SkipList<_KeyType, _Comparator, _Allocator>::RandomHeight() {
  int32_t height = 1;
  while (height < SkipListOption::kMaxHeight &&
         ((rnd_.GetSimpleRandomNum() % SkipListOption::kBranching) == 0)) {
    height++;
  }
  return height;
}
}