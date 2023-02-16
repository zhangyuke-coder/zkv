#pragma once
#include <stdint.h>
#include <string>
#include <string_view>
#include "db/status.h"
namespace zkv {

struct OffSetSize{
    uint64_t offset = 0;
    uint64_t length = 0;
};

class OffsetBuilder final {
public:
    void Encode(const OffSetSize& offset_size, std::string& output);
    DBStatus Decode(const char* input, OffSetSize& offset_size);
    std::string DebugString(const OffSetSize& offset_size);

};
}