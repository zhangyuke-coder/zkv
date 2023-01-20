#include <vector>
#include <string>
#include <iostream>
#include "db/skiplist.h"
#include "memory/area.h"
#include "db/comparator.h"
using namespace std;
using namespace zkv;
static vector<string> kTestKeys = {"corekv", "corekv1", "corekv2", "corekv3", "corekv4", "corekv5"};

int main() {
    // using Table = SkipList<const char*, ByteComparator, SimpleVectorAlloc>;
    ByteComparator byte_comparator;
    // Table tb(byte_comparator);
    SkipList<const char*, ByteComparator, SimpleVectorAlloc> tb(byte_comparator);
    for(int i = 0; i < 1000; ++i) {
        kTestKeys.emplace_back(to_string(i));
    }
    // for(string item : kTestKeys) {
    //     tb.Insert(item.c_str());
    // }
    // string s = "corekv3";
    // tb.Insert("corekv");
    // tb.Insert("corekv1");
    // tb.Insert("corekv2");
    // // tb.Insert("corekv3");
    // tb.Insert("corekv4");
    for(size_t i = 0; i < kTestKeys.size(); i++) {
        tb.Insert(kTestKeys[i].c_str());
    }
    // tb.Insert("corekv2");
    // tb.Insert("asdf");
    // tb.Insert("1");
    // tb.Insert("2");
    // tb.Insert("3");
    // if(tb.Contains("corekv2")) {
    //     cout << "Yes" << endl;
    // } else {
    //     cout << "No" << endl;
    // }
    // cout << tb.Contains("asdf")
    // tb.Insert(to_string(2).c_str());
    // for(auto& item : kTestKeys) {
    //     if(tb.Contains(item.c_str())) {
    //         cout << item << "  " << "true" << endl;
    //     } else {
    //         cout << item << "  " << "false" << endl;
    //     }
        
    // }
    cout << "success" << endl;
    // if(tb.Contains("corekv3")) {
    //         cout << "true" << endl;
    //     } else {
    //         cout << "false" << endl;
    // }
}