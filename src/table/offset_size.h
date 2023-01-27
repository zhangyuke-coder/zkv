#pragma once
#include <stdint.h>
#include <string>
#include <string_view>

namespace zkv {

struct OffSetSize{
    uint64_t offset = 0;
    uint64_t length = 0;
};

class OffsetBuilder final {
public:
    void Encode(const OffSetSize& offset_size, std::string& output);
    
};
}