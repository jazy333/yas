#include "loggers.h"

namespace yas {
Loggers* Loggers::static_loggers_ = new Loggers;
std::shared_ptr<Logger> Loggers::get_default_logger() { return loggers_[""]; }
void Loggers::set_default_logger(std::shared_ptr<Logger> logger) {
  loggers_[""] = logger;
}

void Loggers::set_severity(LogSeverity severity) {
  for (auto& logger : loggers_) {
    logger.second->set_severity(severity);
  }
}

void Loggers::set_logger(std::string name, std::shared_ptr<Logger> logger) {
  if (!logger) return;
  loggers_[name] = logger;
}

std::shared_ptr<Logger> Loggers::get_logger(std::string name) {
  if (loggers_.count(name) == 1)
    return loggers_[name];
  else
    return nullptr;
}

}  // namespace yas