#include <string>
#include <string_view>
#include <iostream>
#include "../comparator.h"

int main() {
    zkv::Comparator* comp = new zkv::ByteComparator();
    std::string a = "asdfg";
    std::string_view b = "asff";
    comp->FindShortest(a, b);
    std::cout << a << std::endl;
}