#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

#include "zstring.hpp"

int main() {
    static_assert(sizeof(zstring) == 24);
}
