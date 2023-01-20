#include <string>
#include <string_view>
#include <iostream>
#include "../codec.h"
using namespace zkv;
using namespace zkv::util;
using namespace std;
int main() {
    char buf[4];
    EncodeFixed32(buf, 0x12345678);
    // cout << buf << endl;
    for(int i = 0; i < 4; ++i) {
        printf("%x \n", buf[i]);
    }
}