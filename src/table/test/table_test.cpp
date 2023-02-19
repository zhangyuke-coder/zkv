#include "table/table.h"
#include "table/table_builder.h"
#include "db/iterator.h"
#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>
#include "db/comparator.h"
#include "file/file.h"
#include "filter/bloomfilter.h"
#include "table/table.h"

using namespace std;
using namespace zkv;
static  vector<string> kTestKeys = {"corekv", "corekv1", "corekv2","corekv3","corekv4","corekv7"};
int main() {
    static const std::string st = "d.txt";

    Options options;
    options.block_compress_type = kSnappyCompression;
    options.filter_policy = std::make_unique<BloomFilter>(30);
    options.comparator = std::make_unique<ByteComparator>();

    FileWriter* file_handler = new FileWriter(st);
    TableBuilder* tb = new TableBuilder(options, file_handler);
    // for(int i = 0; i < 100; i++) {
    //     kTestKeys.emplace_back(to_string(i));
    // }
    // for (const auto& item : kTestKeys) {
    //     tb->Add(item, item);
    // }
    // tb->Finish();
    tb->Add("zyk", "yyds");
    tb->Finish();
    delete file_handler;
    delete tb;

    ReadOptions option_read;
    FileReader file_reader(st);
    Table tab(&options, &file_reader);
    tab.Open(FileTool::GetFileSize(st));

    // cout << "talbe_size: " << FileTool::GetFileSize(st) << endl;
    // tab.ReadBlock()

    auto index_ptr = tab.Index_block();
    // cout << index_ptr->size() << endl;
    auto index_iter = index_ptr->NewIterator(options.comparator);
    // index_iter->SeekToFirst();
    // while(index_iter->Valid()) {
    //     cout << "[" << index_iter->key() << "," << index_iter->value() << "]"
    //         << " ";
    //     index_iter->Next();
    //     cout << "z" << endl;
    // }
    // cout << "success" << endl;
    //     cout << "success" << endl;
}