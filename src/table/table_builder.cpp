#include "table_builder.h"
#include "table_options.h"
#include "db/comparator.h"
#include "../utils/codec.h"
#include "../utils/crc32.h"
#include "footer.h"
namespace zkv{
using namespace util;
TableBuilder::TableBuilder(const Options& options, 
                            FileWriter* file_handler):
                            options_(options),
                            index_options_(options),
                            data_block_builder_(&options_),
                            index_block_builder_(&index_options_),
                            file_handler_(file_handler),
                            filter_block_builder_(options_){
    index_options_.block_restart_interval = 1;
}


void TableBuilder::Add(const std::string_view& key, 
                       const std::string_view& value) {
    if(key.empty()) {
        return;
    }
    if(need_create_index_block_ && options_.comparator) {
        options_.comparator->FindShortest(pre_block_last_key_, key);
        std::string output;
        index_block_offset_size_builder_.Encode(pre_block_offset_size_, output);
        index_block_builder_.Add(pre_block_last_key_, output);
        need_create_index_block_ = false;
    }

    if(filter_block_builder_.Availabe()) {
        filter_block_builder_.Add(key);
    }
    pre_block_last_key_ = key;
    ++entry_count_;
    data_block_builder_.Add(key, value);
    if(data_block_builder_.CurrentSize() >= options_.block_size) {
        Flush();
    }

}




void TableBuilder::Flush() {
    if(data_block_builder_.CurrentSize() == 0) {
        return;
    }
    WriteDataBlock(data_block_builder_, pre_block_offset_size_);
    if (status_ == Status::kSuccess) {
        //在下一轮循环中时，就需要更新我们的index block数据
        need_create_index_block_ = true;
        // 针对剩余的还未刷盘的数据我们需要手动进行刷盘
        status_ = file_handler_->FlushBuffer();
  }
}

void TableBuilder::WriteDataBlock(DataBlockBuilder& data_block_builder,
                                  OffSetSize& offset_size) {
    data_block_builder.Finish();
    const std::string& data = data_block_builder.Data();
    WriteBytesBlock(data, options_.block_compress_type, offset_size);
    data_block_builder.Reset();
}

void TableBuilder::WriteBytesBlock(const std::string& datas,
                        BlockCompressType block_compress_type,
                        OffSetSize& offset_size) {
    std::string compress_data = datas;
    bool compress_success = false;
    BlockCompressType type = block_compress_type;
    switch (block_compress_type) {
    case kSnappyCompression: {
            compress_data = datas;
            compress_success = true;
    } 
    break;
    default:
        type = kNonCompress;
    break;
    }


    offset_size.offset = block_offset_;
    offset_size.length = datas.size();
    status_ = file_handler_->Append(datas.data(), datas.size());
    char trailer[kBlockTrailerSize];
    trailer[0] = static_cast<uint8_t>(type);
    uint32_t crc = crc32::Value(datas.data(), datas.size());
    crc = crc32::Extend(crc, trailer, 1);  // Extend crc to cover block type
    EncodeFixed32(trailer + 1, crc32::Mask(crc));
    status_ = file_handler_->Append(trailer, kBlockTrailerSize);
    if (status_ == Status::kSuccess) {
        block_offset_ += offset_size.length + kBlockTrailerSize;
    }
}





void TableBuilder::Finish() {
    if(!Success()) {
        return;
    }
    Flush();
    OffSetSize filter_block_offset, meta_filter_block_offset, index_block_offset;
    if (filter_block_builder_.Availabe()) {
        // 这部分写的是filter即布隆过滤器部分数据
        filter_block_builder_.Finish();
        const auto& filter_block_data = filter_block_builder_.Data();

        // 这部分不需要进行压缩，所以直接调用WriteBytesBlock函数
        WriteBytesBlock(filter_block_data, BlockCompressType::kNonCompress,
                        filter_block_offset);
        // 该部分是获取布隆过滤器部分数据在整个sst中的位置，然后将这部分数据写入到sst中
        // 这部分目的是针对不同的块可以使用不同的filter_policy
        DataBlockBuilder meta_filter_block(&options_);
        OffsetBuilder filter_block_offset_builder;
        std::string handle_encoding_str;
        filter_block_offset_builder.Encode(filter_block_offset,
                                            handle_encoding_str);
        meta_filter_block.Add(options_.filter_policy->Name(), handle_encoding_str);
        WriteDataBlock(meta_filter_block, meta_filter_block_offset);
    }
    if (need_create_index_block_ && options_.comparator) {
        // 最后一个key这里我们就不做优化了，直接使用(leveldb中是FindShortSuccessor(std::string*
        // key)函数)
        // index中的value保存的是当前key在block中的偏移量和对应的block大小
        std::string output;
        index_block_offset_size_builder_.Encode(pre_block_offset_size_, output);
        index_block_builder_.Add(pre_block_last_key_, output);
        need_create_index_block_ = false;
    }
    WriteDataBlock(index_block_builder_, index_block_offset);

    Footer footer;
    footer.SetFilterBlockMetaData(meta_filter_block_offset);
    footer.SetIndexBlockMetaData(index_block_offset);
    std::string footer_output;
    footer.EncodeTo(&footer_output);
    file_handler_->Append(footer_output.data(), footer_output.size());
    block_offset_ += footer_output.size();
    file_handler_->Close();
}








}