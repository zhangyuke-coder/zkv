#pragma once 
#include <vector>
#include <stdint.h>
#include <atomic>
namespace zkv {
class SimpleVectorAlloc final {
public:
    SimpleVectorAlloc();
    SimpleVectorAlloc(const SimpleVectorAlloc&) = delete;
    SimpleVectorAlloc& operator=(const SimpleVectorAlloc&) = delete;
    ~SimpleVectorAlloc();
    void* Allocate(uint32_t bytes);
    uint32_t MemoryUsage() const {
        return memory_usage_.load(std::memory_order_relaxed);
    }
    void Deallocate(void* p, int32_t n);
private:
    char* AllocateFallback(uint32_t bytes);
    char* AllocateNewBlocks(uint32_t block_bytes);
    char* alloc_ptr_;
    uint32_t alloc_bytes_remaining_;
    std::vector<char*> blocks_;
    std::atomic<uint32_t> memory_usage_;
};
}