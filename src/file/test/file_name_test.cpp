#include <iostream>
#include "file/file_name.h"

int main() {
    std::cout << zkv::FileName::DescriptorFileName("zyk", "h") << std::endl;
    std::cout << zkv::FileName::FileNameSSTable("zyk", 489412) << std::endl;
    std::string s = "aaaaaa";
    auto l = s.rfind('a');
    std::cout << l << std::endl;
}
