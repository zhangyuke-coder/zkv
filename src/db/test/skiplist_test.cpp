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
    // for(int i = 0; i < 100; ++i) {
    //     kTestKeys.emplace_back(to_string(i));
    // }

    // for(size_t i = 0; i < kTestKeys.size(); i++) {
    //     tb.Insert(kTestKeys[i].c_str());
    // }
    // cout << "success" << endl;
    // tb.Insert("zyk");
    // tb.Insert("asd");
    // if(tb.Contains("zyk")) {
    //     cout << "true" << endl;
    // } else {
    //     cout << "false" << endl;
    // }

    // if(tb.Contains("ads")) {
    //     cout << "true" << endl;
    // } else {
    //     cout << "false" << endl;
    // }
    // for(auto s : kTestKeys) {
    //     tb.Insert(s.data());
    // }

    for(auto s : kTestKeys) {
        tb.Insert(s.data());
        // if(tb.Contains(s.data())) {
        //     cout << "true" << endl;
        // } else {
        //     cout << "false" << endl;
        // }
    }





    // tb.Insert(kTestKeys[0].data());
    // tb.Insert(kTestKeys[1].data());
    // tb.Insert(kTestKeys[2].data());
    // tb.Insert(kTestKeys[3].data());
    // tb.Insert(kTestKeys[4].data());
    // tb.Insert(kTestKeys[5].data());
    if(tb.Contains("corekv")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    if(tb.Contains("corekv1")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    if(tb.Contains("corekv2")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    if(tb.Contains("corekv3")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    if(tb.Contains("corekv4")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }
    if(tb.Contains("corekv5")) {
        cout << "true" << endl;
    } else {
        cout << "false" << endl;
    }

}