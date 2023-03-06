#include "table.h"
#include <memory>
#include "../db/comparator.h"
// #include "../logger/log.h"
#include "../utils/codec.h"
#include "../utils/crc32.h"
#include "data_block.h"
#include "footer.h"
#include "table_options.h"
#include "caches/cache.h"
#include <iostream>
#include "table/iterator_wrapper.h"
#include "table/two_level_iterator.h"
namespace zkv {

using namespace util;
Table::Table(const Options* options, const FileReader* file_reader)
    : options_(options), file_reader_(file_reader) {}
DBStatus Table::Open(uint64_t file_size) {
  if (file_size < kEncodedLength) {
    return Status::kInterupt;
  }
  std::string footer_space;
  footer_space.resize(kEncodedLength);
  auto status = file_reader_->Read(file_size - kEncodedLength, kEncodedLength,
                                   &footer_space);
  if (status != Status::kSuccess) {
    return status;
  }
  Footer footer;
  std::string_view st = footer_space;
  status = footer.DecodeFrom(&st);
  std::string index_meta_data;
  ReadBlock(footer.GetIndexBlockMetaData(), index_meta_data);
  std::string real_data(index_meta_data.data(),
                             footer.GetIndexBlockMetaData().length);
  index_meta_data_ = real_data;
  index_block_ = std::make_shared<DataBlock>(index_meta_data_);
  // std::cout << index_block_->NumRestarts() << std::endl;
  // testIndex_block();
  // auto iter = index_block_->NewIterator(options_->comparator);
  // iter->SeekToFirst();
  // DBStatus s;
  //   OffSetSize offset_size;
  //   OffsetBuilder offset_builder;
  //   std::string contents;
  //   offset_builder.Decode(iter->value().data(), offset_size);
  //   s = ReadBlock(offset_size, contents);
  //   std::string_view reals_data(contents.data(),
  //                             offset_size.length);
  //   DataBlock* dataBlock;
  //   if (s == Status::kSuccess) {
  //     dataBlock = new DataBlock(reals_data);
  //   }

  //   auto itdata = dataBlock->NewIterator(options_->comparator);
  //   itdata->SeekToFirst();
  //   while (itdata->Valid()) {
  //     std::cout << "[" << itdata->key() << "," << itdata->value() << "]"
  //     << " ";
  //     itdata->Next();
  //   }
  //   std::cout << std::endl;


  // test_table();


  //sssssssssssssssssssssssssssssssssssssssssssssssssssssssss
  ReadMeta(&footer);
  return status;
}

DBStatus Table::ReadBlock(const OffSetSize& offset_size, std::string& buf) {
  buf.resize(offset_size.length + kBlockTrailerSize);
  file_reader_->Read(offset_size.offset, offset_size.length + kBlockTrailerSize,
                     &buf);
  const char* data = buf.data();
  const uint32_t crc =
      crc32::Unmask(DecodeFixed32(data + offset_size.length + 1));
  const uint32_t actual = crc32::Value(data, offset_size.length + 1);

  if (crc != actual) {
    // LOG(corekv::LogLevel::ERROR, "Invalid Block");
    return Status::kInvalidObject;
  }
  switch (data[offset_size.length]) {
    case kSnappyCompression:
      // LOG(corekv::LogLevel::ERROR, "kSnappyCompression");
      break;
    default:
      // LOG(corekv::LogLevel::ERROR, "kNonCompress");
      break;
  }
  // buf.resize(offset_size.length);
  return Status::kSuccess;
}
void Table::ReadMeta(const Footer* footer) {
  if (options_->filter_policy == nullptr) {
    return;
  }
  std::string filter_meta_data;
  ReadBlock(footer->GetFilterBlockMetaData(), filter_meta_data);
  std::string_view real_data(filter_meta_data.data(),
                             footer->GetFilterBlockMetaData().length);
  std::unique_ptr<DataBlock> meta = std::make_unique<DataBlock>(real_data);
  Iterator* iter = meta->NewIterator(std::make_shared<ByteComparator>());
  std::string_view key = options_->filter_policy->Name();
  iter->Seek(key);
  if (iter->Valid() && iter->key() == key) {
    // LOG(corekv::LogLevel::ERROR, "Hit Key=%s",key.data());
    ReadFilter(iter->value());
  }
  delete iter;
}
void Table::ReadFilter(const std::string_view& filter_handle_value) {
  OffSetSize offset_size;
  OffsetBuilder offset_builder;
  offset_builder.Decode(filter_handle_value.data(), offset_size);
  ReadBlock(offset_size, bf_);
  bf_.resize(offset_size.length);
}
static void DeleteCachedBlock(const uint64_t& key, void* value) {
  DataBlock* block = reinterpret_cast<DataBlock*>(value);
  delete block;
}
static void DeleteBlock(void* arg, void*) {
  delete reinterpret_cast<DataBlock*>(arg);
}

static void ReleaseBlock(void* arg, void* h) {
  CacheNode<uint64_t, DataBlock>* node = reinterpret_cast<CacheNode<uint64_t, DataBlock>*>(arg);
  Cache<uint64_t, DataBlock>* cache = reinterpret_cast<Cache<uint64_t, DataBlock>*>(arg);
  cache->Release(node);
}
bool Table::IsContain(std::string_view key) {
  return options_->filter_policy->MayMatch(key, bf_);
}

void Table::test_table() {
  // auto iter = BlockReader(ReadOptions(), index_meta_data_);
  auto iter = index_block_->NewIterator(options_->comparator);
  auto iterwrap = IteratorWrapper(iter);
  iterwrap.SeekToFirst();
  while (iterwrap.Valid())
  {
    /* code */
    auto itdata = BlockReader(this, ReadOptions(), iterwrap.value());
    auto iterdatawrap = IteratorWrapper(itdata);
    iterdatawrap.SeekToFirst();
    while (iterdatawrap.Valid()) {
      std::cout << "[" << iterdatawrap.key() << "," << iterdatawrap.value() << "]"
		<< " ";
    iterdatawrap.Next();
    }
    std::cout << std::endl;
    iterwrap.Next();
  }
  
  // iter->SeekToFirst();
  // while (iter->Valid()) {
  //   std::cout << "key" << iter->key() << std::endl;
  //   auto itdata = BlockReader(ReadOptions(), iter->value());
  //   itdata->SeekToFirst();
  //   while (itdata->Valid()) {
  //     itdata->Next();
  //   }
  //   std::cout << std::endl;
  //   iter->Next();
  // }
}


Iterator* Table::BlockReader(void* arg, const ReadOptions& options,
                             const std::string_view& index_value) {
  Table* table = reinterpret_cast<Table*>(arg);
  auto* block_cache = table->options_->block_cache;
  DataBlock* block = nullptr;
  CacheNode<uint64_t, DataBlock>* cache_handle = nullptr;
  OffSetSize offset_size;
  OffsetBuilder offset_builder;
  offset_builder.Decode(index_value.data(), offset_size);
  DBStatus s;
  std::string contents;
  if (block_cache != nullptr) {
    uint64_t cache_id = table->table_id_ * 10 + offset_size.offset;
    cache_handle = block_cache->Get(cache_id);
    if (cache_handle != nullptr) {
      block = cache_handle->value;
    } else {
      s = table->ReadBlock(offset_size, contents);
      // std::string_view reals_data(contents.data(),
      //                         offset_size.length);
      std::string* reals_data = new std::string(contents.data(),
                              offset_size.length);
      if (s == Status::kSuccess) {
        block = new DataBlock(*reals_data);
        {
          block_cache->RegistCleanHandle(DeleteCachedBlock);
          block_cache->Insert(cache_id, block);
        }
      }
    }
  } else {
    
    s = table->ReadBlock(offset_size, contents);
    // std::string_view reals_data(contents.data(),
    //                           offset_size.length);
    std::string* reals_data = new std::string(contents.data(),
                              offset_size.length);
    if (s == Status::kSuccess) {
      block = new DataBlock(*reals_data);
    }
  }

  Iterator* iter;
  if (block != nullptr) {
    iter = block->NewIterator(table->options_->comparator);
    if (cache_handle == nullptr) {
      iter->RegisterCleanup(&DeleteBlock, block, nullptr);
    } else {
      iter->RegisterCleanup(&ReleaseBlock, block_cache, cache_handle);
    }
  } else {
    iter = NewErrorIterator(s);
  }
  return iter;
}


// Iterator* Table::NewIterator(std::shared_ptr<Comparator> comparator) {
//   return NewTwoLevelIterator(
//       index_block_->NewIterator(options_->comparator),
//       &Table::BlockReader, const_cast<Table*>(this), ReadOptions());
// }

// class Table::Iter : public Iterator {
// public:
//   Iter(std::shared_ptr<Comparator> comparator, std::shared_ptr<DataBlock::Iter> index_iter
//         ) :
//       comparator_(comparator), index_iter_(index_iter) {

//   }
//    ~Iter() {}
//   bool Valid() const override { 
//       // printf("valid %u, %u\n", current_, restarts_);
//       return data_iter_->Valid();
//   }
//   void SeekToFirst() {
//     index_iter_->SeekToFirst();
//     data_iter_ = BlockReader(ReadOptions(), index_iter_->value());
//   }

// private:
//     std::shared_ptr<Comparator> comparator_;
//     std::shared_ptr<DataBlock::Iter> index_iter_;
//     std::shared_ptr<DataBlock::Iter> data_iter_;
//     // const char* const data_; //block 起始位置
//     // uint32_t const restarts_; //重启点开始位置
//     // uint32_t const num_restarts_; //重启点数量
//     // uint32_t restart_index_; //重启点的偏移


//     // uint32_t current_;  //当前entry偏移
//     // uint32_t offset_ = 0; //下一个entry的偏移


//     // DBStatus status_;
//     // std::string key_;
//     // std::string value_;


// };


} 