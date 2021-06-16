#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

#include "severity.h"

namespace yas {
class Formatter {
 public:
  Formatter(/* args */) = default;
  virtual ~Formatter() = default;
  std::string format(LogSeverity severity, std::string fort, ...) {
    va_list va;
    va_start(va, fort);
    std::string result = format(severity, fort.c_str(), va);
    va_end(va);
    return result;
  }
  virtual std::string format(LogSeverity severity, const char* fort, ...) = 0;

 protected:
  static const int kMaxLogLength=30000;
};
}  // namespace yas