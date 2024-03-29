#pragma once
#include <memory>
#include <unordered_map>
#include "../db/iterator.h"
#include "../db/options.h"
#include "../file/file.h"
#include "block_builder.h"
#include "footer.h"
#include "offset_size.h"
namespace zkv {
class Table final {
public:
    Table(const Options* options, const FileReader* file_reader);
    DBStatus Open(uint64_t file_size);
    DBStatus ReadBlock(const OffSetSize&, std::string&);
    void ReadMeta(const Footer* footer);
    void ReadFilter(const std::string_view& filter_handle_value);
    Iterator* NewIterator(const ReadOptions&) const;
    static Iterator* BlockReader(void* arg, const ReadOptions&,
                                const std::string_view&);
    
    bool IsContain(std::string_view key);
    void test_table();
    Iterator* NewIterator(std::shared_ptr<Comparator> comparator);
private:
    const Options* options_;
    const FileReader* file_reader_;
    uint64_t table_id_ = 0;
    std::string bf_;
    // index_block对象，用于两层迭代器使用
    std::shared_ptr<DataBlock> index_block_;
    std::string index_meta_data_;

};
}  
