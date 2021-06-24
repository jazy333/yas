#include "logger.h"

namespace yas {
Logger::Logger(const Logger& logger)
    : name_(logger.name_), sink_(logger.sink_), severity_(logger.severity_) {}

Logger::Logger(Logger&& logger)
    : name_(std::move(logger.name_)),
      sink_(std::move(logger.sink_)),
      severity_(logger.severity_) {}

Logger& Logger::operator=(const Logger& logger) {
  if (&logger != this) {
    name_ = logger.name_;
    sink_ = logger.sink_;
    severity_ = logger.severity_;
  }
  return *this;
}

Logger& Logger::operator=(Logger&& logger) {
  if (&logger != this) {
    name_ = std::move(logger.name_);
    sink_ = std::move(logger.sink_);
    severity_ = logger.severity_;
  }
  return *this;
}

std::string Logger::get_name() { return name_; }

void Logger::set_name(const std::string& name) { name_ = name; }

std::shared_ptr<Sink>& Logger::get_sink() { return sink_; }

void Logger::set_sink(std::shared_ptr<Sink> sink) { sink_ = sink; }

void Logger::set_severity(LogSeverity severity) {
  severity_ = severity;
  sink_->set_severity(severity);
}
LogSeverity Logger::get_severity() { return severity_; }
void Logger::flush() { return sink_->flush(); }

void Logger::log(LogSeverity severity, const char* format, va_list args) {
  sink_->log(severity, format, args);
}

void Logger::log(LogSeverity severity, const char* file_name, int line,
                 const char* format, ...) {
  va_list va;
  va_start(va, format);
  sink_->log(severity, file_name, line, format, va);
  va_end(va);
}
}  // namespace yas