#include <string>
#include <vector>
#include <iostream>
#include "../string_util.h"

int main() {
    std::vector<std::string> v;
    zkv::string_util::Split("asdf", 's', v);
    for(size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i] << std::endl;
    }
}