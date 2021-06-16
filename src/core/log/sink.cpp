#include "sink.h"

namespace yas {
bool Sink::should_log(LogSeverity severity) {
  if (severity >= severity_) {
    return true;
  }
  return false;
}

void Sink::set_severity(LogSeverity severity) { severity_ = severity; }

LogSeverity Sink::get_severity() { return severity_; }

void Sink::set_formatter(std::unique_ptr<Formatter> f) {
  formatter_ = std::move(f);
}
}  // namespace yas