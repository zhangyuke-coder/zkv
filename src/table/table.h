#pragma once
#include <string_view>
#include "offset_size.h"
#include "db/options.h"
#include "db/status.h"
#include "db/iterator.h"
#include "db/options.h"
namespace zkv {
class DataBlock;
class Footer;
class FileReader;
class Table final {
public:
    Table(const Options* options, const FileReader* file_reader);
    DBStatus Open(uint64_t);
    DBStatus ReadBlock(const OffSetSize&, std::string&);

    void ReadMeta(const Footer* footer);
    void ReadFilter(const std::string_view& filter_handle_value);
    Iterator* NewIterator(const ReadOptions&) const;
    Iterator* BlockReader(const ReadOptions&,
                            const std::string_view&);
    DataBlock* Index_block() {
        return index_block_.get();
    }

private:
    const FileReader* file_reader_;
    const Options* options_;
    std::unique_ptr<DataBlock> index_block_;
    uint64_t table_id_ = 0;
    std::string bf_;
};
}