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
  
  ReadIndex(&footer);
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
    printf("Hit Key=%s \n",key.data());
    ReadFilter(iter->value());
  }
  delete iter;
}
void Table::blockIndex(std::string& index_value) {

}
void Table::ReadIndex(const Footer* footer) {


  std::string index_meta_data;
  ReadBlock(footer->GetIndexBlockMetaData(), index_meta_data);
  index_string_ = std::string_view(index_meta_data.data(),
                             footer->GetIndexBlockMetaData().length);
  index_block_ = std::make_unique<DataBlock>(index_string_);
  auto index_iter = index_block_->NewIterator(options_->comparator);
  index_iter->SeekToFirst();
  while(index_iter->Valid()) {
      // std::cout << "index_key   " << index_iter->key() << std::endl;
      // OffsetBuilder build;
      // OffSetSize off;
      // build.Decode(index_iter->value().data(), off);
      index_key_.emplace_back(index_iter->key());
      index_iter->Next();
  }
  // std::cout << "zxcvxc" << std::endl;
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
  printf("asldjflkasjdflkjsdklfj\n");
  delete reinterpret_cast<DataBlock*>(arg);
}

static void ReleaseBlock(void* arg, void* h) {
  CacheNode<uint64_t, DataBlock>* node = reinterpret_cast<CacheNode<uint64_t, DataBlock>*>(arg);
  Cache<uint64_t, DataBlock>* cache = reinterpret_cast<Cache<uint64_t, DataBlock>*>(arg);
  cache->Release(node);
}

Iterator* Table::BlockReader(OffSetSize offset_size) {
  auto* block_cache = options_->block_cache;
  DataBlock* block = nullptr;
  CacheNode<uint64_t, DataBlock>* cache_handle = nullptr;
  // OffSetSize offset_size;
  // OffsetBuilder offset_builder;
  // offset_builder.Decode(index_value.data(), offset_size);
  DBStatus s;
  std::string contents;
  if (block_cache != nullptr) {
    uint64_t cache_id = table_id_ * 10 + offset_size.offset;
    cache_handle = block_cache->Get(cache_id);
    if (cache_handle != nullptr) {
      block = cache_handle->value;
    } else {
      s = ReadBlock(offset_size, contents);
      if (s == Status::kSuccess) {
        block = new DataBlock(contents);
        {
          block_cache->RegistCleanHandle(DeleteCachedBlock);
          block_cache->Insert(cache_id, block);
        }
      }
    }
  } else {
    s = ReadBlock(offset_size, contents);
    std::string_view real_data(contents.data(),
                             offset_size.length);
    if (s == Status::kSuccess) {
      block = new DataBlock(real_data);
    }
  }
  std::string* a = new std::string("asdfasdf");
 
  Iterator* iter;
  // Iterator* iter = block->NewIterator(options_->comparator);
  if (block != nullptr) {
    iter = block->NewIterator(options_->comparator);
    if (cache_handle == nullptr) {
      iter->RegisterCleanup(&DeleteBlock, block, nullptr);
    } else {
      iter->RegisterCleanup(&ReleaseBlock, block_cache, cache_handle);
    }
  } else {
    iter = NewErrorIterator(s);
  }


  iter->SeekToFirst();
  while(iter->Valid()) {
    // cout << "[" << index_iter->key() << "," << index_iter->value() << "]"
    //     << " ";
    std::cout << "index_key" << iter->key() << std::endl;
    iter->Next();
    // std::cout << "z" << std::endl;
  }
  // print(iter);
  // printf("iter_address: %p \n", iter);
  // return a;
  // return block->NewIterator(options_->comparator);


  // std::string index_meta_data;
  // ReadBlock(offset_size, index_meta_data);
  // std::string_view real_data(index_meta_data.data(),
  //                            offset_size.length);
  
  // index_block_ = std::make_unique<DataBlock>(real_data);
  // Iterator* index_iter = index_block_->NewIterator(std::make_shared<ByteComparator>());

  // index_iter->SeekToFirst();
  // while(index_iter->Valid()) {
  //     // cout << "[" << index_iter->key() << "," << index_iter->value() << "]"
  //     //     << " ";
  //     std::cout << "index_key" << index_iter->key() << std::endl;
  //     index_iter->Next();
  //     // std::cout << "z" << std::endl;
  // }
  return iter;
}






class Table::TwoLevelIter : public Iterator {
public:
    TwoLevelIter(const Options* options, std::string_view index_string) :
      index_string_(index_string), 
      options_(options) {
    
    }
    void test() {

    }
    bool Valid() const override {
      auto index_block_ = std::make_unique<DataBlock>(index_string_);
      auto index_iter = index_block_->NewIterator(options_->comparator);
      index_iter->SeekToFirst();
      while(index_iter->Valid()) {
        std::cout << "index_key   " << index_iter->key() << std::endl;
        // OffsetBuilder build;
        // OffSetSize off;
        // build.Decode(index_iter->value().data(), off);
        // index_key_.emplace_back(index_iter->key());
        index_iter->Next();
      }
      return true;
  }
  

    void SeekToFirst() override{

    }

    void SeekToLast() override{

    }

    void Seek(const std::string_view& target) override {

    }

    void Next() override{

    }

    void Prev() override{

    }
    std::string_view key() const override{

    }
    std::string value() override{

    }

    DBStatus status() const override{

    }
private:
  const Options* options_;
  const std::string_view index_string_;
};
Iterator* Table::NewIterator(const ReadOptions&) {
  return new TwoLevelIter(options_, index_string_);  
}

}  // namespace corekv