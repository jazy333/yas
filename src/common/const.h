#pragma once
#include <unistd.h>
#include <cstdint>

namespace yas {
const int32_t PAGE_SIZE = sysconf(_SC_PAGESIZE);
}