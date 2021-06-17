#pragma once
#include <string>
#include <unordered_map>

#include "file_sink.h"
#include "logger.h"
#include "severity.h"

namespace yas {
class Loggers {
 public:
  Loggers() {
    std::string default_log_name = "";
    std::shared_ptr<Logger> default_logger = std::make_shared<Logger>(
        default_log_name, std::make_shared<FileSink>(stdout));
    loggers_[default_log_name] = default_logger;
  }
  virtual ~Loggers() { delete static_loggers_; }
  static Loggers* instance() { return static_loggers_; }
  std::shared_ptr<Logger> get_default_logger();
  void set_default_logger(std::shared_ptr<Logger> logger);
  void set_severity(LogSeverity severity);
  void set_logger(std::string name, std::shared_ptr<Logger> logger);
  std::shared_ptr<Logger> get_logger(std::string name);

 private:
  std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
  static Loggers* static_loggers_;
};

}  // namespace yas