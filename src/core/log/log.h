#pragma once

#include <cstdarg>
#include <memory>

#include "logger.h"
#include "severity.h"

namespace yas {
std::shared_ptr<Logger> get_default_logger();
void set_default_logger(std::shared_ptr<Logger> logger);
void set_severity(LogSeverity severity);

void log(LogSeverity severity, const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(severity, format, va);
  va_end(va);
}

void log_debug(const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(LogSeverity::DEBUG, format, va);
  va_end(va);
}

void log_info(const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(LogSeverity::INFO, format, va);
  va_end(va);
}

void log_warn(const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(LogSeverity::WARN, format, va);
  va_end(va);
}

void log_error(const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(LogSeverity::ERROR, format, va);
  va_end(va);
}

void log_crit(const char* format, ...) {
  va_list va;
  va_start(va, format);
  get_default_logger()->log(LogSeverity::CRIT, format, va);
  va_end(va);
}

}  // namespace yas