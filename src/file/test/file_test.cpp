#include <iostream>
#include "../file.h"
#include <cmath>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int main() {
    zkv::FileWriter writer("asdf", true);
    writer.Append("asdfqwer", 8);
    writer.Append("asdfqwer", 8);

    writer.FlushBuffer();
    zkv::FileReader reader("./asdf");
    std::string res;
    auto sta = reader.Read(0, 20, &res);
    printf("%s", sta.message);
    std::cout << zkv::FileTool::GetFileSize("./asdf") << std::endl;
    std::cout << res << std::endl;
    printf("%s\n", res.c_str());
}