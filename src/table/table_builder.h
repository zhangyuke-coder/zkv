#pragma once
#include <string_view>
#include "db/options.h"
#include "block_builder.h"
#include "../file/file.h"
#include "offset_size.h"
namespace zkv {


class TableBuilder final {
public:
    TableBuilder(const TableBuilder&) = delete;
    TableBuilder& operator=(const TableBuilder&) = delete;
    TableBuilder(const Options& options, FileWriter* file_handler);
    void Add(const std::string_view& key, const std::string_view& value);
    void Finish();
    bool Success() { return status_ == Status::kSuccess; }
    uint32_t GetFileSize() {
        return block_offset_;
    }
    uint32_t GetEntryNum() {
        return entry_count_;
    }



private:
    void Flush();

    void WriteDataBlock(DataBlockBuilder& data_block, OffSetSize& offset_size);
    void WriteBytesBlock(const std::string& datas,
                        BlockCompressType block_compress_type,
                        OffSetSize& offset_size);









private:
    Options options_;
    Options index_options_;
    FileWriter* file_handler_ = nullptr;
    DataBlockBuilder data_block_builder_;
    DataBlockBuilder index_block_builder_;
    OffsetBuilder index_block_offset_size_builder_;
    FilterBlockBuilder filter_block_builder_;
    bool need_create_index_block_ = false;
    std::string pre_block_last_key_;
    OffSetSize pre_block_offset_size_;
    uint64_t entry_count_ = 0;
    uint32_t block_offset_ = 0;
    DBStatus status_;
};

}