#ifndef _PSTRDATA_HPP_
#define _PSTRDATA_HPP_ 1

#include <stdio.h>

#include "../src/zstring.hpp"

inline void pstrdata(const zstring& s) {
    printf("data|%s|\n", s.data());
    printf("size|%zu|\n", s.size());
    printf("capacity|%zu|\n", s.capacity());
    printf("is_on_heap|%d|\n", s.is_on_heap());
}

#endif
