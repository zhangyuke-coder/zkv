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
  void ReadIndex(const Footer* footer);
  void ReadFilter(const std::string_view& filter_handle_value);
  Iterator* NewIterator(const ReadOptions&);
 Iterator* BlockReader(OffSetSize offset_size);
  void blockIndex(std::string& index_value);
  private:
  class TwoLevelIter;
  const Options* options_;
  const FileReader* file_reader_;
  uint64_t table_id_ = 0;
  std::string bf_;
  // std::unordered_map<std::string_view, OffSetSize> data_key_offset_;
  std::vector<std::string_view> index_key_;
  std::string_view index_string_;
  // index_block对象，用于两层迭代器使用
  std::unique_ptr<DataBlock> index_block_;
};
}  // namespace corekv
