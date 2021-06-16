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
  std::string format(LogSeverity severity, std::string fort, va_list args) {
    return format(severity, fort.c_str(), args);
  }
  virtual std::string format(LogSeverity severity, const char* fort,
                             va_list args) = 0;
  virtual std::string format(LogSeverity severity, const char* file_name,
                             int line, const char* fort, va_list args) = 0;

 protected:
  static const int kMaxLogLength = 30000;
};
}  // namespace yas