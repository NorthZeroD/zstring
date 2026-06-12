#include <stdio.h>

#include "string.hpp"

int main() {
    static_assert(sizeof(string) == 24);
    string str("𰻝𰻝𰻝𰻝𰻝𰻝");

    auto bytes = str.get_self_bytes();
    for (size_t i = 0; i < sizeof(string); ++i) {
        printf("%08hhB ", bytes[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("str.data() : %s\n", str.data_mutable());
    printf("str.is_on_heap() : %d\n", str.is_on_heap());
    printf("str.size() : %zu\n", str.size());
    printf("str.count() : %zu\n", str.count());
    printf("str.capacity() : %zu\n", str.capacity());
    printf("===========================================\n");

    str = "ovo";
    printf("str.data() : %s\n", str.data_mutable());
    printf("str.is_on_heap() : %d\n", str.is_on_heap());
    printf("str.size() : %zu\n", str.size());
    printf("str.count() : %zu\n", str.count());
    printf("str.capacity() : %zu\n", str.capacity());
    printf("===========================================\n");

    str.reserve(100);
    printf("str.data() : %s\n", str.data_mutable());
    printf("str.is_on_heap() : %d\n", str.is_on_heap());
    printf("str.size() : %zu\n", str.size());
    printf("str.count() : %zu\n", str.count());
    printf("str.capacity() : %zu\n", str.capacity());
    printf("===========================================\n");

    str = "awalol-------------------------------------";
    printf("str.data() : %s\n", str.data_mutable());
    printf("str.is_on_heap() : %d\n", str.is_on_heap());
    printf("str.size() : %zu\n", str.size());
    printf("str.count() : %zu\n", str.count());
    printf("str.capacity() : %zu\n", str.capacity());
    printf("===========================================\n");

    str.reserve(3);
    printf("str.data() : %s\n", str.data_mutable());
    printf("str.is_on_heap() : %d\n", str.is_on_heap());
    printf("str.size() : %zu\n", str.size());
    printf("str.count() : %zu\n", str.count());
    printf("str.capacity() : %zu\n", str.capacity());
    printf("===========================================\n");

    string str1{};
    printf("str1.data() : %s\n", str1.data_mutable());
    printf("str1.is_on_heap() : %d\n", str1.is_on_heap());
    printf("str1.size() : %zu\n", str1.size());
    printf("str1.count() : %zu\n", str1.count());
    printf("str1.capacity() : %zu\n", str1.capacity());
    printf("===========================================\n");

    return 0;
}
