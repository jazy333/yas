#pragma once

#include <memory>
#include <string>

#include "formatter.h"
#include "severity.h"
#include "sink.h"

namespace yas {
class Logger {
 public:
  Logger() = default;
  Logger(std::string name, std::shared_ptr<Sink> sink, LogSeverity severity)
      : name_(name), sink_(sink), severity_(severity) {}
  Logger(std::string name, std::shared_ptr<Sink> sink)
      : name_(name), sink_(sink) {}
  virtual ~Logger() = default;
  Logger(const Logger&);
  Logger(Logger&& logger);
  Logger& operator=(const Logger&);
  Logger& operator=(Logger&&);
  std::string get_name();
  void set_name(std::string& name);
  std::shared_ptr<Sink>& get_sink();
  void set_sink(std::shared_ptr<Sink> sink);
  void set_severity(LogSeverity severity);
  LogSeverity get_severity();
  void log(LogSeverity severity, const char* format, va_list args);
  void log(LogSeverity severity, const char* file_name, int line,
           const char* format, ...);
  void flush();

 private:
  std::string name_;
  std::shared_ptr<Sink> sink_;
  LogSeverity severity_ = LogSeverity::INFO;
};
}  // namespace yas