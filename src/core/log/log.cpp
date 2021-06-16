#include "log.h"

#include "loggers.h"

namespace yas {
std::shared_ptr<Logger> get_default_logger() {
  return Loggers::instance()->get_default_logger();
}

void set_default_logger(std::shared_ptr<Logger> logger) {
  Loggers::instance()->set_default_logger(logger);
}

void set_severity(LogSeverity severity) {
  Loggers::instance()->set_severity(severity);
}

}  // namespace yas