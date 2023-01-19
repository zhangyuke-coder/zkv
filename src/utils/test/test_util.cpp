#include <string>
#include <iostream>
#include <stdint.h>
#include "../util.h"
using namespace zkv;
using namespace zkv::util;
using namespace std;
int main() {
    cout << "curTime" << GetCurrentTime() << endl;
    string CurrentTime;
    GetCurrentTimeString(CurrentTime);
    cout << "curTimeString" << CurrentTime << endl;
    cout << "CurrentTid" << GetCurrentTid() << endl;
    cout << "CurrentPid" << GetCurrentPid() << endl;
    if(CheckLittleEndian()) {
        cout << "Yes" << endl;
    } else {
        cout << "No" << endl;
    }
    const char* p = "1";
    uint32_t res = DecodeFixed32(p);
    cout << res << endl;
    char a = 1;
    // uint8_t b = static_cast<uint8_t>(a);
    cout << static_cast<uint32_t>(a) << endl;
}