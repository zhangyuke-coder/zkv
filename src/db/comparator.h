#pragma once
#include <stdint.h>
#include <string>
#include <string_view>
namespace zkv {

class Comparator {
public:
    virtual ~Comparator() = default;
    virtual const char* Name() = 0;
    virtual int32_t Compare(const char* a, const char* b) = 0;
    virtual void FindShortest(std::string& start, const std::string_view& limit) = 0;
    

};
class ByteComparator final : public Comparator {
public:
    const char* Name() override;
    int32_t Compare(const char* a, const char* b) override;
    void FindShortest(std::string& start, const std::string_view& limit) override;
};


};