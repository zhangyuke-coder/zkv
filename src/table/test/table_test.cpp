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
    // ReadOptions option_read;
    // Options option;
    // option.block_compress_type = kSnappyCompression;
    // option.filter_policy = std::make_unique<BloomFilter>(30);
    // option.comparator = std::make_unique<ByteComparator>();
    FileReader file_reader(st);
    Table tab(&options, &file_reader);
    tab.Open(FileTool::GetFileSize(st));
    tab.test_table();
    // for (int i = 0; i < 11; ++i) {
	// 	int idx = i;

	// 	string k = "key";
	// 	k += to_string(idx);
    //     if(tab.IsContain(k)) {
    //         cout << "yes" << endl;
    //     } else {
    //         cout << "no" << endl;
    //     }
	// }

    // tab.testIndex_block();
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