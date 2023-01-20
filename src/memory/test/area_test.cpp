#include <gtest/gtest.h>
#include <iostream>
#include "../area.h"


// TEST(allocTest, Allocate) {
//     int nCount = 4;
//     zkv::SimpleVectorAlloc simple_vector_alloc;
//     int* a = (int *)simple_vector_alloc.Allocate(sizeof(int) * nCount);
//     for(int i = 0; i < nCount; ++i) {
//         a[i] = i;
//     }

// //   int *a = (int *)simple_freelist_alloc.Allocate(sizeof(int) * nCount);
// //   for (int i = 0; i < nCount; ++i) {
// //     a[i] = i;
// //   }

//     std::cout << "a[](" << a << "): ";
//     for (int i = 0; i < nCount; ++i) {
//         std::cout << a[i] << " ";
//     }

// //   simple_freelist_alloc.Deallocate(a, sizeof(int) * nCount);

// //   std::cout << std::endl;
// //   std::cout << "a[](" << a << "): ";
// //   for (int i = 0; i < nCount; ++i) {
// //     std::cout << a[i] << " ";
// //   }
// }
int main() {
    zkv::SimpleVectorAlloc a;
    int* b = (int *)a.Allocate(sizeof(int) * 1000);
    for(int i = 0; i < 1000; ++i) {
        b[i] = i;
    }
    int64_t* c = (int64_t*)a.Allocate(sizeof(int64_t) * 50);
    c[0] =12;
    std::cout << b[999] << std::endl;
    std::cout << c[0] << std::endl;  
    char* d = (char*)a.Allocate(sizeof(char) * 2);
    d[1] = 'f';
    std::cout << d[1] << std::endl;  

}
