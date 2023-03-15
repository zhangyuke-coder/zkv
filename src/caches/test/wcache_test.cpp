#include "caches/cache.h"
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
using namespace std;
using namespace zkv;
static const vector<string> kTestKeys = {"corekv", "corekv1", "corekv2"};
int main() {
    std::shared_ptr<Cache<std::string, std::string>> cache_handler =
        std::make_shared<WTinyLFU<std::string, std::string>>(10);
    cache_handler->RegistCleanHandle([](const std::string&, std::string* value){
        delete value;
        std::cout << "value:"<< std::endl;
    });
    std::string corekv = "corekv";
    std::string* corekv1 = new std::string("corekv");
    cache_handler->Insert(corekv, corekv1);
    CacheNode<std::string, std::string>* node = cache_handler->Get(corekv);

    std::cout << "value:" << *(node->value) << std::endl;
    cache_handler->Release(node);
}