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
#include "table/two_level_iterator.h"
using namespace std;
using namespace zkv;
int main() {
    static const std::string st = "d.txt";

    Options options;
    options.block_compress_type = kSnappyCompression;
    options.filter_policy = std::make_unique<BloomFilter>(30);
    options.comparator = std::make_unique<ByteComparator>();

    FileWriter* file_handler = new FileWriter(st);
    TableBuilder* tb = new TableBuilder(options, file_handler);
    vector<string> kTestKeys = {};
	for (int i = 0; i < 10000; ++i) {
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
    tb->Finish();
    FileReader file_reader(st);
    Table tab(&options, &file_reader);
    tab.Open(FileTool::GetFileSize(st));
    // tab.test_table();
    auto iter = tab.NewIterator(options.comparator);
    // iter->testIter();
    iter->SeekToFirst();
    while(iter->Valid()) {
        std::cout << iter->key() << "  " << iter->value() << std::endl;
        iter->Next();
    }

}