#pragma once
#include <memory>
#include <mutex>
#include <string>

#include "formatter.h"
#include "severity.h"

namespace yas {
class Sink {
 public:
  virtual ~Sink() = default;
  Sink(std::unique_ptr<Formatter> formatter)
      : severity_(LogSeverity::INFO), formatter_(std::move(formatter)) {}
  virtual void log(LogSeverity severity, const char* format, va_list va) = 0;
  virtual void log(LogSeverity severity, const char* file_name, int line,
                   const char* format, va_list va) = 0;
  virtual void flush() = 0;
  virtual void set_formatter(std::unique_ptr<Formatter> f);
  bool should_log(LogSeverity severity);
  void set_severity(LogSeverity severity);
  LogSeverity get_severity();

 protected:
  LogSeverity severity_;
  std::unique_ptr<Formatter> formatter_;
  std::mutex mutex_;
};
}  // namespace yas