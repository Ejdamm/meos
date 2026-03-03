#include "util_header.h"
#include <iostream>

void util_dummy_func() {
#if UTIL_HEADER_WORKS
    std::cout << "Util header works!" << std::endl;
#endif
}
