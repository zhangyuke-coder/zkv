#include <vector>
#include <iostream>
#include "../bloomfilter.h"
#include "../filter_policy.h"
using namespace zkv;

int main() {
    FilterPolicy* filter = new BloomFilter(3);
    std::vector<std::string> keys = {"qwer", "asdf", "zxcv", "tyii"};
    filter->CreateFilter(&keys[0], 4);
    if(filter->MayMatch("qwer", 0, 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }


    if(filter->MayMatch("as", 0, 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
    std::vector<std::string> keys2 = {"123", "345", "456", "678"};
    int32_t pos = filter->Data().size();
    filter->CreateFilter(&keys2[0], 4);
    if(filter->MayMatch("345", pos, 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
    if(filter->MayMatch("678", pos, 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
     if(filter->MayMatch("13", pos, 0)) {
        std::cout << "yes" << std::endl;
    } else {
        std::cout << "no" << std::endl;
    }
}
