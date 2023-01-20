#pragma once
#include <vector>
#include <stdint.h>
#include <array>
#include <bitset>
namespace zkv {

class CountMinSketch final {
    static constexpr uint8_t kCmsDepth = 4;
public: 
    explicit CountMinSketch(uint32_t counter_num);
    ~CountMinSketch() = default;
    void Increment(uint32_t hash_val);
    void Reset();
    void Clear();
    int32_t Estimate(uint32_t hash_val);

private:
    class SingleGroup final {
    public:
        SingleGroup(uint32_t counter_num) {
            Init(counter_num);
        }
        SingleGroup() = default;
        ~SingleGroup() = default;
        void Init(uint32_t counter_num);
        void Increment(uint32_t counter_index);
        void Reset();
        void Clear();
        int32_t Get(uint32_t counter_index);
    private: 
        static constexpr uint8_t kCounterBitLen = 4;
        using CounterType = std::bitset<kCounterBitLen>;
    public:
        std::vector<CounterType> base_;
    };
private:
    uint32_t mask_;
    std::array<uint64_t, kCmsDepth> seeds_;
    std::array<SingleGroup, kCmsDepth> groups_;
};

};