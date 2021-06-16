#pragma once
#include "formatter.h"
#include "severity.h"

namespace yas {
class DefaultFormatter : public Formatter {
 public:
  DefaultFormatter() = default;
  virtual ~DefaultFormatter() = default;
  virtual std::string format(LogSeverity severity, const char* format,
                             va_list args) override;
  virtual std::string format(LogSeverity severity, const char* file_name,
                             int line, const char* format, va_list args) override;

 private:
  std::string do_format(LogSeverity severity, const char* file_name, int line,
                        const char* format, va_list args);
};
}  // namespace yas