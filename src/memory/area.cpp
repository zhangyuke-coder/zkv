#include "area.h"
namespace zkv{
SimpleVectorAlloc::SimpleVectorAlloc()
    : alloc_ptr_(nullptr), alloc_bytes_remaining_(0), memory_usage_(0) {}
SimpleVectorAlloc::~SimpleVectorAlloc() {
    for(uint32_t i = 0; i < blocks_.size(); ++i) {
        delete[] blocks_[i];
    }
}
void SimpleVectorAlloc::Deallocate(void*, int32_t) {

}
char* SimpleVectorAlloc::AllocateFallback(uint32_t bytes) {
    auto tmp_ptr = AllocateNewBlocks(bytes * 2);
    if(!alloc_ptr_) alloc_ptr_ = tmp_ptr;
    alloc_bytes_remaining_ += bytes * 2;
    return alloc_ptr_;
}

void* SimpleVectorAlloc::Allocate(uint32_t bytes) {
    const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
    static_assert((align & (align - 1)) == 0, "Pointer size should be a power of 2");
    uint32_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
    uint32_t slop = (current_mod == 0 ? 0 : align - current_mod);
    uint32_t needed = bytes + slop;
    char* result = nullptr;
    if(needed <= alloc_bytes_remaining_) {
        result = alloc_ptr_ + slop;
        alloc_ptr_ += needed;
        alloc_bytes_remaining_ -= needed;
    } else {
        AllocateFallback(bytes);
        return Allocate(bytes);
    }
    return result;
}
char* SimpleVectorAlloc::AllocateNewBlocks(uint32_t block_bytes) {
    char* result = new char[block_bytes];
    blocks_.push_back(result);
    memory_usage_.fetch_add(block_bytes + sizeof(char*), std::memory_order_relaxed);
    return result;
}
};