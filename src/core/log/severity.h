#pragma once

#include <cstdint>

namespace yas {
enum LogSeverity: uint8_t {DEBUG, INFO, WARN, ERROR, CRIT, ALL};
static const char* SeverityStrings[] = {
    "D",  // debug
    "I",  // info
    "W",  // warn
    "E",  // error
    "C"   // crit
};

}  // namespace yas