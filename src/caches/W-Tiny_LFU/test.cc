#include "WindowsTinyLFU.hpp"
#include <iostream>
#include <string>
int main() {
    WindowsTinyLFU<std::string> cache(20);
    cache.Put("asdfas", "asdfas");
    cache.Put("quweor", "asdfas");
    cache.Put("asdj123", "asdfas");
    auto node = cache.Get("asdfas");
    if(node.second) {
        std::cout << node.first << std::endl;
    }
}