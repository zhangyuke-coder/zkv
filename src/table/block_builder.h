#pragma once
#include <string>
#include <string_view>
#include "../db/options.h"
namespace zkv {

class DataBlockBuilder final {
public:
    DataBlockBuilder(const Options* options);
    void Add(const std::string_view& key, const std::string_view& value);
    void Finish();

    const uint64_t CurrentSize() {
        return buffer_.size() + restarts_.size() * sizeof(uint32_t) +
                sizeof(uint32_t);
    }
    const std::string& Data() { return buffer_; }
    void Reset() {
        restarts_.clear();
        restarts_.emplace_back(0);
        buffer_.clear();

    }


private:
    void AddRestartPointers();

private:
    bool is_finished_ = false;
    std::string buffer_;
    std::vector<uint32_t> restarts_;
    const Options* options_;
    uint32_t restart_pointer_counter_ = 0;
    std::string pre_key_;
};









class FilterBlockBuilder final {
public:
    FilterBlockBuilder(const Options& options);
    bool Availabe() { return policy_filter_ != nullptr; }
    void Add(const std::string_view& key);
    void CreateFilter();
    bool MayMatch(const std::string_view& key);
    bool MayMatch(const std::string_view& key,
                const std::string_view& bf_datas);
    const std::string& Data();
    void Finish(); 

private:
    std::string buffer_;
    std::vector<std::string> datas_;
    FilterPolicy* policy_filter_ = nullptr;
};
}