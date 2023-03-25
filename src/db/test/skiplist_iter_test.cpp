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
    tb.Insert(kTestKeys[0].data());
    tb.Insert(kTestKeys[1].data());
    tb.Insert(kTestKeys[2].data());
    tb.Insert(kTestKeys[3].data());
    tb.Insert(kTestKeys[4].data());
    tb.Insert(kTestKeys[5].data());
    SkipList<const char*, ByteComparator, SimpleVectorAlloc>::Iterator it(&tb);
    it.SeekToFirst();
    while(it.Valid()) {
        cout << it.key() << endl;
        it.Next();
    }

}