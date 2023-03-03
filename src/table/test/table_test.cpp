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
// static  vector<string> kTestKeys = {"corekv", "corekv1", "corekv2","corekv3","corekv4","corekv7"};
int main() {
    static const std::string st = "d.txt";

    Options options;
    options.block_compress_type = kSnappyCompression;
    options.filter_policy = std::make_unique<BloomFilter>(30);
    options.comparator = std::make_unique<ByteComparator>();

    FileWriter* file_handler = new FileWriter(st);
    TableBuilder* tb = new TableBuilder(options, file_handler);
    vector<string> kTestKeys = {};
	for (int i = 0; i < 1000; ++i) {
		int idx = i;

		string k = "key";
		k += to_string(idx);
		string_view key = static_cast<string_view>(k);
		kTestKeys.emplace_back(key);
	}
	sort(kTestKeys.begin(), kTestKeys.end());
    for (const auto& item : kTestKeys) {
        tb->Add(item, item);
    }
    // tb->Finish();
    // tb->Add("zyk", "yyds");
    // tb->Add("zysk", "yyds");
    // tb->Add("z1sk", "yyds");
    tb->Finish();
    // cout << tb->GetFileSize() << endl;
    // cout << tb->GetFileSize() << endl;
    ReadOptions option_read;
    Options option;
    option.block_compress_type = kSnappyCompression;
    option.filter_policy = std::make_unique<BloomFilter>(30);
    option.comparator = std::make_unique<ByteComparator>();
    FileReader file_reader(st);
    Table tab(&option, &file_reader);
    tab.Open(FileTool::GetFileSize(st));
    auto it = tab.NewIterator(ReadOptions());
    it->Valid();
    // cout << "talbe_size: " << FileTool::GetFileSize(st) << endl;
    // tab.ReadBlock()

    // auto index_ptr = tab.Index_block();
    // cout << index_ptr->size() << endl;
    // cout << index_ptr->NumRestarts() << endl;
    // cout << index_ptr->data_ << endl;
    // auto index_iter = index_ptr->NewIterator(std::make_shared<ByteComparator>());
    // index_iter->SeekToFirst();
    // while(index_iter->Valid()) {
    //     // cout << "[" << index_iter->key() << "," << index_iter->value() << "]"
    //     //     << " ";
    //     cout << "[" << index_iter->key() << endl;
    //     index_iter->Next();
    //     cout << "z" << endl;
    // }
    // auto index_iter = tab.GetIndexBlockIter();
    // std::string_view key = "zyk";
    // index_iter->Seek(key);
    // if (index_iter->Valid() && index_iter->key() == key) {
    // // LOG(corekv::LogLevel::ERROR, "Hit Key=%s",key.data());
    //     printf("Hit zyasd=%s \n",key.data());
    //     // ReadFilter(iter->value());
    // }
    // tab.Print();
    delete file_handler;
    delete tb;
    // delete index_iter;
    // cout << "success" << endl;
    // cout << "success" << endl;
}


// #include <iostream>
// using namespace std;


// class base {
// public:
//     base() {
//         cout << "base" << endl;
//     }
//     virtual void print(){};
// };

// class son : public base {
// public:
//     son() {
//         cout << "son" << endl;
//     }
//     virtual void print() override{
//         cout << "asdf" << endl;
//     }
// };
// base* test() {
//     base* iter;
//     iter = new son();
//     // iter->print();
//     return iter;
// }
// int main() {
//     base* it = test();
//     // base* it = new son();
    
//     it->print();
// }