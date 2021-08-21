#pragma once

#include <cstdarg>
#include <memory>

#include "logger.h"
#include "severity.h"

namespace yas {
std::shared_ptr<Logger> get_default_logger();
void set_default_logger(std::shared_ptr<Logger> logger);
void set_severity(LogSeverity severity);

void log(LogSeverity severity, const char* format, ...);
void log_debug(const char* format, ...);
void log_info(const char* format, ...);
void log_warn(const char* format, ...);
void log_error(const char* format, ...);
void log_crit(const char* format, ...);

#define LOG(severity, ...)                                                \
  do {                                                                    \
    get_default_logger()->log(severity, __FILE__, __LINE__, __VA_ARGS__); \
  }

#define LOG_DEBUG(...)                                                \
  do {                                                                \
    get_default_logger()->log(LogSeverity::DEBUG, __FILE__, __LINE__, \
                              __VA_ARGS__);                           \
  } while (0)

#define LOG_INFO(...)                                                \
  do {                                                               \
    get_default_logger()->log(LogSeverity::INFO, __FILE__, __LINE__, \
                              __VA_ARGS__);                          \
  } while (0)

#define LOG_WARN(...)                                                \
  do {                                                               \
    get_default_logger()->log(LogSeverity::WARN, __FILE__, __LINE__, \
                              __VA_ARGS__);                          \
  } while (0)

#define LOG_ERROR(...)                                                \
  do {                                                                \
    get_default_logger()->log(LogSeverity::ERROR, __FILE__, __LINE__, \
                              __VA_ARGS__);                           \
  } while (0)

#define LOG_CRIT(...)                                                \
  do {                                                               \
    get_default_logger()->log(LogSeverity::CRIT, __FILE__, __LINE__, \
                              __VA_ARGS__);                          \
  } while (0)

#define LOG_IF(condition, severity, ...)                                    \
  do {                                                                      \
    if ((condition))                                                        \
      get_default_logger()->log(severity, __FILE__, __LINE__, __VA_ARGS__); \
  } while (0)

#define LOG_DEBUG_IF(condition, ...)                                    \
  do {                                                                  \
    if ((condition))                                                    \
      get_default_logger()->log(LogSeverity::DEBUG, __FILE__, __LINE__, \
                                __VA_ARGS__);                           \
  } while (0)

#define LOG_INFO_IF(condition, ...)                                    \
  do {                                                                 \
    if ((condition))                                                   \
      get_default_logger()->log(LogSeverity::INFO, __FILE__, __LINE__, \
                                __VA_ARGS__);                          \
  } while (0)

#define LOG_WARN_IF(condition, ...)                                    \
  do {                                                                 \
    if ((condition))                                                   \
      get_default_logger()->log(LogSeverity::WARN, __FILE__, __LINE__, \
                                __VA_ARGS__);                          \
  } while (0)

#define LOG_ERROR_IF(condition, ...)                                    \
  do {                                                                  \
    if ((condition))                                                    \
      get_default_logger()->log(LogSeverity::ERROR, __FILE__, __LINE__, \
                                __VA_ARGS__);                           \
  } while (0)

#define LOG_CRIT_IF(condition, ...)                                    \
  do {                                                                 \
    if ((condition))                                                   \
      get_default_logger()->log(LogSeverity::CRIT, __FILE__, __LINE__, \
                                __VA_ARGS__);                          \
  } while (0)

}  // namespace yas