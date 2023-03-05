#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// #include "../db/options.h"
#include "db/options.h"
// #include "../logger/log.h"
#include "db/comparator.h"
#include "file/file.h"
#include "filter/bloomfilter.h"
#include "table/block_builder.h"
#include "table/data_block.h"

using namespace std;
using namespace zkv;
Options options;
Iterator* getIter(string_view contents) {
	DataBlock* dataBlock = new DataBlock(contents);
	// cout << dataBlock->NumRestarts() << endl;
	Iterator* iter = dataBlock->NewIterator(options.comparator);
	delete dataBlock;
	return iter;
}
int main() {
	options.block_restart_interval = 16;
	options.block_compress_type = kSnappyCompression;
	options.filter_policy = std::make_unique<BloomFilter>(30);
	options.comparator = std::make_unique<ByteComparator>();
	vector<string> kTestKeys = {};
	for (int i = 0; i < 10; ++i) {
		int idx = i;

		string k = "key";
		k += to_string(idx);
		string_view key = static_cast<string_view>(k);
		kTestKeys.emplace_back(key);
	}
	sort(kTestKeys.begin(), kTestKeys.end());



	DataBlockBuilder* blockBuilder = new DataBlockBuilder(&options);
	for (const auto& k : kTestKeys) {
		const string_view key = static_cast<string_view>(k);
		// string v = "_value:";
		// v += k;
		// v += "_";
		// const string_view value = static_cast<string_view>(v);
		blockBuilder->Add(key, key);
	}
	blockBuilder->Finish();

	string data = blockBuilder->Data();
	delete blockBuilder;

	string_view contents = data;
	// DataBlock* dataBlock = new DataBlock(contents);
	// // cout << dataBlock->NumRestarts() << endl;
	// Iterator* iter = dataBlock->NewIterator(options.comparator);
	auto iter = getIter(contents);
	iter->SeekToFirst();
	while (iter->Valid()) {
		cout << "[" << iter->key() << "," << iter->value() << "]"
		<< " ";
		iter->Next();
	}
	cout << endl;
	// iter->SeekToLast();
	// while (iter->Valid()) {
	// 	cout << "[" << iter->key() << "," << iter->value() << "]"
	// 			<< " ";
	// 	iter->Prev();
	// }
	// cout << endl;
	// iter->Seek("key2");
	// cout << iter->value();
	// for (int i = 0; i < 1000; i += 3) {
	// 	string_view key = static_cast<string_view>(kTestKeys[i]);
	// 	iter->Seek(key);
	// 	cout << "[" << iter->key() << "," << iter->value() << "]"
	// 		<< " ";
	// }
	// cout << endl;
	// delete dataBlock;
}