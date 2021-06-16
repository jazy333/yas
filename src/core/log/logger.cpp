#include "logger.h"

namespace yas {
std::string Logger::get_name() { return name_; }
void Logger::set_name(std::string& name) { name_ = name; }
std::unique_ptr<Sink>& Logger::get_sink() { return sink_; }
void Logger::set_sink(std::unique_ptr<Sink> sink) { sink_ = std::move(sink); }

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