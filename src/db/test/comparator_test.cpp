#include <string>
#include <string_view>
#include <iostream>
#include "../comparator.h"

int main() {
    zkv::Comparator* comp = new zkv::ByteComparator();
    std::string a = "asdfgd";
    std::string_view b = "asdfge";
    comp->FindShortest(a, b);
    std::cout << a << std::endl;
}