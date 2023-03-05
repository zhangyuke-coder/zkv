#pragma once
#include <stdint.h>
#include <string_view>
#include <memory>
#include <db/iterator.h>
namespace zkv {
class Comparator;
class DataBlock {
public:
    explicit DataBlock(const std::string_view& contents);

    DataBlock(const DataBlock&) = delete;
    DataBlock& operator=(const DataBlock&) = delete;
    ~DataBlock();
    size_t size() const { return size_; }
    Iterator* NewIterator(std::shared_ptr<Comparator> comparator);






public:
    class Iter;
    uint32_t NumRestarts() const;

    const char* data_;
    size_t size_;
    uint32_t restart_offset_;  // Offset in data_ of restart array
    bool owned_;               // Block owns data_[]
};
}